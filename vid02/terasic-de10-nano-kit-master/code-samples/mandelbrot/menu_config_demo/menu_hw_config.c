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
#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "menu_main/menu_main.h"
#include "do_fluid_movement/movement_functions.h"

static void do_config_0(void);
static void do_config_1(void);
static void do_config_2(void);
static void do_config_3(void);
static void do_config_4(void);
static void do_config_5(void);
static void do_config_6(void);
static void do_config_7(void);
static void do_config_8(void);
static void do_config_9(void);
static void do_config_10(void);
static void do_config_11(void);
static void do_back(void);

static struct menu_struct *g_back_menu = NULL;

struct hw_configs {
	const char *name;
	uint32_t hw_engine_count;
};

static const char name_0[]  = "FPGA (12 accelerator)";
static const char name_1[]  = "FPGA (11 accelerator)";
static const char name_2[]  = "FPGA (10 accelerator)";
static const char name_3[]  = "FPGA (9 accelerator)";
static const char name_4[]  = "FPGA (8 accelerator)";
static const char name_5[]  = "FPGA (7 accelerator)";
static const char name_6[]  = "FPGA (6 accelerator)";
static const char name_7[]  = "FPGA (5 accelerator)";
static const char name_8[]  = "FPGA (4 accelerator)";
static const char name_9[]  = "FPGA (3 accelerator)";
static const char name_10[] = "FPGA (2 accelerator)";
static const char name_11[] = "FPGA (1 accelerator)";

static struct hw_configs the_hw_configs[] = {
	{name_0, 12},
	{name_1, 11},
	{name_2, 10},
	{name_3,  9},
	{name_4,  8},
	{name_5,  7},
	{name_6,  6},
	{name_7,  5},
	{name_8,  4},
	{name_9,  3},
	{name_10, 2},
	{name_11, 1},
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
	{name_8,
		NULL, do_config_8},
	{name_9,
		NULL, do_config_9},
	{name_10,
		NULL, do_config_10},
	{name_11,
		NULL, do_config_11},
	{0},
};

static struct menu_struct menu = {
	"Configure Hardware Algorithm",
	&cousine_bold_20,
	menu_items
};

static void apply_config(int index) {
	set_g_mandelbrot_hw_engine_count(the_hw_configs[index].hw_engine_count);
	set_g_crawl_hw_str_ptr((char *)the_hw_configs[index].name);
	do_back();
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

static void do_config_8(void) {
	apply_config(8);
}

static void do_config_9(void) {
	apply_config(9);
}

static void do_config_10(void) {
	apply_config(10);
}

static void do_config_11(void) {
	apply_config(11);
}

static void do_back(void) {
	set_g_current_menu(g_back_menu);
}

void do_hw_config(void) {
	g_back_menu = get_g_current_menu();
	set_g_current_menu(&menu);
}

void default_hw_config(void) {
	do_config_0();
}

