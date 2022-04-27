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

#include "menu_config_demo/menu_config_demo.h"
#include "menu_support/menu_support.h"
#include "font_support/font_def_public.h"
#include "menu_main/menu_main.h"
#include "do_fluid_movement/movement_functions.h"
#include "mandelbrot_sw_neon_float/mandelbrot_sw_neon_float.h"
#include "mandelbrot_sw_neon_int/mandelbrot_sw_neon_int.h"
#include "mandelbrot_sw_float/mandelbrot_sw_float.h"
#include "mandelbrot_sw_int/mandelbrot_sw_int.h"

static void do_live_fb(void);
static void do_buffered_fb(void);
static void do_back(void);

static struct menu_struct *g_back_menu = NULL;

static struct menu_item_struct menu_items[] = {
	{"Back",
		NULL, do_back},
	{"Draw directly into live frame buffer",
		NULL, do_live_fb},
	{"Draw into buffered frame buffer",
		NULL, do_buffered_fb},
	{0}
};

static struct menu_struct menu = {
	"Configure Drawing Mode",
	&cousine_bold_20,
	menu_items
};

static void do_live_fb(void) {
	set_g_buffered_video_mode(0);
	do_back();
}

static void do_buffered_fb(void) {
	set_g_buffered_video_mode(1);
	do_back();
}

static void do_back(void) {
	set_g_current_menu(g_back_menu);
}

void do_buf_vid_config(void) {
	g_back_menu = get_g_current_menu();
	set_g_current_menu(&menu);
}

void default_buf_vid_config(void) {
	do_buffered_fb();
}

