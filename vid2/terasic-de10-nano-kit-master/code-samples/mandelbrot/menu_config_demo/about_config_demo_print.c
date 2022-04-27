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

#include "menu_config_demo/menu_config_demo.h"
#include "do_fluid_movement/movement_functions.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "font_support/font_def_public.h"
#include "gt_timer/gt_timer.h"
#include "mandelbrot.h"
#include "de10_init/de10_init.h"

void about_config_demo_print(void) {

	struct abc_font_struct *original_font;
	int i;
	const char *display_strings[] = {
"The Continuous Demo Configuration menu allows you to select",
"a hardware algorithm and a software algorithm that are used",
"during the continuous demo mode.",
" ",
"The Configure Hardware Algorithm menu allows you to select",
"between 1 to 12 accelerators that are enabled in the FPGA",
"fabric while the demo is calculated with the hardware",
"algorithm.",
" ",
"The Configure Software Algorithm menu allows you to select 1",
"of 8 possible software algorithms.  The software algorithms",
"all run on the Cortex A9 processor however they differ along",
"these parameters:",
"-- floating point vs integer math used for the computation",
"-- regular C vs NEON optimized vector intrinsics",
"-- single thread vs multithreaded",
"The names displayed in the menu indicate each configuration.",
" ",
"The Configure Drawing Mode menu allows you to select how the",
"demo draws into the frame buffer.  Directly or buffered.",
" ",
"The Show Current Settings menu displays the current hardware",
"and software algorithms that are used by the continuous demo",
"mode as well as the drawing mode that is used.",
" ",
"Press KEY0 or KEY1 to continue...",
NULL
};

	//
	// Font metrics for 640 x 480 display:
	//	cousine_bold_16: 62 characters x 26 rows
	//	cousine_bold_18: 57 characters x 23 rows
	//	cousine_bold_20: 52 characters x 20 rows
	//	cousine_bold_30: 34 characters x 14 rows
	//
	original_font = get_console_font();
	set_console_font(&cousine_bold_16);

	console_clear();

	i = 0;
	while(display_strings[i] != NULL)
		console_print((char *)(display_strings[i++]));

	get_input_key();

	set_console_font(original_font);
}

