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
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include "linux/input.h"

#include "explore/explore.h"
#include "de10_init/de10_init.h"
#include "font_support/font_def_public.h"
#include "font_support/font_public.h"
#include "pix_map_routines/pix_map_base_routines.h"
#include "mandelbrot.h"
#include "gpio_support/gpio_support.h"
#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "menu_support/menu_support.h"
#include "console_support/console_support.h"

#define INPUT_DEV_NODE "/dev/input/by-path/platform-ffc04000.i2c-event"
#define SYSFS_DEVICE_DIR "/sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/"

#define EV_CODE_X (0)
#define EV_CODE_Y (1)
#define EV_CODE_Z (2)

#define CURSOR_THRESHOLD	(60)

static struct abc_font_struct *g_ebanner_font = &cousine_bold_18;
static uint32_t g_ebanner_height;
static uint32_t g_ebanner_coord_width;
static uint32_t g_ebanner_zoom_width;
static uint32_t g_ebanner_current_offset;
static uint32_t g_ebanner_start_pixel;
static pthread_mutex_t g_coord_mutex;
static pthread_cond_t g_coord_cond;
static uint32_t g_coord_count;

static int32_t g_center_x_int32;
static int32_t g_center_y_int32;
static int32_t g_x_dim_int32;
static uint32_t g_pixmap_width;
static uint32_t g_mandel_height;
static int g_max_iters;
static int g_zoom_in;
static int32_t g_max_zoom_out;
static int32_t g_min_zoom_in;

static void * do_coord_ebanner_thread(void *arg);
static void * do_zoom_ebanner_thread(void *arg);
static void *do_ebanner_crawler_thread(void *arg);
static void *do_cursor_thread(void *arg);
static void write_sysfs_cntl_file(const char *dir_name, const char *file_name,
		const char *write_str);
static double fixed_2_double(int32_t fixed_in);

