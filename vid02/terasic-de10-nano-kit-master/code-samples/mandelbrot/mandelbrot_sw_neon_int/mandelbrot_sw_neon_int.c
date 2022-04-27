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


// calculate the mandelbrot algorithm with intrinsic integer math
static void vect4_int_mandelbrot(int32_t cr[VECTOR_4], int32_t ci[VECTOR_4],
				uint32_t iters[VECTOR_4], uint32_t max_iter) {

	int64x2_t a_xsqr_v = vmovq_n_s64(0x0);
	int64x2_t a_ysqr_v = vmovq_n_s64(0x0);
	int32x2_t a_x_v = vdup_n_s32(0x0);
	int32x2_t a_y_v = vdup_n_s32(0x0);
	int32x2_t a_cr_v = vld1_s32(cr);
	int32x2_t a_ci_v = vld1_s32(ci);
	int64x2_t a_xsqr_plus_ysqr_v;
	int32x2_t a_short_xsqr_plus_ysqr_v;
	uint32x2_t a_xsqr_plus_ysqr_lt_4p0_v;
	uint32x2_t a_completion_mask_v = vdup_n_u32(0xFFFFFFFF);
	uint32x2_t a_iters_v = vdup_n_u32(0);
	uint32_t iter = 0;
	int i;

	int64x2_t b_xsqr_v = vmovq_n_s64(0x0);
	int64x2_t b_ysqr_v = vmovq_n_s64(0x0);
	int32x2_t b_x_v = vdup_n_s32(0x0);
	int32x2_t b_y_v = vdup_n_s32(0x0);
	int32x2_t b_cr_v = vld1_s32(&cr[2]);
	int32x2_t b_ci_v = vld1_s32(&ci[2]);
	int64x2_t b_xsqr_plus_ysqr_v;
	int32x2_t b_short_xsqr_plus_ysqr_v;
	uint32x2_t b_xsqr_plus_ysqr_lt_4p0_v;
	uint32x2_t b_completion_mask_v = vdup_n_u32(0xFFFFFFFF);
	uint32x2_t b_iters_v = vdup_n_u32(0);

	while(iter < max_iter) {
		for(i = 0 ; i < LOOP_CNT ; i++) {
			//if(xsqr + ysqr) < 0x0400000000000000LL)
			a_xsqr_plus_ysqr_v = vaddq_s64(a_xsqr_v, a_ysqr_v);
			b_xsqr_plus_ysqr_v = vaddq_s64(b_xsqr_v, b_ysqr_v);
			a_short_xsqr_plus_ysqr_v = vqmovn_s64(
				vshrq_n_s64(a_xsqr_plus_ysqr_v, 56));
			b_short_xsqr_plus_ysqr_v = vqmovn_s64(
				vshrq_n_s64(b_xsqr_plus_ysqr_v, 56));
			a_xsqr_plus_ysqr_lt_4p0_v = vclt_s32(
				a_short_xsqr_plus_ysqr_v, vdup_n_s32(0x04));
			b_xsqr_plus_ysqr_lt_4p0_v = vclt_s32(
				b_short_xsqr_plus_ysqr_v, vdup_n_s32(0x04));

			a_completion_mask_v = vand_u32(
				a_xsqr_plus_ysqr_lt_4p0_v, a_completion_mask_v);
			b_completion_mask_v = vand_u32(
				b_xsqr_plus_ysqr_lt_4p0_v, b_completion_mask_v);
			a_xsqr_plus_ysqr_lt_4p0_v = vand_u32(
				a_xsqr_plus_ysqr_lt_4p0_v, a_completion_mask_v);
			b_xsqr_plus_ysqr_lt_4p0_v = vand_u32(
				b_xsqr_plus_ysqr_lt_4p0_v, b_completion_mask_v);

			// increment the active elements
			a_iters_v = vadd_u32(a_iters_v,
				vbsl_u32(a_xsqr_plus_ysqr_lt_4p0_v,
				vdup_n_u32(1), vdup_n_u32(0)));
			b_iters_v = vadd_u32(b_iters_v,
				vbsl_u32(b_xsqr_plus_ysqr_lt_4p0_v,
				vdup_n_u32(1), vdup_n_u32(0)));

			// xsqr = (x * x);
			a_xsqr_v = vmull_s32(a_x_v, a_x_v);
			b_xsqr_v = vmull_s32(b_x_v, b_x_v);
			// ysqr = (y * y);
			a_ysqr_v = vmull_s32(a_y_v, a_y_v);
			b_ysqr_v = vmull_s32(b_y_v, b_y_v);
			// y = (2 * x * y) + ci;
			a_y_v = vadd_s32(vmovn_s64(vshrq_n_s64(
				vmull_s32(a_x_v, a_y_v), 27)), a_ci_v);
			b_y_v = vadd_s32(vmovn_s64(vshrq_n_s64(
				vmull_s32(b_x_v, b_y_v), 27)), b_ci_v);
			// x = xsqr - ysqr + cr;
			a_x_v = vadd_s32(vqmovn_s64(vshrq_n_s64(
				vsubq_s64(a_xsqr_v, a_ysqr_v), 28)), a_cr_v);
			b_x_v = vadd_s32(vqmovn_s64(vshrq_n_s64(
				vsubq_s64(b_xsqr_v, b_ysqr_v), 28)), b_cr_v);
		}

		if(vget_lane_u32(a_completion_mask_v, 0) == 0)
			if(vget_lane_u32(a_completion_mask_v, 1) == 0)
				if(vget_lane_u32(b_completion_mask_v, 0) == 0)
					if(vget_lane_u32(b_completion_mask_v, 1)
									== 0)
						break;

		iter += LOOP_CNT;
	}

	vst1_u32(iters, a_iters_v);
	vst1_u32(&iters[2], b_iters_v);
	return;
}

