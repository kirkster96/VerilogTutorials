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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include "menu_config_demo/menu_config_demo.h"
#include "do_fluid_movement/movement_functions.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "font_support/font_def_public.h"
#include "gt_timer/gt_timer.h"
#include "mandelbrot.h"
#include "de10_init/de10_init.h"

#define BENCHMARK_STATEMENT_FILE "/usr/share/mandelbrot/benchmark.txt"
#define READ_BUFFER_SIZE	(16 *1024)

extern char g_version_str[];

void about_main_print(void) {

	struct abc_font_struct *original_font;
	char string_buffer[128];
	char read_buffer[READ_BUFFER_SIZE];
	char *current_ptr;
	char *end_of_data;
	char *end_of_line;
	int fd;
	ssize_t read_size;
	int i;

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

	console_print((char *)"DE10-Nano Mandelbrot Demo");

	console_print((char *)"");

	snprintf(string_buffer, 128, "Version: %s '%s'",
						get_g_version_digits_str(),
						g_version_str);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "Built on %s at %s", get_g_build_date(),
							get_g_build_time());
	console_print(string_buffer);

	console_print((char *)"");

	snprintf(string_buffer, 128, "HDMI width: %d", get_fb_width());
	console_print(string_buffer);

	snprintf(string_buffer, 128, "HDMI height: %d", get_fb_height());
	console_print(string_buffer);

	console_print((char *)"");

	console_print((char *)"Timing for CPU and Global Timer:");

	gt_console_print_stats();

	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	console_clear();

	fd = open(BENCHMARK_STATEMENT_FILE, O_RDONLY);
	if(fd < 0) {
		if(errno == ENOENT) {
			console_print((char *)"Benchmark statement file not found:");
			snprintf(string_buffer, 128, "'%s'",
						BENCHMARK_STATEMENT_FILE);
			console_print(string_buffer);
		} else {
			assert(0);
		}
	} else {
		memset(read_buffer, '\0', READ_BUFFER_SIZE);
		read_size = read(fd, read_buffer, READ_BUFFER_SIZE - 1);
		assert(read_size >= 0);
		close(fd);
		if(read_size > 0) {
			// verify that there are no carriage return characters
			end_of_data = strchr(read_buffer,'\r');
			assert(end_of_data == NULL);
			// find the end of the file data
			end_of_data = strchr(read_buffer,'\0');
			current_ptr = read_buffer;
			for(i = 0 ; i < 26 ; i++) {
				end_of_line = strchr(current_ptr,'\n');
				if(end_of_line == NULL) {
					if(current_ptr < end_of_data) {
						console_print(current_ptr);
						current_ptr = end_of_data;
					}
					break;
				} else {
					*end_of_line = '\0';
					console_print(current_ptr);
					current_ptr = end_of_line + 1;
				}
			}

			get_input_key();

			while(current_ptr < end_of_data){
				for(i = 0 ; i < 13 ; i++) {
					end_of_line = strchr(current_ptr,'\n');
					if(end_of_line == NULL) {
						if(current_ptr < end_of_data) {
							console_print(
								current_ptr);
							current_ptr =
								end_of_data;
						}
						break;
					} else {
						*end_of_line = '\0';
						console_print(current_ptr);
						current_ptr = end_of_line + 1;
					}
				}

				if(current_ptr < end_of_data)
					get_input_key();
			}
		} else {
			console_print((char *)"Benchmark statement file appears to be empty");
		}
	}

	console_print((char *)"");

	console_print((char *)"Press KEY0 or KEY1 to continue...");

	get_input_key();

	set_console_font(original_font);
}