void do_explore(void) {

	void *pixel_buf_ptr = get_g_mandelbrot_fb_map();
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();

	double center_x = -0.5;
	double center_y = 0.0;
	double x_dim = 4.25;
	int max_iters = 25;

	uint32_t mandel_height;
	int result;
	pthread_t coord_ebanner_thread;
	pthread_t zoom_ebanner_thread;
	pthread_t ebanner_crawler_thread;
	pthread_t cursor_thread;

	int key_in;

	int64_t center_x_int64;
	int64_t center_y_int64;
	int64_t x_dim_int64;
	int32_t last_center_x_int32;
	int32_t last_center_y_int32;

	int milli_wait;

	center_x_int64 = (int64_t)(center_x * 0x0100000000000000LL);
	center_y_int64 = (int64_t)(center_y * 0x0100000000000000LL);
	x_dim_int64    = (int64_t)(x_dim    * 0x0100000000000000LL);

	// calculate ebanner geometry
	g_ebanner_height = g_ebanner_font[0].bounds_height;
	if(g_ebanner_height & 0x01)
		g_ebanner_height++;
	g_ebanner_coord_width = get_string_line_width(g_ebanner_font,
		(char *)"X:-0.0000000000 Y:-0.0000000000");
	g_ebanner_coord_width += EBANNER_WIDTH_AIR;
	g_ebanner_zoom_width = get_string_line_width(g_ebanner_font,
				(char *)"ITERS:8000  ZOOM:OUT LIMIT");
	g_ebanner_zoom_width += EBANNER_WIDTH_AIR;
	g_ebanner_current_offset = 0;
	mandel_height = pixmap_height - g_ebanner_height;
	g_ebanner_start_pixel = mandel_height * pixmap_width;

	fill_pix_map_32(
		(char *)(&((uint32_t *)pixel_buf_ptr)[g_ebanner_start_pixel]),	// char *dest_ptr,
		pixmap_width,		// long dest_active_width,
		g_ebanner_height,	// long dest_active_height,
		pixmap_width,		// long dest_line_width,
		0x00000000		// long fill_color
	);

	// setup the globals for other threads to reference
	g_center_x_int32 = (int32_t)((center_x_int64 >> 28) & 0xFFFFFFFF);
	g_center_y_int32 = (int32_t)((center_y_int64 >> 28) & 0xFFFFFFFF);
	g_x_dim_int32    = (int32_t)((x_dim_int64    >> 28) & 0xFFFFFFFF);
	g_pixmap_width = pixmap_width;
	g_mandel_height = mandel_height;
	g_zoom_in = 1;
	g_max_iters = max_iters;

	g_max_zoom_out = g_x_dim_int32;
	g_min_zoom_in = pixmap_width;

	// start ebanner threads
	g_coord_count = 0;

	result = pthread_cond_init(&g_coord_cond, NULL);
	assert(result == 0);

	result = pthread_mutex_init(&g_coord_mutex, NULL);
	assert(result == 0);

	result = pthread_create(&coord_ebanner_thread, NULL,
						do_coord_ebanner_thread, NULL);
	assert(result == 0);

	result = pthread_create(&zoom_ebanner_thread, NULL,
						do_zoom_ebanner_thread, NULL);
	assert(result == 0);

	result = pthread_create(&ebanner_crawler_thread, NULL,
					do_ebanner_crawler_thread, NULL);
	assert(result == 0);

	// main loop
	milli_wait = 500;
	while(1) {

		draw_color_frame_dma_hw_mandelbrot(
			fixed_2_double(g_center_x_int32),
			fixed_2_double(g_center_y_int32),
			fixed_2_double(g_x_dim_int32),
			g_max_iters,
			(void *)MANDELBROT_FRAME_BUFFER_BASE,
			pixmap_width,
			mandel_height,
			12		// hw_engine_count
		);

		if(key1_still_down(milli_wait)) {
			key_in = 1;
			milli_wait = 150;
		} else {
			result = pthread_create(&cursor_thread, NULL, do_cursor_thread,
							NULL);
			assert(result == 0);

			last_center_x_int32 = g_center_x_int32;
			last_center_y_int32 = g_center_y_int32;

			key_in = get_input_key();

			result = pthread_cancel(cursor_thread);
			assert(result == 0);

			result = pthread_join(cursor_thread, NULL);
			assert(result == 0);

			milli_wait = 500;
		}

		if(key_in == 0) {

			// discard any cursor movement
			g_center_x_int32 = last_center_x_int32;
			g_center_y_int32 = last_center_y_int32;

			menu_explore_start();

			if(check_exit_explore_mode() == 1)
				break;
		} else {

			if(g_zoom_in == 1) {
				g_x_dim_int32 = (double)(g_x_dim_int32) /
								(double)(1.1);
				if(g_x_dim_int32 < g_min_zoom_in) {
					g_x_dim_int32 = g_min_zoom_in;
				}
			} else {
				g_x_dim_int32 = (double)(g_x_dim_int32) *
								(double)(1.1);
				if(g_x_dim_int32 > g_max_zoom_out) {
					g_x_dim_int32 = g_max_zoom_out;
				}
			}
		}
	}

	result = pthread_cancel(coord_ebanner_thread);
	assert(result == 0);

	result = pthread_cancel(zoom_ebanner_thread);
	assert(result == 0);

	result = pthread_cancel(ebanner_crawler_thread);
	assert(result == 0);

	result = pthread_join(coord_ebanner_thread, NULL);
	assert(result == 0);

	result = pthread_join(zoom_ebanner_thread, NULL);
	assert(result == 0);

	result = pthread_join(ebanner_crawler_thread, NULL);
	assert(result == 0);
}

static void thread_cleanup_mutex_unlock(void *arg) {
	pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)arg;
	int result;

	result = pthread_mutex_unlock(mutex_ptr);
	assert(result == 0);
}

static void thread_cleanup_free(void *arg) {
	free(arg);
}

