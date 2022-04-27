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
#include <assert.h>

#include "do_fluid_movement/movement_functions.h"
#include "de10_init/de10_init.h"
#include "font_support/font_def_public.h"
#include "font_support/font_public.h"
#include "pix_map_routines/pix_map_base_routines.h"
#include "mandelbrot.h"

static struct abc_font_struct *g_banner_font = &cousine_bold_18;
static uint32_t g_banner_height;
static uint32_t g_banner_fps_width;
static uint32_t g_banner_cpu_width;
static uint32_t g_banner_current_offset;
static uint32_t g_banner_start_pixel;
static float g_latest_fps_value;
pthread_mutex_t g_fps_mutex;
pthread_cond_t g_fps_cond;
uint32_t g_fps_count;
static char *g_crawl_algo_str_ptr = NULL;
static char *g_crawl_hw_str_ptr = NULL;
static char *g_crawl_sw_str_ptr = NULL;
static uint32_t g_buffered_video_mode = 1;

static void * do_fps_banner_thread(void *arg);
static void * do_cpu_banner_thread(void *arg);
static void *do_banner_crawler_thread(void *arg);

// This routine is the loop overhead for the continuous demo.
// The coordinates below are some of the interesting points in the Mandelbrot
// set.
void do_fluid_continuous_demo(void) {

	void *pixel_buf_ptr;
	uint32_t pixmap_width = get_fb_width();
	uint32_t pixmap_height = get_fb_height();

	struct coordinates {
		double x;		// x_center
		double y;		// y_center
		double dim_x;		// x_dim(final zoom width of x axis)
		int max_iterations;	// max iterations
	} demo_locations[NUMBER_OF_COORDINATES] = {
{-1.760, 0.0, 0.05, 500},						  // 1
{-1.4781768172979355, 0.0, 0.01508731722831726, 50},			  // 2
{-0.006125070457284523, -0.8077499668488658, 1.6338581430231508E-4, 500}, // 3
{-0.5570354129425543, 0.6352957489439464, 9.274065304888445E-4, 100},	  // 4
{0.42450820043388005, 0.2075353308053001, 3.301533972620563E-4, 100}, 	  // 5
{-0.7625592537807768, 0.08955441532683481, 4.171464390594348E-4, 200},	  // 6
{0.35805511474609375, 0.6436386108398438, 0.0259552001953125, 50},	  // 7
{-0.10489989636229237, 0.9278526131989105, 2.3876997962775084E-4, 500},	  // 8
{-1.447650909, 0.000000000, 0.003, 500},				  // 9
{-1.457741737, 0.000000000, 0.000366211, 500},				  // 10
{-0.563892365, 0.667407990, 0.001464844, 500},				  // 11
{-0.597400665, 0.663105011, 0.005859375, 500},				  // 12
{-0.488316298, 0.609600782, 0.000122070, 700},				  // 13
	};

	double previous_x = 0.25;
	double previous_y = 0.0;
	double next_x, next_y, next_dim_x;
	int i, next_maxiterations;
	uint32_t mandel_height;
	int result;
	pthread_t fps_banner_thread;
	pthread_t cpu_banner_thread;
	pthread_t banner_crawler_thread;

	// configure hardware algorithm to start with
	set_g_draw_with_hw(1);
	g_crawl_algo_str_ptr = g_crawl_hw_str_ptr;

	// calculate banner geometry
	g_banner_height = g_banner_font[0].bounds_height;
	if(g_banner_height & 0x01)
		g_banner_height++;
	g_banner_fps_width = get_string_line_width(g_banner_font,
							(char *)"FPS:000.00");
	g_banner_fps_width += BANNER_WIDTH_AIR;
	g_banner_cpu_width = get_string_line_width(g_banner_font,
						(char *)"XX algo  %CPU:000.00");

	g_banner_cpu_width += BANNER_WIDTH_AIR;
	g_banner_current_offset = 0;
	mandel_height = pixmap_height - g_banner_height;
	g_banner_start_pixel = mandel_height * pixmap_width;

	pixel_buf_ptr = get_g_mandelbrot_fb_map();
	fill_pix_map_32(
		(char *)(&((uint32_t *)pixel_buf_ptr)[g_banner_start_pixel]),	// char *dest_ptr,
		pixmap_width,		// long dest_active_width,
		g_banner_height,	// long dest_active_height,
		pixmap_width,		// long dest_line_width,
		0x00000000		// long fill_color
	);

	pixel_buf_ptr = get_g_spare_0_fb_map();
	fill_pix_map_32(
		(char *)(&((uint32_t *)pixel_buf_ptr)[g_banner_start_pixel]),	// char *dest_ptr,
		pixmap_width,		// long dest_active_width,
		g_banner_height,	// long dest_active_height,
		pixmap_width,		// long dest_line_width,
		0x00000000		// long fill_color
	);

	pixel_buf_ptr = get_g_spare_1_fb_map();
	fill_pix_map_32(
		(char *)(&((uint32_t *)pixel_buf_ptr)[g_banner_start_pixel]),	// char *dest_ptr,
		pixmap_width,		// long dest_active_width,
		g_banner_height,	// long dest_active_height,
		pixmap_width,		// long dest_line_width,
		0x00000000		// long fill_color
	);

	pixel_buf_ptr = get_g_mandelbrot_fb_map();

	// start banner threads
	if(g_buffered_video_mode == 1)
		start_buffered_video();

	g_fps_count = 0;

	result = pthread_cond_init(&g_fps_cond, NULL);
	assert(result == 0);

	result = pthread_mutex_init(&g_fps_mutex, NULL);
	assert(result == 0);

	result = pthread_create(&fps_banner_thread, NULL, do_fps_banner_thread,
									NULL);
	assert(result == 0);

	result = pthread_create(&cpu_banner_thread, NULL,
						do_cpu_banner_thread, NULL);
	assert(result == 0);

	result = pthread_create(&banner_crawler_thread, NULL,
						do_banner_crawler_thread, NULL);
	assert(result == 0);

	// do continuous loop
	set_g_draw_interrupted(0);
	while(1) {

		for(i = 0; i < NUMBER_OF_COORDINATES; i++) {
			/* read in the contents for the next zoom */
			next_x = demo_locations[i].x;
			next_y = demo_locations[i].y;
			next_dim_x = demo_locations[i].dim_x;
			next_maxiterations = demo_locations[i].max_iterations;

			do_fluid_move(previous_x, previous_y, next_x, next_y,
				pixel_buf_ptr, pixmap_width, mandel_height);

			if(get_g_draw_interrupted() == 1)
				break;

			do_fluid_zoom_in(next_x, next_y, next_dim_x,
				next_maxiterations, pixel_buf_ptr, pixmap_width,
				mandel_height);

			if(get_g_draw_interrupted() == 1)
				break;

			/* save off x and y to do the next move when the loop
			   rolls around */
			previous_x = next_x;
			previous_y = next_y;
		}

		if(get_g_draw_interrupted() == 1)
			break;
	}

	result = pthread_cancel(fps_banner_thread);
	assert(result == 0);

	result = pthread_cancel(cpu_banner_thread);
	assert(result == 0);

	result = pthread_cancel(banner_crawler_thread);
	assert(result == 0);

	result = pthread_join(fps_banner_thread, NULL);
	assert(result == 0);

	result = pthread_join(cpu_banner_thread, NULL);
	assert(result == 0);

	result = pthread_join(banner_crawler_thread, NULL);
	assert(result == 0);

	if(g_buffered_video_mode == 1)
		stop_buffered_video();
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

static void wait_fps_cond(void) {

	int result;
	uint32_t my_count;

	result = pthread_mutex_lock(&g_fps_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &g_fps_mutex);

	my_count = g_fps_count;

	while(my_count == g_fps_count) {
		result = pthread_cond_wait(&g_fps_cond,
					&g_fps_mutex);
		assert(result == 0);
	}

	pthread_cleanup_pop(1);
}

static void * do_fps_banner_thread(void *arg) {

	uint32_t *offscreen_draw_buffer;
	uint32_t *blend_buffer;
	char string_buffer[FPS_STR_LEN];
	int snprintf_length;
	uint32_t *onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
	uint32_t pixmap_width = get_fb_width();
	uint32_t start_pixel = g_banner_start_pixel;

	(void)arg;

	offscreen_draw_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					g_banner_fps_width * g_banner_height));
	assert(offscreen_draw_buffer != NULL);
	pthread_cleanup_push(thread_cleanup_free, offscreen_draw_buffer);

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

	while(1) {
		snprintf_length = snprintf(string_buffer, FPS_STR_LEN,
					"FPS:%3.2f", g_latest_fps_value);
		assert(snprintf_length < FPS_STR_LEN);

		fill_pix_map_32(
			(char *)(offscreen_draw_buffer),// char *dest_ptr,
			g_banner_fps_width,		// long dest_active_width,
			g_banner_height,		// long dest_active_height,
			g_banner_fps_width,		// long dest_line_width,
			0x00000000			// long fill_color
		);

		draw_string_line(
			g_banner_font,		// struct abc_font_struct *theFontStruct,     // pointer to the font structure
			offscreen_draw_buffer,	// uint32_t *theDrawBuffer,     // this is the buffer to draw the font into
			g_banner_fps_width,	// uint32_t theDrawBufferWidth, // this is the pixel line width of the draw buffer
			blend_buffer,		// uint32_t *theBlendBuffer,    // this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,		// uint32_t theTextColor,       // this is the color of the text we draw 32-bit word is this format [0BGR]
			string_buffer,		// char *the_string,           // pointer to the string to draw
			g_banner_height,	// uint32_t under_line,         // the horizontal line number above which the text will draw
			g_banner_current_offset,// uint32_t start_x,            // the vertical column number that text begins to draw
			g_banner_fps_width	// uint32_t max_width           // maximum horizontal width to draw
		);
		
		onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_fps_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_fps_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		onscreen_buffer = (uint32_t *)get_g_spare_0_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_fps_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_fps_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		onscreen_buffer = (uint32_t *)get_g_spare_1_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_fps_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_fps_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		wait_fps_cond();
	}

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	return(NULL);
}

