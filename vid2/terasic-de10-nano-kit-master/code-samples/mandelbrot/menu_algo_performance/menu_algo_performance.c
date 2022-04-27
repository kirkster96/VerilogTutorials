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

#include "menu_algo_performance/menu_algo_performance.h"
#include "menu_support/menu_support.h"
#include "font_support/font_def_public.h"

static void do_back(void);

static struct menu_struct *g_back_menu = NULL;

static struct menu_item_struct menu_items[] = {
	{"Back",
		NULL, do_back},
	{"Hardware Algorithm Performance",
		NULL, do_hw_performance},
	{"Software Algorithm Performance",
		NULL, do_sw_performance},
	{"About this menu",
		NULL, about_algo_performance_print},
	{0}
};

static struct menu_struct menu = {
	"Algorithm Performance",
	&cousine_bold_20,
	menu_items
};

static void do_back(void) {
	set_g_current_menu(g_back_menu);
}

void do_menu_algo_performance(void) {
	g_back_menu = get_g_current_menu();
	set_g_current_menu(&menu);
}