static void wait_coord_cond(void) {

	int result;
	uint32_t my_count;

	result = pthread_mutex_lock(&g_coord_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &g_coord_mutex);

	my_count = g_coord_count;

	while(my_count == g_coord_count) {
		result = pthread_cond_wait(&g_coord_cond,
					&g_coord_mutex);
		assert(result == 0);
	}

	pthread_cleanup_pop(1);
}

static void * do_coord_ebanner_thread(void *arg) {

	uint32_t *offscreen_draw_buffer;
	uint32_t *blend_buffer;
	char string_buffer[COORD_STR_LEN];
	int snprintf_length;
	uint32_t *onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
	uint32_t pixmap_width = get_fb_width();
	uint32_t start_pixel = g_ebanner_start_pixel;

	(void)arg;

	offscreen_draw_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					g_ebanner_coord_width *
					g_ebanner_height));
	assert(offscreen_draw_buffer != NULL);
	pthread_cleanup_push(thread_cleanup_free, offscreen_draw_buffer);

	blend_buffer = (uint32_t *)(malloc(sizeof(uint32_t) * 64 * 64));
	assert(blend_buffer != NULL);
	pthread_cleanup_push(thread_cleanup_free, blend_buffer);

	fill_pix_map_32(
		(char *)(blend_buffer),		// char *dest_ptr,
		64,				// long dest_active_width,
		64,				// long dest_active_height,
		64,				// long dest_line_width,
		0x00000000			// long fill_color
	);

	while(1) {
		snprintf_length = snprintf(string_buffer, COORD_STR_LEN,
			"X:%- 13.10f Y:%- 13.10f",
			fixed_2_double(g_center_x_int32),
			fixed_2_double(g_center_y_int32));
		assert(snprintf_length < COORD_STR_LEN);

		fill_pix_map_32(
			(char *)(offscreen_draw_buffer),// char *dest_ptr,
			g_ebanner_coord_width,		// long dest_active_width,
			g_ebanner_height,		// long dest_active_height,
			g_ebanner_coord_width,		// long dest_line_width,
			0x00000000			// long fill_color
		);

		draw_string_line(
			g_ebanner_font,			// struct abc_font_struct *theFontStruct,	// pointer to the font structure
			offscreen_draw_buffer,		// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
			g_ebanner_coord_width,		// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
			blend_buffer,			// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,			// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
			string_buffer,			// char *the_string,		// pointer to the string to draw
			g_ebanner_height,		// uint32_t under_line,		// the horizontal line number above which the text will draw
			g_ebanner_current_offset,	// uint32_t start_x,		// the vertical column number that text begins to draw
			g_ebanner_coord_width		// uint32_t max_width		// maximum horizontal width to draw
		);

		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_ebanner_coord_width,			// long src_active_width,
			g_ebanner_height,			// long src_active_height,
			g_ebanner_coord_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),	// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		wait_coord_cond();
	}

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	return(NULL);
}

void signal_coord_cond(void) {
	int result;

	result = pthread_mutex_lock(&g_coord_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &g_coord_mutex);

	g_coord_count++;

	result = pthread_cond_broadcast(&g_coord_cond);
	assert(result == 0);

	pthread_cleanup_pop(1);
}