void signal_fps_cond(void) {
	int result;

	result = pthread_mutex_lock(&g_fps_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &g_fps_mutex);

	g_fps_count++;

	result = pthread_cond_broadcast(&g_fps_cond);
	assert(result == 0);

	pthread_cleanup_pop(1);
}

static void * do_cpu_banner_thread(void *arg) {

	float cpu_utilization = 0.0;
	uint32_t *offscreen_draw_buffer;
	uint32_t *blend_buffer;
	char string_buffer[CPU_STR_LEN];
	int snprintf_length;
	uint32_t *onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
	uint32_t pixmap_width = get_fb_width();
	uint32_t start_pixel = g_banner_start_pixel + g_banner_fps_width;
	int offset_increment_flag = 1;

	(void)arg;

	offscreen_draw_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					g_banner_cpu_width * g_banner_height));
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

	init_cpu_load();

	while(1) {

		cpu_utilization = calculate_cpu_load();

		snprintf_length = snprintf(string_buffer, CPU_STR_LEN,
					"%s algo  %%CPU:%0.2f",
					(get_g_draw_with_hw()) ? "HW" : "SW",
					cpu_utilization);
		assert(snprintf_length < CPU_STR_LEN);

		fill_pix_map_32(
			(char *)(offscreen_draw_buffer),// char *dest_ptr,
			g_banner_cpu_width,		// long dest_active_width,
			g_banner_height,		// long dest_active_height,
			g_banner_cpu_width,		// long dest_line_width,
			0x00000000			// long fill_color
		);

		draw_string_line(
			g_banner_font,		// struct abc_font_struct *theFontStruct,     // pointer to the font structure
			offscreen_draw_buffer,	// uint32_t *theDrawBuffer,     // this is the buffer to draw the font into
			g_banner_cpu_width,	// uint32_t theDrawBufferWidth, // this is the pixel line width of the draw buffer
			blend_buffer,		// uint32_t *theBlendBuffer,    // this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,		// uint32_t theTextColor,       // this is the color of the text we draw 32-bit word is this format [0BGR]
			string_buffer,		// char *the_string,           // pointer to the string to draw
			g_banner_height,	// uint32_t under_line,         // the horizontal line number above which the text will draw
			g_banner_current_offset,// uint32_t start_x,            // the vertical column number that text begins to draw
			g_banner_cpu_width	// uint32_t max_width           // maximum horizontal width to draw
		);

		onscreen_buffer = (uint32_t *)get_g_mandelbrot_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_cpu_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_cpu_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		onscreen_buffer = (uint32_t *)get_g_spare_0_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_cpu_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_cpu_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		onscreen_buffer = (uint32_t *)get_g_spare_1_fb_map();
		copy_pix_map_32_to_32(
			(char *)offscreen_draw_buffer,		// char *src_ptr,
			g_banner_cpu_width,			// long src_active_width,
			g_banner_height,			// long src_active_height,
			g_banner_cpu_width,			// long src_line_width,
			(char *)(&onscreen_buffer[start_pixel]),// char *dest_ptr,
			pixmap_width				// long dest_line_width
		);

		sleep(1);

		if(offset_increment_flag == 1) {
			if(g_banner_current_offset < (BANNER_WIDTH_AIR - 2)) {
				g_banner_current_offset++;
			} else {
				offset_increment_flag = 0;
				g_banner_current_offset--;
			}
		} else {
			if(g_banner_current_offset > 0) {
				g_banner_current_offset--;
			} else {
				offset_increment_flag = 1;
				g_banner_current_offset++;
			}
		}

		signal_fps_cond();
	}

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	return(NULL);
}

