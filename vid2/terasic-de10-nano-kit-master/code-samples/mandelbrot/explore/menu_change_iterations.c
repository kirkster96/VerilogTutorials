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

#include "menu_support/menu_support.h"
#include "font_support/font_def_public.h"
#include "explore/explore.h"

static void do_config_0(void);
static void do_config_1(void);
static void do_config_2(void);
static void do_config_3(void);
static void do_config_4(void);
static void do_config_5(void);
static void do_config_6(void);
static void do_config_7(void);
static void do_back(void);

static struct menu_struct *g_back_menu = NULL;

struct iters_configs {
	const char *name;
	int32_t iters;
};

static const char name_0[] = "25 iterations";
static const char name_1[] = "100 iterations";
static const char name_2[] = "250 iterations";
static const char name_3[] = "500 iterations";
static const char name_4[] = "1000 iterations";
static const char name_5[] = "2000 iterations";
static const char name_6[] = "4000 iterations";
static const char name_7[] = "8000 iterations";

static struct iters_configs the_iters_configs[] = {
	{name_0, 25},
	{name_1, 100},
	{name_2, 250},
	{name_3, 500},
	{name_4, 1000},
	{name_5, 2000},
	{name_6, 4000},
	{name_7, 8000},
};

static struct menu_item_struct menu_items[] = {
	{"Back",
		NULL, do_back},
	{name_0,
		NULL, do_config_0},
	{name_1,
		NULL, do_config_1},
	{name_2,
		NULL, do_config_2},
	{name_3,
		NULL, do_config_3},
	{name_4,
		NULL, do_config_4},
	{name_5,
		NULL, do_config_5},
	{name_6,
		NULL, do_config_6},
	{name_7,
		NULL, do_config_7},
	{0}
};

static struct menu_struct menu = {
	"Change Iteration Count",
	&cousine_bold_20,
	menu_items
};

static void apply_config(int index) {
	set_new_max_iters(the_iters_configs[index].iters);
	menu_explore_return();
}

static void do_config_0(void) {
	apply_config(0);
}

static void do_config_1(void) {
	apply_config(1);
}

static void do_config_2(void) {
	apply_config(2);
}

static void do_config_3(void) {
	apply_config(3);
}

static void do_config_4(void) {
	apply_config(4);
}

static void do_config_5(void) {
	apply_config(5);
}

static void do_config_6(void) {
	apply_config(6);
}

static void do_config_7(void) {
	apply_config(7);
}

static void do_back(void) {
	set_g_current_menu(g_back_menu);
}

void do_change_iterations(void) {
	g_back_menu = get_g_current_menu();
	set_g_current_menu(&menu);
}