static void * do_zoom_ebanner_thread(void *arg) {

	uint32_t *offscreen_draw_buffer;
	uint32_t *blend_buffer;
	char string_buffer[ZOOM_STR_LEN];
	int snprintf_length;
	uint32_t *onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
	uint32_t pixmap_width = get_fb_width();
	uint32_t start_pixel = g_ebanner_start_pixel + g_ebanner_coord_width;
	int offset_increment_flag = 1;

	(void)arg;

	offscreen_draw_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					g_ebanner_zoom_width *
					g_ebanner_height));
	assert(offscreen_draw_buffer != NULL);
	pthread_cleanup_push(thread_cleanup_free, offscreen_draw_buffer);

	blend_buffer = (uint32_t *)(malloc(sizeof(uint32_t) * 64 * 64));
	assert(blend_buffer != NULL);
	pthread_cleanup_push(thread_cleanup_free, blend_buffer);

	fill_pix_map_32(
		(char *)(blend_buffer),		// char *dest_ptr,
		64,				// long dest_active_width,
		64,				// long dest_active_height,
		64,				// long dest_line_width,
		0x00000000			// long fill_color
	);

	while(1) {

		snprintf_length = snprintf(string_buffer, ZOOM_STR_LEN,
					"ITERS:%-4d  ZOOM:%s",
					g_max_iters,
					(g_zoom_in) ?
					(g_x_dim_int32 == g_min_zoom_in) ?
						"IN LIMIT" :
						"IN" :
					(g_x_dim_int32 == g_max_zoom_out) ?
						"OUT LIMIT" :
						"OUT");
		assert(snprintf_length < ZOOM_STR_LEN);

		fill_pix_map_32(
			(char *)(offscreen_draw_buffer),// char *dest_ptr,
			g_ebanner_zoom_width,		// long dest_active_width,
			g_ebanner_height,		// long dest_active_height,
			g_ebanner_zoom_width,		// long dest_line_width,
			0x00000000			// long fill_color
		);

		draw_string_line(
			g_ebanner_font,			// struct abc_font_struct *theFontStruct,	// pointer to the font structure
			offscreen_draw_buffer,		// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
			g_ebanner_zoom_width,		// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
			blend_buffer,			// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,			// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
			string_buffer,			// char *the_string,		// pointer to the string to draw
			g_ebanner_height,		// uint32_t under_line,		// the horizontal line number above which the text will draw
			g_ebanner_current_offset,	// uint32_t start_x,		// the vertical column number that text begins to draw
			g_ebanner_zoom_width		// uint32_t max_width		// maximum horizontal width to draw
		);

		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_ebanner_zoom_width,			// long src_active_width,
			g_ebanner_height,			// long src_active_height,
			g_ebanner_zoom_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),	// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		sleep(1);

		if(offset_increment_flag == 1) {
			if(g_ebanner_current_offset < (EBANNER_WIDTH_AIR - 2)) {
				g_ebanner_current_offset++;
			} else {
				offset_increment_flag = 0;
				g_ebanner_current_offset--;
			}
		} else {
			if(g_ebanner_current_offset > 0) {
				g_ebanner_current_offset--;
			} else {
				offset_increment_flag = 1;
				g_ebanner_current_offset++;
			}
		}

		signal_coord_cond();
	}

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	return(NULL);
}