static void *do_banner_crawler_thread(void *arg) {

	int i;
	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t font_height;
	uint32_t string_width;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t first_crawl_pixel;
	const char *crawl_format_str;
	char crawl_str[1024];
	int snprintf_length;
	uint32_t crawl_buffer_width;
	uint32_t crawl_buffer_height;
	uint32_t *crawl_offscreen_buffer;
	uint32_t pre_crawl_width;
	uint32_t onscreen_crawl_width;
	uint32_t *blend_buffer;
	char *last_crawl_algo_str_ptr;

	(void)arg;

	while(1) {
		crawl_format_str =
		"Mandelbrot fractal currently drawn with the %s algorithm.  "
		"Press the KEY0 push button to stop the demo and enter the menu.  "
		"Press the KEY1 push button to toggle between the hardware and "
		"software computation algorithms.";

		snprintf_length = snprintf(crawl_str, 1024,
					crawl_format_str, g_crawl_algo_str_ptr);
		assert(snprintf_length < 1024);

		last_crawl_algo_str_ptr = g_crawl_algo_str_ptr;

		pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
		pixmap_width = get_fb_width();
		font_height = g_banner_height;

		pre_crawl_width = g_banner_fps_width + g_banner_cpu_width + 10;
		assert(pre_crawl_width < pixmap_width);
		onscreen_crawl_width = pixmap_width - pre_crawl_width;
		assert(onscreen_crawl_width >= 200);
		if(onscreen_crawl_width > 400)
			onscreen_crawl_width = 400;

		first_crawl_pixel = g_banner_start_pixel + pixmap_width -
							onscreen_crawl_width;

		string_width = get_string_line_width(
					g_banner_font,	// struct abc_font_struct *theFontStruct,     // pointer to the font structure
					crawl_str	// char *the_string           // pointer to the string to draw
				);

		crawl_buffer_width = (onscreen_crawl_width * 2) + string_width;
		crawl_buffer_height = font_height;
		crawl_offscreen_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
				crawl_buffer_width * crawl_buffer_height));
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
			g_banner_font,		// struct abc_font_struct *theFontStruct,	// pointer to the font structure
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

				pixmap_buffer =
					(uint32_t *)get_g_mandelbrot_fb_map();
				copy_pix_map_32_to_32(
					(char *)(&crawl_offscreen_buffer[i]),		// char *src_ptr,
					onscreen_crawl_width,				// long src_active_width,
					crawl_buffer_height,				// long src_active_height,
					crawl_buffer_width,				// long src_line_width,
					(char *)(&pixmap_buffer[first_crawl_pixel]),	// char *dest_ptr,
					pixmap_width					// long dest_line_width
				);

				pixmap_buffer =
					(uint32_t *)get_g_spare_0_fb_map();
				copy_pix_map_32_to_32(
					(char *)(&crawl_offscreen_buffer[i]),		// char *src_ptr,
					onscreen_crawl_width,				// long src_active_width,
					crawl_buffer_height,				// long src_active_height,
					crawl_buffer_width,				// long src_line_width,
					(char *)(&pixmap_buffer[first_crawl_pixel]),	// char *dest_ptr,
					pixmap_width					// long dest_line_width
				);

				pixmap_buffer =
					(uint32_t *)get_g_spare_1_fb_map();
				copy_pix_map_32_to_32(
					(char *)(&crawl_offscreen_buffer[i]),		// char *src_ptr,
					onscreen_crawl_width,				// long src_active_width,
					crawl_buffer_height,				// long src_active_height,
					crawl_buffer_width,				// long src_line_width,
					(char *)(&pixmap_buffer[first_crawl_pixel]),	// char *dest_ptr,
					pixmap_width					// long dest_line_width
				);

				usleep(10000);
				if(last_crawl_algo_str_ptr !=
						g_crawl_algo_str_ptr)
					break;
			};

			if(last_crawl_algo_str_ptr !=
					g_crawl_algo_str_ptr)
				break;
		};
		pthread_cleanup_pop(1);
		pthread_cleanup_pop(1);
	}

	return(NULL);
}

void set_g_latest_fps_value(float new_value) {
	g_latest_fps_value = new_value;
}

char *get_g_crawl_algo_str_ptr(void) {
	return(g_crawl_algo_str_ptr);
}

char *get_g_crawl_hw_str_ptr(void) {
	return(g_crawl_hw_str_ptr);
}

char *get_g_crawl_sw_str_ptr(void) {
	return(g_crawl_sw_str_ptr);
}

void set_g_crawl_algo_str_ptr(char *new_str_ptr) {
	g_crawl_algo_str_ptr = new_str_ptr;
}

void set_g_crawl_hw_str_ptr(char *new_str_ptr) {
	g_crawl_hw_str_ptr = new_str_ptr;
}

void set_g_crawl_sw_str_ptr(char *new_str_ptr) {
	g_crawl_sw_str_ptr = new_str_ptr;
}

uint32_t get_g_banner_height(void) {
	
	return(g_banner_height);
}

uint32_t get_g_banner_start_pixel(void) {
	
	return(g_banner_start_pixel);
}

uint32_t get_g_buffered_video_mode(void) {
	return(g_buffered_video_mode);
}

void set_g_buffered_video_mode(uint32_t new_mode) {
	g_buffered_video_mode = new_mode;
}

