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
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#include "movement_functions.h"
#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "gpio_support/gpio_support.h"
#include "gt_timer/gt_timer.h"
#include "mandelbrot_sw_neon_float/mandelbrot_sw_neon_float.h"
#include "mandelbrot_sw_support/mandelbrot_sw_support.h"
#include "de10_init/de10_init.h"

struct coordinate_struct {
	double center_x;
	double center_y;
	double x_dim;
	int i_cur_max_iters;
	void *pixel_buf_ptr;
	uint32_t pixmap_width;
	uint32_t pixmap_height;
};

static uint32_t g_draw_with_hw = 0;
static uint32_t g_draw_interrupted = 0;
static uint32_t g_draw_interrupt_key = 0;
static void (*g_sw_algorithm)(float, float, float, uint32_t, void *,
						uint32_t, uint32_t) = NULL;

static void thread_cleanup_mutex_unlock(void *arg) {
	pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)arg;
	int result;

	result = pthread_mutex_unlock(mutex_ptr);
	assert(result == 0);
}

static void *the_draw_thread(void *arg) {

	struct coordinate_struct *coord_s_ptr = (struct coordinate_struct *)arg;
	uint64_t start_time;
	uint64_t end_time;
	uint32_t tick_count;
	float fps;
	int result;

	start_time = gt_get_value();

	if(g_draw_with_hw == 1) {
		draw_color_frame_dma_hw_mandelbrot(
			coord_s_ptr->center_x,
			coord_s_ptr->center_y,
			coord_s_ptr->x_dim,
			coord_s_ptr->i_cur_max_iters,
			coord_s_ptr->pixel_buf_ptr,
			coord_s_ptr->pixmap_width,
			coord_s_ptr->pixmap_height,
			get_g_mandelbrot_hw_engine_count()
		);
	} else {
		set_g_mandelbrot_sw_colorize(1);

		g_sw_algorithm(
			coord_s_ptr->center_x,
			coord_s_ptr->center_y,
			coord_s_ptr->x_dim,
			coord_s_ptr->i_cur_max_iters,
			coord_s_ptr->pixel_buf_ptr,
			coord_s_ptr->pixmap_width,
			coord_s_ptr->pixmap_height
		);
	}

	end_time = gt_get_value();

	if(end_time < (start_time + get_g_gt_samples_ave()))
		tick_count = get_g_gt_samples_ave();
	else
		tick_count = end_time - start_time;
	tick_count -= get_g_gt_samples_ave();
	fps = (float)get_g_mpu_periph_clk_rate() / (float)tick_count;

	result = pthread_mutex_lock(&g_fps_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &g_fps_mutex);

	set_g_latest_fps_value(fps);
	g_fps_count++;

	result = pthread_cond_broadcast(&g_fps_cond);
	assert(result == 0);

	pthread_cleanup_pop(1);

	return(NULL);
}

static void *the_interrupt_draw_thread(void *arg) {

	int result;
	pthread_t *draw_thread = (pthread_t *)arg;

	g_draw_interrupt_key = get_input_key();
	g_draw_interrupted = 1;

	result = pthread_cancel(*draw_thread);
	assert(result == 0);

	return(NULL);
}

void draw(double center_x, double center_y, double x_dim, int i_cur_max_iters,
		void *pixel_buf_ptr, uint32_t pixmap_width,
		uint32_t pixmap_height) {

	int result;
	struct coordinate_struct coord_s;
	pthread_t draw_thread;
	pthread_t interrupt_draw_thread;

	coord_s.center_x = center_x;
	coord_s.center_y = center_y;
	coord_s.x_dim = x_dim;
	coord_s.i_cur_max_iters = i_cur_max_iters;
	coord_s.pixmap_width = pixmap_width;
	coord_s.pixmap_height = pixmap_height;

	while(1) {
		
		// pass in the proper pixel buffer pointer, virtual for software
		// physical for hardware
		coord_s.pixel_buf_ptr = pixel_buf_ptr;

		if(g_draw_with_hw == 1) {
			if(get_g_mandelbrot_fb_map() == pixel_buf_ptr) {
				coord_s.pixel_buf_ptr =
						(void *)MANDELBROT_FRAME_BUFFER_BASE;
			} else if(get_g_spare_0_fb_map() == pixel_buf_ptr) {
				coord_s.pixel_buf_ptr =
						(void *)SPARE_0_FRAME_BUFFER_BASE;
			} else if(get_g_spare_1_fb_map() == pixel_buf_ptr) {
				coord_s.pixel_buf_ptr =
						(void *)SPARE_1_FRAME_BUFFER_BASE;
			}
		}

		// start the draw thread
		result = pthread_create(&draw_thread, NULL, the_draw_thread,
								&coord_s);
		assert(result == 0);

		// start the interrupt draw thread
		result = pthread_create(&interrupt_draw_thread, NULL,
					the_interrupt_draw_thread,
					&draw_thread);
		assert(result == 0);

		// join the draw thread
		result = pthread_join(draw_thread, NULL);
		assert(result == 0);

		// cancel and join the interrupt draw thread
		result = pthread_cancel(interrupt_draw_thread);
		assert((result == 0) || (result == ESRCH));

		result = pthread_join(interrupt_draw_thread, NULL);
		assert(result == 0);

		if(g_draw_interrupted == 1) {
			if(g_draw_interrupt_key == 1) {
				if(g_draw_with_hw == 0) {
					g_draw_with_hw = 1;
					set_g_crawl_algo_str_ptr(
						get_g_crawl_hw_str_ptr());
					g_draw_interrupted = 0;
				} else {
					g_draw_with_hw = 0;
					set_g_crawl_algo_str_ptr(
						get_g_crawl_sw_str_ptr());
					g_draw_interrupted = 0;
				}
			} else {
				break;
			}
		} else {
			break;
		}
	}
}

void set_g_draw_with_hw(uint32_t new_draw_with_hw) {
	g_draw_with_hw = new_draw_with_hw;
}

uint32_t get_g_draw_with_hw(void) {
	return(g_draw_with_hw);
}

void set_g_draw_interrupted(uint32_t new_draw_interrupt) {
	g_draw_interrupted = new_draw_interrupt;
}

uint32_t get_g_draw_interrupted(void) {
	return(g_draw_interrupted);
}

void set_g_sw_algorithm(void (*new_sw_algorithm)(float, float, float, uint32_t,
						void *, uint32_t, uint32_t)) {
	g_sw_algorithm = new_sw_algorithm;
}

