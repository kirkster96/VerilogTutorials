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
#include <stdint.h>
#include <assert.h>

#include "menu_config_demo/menu_config_demo.h"
#include "do_fluid_movement/movement_functions.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "font_support/font_def_public.h"
#include "de10_init/de10_init.h"
#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "gt_timer/gt_timer.h"
#include "mandelbrot_sw_support/mandelbrot_sw_support.h"

void do_algo_performance(char *name,
				uint32_t hw_engine_count,
				void (*sw_algorithm)(float, float, float,
					uint32_t, void *, uint32_t, uint32_t)
				) {

	struct abc_font_struct *original_font;
	char string_buffer[128];
	double center_x = -0.5;
	double center_y = 0.0;
	double x_dim = 4.25;
	int max_iters = 25;
	void *pixel_buf_ptr;
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();
	uint64_t start_time;
	uint64_t end_time;
	uint32_t tick_count;
	float run_time_seconds;

	//
	// Font metrics for 640 x 480 display:
	//	cousine_bold_16: 62 characters x 26 rows
	//	cousine_bold_18: 57 characters x 23 rows
	//	cousine_bold_20: 52 characters x 20 rows
	//	cousine_bold_30: 34 characters x 14 rows
	//
	original_font = get_console_font();
	set_console_font(&cousine_bold_16);

	console_clear();

	console_print((char *)"Calculating performance for algorithm:");

	snprintf(string_buffer, 128, "%s", name);
	console_print(string_buffer);

	// allocate buffer for the result images
	pixel_buf_ptr = malloc(sizeof(uint32_t) * pixmap_width * pixmap_height);
	assert(pixel_buf_ptr != NULL);

	// time the algorithm
	start_time = gt_get_value();

	if(sw_algorithm == NULL) {
		draw_frame_dma_hw_mandelbrot(
			center_x,
			center_y,
			x_dim,
			max_iters,
			(void *)SPARE_0_FRAME_BUFFER_BASE,
			pixmap_width,
			pixmap_height,
			hw_engine_count
		);
	} else {
		set_g_mandelbrot_sw_colorize(0);

		sw_algorithm(
			center_x,
			center_y,
			x_dim,
			max_iters,
			pixel_buf_ptr,
			pixmap_width,
			pixmap_height
		);
	}

	end_time = gt_get_value();

	// free the buffer
	free(pixel_buf_ptr);

	if(end_time < (start_time + get_g_gt_samples_ave()))
		tick_count = get_g_gt_samples_ave();
	else
		tick_count = end_time - start_time;
	tick_count -= get_g_gt_samples_ave();
	run_time_seconds = ((float)1 / (float)get_g_mpu_periph_clk_rate()) *
							(float)tick_count;

	snprintf(string_buffer, 128, "Computed fractal in %0.6f seconds",
							run_time_seconds);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "Image Width: %d", pixmap_width);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "Image Height: %d", pixmap_height);
	console_print(string_buffer);

	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

