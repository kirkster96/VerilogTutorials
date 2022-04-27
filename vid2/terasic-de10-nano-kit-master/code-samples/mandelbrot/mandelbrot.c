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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "de10_init/de10_init.h"
#include "gt_timer/gt_timer.h"
#include "menu_support/menu_support.h"
#include "console_support/console_support.h"
#include "gpio_support/gpio_support.h"
#include "menu_main/menu_main.h"
#include "menu_config_demo/menu_config_demo.h"
#include "do_fluid_movement/movement_functions.h"

static char *poweroff_command_array[] = {
	(char *)"/sbin/poweroff",
	NULL
};

static char *reboot_command_array[] = {
	(char *)"/sbin/reboot",
	NULL
};

static const char g_version_digits_str[] = "0.5";
static const char g_build_date[] = __DATE__;
static const char g_build_time[] = __TIME__;
static uint32_t g_poweroff_flag = 0;
static uint32_t g_reboot_flag = 0;
static uint32_t g_enable_exit_only = 0;

int main(int argc, char *argv[]) {

	int i;
	int result;
	const char *standalone_str = "--standalone";
	size_t standalone_str_len = strnlen(standalone_str, 100);

	// check for debug mode switch
	g_enable_exit_only = 1;
	if(argc > 1) {
		for(i = 1 ; i < argc ; i++) {
			result = strncmp(standalone_str, argv[i], standalone_str_len);
			if(result == 0) {
				g_enable_exit_only = 0;
			}
		}
	}

	if(g_enable_exit_only == 0)
		no_exit_menu();

	// initialize everything
	de10_init();
	gt_initialize_everything();
	init_gpio_support();
	init_menu_support();
	init_console_support();
	clear_screen_to_black();
	set_frame_buffer_mandelbrot();

	// configure the default hw and sw demo configurations
	default_hw_config();
	default_sw_config();
	default_buf_vid_config();

	// start the continuous fluid demo
	do_fluid_continuous_demo();

	// start the main menu
	menu_main_start();

	// release everything
	set_frame_buffer_default();
	release_console_support();
	release_menu_support();
	release_gpio_support();
	gt_release_everything();
	de10_release();

	// stop the program, poweroff or reboot the system
	if(g_poweroff_flag != 0) {
		execv(poweroff_command_array[0], poweroff_command_array);
		assert(0);
	}

	if(g_reboot_flag != 0) {
		execv(reboot_command_array[0], reboot_command_array);
		assert(0);
	}

	return(0);
}

void set_g_reboot_flag(uint32_t new_reboot_flag) {
	g_reboot_flag = new_reboot_flag;
}

void set_g_poweroff_flag(uint32_t new_poweroff_flag) {
	g_poweroff_flag = new_poweroff_flag;
}

uint32_t get_g_enable_exit_only(void) {
	return(g_enable_exit_only);
}

const char *get_g_version_digits_str(void) {
	return(g_version_digits_str);
}

const char *get_g_build_date(void) {
	return(g_build_date);
}

const char *get_g_build_time(void) {
	return(g_build_time);
}

