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

static void do_reboot(void);
static void do_poweroff(void);
static void do_exit(void);

static struct menu_item_struct menu_items[] = {
	{"Run Continuous Demo",
		NULL, do_fluid_continuous_demo},
	{"Configure Continuous Demo",
		NULL, do_config_demo},
	{"Algorithm Performance",
		NULL, do_menu_algo_performance},
	{"Algorithm Accuracy",
		NULL, do_menu_algo_accuracy},
	{"Explore",
		NULL, do_explore},
	{"About Demo",
		NULL, about_main_print},
	{"Reboot",
		NULL, do_reboot},
	{"Poweroff",
		NULL, do_poweroff},
	{"Exit to Console",
		NULL, do_exit},
	{0}
};

static struct menu_struct menu = {
	"Main Menu",
	&cousine_bold_30,
	menu_items
};

static uint32_t g_exit_menu_request = 0;

void no_exit_menu(void) {

	int index = 0;
	int result;

	while(menu_items[index].name != NULL) {
		result = strncmp(menu_items[index].name, "Exit to Console", 15);
		if(result == 0) {
			menu_items[index].name = NULL;
			break;
		}

		index++;
	}
}

void menu_main_start(void) {

	void (*action)(void);

	assert(get_g_menu_init() == 1);

	set_g_current_menu(&menu);

	while(g_exit_menu_request == 0) {
		start_menu(&action);
		assert(action != NULL);
		action();
	}
}

void menu_main_return(void) {
	set_g_current_menu(&menu);
}

static void do_reboot(void) {
	set_g_reboot_flag(1);
	g_exit_menu_request = 1;
}

static void do_poweroff(void) {
	set_g_poweroff_flag(1);
	g_exit_menu_request = 1;
}

static void do_exit(void) {
	if(get_g_enable_exit_only() != 0)
		g_exit_menu_request = 1;
}