// loop overhead for the intrinsic integer mandelbrot algorithm
void draw_vect4_int_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height) {

	uint32_t max_iter;
	uint32_t iters_vect[VECTOR_4];
	uint32_t i;
	uint32_t j;
	uint32_t k;
	int64_t leftmost_x;
	int64_t current_x;
	int64_t current_y;
	int32_t x_vect[VECTOR_4];
	int32_t y_vect[VECTOR_4];
	int64_t x_dim;
	int64_t center_x;
	int64_t center_y;
	int64_t step_dim;

	// set the initial center point, x dimension, and maximum iterations
	center_x = (int64_t)(in_x * 0x0100000000000000LL);
	center_y = (int64_t)(in_y * 0x0100000000000000LL);
	x_dim = (int64_t)(in_x_dim * 0x0100000000000000LL);
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;
	step_dim &= 0xFFFFFFFFF0000000;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	current_x &= 0xFFFFFFFFF0000000;
	leftmost_x = current_x;
	current_y = center_y + ((pixmap_height / 2) * step_dim);
	current_y &= 0xFFFFFFFFF0000000;

	//process the image

	// i counts the rows in the image
	for(i = 0 ; i < pixmap_height ; i++) {
		// j counts the columns in the row
		for(j = 0 ; j < pixmap_width ; j += VECTOR_4) {
			// allow thread to cancel here
			pthread_testcancel();
			// evaluate this coordinate
			for(k = 0 ; k < VECTOR_4 ; k++) {
				x_vect[k] = (int32_t)(current_x >> 28);
				// increment to next coordinate
				current_x += step_dim;
			}

			for(k = 0 ; k < VECTOR_4 ; k++) {
				y_vect[k] = (int32_t)(current_y >> 28);
			}

			vect4_int_mandelbrot(x_vect, y_vect, iters_vect,
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
		// reset coordinate to first column of image
		current_x = leftmost_x;
		// increment coordinate to next row in image
		current_y -= step_dim;
	}
}

struct thread_s {
	uint32_t pixmap_width;
	void *pixel_buf_ptr;
	uint32_t i;
	uint32_t max_iter;
	int64_t current_x;
	int64_t current_y;
	int64_t step_dim;
};

static void *vect4_int_mandelbrot_thread(void *arg) {

	struct thread_s *thread_s_ptr = (struct thread_s *)arg;
	uint32_t pixmap_width = thread_s_ptr->pixmap_width;
	void *pixel_buf_ptr = thread_s_ptr->pixel_buf_ptr;
	uint32_t i = thread_s_ptr->i;
	uint32_t max_iter = thread_s_ptr->max_iter;
	int64_t current_x = thread_s_ptr->current_x;
	int64_t current_y = thread_s_ptr->current_y;
	int64_t step_dim = thread_s_ptr->step_dim;

	uint32_t j;
	int k;
	int32_t x_vect[VECTOR_4];
	int32_t y_vect[VECTOR_4];
	uint32_t iters_vect[VECTOR_4];

	for(j = 0 ; j < pixmap_width ; j += VECTOR_4) {
		// allow thread to cancel here
		pthread_testcancel();
		// evaluate this coordinate
		for(k = 0 ; k < VECTOR_4 ; k++) {
			x_vect[k] = (int32_t)(current_x >> 28);
			// increment to next coordinate
			current_x += step_dim;
		}

		for(k = 0 ; k < VECTOR_4 ; k++) {
			y_vect[k] = (int32_t)(current_y >> 28);
		}

		vect4_int_mandelbrot(x_vect, y_vect, iters_vect, max_iter);

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

// mt loop overhead for the intrinsic integer mandelbrot algorithm
void draw_vect4_int_mandelbrot_mt(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height) {

	uint32_t max_iter;
	uint32_t i;
	int64_t leftmost_x;
	int64_t current_x;
	int64_t current_y;
	int64_t x_dim;
	int64_t center_x;
	int64_t center_y;
	int64_t step_dim;
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
	center_x = (int64_t)(in_x * 0x0100000000000000LL);
	center_y = (int64_t)(in_y * 0x0100000000000000LL);
	x_dim = (int64_t)(in_x_dim * 0x0100000000000000LL);
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;
	step_dim &= 0xFFFFFFFFF0000000;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	current_x &= 0xFFFFFFFFF0000000;
	leftmost_x = current_x;
	current_y = center_y + ((pixmap_height / 2) * step_dim);
	current_y &= 0xFFFFFFFFF0000000;

	//process the image

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
				vect4_int_mandelbrot_thread, next_struct_alloc);
		assert(result == 0);

		// reset coordinate to first column of image
		current_x = leftmost_x;
		// increment coordinate to next row in image
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

