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
#include <string.h>

#include "menu_main/menu_main.h"
#include "menu_support/menu_support.h"
#include "font_support/font_def_public.h"
#include "mandelbrot.h"
#include "do_fluid_movement/movement_functions.h"
#include "menu_config_demo/menu_config_demo.h"
#include "gt_timer/gt_timer.h"
#include "menu_algo_performance/menu_algo_performance.h"
#include "menu_algo_accuracy/menu_algo_accuracy.h"
#include "explore/explore.h"


static void do_exit_explore_mode(void);
static void do_exit_explore_menu(void);

static struct menu_item_struct menu_items[] = {
	{"Close Menu",
		NULL, do_exit_explore_menu},
	{"Change Zoom Direction",
		NULL, do_change_zoom_direction},
	{"Change Iteration Count",
		NULL, do_change_iterations},
	{"Current Image Details",
		NULL, do_current_explore_print},
	{"About this menu",
		NULL, about_explore_print},
	{"Exit Explore Mode",
		NULL, do_exit_explore_mode},
	{0}
};

static struct menu_struct menu = {
	"Explore Menu",
	&cousine_bold_20,
	menu_items
};

static int g_exit_explore_menu = 0;
static int g_exit_explore_mode = 0;

void menu_explore_start(void) {

	void (*action)(void);

	assert(get_g_menu_init() == 1);

	set_g_current_menu(&menu);

	g_exit_explore_menu = 0;
	g_exit_explore_mode = 0;

	while(g_exit_explore_menu == 0) {
		start_menu(&action);
		assert(action != NULL);
		action();
	}
}

void menu_explore_return(void) {
	set_g_current_menu(&menu);
}

int check_exit_explore_mode(void) {
	return(g_exit_explore_mode);
}

static void do_exit_explore_mode(void) {
	menu_main_return();
	g_exit_explore_menu = 1;
	g_exit_explore_mode = 1;
}

static void do_exit_explore_menu(void) {
	g_exit_explore_menu = 1;
}