static void *do_ebanner_crawler_thread(void *arg) {

	int i;
	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t font_height;
	uint32_t string_width;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t first_crawl_pixel;
	char crawl_str[1024];
	int snprintf_length;
	uint32_t crawl_buffer_width;
	uint32_t crawl_buffer_height;
	uint32_t *crawl_offscreen_buffer;
	uint32_t pre_crawl_width;
	uint32_t onscreen_crawl_width;
	uint32_t *blend_buffer;

	(void)arg;

	while(1) {

		snprintf_length = snprintf(crawl_str, 1024,
			"Explore the Mandelbrot fractal.  "
			"Tip the DE10-Nano board to move the onscreen cursor using the accelerometer.  "
			"Press the KEY1 push button to zoom in and out of the coordinate under the cursor.  "
			"Hold KEY1 down to continue the zoom.  "
			"Press the KEY0 push button to bring up the Explore menu. "
			"See menu for usability advice.");
		assert(snprintf_length < 1024);

		pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
		pixmap_width = get_fb_width();
		font_height = g_ebanner_height;

		pre_crawl_width = g_ebanner_coord_width + g_ebanner_zoom_width +
									10;
		assert(pre_crawl_width < pixmap_width);
		onscreen_crawl_width = pixmap_width - pre_crawl_width;
		assert(onscreen_crawl_width >= 200);
		if(onscreen_crawl_width > 400)
			onscreen_crawl_width = 400;

		first_crawl_pixel = g_ebanner_start_pixel + pixmap_width -
							onscreen_crawl_width;

		string_width = get_string_line_width(
					g_ebanner_font,	// struct abc_font_struct *theFontStruct,	// pointer to the font structure
					crawl_str	// char *the_string				// pointer to the string to draw
				);

		crawl_buffer_width = (onscreen_crawl_width * 2) + string_width;
		crawl_buffer_height = font_height;
		crawl_offscreen_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					crawl_buffer_width *
					crawl_buffer_height));
		assert(crawl_offscreen_buffer != NULL);

		pthread_cleanup_push(thread_cleanup_free,
							crawl_offscreen_buffer);

		fill_pix_map_32(
			(char *)crawl_offscreen_buffer,	// char *dest_ptr,
			crawl_buffer_width,		// long dest_active_width,
			crawl_buffer_height,		// long dest_active_height,
			crawl_buffer_width,		// long dest_line_width,
			0x00000000			// long fill_color
		);

		blend_buffer = (uint32_t *)(malloc(sizeof(uint32_t) * 64 * 64));
		assert(blend_buffer != NULL);
		pthread_cleanup_push(thread_cleanup_free, blend_buffer);

		fill_pix_map_32(
			(char *)(blend_buffer),	// char *dest_ptr,
			64,			// long dest_active_width,
			64,			// long dest_active_height,
			64,			// long dest_line_width,
			0x00000000		// long fill_color
		);

		start_x = onscreen_crawl_width;

		start_y = font_height;

		draw_string_line(
			g_ebanner_font,		// struct abc_font_struct *theFontStruct,	// pointer to the font structure
			crawl_offscreen_buffer,	// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
			crawl_buffer_width,	// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
			blend_buffer,		// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,		// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
			crawl_str,		// char *the_string,		// pointer to the string to draw
			start_y,		// uint32_t under_line,		// the horizontal line number above which the text will draw
			start_x,		// uint32_t start_x,		// the vertical column number that text begins to draw
			string_width + 1	// uint32_t max_width		// maximum horizontal width to draw
		);

		while(1) {
			for(i = 0 ; i < (int)(crawl_buffer_width -
						onscreen_crawl_width) ; i++) {
				copy_pix_map_32_to_32(
					(char *)(&crawl_offscreen_buffer[i]),	// char *src_ptr,
					onscreen_crawl_width,			// long src_active_width,
					crawl_buffer_height,			// long src_active_height,
					crawl_buffer_width,			// long src_line_width,
					(char *)(&pixmap_buffer[first_crawl_pixel]),	// char *dest_ptr,
					pixmap_width				// long dest_line_width
				);
				usleep(10000);
			};
		};
		pthread_cleanup_pop(1);
		pthread_cleanup_pop(1);
	}

	return(NULL);
}

static void do_cursor_thread_cleanup(void *arg) {

	int result;
	int *event_dev_fd = (int *)arg;

	// close the device node
	result = close(*event_dev_fd);
	assert(result >= 0);

	// disable adxl
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "1");
}

