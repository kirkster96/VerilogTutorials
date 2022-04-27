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

#include "mandelbrot.h"
#include "de10_init/de10_init.h"
#include "menu_support/menu_support.h"
#include "pix_map_routines/pix_map_base_routines.h"
#include "font_support/font_public.h"
#include "font_support/font_def_public.h"
#include "gpio_support/gpio_support.h"

#define CONSOLE_WIDTH	(640)
#define CONSOLE_HEIGHT	(480)

static uint32_t g_console_init = 0;
static uint32_t *g_offscreen_console_buffer = NULL;
static uint32_t *g_console_blend_buffer = NULL;
static struct abc_font_struct *g_consoleFont = NULL;

void console_clear(void) {

	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t pixmap_height;
	uint32_t rows_above_console;
	uint32_t cols_left_of_console;
	uint32_t first_console_pixel;

	assert(g_console_init == 1);

	pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
	pixmap_width = get_fb_width();
	pixmap_height = get_fb_height();

	rows_above_console = (pixmap_height / 2) - (CONSOLE_HEIGHT / 2);
	cols_left_of_console = (pixmap_width / 2) - (CONSOLE_WIDTH / 2);
	first_console_pixel = (rows_above_console * pixmap_width) +
							cols_left_of_console;

	fill_pix_map_32(
		(char *)g_offscreen_console_buffer,	// char *dest_ptr,
		CONSOLE_WIDTH,				// long dest_active_width,
		CONSOLE_HEIGHT,				// long dest_active_height,
		CONSOLE_WIDTH,				// long dest_line_width,
		0x00000000				// long fill_color
	);

	copy_pix_map_32_to_32(
		(char *)g_offscreen_console_buffer,		// char *src_ptr,
		CONSOLE_WIDTH,					// long src_active_width,
		CONSOLE_HEIGHT,					// long src_active_height,
		CONSOLE_WIDTH,					// long src_line_width,
		(char *)(&pixmap_buffer[first_console_pixel]),	// char *dest_ptr,
		pixmap_width					// long dest_line_width
	);
}

void console_print(char *new_line) {

	uint32_t *pixmap_buffer;
	uint32_t pixmap_width;
	uint32_t pixmap_height;
	uint32_t font_height;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t rows_above_console;
	uint32_t cols_left_of_console;
	uint32_t first_console_pixel;
	uint32_t scroll_pixel;
	uint32_t fill_pixel;

	assert(g_console_init == 1);

	pixmap_buffer = (uint32_t *)(get_g_mandelbrot_fb_map());
	pixmap_width = get_fb_width();
	pixmap_height = get_fb_height();
	font_height = g_consoleFont[0].bounds_height;

	rows_above_console = (pixmap_height / 2) - (CONSOLE_HEIGHT / 2);
	cols_left_of_console = (pixmap_width / 2) - (CONSOLE_WIDTH / 2);
	first_console_pixel = (rows_above_console * pixmap_width) +
							cols_left_of_console;

	scroll_pixel = font_height * CONSOLE_WIDTH;

	copy_pix_map_32_to_32(
		(char *)(&g_offscreen_console_buffer[scroll_pixel]),	// char *src_ptr,
		CONSOLE_WIDTH,						// long src_active_width,
		CONSOLE_HEIGHT - font_height,				// long src_active_height,
		CONSOLE_WIDTH,						// long src_line_width,
		(char *)g_offscreen_console_buffer,			// char *dest_ptr,
		CONSOLE_WIDTH						// long dest_line_width
	);

	fill_pixel = (CONSOLE_HEIGHT - font_height) * CONSOLE_WIDTH;

	fill_pix_map_32(
		(char *)(&g_offscreen_console_buffer[fill_pixel]),	// char *dest_ptr,
		CONSOLE_WIDTH,						// long dest_active_width,
		font_height,						// long dest_active_height,
		CONSOLE_WIDTH,						// long dest_line_width,
		0x00000000						// long fill_color
	);

	start_x = 5;

	start_y = CONSOLE_HEIGHT;

	draw_string_line(
		g_consoleFont,			// struct abc_font_struct *theFontStruct,	// pointer to the font structure
		g_offscreen_console_buffer,	// uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
		CONSOLE_WIDTH,			// uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
		g_console_blend_buffer,		// uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
		0x00FFFFFF,			// uint32_t theTextColor,	// this is the color of the text we draw 32-bit word is this format [0BGR]
		new_line, 			// char *the_string,		// pointer to the string to draw
		start_y,			// uint32_t under_line,		// the horizontal line number above which the text will draw
		start_x,			// uint32_t start_x,		// the vertical column number that text begins to draw
		CONSOLE_WIDTH - 10		// uint32_t max_width		// maximum horizontal width to draw
	);

	copy_pix_map_32_to_32(
		(char *)g_offscreen_console_buffer,		// char *src_ptr,
		CONSOLE_WIDTH,					// long src_active_width,
		CONSOLE_HEIGHT,					// long src_active_height,
		CONSOLE_WIDTH,					// long src_line_width,
		(char *)(&pixmap_buffer[first_console_pixel]),	// char *dest_ptr,
		pixmap_width					// long dest_line_width
	);
}

void init_console_support(void) {

	assert(g_console_init == 0);
	g_console_init = 1;

	g_offscreen_console_buffer = (uint32_t *)(malloc(sizeof(uint32_t) *
					CONSOLE_WIDTH * CONSOLE_HEIGHT));
	assert(g_offscreen_console_buffer != NULL);

	fill_pix_map_32(
		(char *)(g_offscreen_console_buffer),	// char *dest_ptr,
		CONSOLE_WIDTH,				// long dest_active_width,
		CONSOLE_HEIGHT,				// long dest_active_height,
		CONSOLE_WIDTH,				// long dest_line_width,
		0x00000000				// long fill_color
	);

	g_console_blend_buffer = (uint32_t *)(malloc(sizeof(uint32_t) * 64 *
									64));
	assert(g_console_blend_buffer != NULL);

	fill_pix_map_32(
		(char *)(g_console_blend_buffer),	// char *dest_ptr,
		64,					// long dest_active_width,
		64,					// long dest_active_height,
		64,					// long dest_line_width,
		0x00000000				// long fill_color
	);

	g_consoleFont = &cousine_bold_20;
}

void release_console_support(void) {

	assert(g_console_init == 1);

	free(g_offscreen_console_buffer);
	free(g_console_blend_buffer);
}

struct abc_font_struct *get_console_font(void) {
	return(g_consoleFont);
}

void set_console_font(struct abc_font_struct *new_font) {
	g_consoleFont = new_font;
}

