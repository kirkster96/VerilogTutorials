//
// Copyright (c) 2017 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arm_neon.h>
#include <assert.h>

#include "mandelbrot_sw_support/mandelbrot_sw_support.h"

#define VECTOR_4	(4)
#define LOOP_CNT	(2)
#define THREAD_POOL_CNT	(10)

// calculate the mandelbrot algorithm with intrinsic floating point math
static void vect4_float_mandelbrot(float cr[VECTOR_4], float ci[VECTOR_4],
		uint32_t iters[VECTOR_4], uint32_t max_iter) {

	float32x4_t xsqr_v = vdupq_n_f32(0.0);
	float32x4_t ysqr_v = vdupq_n_f32(0.0);
	float32x4_t x_v = vdupq_n_f32(0.0);
	float32x4_t y_v = vdupq_n_f32(0.0);
	float32x4_t xsqr_plus_ysqr_v = vdupq_n_f32(0.0);
	float32x4_t cr_v = vld1q_f32(cr);
	float32x4_t ci_v = vld1q_f32(ci);
	uint32x4_t iters_v = vdupq_n_u32(0);
	uint32x4_t xsqr_plus_ysqr_lt_4p0_v;
	uint32x4_t completion_mask_v = vdupq_n_u32(0xFFFFFFFF);
	uint32_t iter = 0;
	uint32x2_t temp_sum_v;
	int i;

	while(iter < max_iter) {
		for(i = 0 ; i < LOOP_CNT ; i++) {
			// loop until no vector element is less than 4.0 distance
			xsqr_plus_ysqr_lt_4p0_v = vcltq_f32(
				xsqr_plus_ysqr_v, vdupq_n_f32(4.0));
			// use latest result to mask our completion mask
			completion_mask_v = vandq_u32(
				xsqr_plus_ysqr_lt_4p0_v, completion_mask_v);
			// use latest completion mask to mask latest result
			xsqr_plus_ysqr_lt_4p0_v = vandq_u32(
				xsqr_plus_ysqr_lt_4p0_v, completion_mask_v);

			// increment the active elements
			iters_v = vaddq_u32(iters_v,
					vbslq_u32(xsqr_plus_ysqr_lt_4p0_v,
					vdupq_n_u32(1), vdupq_n_u32(0)));

			// compute x^2 and y^2
			xsqr_v = vmulq_f32(x_v, x_v);
			ysqr_v = vmulq_f32(y_v, y_v);
			// new y = (2 * x * y) + ci
			y_v = vaddq_f32(vmulq_f32(vdupq_n_f32(2.0),
						vmulq_f32(x_v, y_v)), ci_v);
			// new x = (x^2 - y^2) + cr
			x_v = vaddq_f32(vsubq_f32(xsqr_v, ysqr_v), cr_v);
			// calculate distance vector x^2 + y^2
			xsqr_plus_ysqr_v = vaddq_f32(xsqr_v, ysqr_v);
		}

		// when all completion mask elements are zero we are done
		temp_sum_v = vpadd_u32(vget_high_u32(completion_mask_v),
					vget_low_u32(completion_mask_v));
		if(vget_lane_u32(temp_sum_v, 0) == 0)
			if(vget_lane_u32(temp_sum_v, 1) == 0)
				break;

		iter += LOOP_CNT;
	}

	// save iteration counts
	vst1q_u32(iters, iters_v);
	return;
}

// loop overhead for the floating point mandelbrot intrinsic algorithm
void draw_vect4_float_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height) {

	uint32_t max_iter;
	uint32_t iters_vect[VECTOR_4];
	uint32_t i;
	uint32_t j;
	int k;
	float leftmost_x;
	float current_x;
	float current_y;
	float x_vect[VECTOR_4];
	float y_vect[VECTOR_4];
	float x_dim;
	float center_x;
	float center_y;
	float step_dim;

	// verify pixmap_width is modulo 4
	assert((pixmap_width % 4) == 0);

	// set the initial center point, x dimension, and maximum iterations
	center_x = in_x;
	center_y = in_y;
	x_dim = in_x_dim;
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	leftmost_x = current_x;
	current_y = center_y + ((pixmap_height / 2) * step_dim);

	// process the image

	// i counts the rows in the image
	for(i = 0 ; i < pixmap_height ; i++) {
		// j counts the columns in each row
		for(j = 0 ; j < pixmap_width ; j += VECTOR_4) {
			// allow thread to cancel here
			pthread_testcancel();
			// evaluate this coordinate
			for(k = 0 ; k < VECTOR_4 ; k++) {
				x_vect[k] = current_x;
				// increment to next coordinate
				current_x += step_dim;
			}

			for(k = 0 ; k < VECTOR_4 ; k++) {
				y_vect[k] = current_y;
			}

			vect4_float_mandelbrot(x_vect, y_vect, iters_vect,
								 max_iter);

			for(k = 0 ; k < VECTOR_4 ; k++) {
				if(iters_vect[k] >= max_iter) {
					// it's in the set
					set_pixel_value(max_iter, pixel_buf_ptr,
							pixmap_width, i, j + k,
							max_iter);
				} else {
					// it's out of the set
					set_pixel_value(max_iter, pixel_buf_ptr,
							pixmap_width, i, j + k,
							iters_vect[k]);
				}
			}
		}
		// at the end of each row, reset coordinate to the first columm
		current_x = leftmost_x;
		// and increment to the next coordinate row
		current_y -= step_dim;
	}
}

struct thread_s {
	uint32_t pixmap_width;
	void *pixel_buf_ptr;
	uint32_t i;
	uint32_t max_iter;
	float current_x;
	float current_y;
	float step_dim;
};

