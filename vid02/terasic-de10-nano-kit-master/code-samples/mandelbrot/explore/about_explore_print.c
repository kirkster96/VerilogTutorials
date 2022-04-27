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

#include "explore/explore.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "font_support/font_def_public.h"

void about_explore_print(void) {

	struct abc_font_struct *original_font;
	int i;
	const char *display_strings[] = {
"The Change Zoom menu allows you to toggle the zoom direction",
"when the KEY1 push button is pressed in Explore mode.  The",
"current setting is displayed on the information banner along",
"the bottom of the screen.  The image will zoom 10% on each",
"KEY1 press.",
" ",
"The Change Iteration Count menu allows you to select one of 8",
"iteration counts to be used during the fractal computation.",
"The more iterations that are applied reveals more detail in",
"the image.",
" ",
"The Current Image Details menu will display various",
"statistics about the current image displayed in the Explore",
"mode.",
" ",
"The Close Menu option returns you to the last image displayed",
"in Explore mode, and the Exit Explore Mode menu exits Explore",
"mode.",
" ",
"ADVICE: ensure that the DE10-Nano is laying on a flat surface",
"so the accelerometer calibrates to a proper orientation.  Tip",
"the DE10-Nano board to move the cursor on the screen and then",
"return it to the flat surface and press KEY1 to zoom.  You",
"can recalibrate the accelerometer by closing the menu.",
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

