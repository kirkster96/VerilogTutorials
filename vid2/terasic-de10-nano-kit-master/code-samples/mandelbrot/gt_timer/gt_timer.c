/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <assert.h>

#include "gt_timer/gt_timer.h"
#include "console_support/console_support.h"

static int g_gt_timer_init = 0;
static int g_mpuclk_rate;
static int g_mpu_periph_clk_rate;
static int g_devmem_fd;
static void *g_periphbase_map = NULL;
static uint32_t g_gt_samples_ave = 0;
static volatile uint32_t *g_global_timer_ptr = NULL;

uint64_t gt_get_value(void) {

	uint32_t gt_lo;
	uint32_t gt_hi_0;
	uint32_t gt_hi_1;
	uint64_t gt_value;

	assert(g_gt_timer_init == 1);

	do {
		gt_hi_0 = g_global_timer_ptr[1];
		gt_lo = g_global_timer_ptr[0];
		gt_hi_1 = g_global_timer_ptr[1];

	} while(gt_hi_0 != gt_hi_1);

	gt_value = ((uint64_t)gt_hi_0 << 32) |
				((uint64_t)gt_lo & (uint64_t)0xFFFFFFFF);

	return gt_value;
}

void gt_initialize_everything(void) {

	int i;
	int fd;
	int result;
	char in_str[16];
	uint64_t gt_samples[10];
	uint64_t gt_samples_sum;
	uint32_t tick_count;
	float sec_time;
	float us_time;
	uint32_t periphbase;

	assert(g_gt_timer_init == 0);
	g_gt_timer_init = 1;

	/* gather system info */
	fd = open("/sys/kernel/debug/clk/mpuclk/clk_rate", O_RDONLY);
	assert(fd >= 0);

	result = read(fd, in_str, 16);
	assert(result >= 0);

	close(fd);
	g_mpuclk_rate = atoi(in_str);

	fd = open("/sys/kernel/debug/clk/mpu_periph_clk/clk_rate", O_RDONLY);
	assert(fd >= 0);

	result = read(fd, in_str, 16);
	assert(result >= 0);

	close(fd);
	g_mpu_periph_clk_rate = atoi(in_str);

	/* open the /dev/mem device */
	g_devmem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	assert(g_devmem_fd >= 0);

	/* mmap periphbase - on CV SoC PERIPHBASE should be 0xFFFEC000 but we
	 * will read the value out of the CBAR and mask it just to be certain
	 */
	asm("mrc p15, 4, %0, c15, c0, 0" : "=r" (periphbase));
	periphbase &= PERIPHBASE_MASK;

	g_periphbase_map = mmap(NULL, sysconf(_SC_PAGE_SIZE),
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			periphbase);
	assert(g_periphbase_map != MAP_FAILED);

	/* calculate global timer pointer */
	g_global_timer_ptr = (uint32_t*)((uint32_t)g_periphbase_map +
							GLOBAL_TIMER_OFFSET);

	/* calibrate gt_get_value */
	gt_samples[0] = gt_get_value();
	gt_samples[1] = gt_get_value();
	gt_samples[2] = gt_get_value();
	gt_samples[3] = gt_get_value();
	gt_samples[4] = gt_get_value();
	gt_samples[5] = gt_get_value();
	gt_samples[6] = gt_get_value();
	gt_samples[7] = gt_get_value();
	gt_samples[8] = gt_get_value();
	gt_samples[9] = gt_get_value();

	gt_samples_sum = 0;
	for(i = 0 ; i < 9 ; i++) {
		gt_samples_sum += gt_samples[i + 1] - gt_samples[i];
	}
	g_gt_samples_ave = gt_samples_sum / 9;

	tick_count = g_gt_samples_ave;
	sec_time = ((float)1 /
			(float)g_mpu_periph_clk_rate) * (float)tick_count;
	us_time = sec_time * 1000000;

	printf("                 CPU Frequency: %d hertz\n", g_mpuclk_rate);
	printf("                  GT Frequency: %d hertz\n", g_mpu_periph_clk_rate);
	printf("  GT access average (GT ticks): %d\n", g_gt_samples_ave);
	printf("        GT access average (us): %f\n", us_time);
}

void gt_release_everything(void) {

	int result;

	assert(g_gt_timer_init == 1);

	/* munmap and close /dev/mem */
	result = munmap(g_periphbase_map, sysconf(_SC_PAGE_SIZE));
	assert(result >= 0);

	close(g_devmem_fd);
}

void gt_print_results(uint64_t start_time, uint64_t end_time) {

	uint32_t tick_count;
	float sec_time;
	float us_time;

	assert(g_gt_timer_init == 1);

	if(end_time < (start_time + g_gt_samples_ave))
		tick_count = g_gt_samples_ave;
	else
		tick_count = end_time - start_time;
	tick_count -= g_gt_samples_ave;
	sec_time = ((float)1 /
			(float)g_mpu_periph_clk_rate) * (float)tick_count;
	us_time = sec_time * 1000000;
	printf("Duration (GT ticks): %d\n", tick_count);
	printf("      Duration (us): %f\n", us_time);
}

void gt_console_print_stats(void) {

	uint32_t tick_count;
	float sec_time;
	float us_time;
	char string_buffer[128];

	snprintf(string_buffer, 128, "               CPU Frequency: %d hertz\n", g_mpuclk_rate);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "                GT Frequency: %d hertz\n", g_mpu_periph_clk_rate);
	console_print(string_buffer);

	snprintf(string_buffer, 128, "GT access average (GT ticks): %d\n", g_gt_samples_ave);
	console_print(string_buffer);

	tick_count = g_gt_samples_ave;
	sec_time = ((float)1 /
			(float)g_mpu_periph_clk_rate) * (float)tick_count;
	us_time = sec_time * 1000000;

	snprintf(string_buffer, 128, "      GT access average (us): %f\n", us_time);
	console_print(string_buffer);

}

uint32_t get_g_gt_samples_ave(void) {
	return(g_gt_samples_ave);
}

uint32_t get_g_mpu_periph_clk_rate(void) {
	return(g_mpu_periph_clk_rate);
}

