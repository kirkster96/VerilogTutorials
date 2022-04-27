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
#include <unistd.h>

#include "mandelbrot.h"
#include "de10_init/de10_init.h"
#include "menu_support/menu_support.h"
#include "pix_map_routines/pix_map_base_routines.h"
#include "font_support/font_public.h"
#include "font_support/font_def_public.h"
#include "gpio_support/gpio_support.h"

#define MENU_WIDTH	(640)
#define MENU_HEIGHT	(480)

static const char *selected_str   = " > ";
static const char *unselected_str = "   ";

static uint32_t g_menu_init = 0;
static uint32_t *g_offscreen_menu_buffer = NULL;
static uint32_t *g_menu_blend_buffer = NULL;
static struct menu_struct *g_current_menu = NULL;

static void initialize_menu_selector(struct menu_struct *menu) {

	int i;

	i = 0;
	while(menu->menu_items[i].name != NULL) {
		if(i == 0)
			menu->menu_items[i].selector_str = selected_str;
		else
			menu->menu_items[i].selector_str = unselected_str;

		i++;
	};
}

static void increment_menu_selector(struct menu_struct *menu) {

	int i;
	const char *last_selector_str;
	const char *this_selector_str;

	i = 0;
	last_selector_str = menu->menu_items[i].selector_str;
	menu->menu_items[i].selector_str = unselected_str;
	i++;
	while(menu->menu_items[i].name != NULL) {

		this_selector_str = menu->menu_items[i].selector_str;
		menu->menu_items[i].selector_str = last_selector_str;
		last_selector_str = this_selector_str;
		i++;
	};

	if(last_selector_str == selected_str)
		menu->menu_items[0].selector_str = last_selector_str;
}

static void get_menu_action(struct menu_struct *menu, void (**action)(void)) {

	int i;

	i = 0;
	while(menu->menu_items[i].name != NULL) {
		if(menu->menu_items[i].selector_str == selected_str) {
			*action = menu->menu_items[i].action;
			return;
		}

		i++;
	};

	assert(0);
}

