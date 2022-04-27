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

void do_current_settings(void) {

	struct abc_font_struct *original_font;
	char string_buffer[128];

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

	console_print((char *)"HW CONFIG:");

	snprintf(string_buffer, 128, "%s", get_g_crawl_hw_str_ptr());
	console_print(string_buffer);

	console_print((char *)"");

	console_print((char *)"SW CONFIG:");

	snprintf(string_buffer, 128, "%s", get_g_crawl_sw_str_ptr());
	console_print(string_buffer);

	console_print((char *)"");

	console_print((char *)"DRAWING MODE:");

	snprintf(string_buffer, 128, "%s", get_g_buffered_video_mode() ?
						"Buffered Video Mode" :
						"Live Frame Buffer Mode");
	console_print(string_buffer);

	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