static void *do_cursor_thread(void *arg) {

	uint32_t *pixel_buf_ptr = (uint32_t *)get_g_mandelbrot_fb_map();
	uint32_t *pixmap_under_cursor[CURSOR_WIDTH * CURSOR_HEIGHT];
	uint8_t cursor_alpha_mask[CURSOR_WIDTH * CURSOR_HEIGHT] = {
0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 };
	uint32_t cursor_center_x = g_pixmap_width  / 2;
	uint32_t cursor_center_y = g_mandel_height / 2;
	uint32_t cursor_start_pixel;
	int event_dev_fd;
	const char *input_dev_node = INPUT_DEV_NODE;
	struct input_absinfo the_absinfo;
	int result;
	int x_input;
	int y_input;
	int cursor_moved;

	int32_t step_dim;
	int32_t next_center_x;
	int32_t next_center_y;
	int32_t max_fixed_point;
	int32_t min_fixed_point;

	(void)arg;

	// calculate step dimension
	step_dim = g_x_dim_int32 / g_pixmap_width;
	max_fixed_point = 0x7FFFFFFF - step_dim;
	min_fixed_point = 0x80000001 + step_dim;

	// enable adxl
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "0");

	// set the sample rate to maximum
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "rate", "15");

	// do not auto sleep
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "autosleep", "0");

	// do not auto sleep
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "calibrate", "1");

	// open the event device node
	event_dev_fd = open(input_dev_node, O_RDONLY | O_SYNC);
	assert(event_dev_fd >= 0);

	pthread_cleanup_push(do_cursor_thread_cleanup, &event_dev_fd);

	while(1) {
		cursor_start_pixel = ((cursor_center_y - (CURSOR_HEIGHT / 2)) *
					g_pixmap_width) +
					(cursor_center_x - (CURSOR_WIDTH / 2));

		// save the current pixels under the cursor
		copy_pix_map_32_to_32(
			(char *)(&pixel_buf_ptr[cursor_start_pixel]),	// char *src_ptr,
			CURSOR_WIDTH,					// long src_active_width,
			CURSOR_HEIGHT,					// long src_active_height,
			g_pixmap_width,					// long src_line_width,
			(char *)(pixmap_under_cursor),			// char *dest_ptr,
			CURSOR_WIDTH					// long dest_line_width
		);

		// blend the cursor onto the screen
		blend_8_with_32_to_32(
			(char *)cursor_alpha_mask,	// char *alpha_ptr,
			CURSOR_WIDTH,			// long alpha_active_width,
			CURSOR_HEIGHT,			// long alpha_active_height,
			0x00FFFFFF,			// long alpha_color,
			CURSOR_WIDTH,			// long alpha_line_width,
			(char *)(pixmap_under_cursor),	// char *src_ptr,
			CURSOR_WIDTH,			// long src_line_width,
			(char *)(&pixel_buf_ptr[cursor_start_pixel]),	// char *dest_ptr,
			g_pixmap_width	// long dest_line_width
		);

		signal_coord_cond();

		next_center_x = g_center_x_int32;
		next_center_y = g_center_y_int32;

		cursor_moved = 0;
		while(1) {
			usleep(1000);
			// read the accelerometer
			result = ioctl (event_dev_fd, EVIOCGABS(EV_CODE_X),
					&the_absinfo);
			assert(result >= 0);

			x_input = the_absinfo.value;

			result = ioctl (event_dev_fd, EVIOCGABS(EV_CODE_Y),
					&the_absinfo);
			assert(result >= 0);

			y_input = the_absinfo.value;

			// adjust cursor location
			if(x_input > CURSOR_THRESHOLD) {
				next_center_x = g_center_x_int32 + step_dim;
				if(next_center_x < max_fixed_point) {
					if(((cursor_center_x + 1) +
							(CURSOR_WIDTH / 2)) <
							g_pixmap_width) {
						cursor_center_x++;
						cursor_moved = 1;
					}
				}
			} else if(x_input < -CURSOR_THRESHOLD) {
				next_center_x = g_center_x_int32 - step_dim;
				if(next_center_x > min_fixed_point) {
					if(((cursor_center_x - 1) -
						(CURSOR_WIDTH / 2)) > 0) {
						cursor_center_x--;
						cursor_moved = 1;
					}
				}
			}

			if(y_input < -CURSOR_THRESHOLD) {
				next_center_y = g_center_y_int32 - step_dim;
				if(next_center_y > min_fixed_point) {
					if(((cursor_center_y + 1) +
							(CURSOR_HEIGHT / 2)) <
							g_mandel_height) {
						cursor_center_y++;
						cursor_moved = 1;
					}
				}
			} else if(y_input > CURSOR_THRESHOLD) {
				next_center_y = g_center_y_int32 + step_dim;
				if(next_center_x < max_fixed_point) {
					if(((cursor_center_y - 1) -
						(CURSOR_HEIGHT / 2)) > 0) {
						cursor_center_y--;
						cursor_moved = 1;
					}
				}
			}

			if(cursor_moved == 1)
				break;
		}

		g_center_x_int32 = next_center_x;
		g_center_y_int32 = next_center_y;

		// restore the current pixels under the cursor
		copy_pix_map_32_to_32(
			(char *)(pixmap_under_cursor),			// char *src_ptr,
			CURSOR_WIDTH,					// long src_active_width,
			CURSOR_HEIGHT,					// long src_active_height,
			CURSOR_WIDTH,					// long src_line_width,
			(char *)(&pixel_buf_ptr[cursor_start_pixel]),	// char *dest_ptr,
			g_pixmap_width					// long dest_line_width
		);
	}

	pthread_cleanup_pop(1);

	return(NULL);
}

