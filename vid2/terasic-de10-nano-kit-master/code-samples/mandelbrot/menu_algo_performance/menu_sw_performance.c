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
#include "menu_main/menu_main.h"
#include "do_fluid_movement/movement_functions.h"
#include "mandelbrot_sw_neon_float/mandelbrot_sw_neon_float.h"
#include "mandelbrot_sw_neon_int/mandelbrot_sw_neon_int.h"
#include "mandelbrot_sw_float/mandelbrot_sw_float.h"
#include "mandelbrot_sw_int/mandelbrot_sw_int.h"

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

struct sw_configs {
	const char *name;
	const char *crawl_name;
	void (*sw_algorithm)(float, float, float, uint32_t, void *,
							uint32_t, uint32_t);
};

static const char name_0[] = "Multithreaded Cortex A9 NEON FP VI";
static const char crawl_name_0[] = "Multithreaded Cortex A9 NEON Floating Point Vector Intrinsics";

static const char name_1[] = "Single thread Cortex A9 NEON FP VI";
static const char crawl_name_1[] = "Single thread Cortex A9 NEON Floating Point Vector Intrinsics";

static const char name_2[] = "Multithreaded Cortex A9 NEON INT VI";
static const char crawl_name_2[] = "Multithreaded Cortex A9 NEON Integer Vector Intrinsics";

static const char name_3[] = "Single thread Cortex A9 NEON INT VI";
static const char crawl_name_3[] = "Single thread Cortex A9 NEON Integer Vector Intrinsics";

static const char name_4[] = "Multithreaded Cortex A9 FP";
static const char crawl_name_4[] = "Multithreaded Cortex A9 Floating Point";

static const char name_5[] = "Single thread Cortex A9 FP";
static const char crawl_name_5[] = "Single thread Cortex A9 Floating Point";

static const char name_6[] = "Multithreaded Cortex A9 INT";
static const char crawl_name_6[] = "Multithreaded Cortex A9 Integer";

static const char name_7[] = "Single thread Cortex A9 INT";
static const char crawl_name_7[] = "Single thread Cortex A9 Integer";

static struct sw_configs the_sw_configs[] = {
	{name_0,
		crawl_name_0,
		&draw_vect4_float_mandelbrot_mt},
	{name_1,
		crawl_name_1,
		&draw_vect4_float_mandelbrot},
	{name_2,
		crawl_name_2,
		&draw_vect4_int_mandelbrot_mt},
	{name_3,
		crawl_name_3,
		&draw_vect4_int_mandelbrot},
	{name_4,
		crawl_name_4,
		&draw_float_mandelbrot_mt},
	{name_5,
		crawl_name_5,
		&draw_float_mandelbrot},
	{name_6,
		crawl_name_6,
		&draw_int_mandelbrot_mt},
	{name_7,
		crawl_name_7,
		&draw_int_mandelbrot},
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
	"Software Algorithm Performance",
	&cousine_bold_20,
	menu_items
};

static void apply_config(int index) {
	do_algo_performance((char *)the_sw_configs[index].crawl_name,
					0,
					the_sw_configs[index].sw_algorithm);
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

void do_sw_performance(void) {
	g_back_menu = get_g_current_menu();
	set_g_current_menu(&menu);
}