static void draw_menu(struct menu_struct *menu) {

	int i;
	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t pixmap_height;
	uint32_t font_height;
	uint32_t string_width;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t rows_above_menu;
	uint32_t cols_left_of_menu;
	uint32_t first_menu_pixel;
	uint32_t menu_height_less_crawl;

	pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
	pixmap_width = get_fb_width();
	pixmap_height = get_fb_height();
	font_height = menu->menu_font[0].bounds_height;

	rows_above_menu = (pixmap_height / 2) - (MENU_HEIGHT / 2);
	cols_left_of_menu = (pixmap_width / 2) - (MENU_WIDTH / 2);
	first_menu_pixel = (rows_above_menu * pixmap_width) + cols_left_of_menu;

	fill_pix_map_32(
		(char *)g_offscreen_menu_buffer,// char *dest_ptr,
		MENU_WIDTH,			// long dest_active_width,
		MENU_HEIGHT,			// long dest_active_height,
		MENU_WIDTH,			// long dest_line_width,
		0x00000000			// long fill_color
	);

	string_width = get_string_line_width(
				menu->menu_font,		// struct abc_font_struct *theFontStruct,	// pointer to the font structure
				(char *)menu->menu_title	// char *the_string				// pointer to the string to draw
			);

	assert(string_width <= MENU_WIDTH);

	start_x = (MENU_WIDTH / 2) - (string_width / 2);

	menu_height_less_crawl = MENU_HEIGHT - font_height;
	start_y = font_height + (font_height / 2);
	assert(start_y <= (menu_height_less_crawl - font_height));

	draw_string_line(
		menu->menu_font,		// struct abc_font_struct *theFontStruct,	// pointer to the font structure
		g_offscreen_menu_buffer,	// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
		MENU_WIDTH,			// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
		g_menu_blend_buffer,		// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
		0x00FFFFFF,			// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
		(char *)menu->menu_title,	// char *the_string,		// pointer to the string to draw
		start_y,			// uint32_t under_line,		// the horizontal line number above which the text will draw
		start_x,			// uint32_t start_x,		// the vertical column number that text begins to draw
		string_width + 1		// uint32_t max_width		// maximum horizontal width to draw
	);

	start_y += font_height / 2;
	i = 0;
	while(menu->menu_items[i].name != NULL) {

		start_x = 0;

		start_y += font_height;
		assert(start_y <= (menu_height_less_crawl - font_height));

		string_width = get_string_line_width(
				menu->menu_font,				// struct abc_font_struct *theFontStruct,	// pointer to the font structure
				(char *)menu->menu_items[i].selector_str	// char *the_string				// pointer to the string to draw
			);

		assert(string_width <= (MENU_WIDTH - 1));

		draw_string_line(
			menu->menu_font,				// struct abc_font_struct *theFontStruct,	// pointer to the font structure
			g_offscreen_menu_buffer,			// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
			MENU_WIDTH,					// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
			g_menu_blend_buffer,				// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,					// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
			(char *)menu->menu_items[i].selector_str,	// char *the_string,		// pointer to the string to draw
			start_y,					// uint32_t under_line,		// the horizontal line number above which the text will draw
			start_x,					// uint32_t start_x,		// the vertical column number that text begins to draw
			string_width + 1				// uint32_t max_width		// maximum horizontal width to draw
		);

		start_x += string_width;

		string_width = get_string_line_width(
					menu->menu_font,			// struct abc_font_struct *theFontStruct,	// pointer to the font structure
					(char *)menu->menu_items[i].name	// char *the_string				// pointer to the string to draw
				);

		assert(string_width <= (MENU_WIDTH - 1 - start_x));

		draw_string_line(
			menu->menu_font,			// struct abc_font_struct *theFontStruct,	// pointer to the font structure
			g_offscreen_menu_buffer,		// uint32_t *theDrawBuffer,     // this is the buffer to draw the font into
			MENU_WIDTH,				// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
			g_menu_blend_buffer,			// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
			0x00FFFFFF,				// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
			(char *)menu->menu_items[i].name,	// char *the_string,		// pointer to the string to draw
			start_y,				// uint32_t under_line,		// the horizontal line number above which the text will draw
			start_x,				// uint32_t start_x,		// the vertical column number that text begins to draw
			string_width + 1			// uint32_t max_width		// maximum horizontal width to draw
		);

		i++;
	};

	copy_pix_map_32_to_32(
		(char *)g_offscreen_menu_buffer,	// char *src_ptr,
		MENU_WIDTH,				// long src_active_width,
		menu_height_less_crawl,			// long src_active_height,
		MENU_WIDTH,				// long src_line_width,
		(char *)(&pixmap_buffer[first_menu_pixel]),	// char *dest_ptr,
		pixmap_width				// long dest_line_width
	);
}

static void menu_crawler_thread_cleanup(void *arg) {
	free(arg);
}

