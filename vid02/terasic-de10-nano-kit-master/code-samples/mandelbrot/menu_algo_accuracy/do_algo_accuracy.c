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
#include <string.h>

#include "menu_config_demo/menu_config_demo.h"
#include "do_fluid_movement/movement_functions.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "font_support/font_def_public.h"
#include "de10_init/de10_init.h"
#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "gt_timer/gt_timer.h"
#include "mandelbrot_sw_support/mandelbrot_sw_support.h"
#include "mandelbrot_sw_float/mandelbrot_sw_float.h"
#include "mandelbrot_sw_int/mandelbrot_sw_int.h"
#include "mandelbrot_sw_neon_float/mandelbrot_sw_neon_float.h"
#include "mandelbrot_sw_neon_int/mandelbrot_sw_neon_int.h"

static void compare_images(uint32_t *first_results_buffer,
					uint32_t *second_results_buffer,
					uint32_t pixmap_width,
					uint32_t pixmap_height,
					uint32_t max_iters) {

	int in_set_count = 0;
	int diff_is_one_count = 0;
	int diff_is_gt_one_count = 0;
	uint32_t first_pixel_value;
	uint32_t second_pixel_value;
	uint32_t pixel_diff;
	uint32_t i;
	uint32_t j;
	char string_buffer[128];

	for(i = 0 ; i < pixmap_height ; i++) {
		for(j = 0 ; j < pixmap_width ; j++) {
			first_pixel_value =
					*(first_results_buffer +
						(i * pixmap_width) + j);

			second_pixel_value =
					*(second_results_buffer +
						(i * pixmap_width) + j);

			if(first_pixel_value != second_pixel_value) {
				if(first_pixel_value > second_pixel_value) {
					pixel_diff = first_pixel_value -
							second_pixel_value;
				} else {
					pixel_diff = second_pixel_value -
							first_pixel_value;
				}

				if((first_pixel_value == max_iters) ||
					(second_pixel_value == max_iters)) {
					in_set_count++;
				} else if(pixel_diff == 1) {
					diff_is_one_count++;
				} else {
					diff_is_gt_one_count++;
				}
			}
		}
	}

	if(	(in_set_count != 0) ||
		(diff_is_one_count != 0) ||
		(diff_is_gt_one_count != 0)) {

		snprintf(string_buffer, 128, "Total Pixel Count = %u",
						pixmap_width * pixmap_height);
		console_print(string_buffer);
		snprintf(string_buffer, 128, "Set difference count = %u",
								in_set_count);
		console_print(string_buffer);
		snprintf(string_buffer, 128, "Difference of one count = %u",
							diff_is_one_count);
		console_print(string_buffer);
		snprintf(string_buffer, 128, "Difference of > one count = %u",
							diff_is_gt_one_count);
		console_print(string_buffer);
	} else {
		console_print((char *)"Images are identical.");
	}
}

void do_int_float_accuracy(void) {

	struct abc_font_struct *original_font;
	double center_x = -0.5;
	double center_y = 0.0;
	double x_dim = 4.25;
	int max_iters = 25;
	void *pixel_buf_ptr_0;
	void *pixel_buf_ptr_1;
	uint32_t pixel_buf_size;
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();

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

	// allocate buffers for the result images
	pixel_buf_size = sizeof(uint32_t) * pixmap_width * pixmap_height;

	pixel_buf_ptr_0 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_0 != NULL);

	pixel_buf_ptr_1 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_1 != NULL);

	set_g_mandelbrot_sw_colorize(0);

	console_print((char *)"Calculating floating point base image.");

	memset(pixel_buf_ptr_0, 0,  pixel_buf_size);

	draw_float_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_0,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Calculating integer base image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_int_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	// free the buffer
	free(pixel_buf_ptr_0);
	free(pixel_buf_ptr_1);


	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

void do_float_accuracy(void) {

	struct abc_font_struct *original_font;
	double center_x = -0.5;
	double center_y = 0.0;
	double x_dim = 4.25;
	int max_iters = 25;
	void *pixel_buf_ptr_0;
	void *pixel_buf_ptr_1;
	uint32_t pixel_buf_size;
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();

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

	// allocate buffers for the result images
	pixel_buf_size = sizeof(uint32_t) * pixmap_width * pixmap_height;

	pixel_buf_ptr_0 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_0 != NULL);

	pixel_buf_ptr_1 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_1 != NULL);

	set_g_mandelbrot_sw_colorize(0);

	console_print((char *)"Calculating floating point base image.");

	memset(pixel_buf_ptr_0, 0,  pixel_buf_size);

	draw_float_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_0,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Calculating multithreaded floating point image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_float_mandelbrot_mt(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	console_print((char *)"");

	console_print((char *)"Calculating floating point vector intrisics image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_vect4_float_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	console_print((char *)"");

	console_print((char *)"Calculating multithreaded floating point vector intrisics");
	console_print((char *)"image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_vect4_float_mandelbrot_mt(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	// free the buffer
	free(pixel_buf_ptr_0);
	free(pixel_buf_ptr_1);


	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

void do_int_accuracy(void) {

	struct abc_font_struct *original_font;
	double center_x = -0.5;
	double center_y = 0.0;
	double x_dim = 4.25;
	int max_iters = 25;
	void *pixel_buf_ptr_0;
	void *pixel_buf_ptr_1;
	uint32_t pixel_buf_size;
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();

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

	// allocate buffers for the result images
	pixel_buf_size = sizeof(uint32_t) * pixmap_width * pixmap_height;

	pixel_buf_ptr_0 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_0 != NULL);

	pixel_buf_ptr_1 = malloc(pixel_buf_size);
	assert(pixel_buf_ptr_1 != NULL);

	set_g_mandelbrot_sw_colorize(0);

	console_print((char *)"Calculating integer base image.");

	memset(pixel_buf_ptr_0, 0,  pixel_buf_size);

	draw_int_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_0,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Calculating multithreaded integer image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_int_mandelbrot_mt(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	console_print((char *)"");

	console_print((char *)"Calculating integer vector intrisics image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_vect4_int_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	console_print((char *)"");

	console_print((char *)"Calculating multithreaded integer vector intrisics image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);

	draw_vect4_int_mandelbrot_mt(
		center_x,
		center_y,
		x_dim,
		max_iters,
		pixel_buf_ptr_1,
		pixmap_width,
		pixmap_height
	);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	console_print((char *)"");

	console_print((char *)"Calculating FPGA image.");

	memset(pixel_buf_ptr_1, 0,  pixel_buf_size);
	memset(get_g_spare_0_fb_map(), 0,  pixel_buf_size);

	draw_frame_dma_hw_mandelbrot(
		center_x,
		center_y,
		x_dim,
		max_iters,
		(void *)SPARE_0_FRAME_BUFFER_BASE,
		pixmap_width,
		pixmap_height,
		12		// hw_engine_count
	);

	memcpy(pixel_buf_ptr_1, get_g_spare_0_fb_map(), pixel_buf_size);

	console_print((char *)"Comparing images.");

	compare_images((uint32_t *)pixel_buf_ptr_0, (uint32_t *)pixel_buf_ptr_1,
				pixmap_width, pixmap_height, max_iters);

	// free the buffer
	free(pixel_buf_ptr_0);
	free(pixel_buf_ptr_1);


	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