void write_sysfs_cntl_file(const char *dir_name, const char *file_name,
						const char *write_str) {

	char path[PATH_MAX];
	int path_length;
	int file_fd;
	int result;

	// create the path to the file we need to open
	path_length = snprintf(path, PATH_MAX, "%s/%s",	dir_name, file_name);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	// open the file
	file_fd = open(path, O_WRONLY | O_SYNC);
	assert(file_fd >= 0);

	// write the string to the file
	result = write(file_fd, write_str, strlen(write_str));
	assert((size_t)(result) == strlen(write_str));

	// close the file
	result = close(file_fd);
	assert(result == 0);
}

static double fixed_2_double(int32_t fixed_in) {

	uint32_t weight = 0x10000000;
	double result = 0.0;
	int i;
	int is_negative = 0;

	if(fixed_in < 0) {
		fixed_in *= -1;
		is_negative = 1;
	}

	for(i = 0 ; i < 28 ; i++) {
		if(fixed_in & 0x00000001)
			result += (double)1.0 / (double)weight;

		fixed_in >>= 1;
		weight >>= 1;
	}

	weight = 0x00000001;

	for(i = 0 ; i < 3 ; i++) {
		if(fixed_in & 0x00000001)
			result += (double)(weight);

		fixed_in >>= 1;
		weight <<= 1;
	}

	if(is_negative)
		result *= -1.0;

	return(result);
}

void set_new_max_iters(int new_max_iters) {
	g_max_iters = new_max_iters;
}

void set_new_zoom_direction(int new_zoom) {
	g_zoom_in = new_zoom;
}

void explore_console_print(void) {

	char string_buffer[128];

	console_print((char *)"Floating point and fixed point representations are shown");
	console_print((char *)"below.  The fixed point representation is a signed 32-bit");
	console_print((char *)"integer where the upper 4-bits represent the integer and lower");
	console_print((char *)"28-bits represent the fraction.  So 1.0 fixed point would be");
	console_print((char *)"0x10000000.");
	console_print((char *)"");
	console_print((char *)"Current Coordinate:");

	snprintf(string_buffer, 128,
			"X:%- 13.10f Y:%- 13.10f | X:0x%08X  Y:0x%08X",
			fixed_2_double(g_center_x_int32),
			fixed_2_double(g_center_y_int32),
			g_center_x_int32,
			g_center_y_int32);
	console_print(string_buffer);

	console_print((char *)"");

	snprintf(string_buffer, 128, "Current X dimension: %- 13.10f | 0x%08X",
			fixed_2_double(g_x_dim_int32),
			g_x_dim_int32);
	console_print(string_buffer);

	console_print((char *)"");

	snprintf(string_buffer, 128, "Current iterations: %d", g_max_iters);
	console_print(string_buffer);

	console_print((char *)"");

	snprintf(string_buffer, 128, "Image Width : %d", g_pixmap_width);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "Image Height: %d", g_mandel_height);
	console_print(string_buffer);
}