static void *menu_crawler_thread(void *arg) {

	int i;
	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t pixmap_height;
	uint32_t font_height;
	uint32_t string_width;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t rows_above_menu;
	uint32_t cols_left_of_menu;
	uint32_t first_menu_pixel;
	uint32_t first_crawl_pixel;
	const char *crawl_string =
			"Press the KEY0 push button to select menu item.  "
			"Press the KEY1 push button to sequence through menu items.  "
			"Hold KEY1 down to continuously sequence through menu items.";
	uint32_t crawl_buffer_width;
	uint32_t crawl_buffer_height;
	uint32_t *crawl_offscreen_buffer;

	(void)arg;

	pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
	pixmap_width = get_fb_width();
	pixmap_height = get_fb_height();
	font_height = g_current_menu->menu_font[0].bounds_height;

	string_width = get_string_line_width(
				g_current_menu->menu_font,	// struct abc_font_struct *theFontStruct,	// pointer to the font structure
				(char *)crawl_string		// char *the_string				// pointer to the string to draw
			);

	crawl_buffer_width = (MENU_WIDTH * 2) + string_width;
	crawl_buffer_height = font_height;
	crawl_offscreen_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
				crawl_buffer_width * crawl_buffer_height));
	assert(crawl_offscreen_buffer != NULL);

	pthread_cleanup_push(menu_crawler_thread_cleanup, crawl_offscreen_buffer);

	fill_pix_map_32(
		(char *)crawl_offscreen_buffer,	// char *dest_ptr,
		crawl_buffer_width,		// long dest_active_width,
		crawl_buffer_height,		// long dest_active_height,
		crawl_buffer_width,		// long dest_line_width,
		0x00000000			// long fill_color
	);

	start_x = MENU_WIDTH;

	start_y = font_height;
	assert(start_y <= (MENU_HEIGHT - font_height));

	draw_string_line(
		g_current_menu->menu_font,	// struct abc_font_struct *theFontStruct,	// pointer to the font structure
		crawl_offscreen_buffer,		// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
		crawl_buffer_width,		// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
		g_menu_blend_buffer,		// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
		0x00FFFFFF,			// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
		(char *)crawl_string,		// char *the_string,		// pointer to the string to draw
		start_y,			// uint32_t under_line,		// the horizontal line number above which the text will draw
		start_x,			// uint32_t start_x,		// the vertical column number that text begins to draw
		string_width + 1		// uint32_t max_width		// maximum horizontal width to draw
	);

	rows_above_menu = (pixmap_height / 2) - (MENU_HEIGHT / 2);
	cols_left_of_menu = (pixmap_width / 2) - (MENU_WIDTH / 2);
	first_menu_pixel = (rows_above_menu * pixmap_width) + cols_left_of_menu;
	first_crawl_pixel = (MENU_HEIGHT - crawl_buffer_height) * pixmap_width;
	first_crawl_pixel += first_menu_pixel;

	while(1) {
		for(i = 0 ; i < (int)(crawl_buffer_width - MENU_WIDTH) ; i++) {
			copy_pix_map_32_to_32(
				(char *)(&crawl_offscreen_buffer[i]),	// char *src_ptr,
				MENU_WIDTH,				// long src_active_width,
				crawl_buffer_height,			// long src_active_height,
				crawl_buffer_width,			// long src_line_width,
				(char *)(&pixmap_buffer[first_crawl_pixel]),	// char *dest_ptr,
				pixmap_width				// long dest_line_width
			);
			usleep(10000);
		};
	};
	pthread_cleanup_pop(1);
	return(NULL);
}

void init_menu_support(void) {

	assert(g_menu_init == 0);
	g_menu_init = 1;

	g_offscreen_menu_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
						MENU_WIDTH * MENU_HEIGHT));
	assert(g_offscreen_menu_buffer != NULL);

	g_menu_blend_buffer = (uint32_t *)(malloc(sizeof(uint32_t) * 64 * 64));
	assert(g_menu_blend_buffer != NULL);

	fill_pix_map_32(
		(char *)(g_menu_blend_buffer),	// char *dest_ptr,
		64,				// long dest_active_width,
		64,				// long dest_active_height,
		64,				// long dest_line_width,
		0x00000000			// long fill_color
	);
}

void release_menu_support(void) {

	assert(g_menu_init == 1);

	free(g_offscreen_menu_buffer);
	free(g_menu_blend_buffer);
}

void start_menu(void (**return_action)(void)) {

	int result;
	int input_key;
	void (*action)(void) = NULL;
	pthread_t crawler_thread;
	int milli_wait;
	int first_pass;

	assert(g_menu_init == 1);

	initialize_menu_selector(g_current_menu);
	draw_menu(g_current_menu);

	result = pthread_create(&crawler_thread, NULL, menu_crawler_thread,
								NULL);
	assert(result == 0);

	while(1) {
		input_key = get_input_key();
		if(input_key == 1) {
			first_pass = 1;
			do {
				increment_menu_selector(g_current_menu);
				draw_menu(g_current_menu);
				if(first_pass == 1) {
					milli_wait = 500;
					first_pass = 0;
				} else {
					milli_wait = 150;
				}
			} while(key1_still_down(milli_wait));
		} else {
			get_menu_action(g_current_menu, &action);
			*return_action = action;

			result = pthread_cancel(crawler_thread);
			assert(result == 0);

			result = pthread_join(crawler_thread, NULL);
			assert(result == 0);

			return;
		}
	}
}

uint32_t get_g_menu_init(void) {
	return(g_menu_init);
}

void set_g_current_menu(struct menu_struct *new_menu) {
	g_current_menu = new_menu;
}

struct menu_struct *get_g_current_menu(void) {
	return(g_current_menu);
}