static void *vect4_float_mandelbrot_thread(void *arg) {

	struct thread_s *thread_s_ptr = (struct thread_s *)arg;
	uint32_t pixmap_width = thread_s_ptr->pixmap_width;
	void *pixel_buf_ptr = thread_s_ptr->pixel_buf_ptr;
	uint32_t i = thread_s_ptr->i;
	uint32_t max_iter = thread_s_ptr->max_iter;
	float current_x = thread_s_ptr->current_x;
	float current_y = thread_s_ptr->current_y;
	float step_dim = thread_s_ptr->step_dim;

	uint32_t j;
	int k;
	float x_vect[VECTOR_4];
	float y_vect[VECTOR_4];
	uint32_t iters_vect[VECTOR_4];

	for(j = 0 ; j < pixmap_width ; j += VECTOR_4) {
		// allow thread to cancel here
		pthread_testcancel();
		// evaluate this coordinate
		for(k = 0 ; k < VECTOR_4 ; k++) {
			x_vect[k] = current_x;
			// increment to next coordinate
			current_x += step_dim;
		}

		for(k = 0 ; k < VECTOR_4 ; k++) {
			y_vect[k] = current_y;
		}

		vect4_float_mandelbrot(x_vect, y_vect, iters_vect,
							 max_iter);

		for(k = 0 ; k < VECTOR_4 ; k++) {
			if(iters_vect[k] >= max_iter) {
				// it's in the set
				set_pixel_value(max_iter, pixel_buf_ptr,
							pixmap_width, i, j + k,
							max_iter);
			} else {
				// it's out of the set
				set_pixel_value(max_iter, pixel_buf_ptr,
							pixmap_width, i, j + k,
							iters_vect[k]);
			}
		}
	}

	return(NULL);
}

static void thread_cleanup_thread_pool(void *arg) {

	pthread_t *thread_pool = (pthread_t *)arg;
	int pool_index;
	int result;

	for(pool_index = 0 ; pool_index < THREAD_POOL_CNT ; pool_index++) {

		if(thread_pool[pool_index] != 0) {

			result = pthread_cancel(thread_pool[pool_index]);
			assert((result == 0) || (result == ESRCH));

			result = pthread_join( thread_pool[pool_index], NULL);
			assert(result == 0);

			thread_pool[pool_index] = 0;
		}
	}
}

// mt loop overhead for the floating point mandelbrot intrinsic algorithm
void draw_vect4_float_mandelbrot_mt(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height) {

	uint32_t max_iter;
	uint32_t i;
	float leftmost_x;
	float current_x;
	float current_y;
	float x_dim;
	float center_x;
	float center_y;
	float step_dim;
	struct thread_s the_thread_s[THREAD_POOL_CNT];
	struct thread_s *next_struct_alloc;
	pthread_t thread_pool[THREAD_POOL_CNT] = {0};
	pthread_t *next_thread_alloc;
	int pool_index;
	int result;
	int threads_joined;

	// verify pixmap_width is modulo 4
	assert((pixmap_width % 4) == 0);

	pthread_cleanup_push(thread_cleanup_thread_pool, &thread_pool[0]);

	// set the initial center point, x dimension, and maximum iterations
	center_x = in_x;
	center_y = in_y;
	x_dim = in_x_dim;
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	leftmost_x = current_x;
	current_y = center_y + ((pixmap_height / 2) * step_dim);

	// process the image

	// i counts the rows in the image
	for(i = 0 ; i < pixmap_height ; i++) {

		pthread_testcancel();

		next_thread_alloc = NULL;
		while(next_thread_alloc == NULL) {
			for(pool_index = 0 ; pool_index < THREAD_POOL_CNT ;
								pool_index++) {

				if(thread_pool[pool_index] == 0) {

					next_thread_alloc =
						&thread_pool[pool_index];

					next_struct_alloc =
						&the_thread_s[pool_index];

					the_thread_s[pool_index].pixmap_width =
								pixmap_width;
					the_thread_s[pool_index].pixel_buf_ptr =
								pixel_buf_ptr;
					the_thread_s[pool_index].i = i;
					the_thread_s[pool_index].max_iter =
								max_iter;
					the_thread_s[pool_index].current_x =
								current_x;
					the_thread_s[pool_index].current_y =
								current_y;
					the_thread_s[pool_index].step_dim =
								step_dim;

					break;
				}
			}

			if(next_thread_alloc != NULL)
				break;

			threads_joined = 0;
			for(pool_index = 0 ; pool_index < THREAD_POOL_CNT ;
								pool_index++) {
				result = pthread_tryjoin_np(
						thread_pool[pool_index], NULL);
				if(result == 0) {
					thread_pool[pool_index] = 0;
					threads_joined++;
				}
			}

			pthread_testcancel();
			if(threads_joined == 0)
				pthread_yield();
		}

		result = pthread_create(next_thread_alloc, NULL,
			vect4_float_mandelbrot_thread, next_struct_alloc);
		assert(result == 0);

		// at the end of each row, reset coordinate to the first columm
		current_x = leftmost_x;
		// and increment to the next coordinate row
		current_y -= step_dim;
	}

	for(pool_index = 0 ; pool_index < THREAD_POOL_CNT ; pool_index++) {

		if(thread_pool[pool_index] != 0) {

			result = pthread_join( thread_pool[pool_index], NULL);
			assert(result == 0);

			thread_pool[pool_index] = 0;
		}
	}

	pthread_cleanup_pop(0);
}

