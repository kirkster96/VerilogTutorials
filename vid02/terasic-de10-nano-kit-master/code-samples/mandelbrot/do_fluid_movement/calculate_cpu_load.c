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
#include <fcntl.h>

static uint32_t g_user_time = 0;
static uint32_t g_nice_time = 0;
static uint32_t g_system_time = 0;
static uint32_t g_idle_time = 0;
static uint32_t g_iowait_time = 0;
static uint32_t g_irq_time = 0;
static uint32_t g_softirq_time = 0;

float calculate_cpu_load(void) {

	int result;
	int file_fd;
	char buffer[4096];

	uint32_t user_time;
	uint32_t nice_time;
	uint32_t system_time;
	uint32_t idle_time;
	uint32_t iowait_time;
	uint32_t irq_time;
	uint32_t softirq_time;

	uint32_t user_time_diff;
	uint32_t nice_time_diff;
	uint32_t system_time_diff;
	uint32_t idle_time_diff;
	uint32_t iowait_time_diff;
	uint32_t irq_time_diff;
	uint32_t softirq_time_diff;

	uint32_t total_time;
	float load_percent;

	file_fd = open("/proc/stat", O_RDONLY | O_SYNC);
	assert(file_fd >= 0);

	result = read(file_fd, buffer, 4096);
	assert(result >= 0);

	result = close(file_fd);
	assert(result >= 0);

	result = sscanf(buffer, "cpu %u %u %u %u %u %u %u",
			&user_time, &nice_time, &system_time, &idle_time,
			&iowait_time, &irq_time, &softirq_time);
	assert(result == 7);

	user_time_diff    = user_time    - g_user_time;
	nice_time_diff    = nice_time    - g_nice_time;
	system_time_diff  = system_time  - g_system_time;
	idle_time_diff    = idle_time    - g_idle_time;
	iowait_time_diff  = iowait_time  - g_iowait_time;
	irq_time_diff     = irq_time     - g_irq_time;
	softirq_time_diff = softirq_time - g_softirq_time;

	g_user_time    = user_time;
	g_nice_time    = nice_time;
	g_system_time  = system_time;
	g_idle_time    = idle_time;
	g_iowait_time  = iowait_time;
	g_irq_time     = irq_time;
	g_softirq_time = softirq_time;

	total_time =	user_time_diff +
			nice_time_diff +
			system_time_diff +
			idle_time_diff +
			iowait_time_diff +
			irq_time_diff +
			softirq_time_diff;

	load_percent = 100.0 - (((float)idle_time_diff * 100) /
							((float)total_time));

	return(load_percent);
}

void init_cpu_load(void) {

	int result;
	int file_fd;
	char buffer[4096];

	uint32_t user_time;
	uint32_t nice_time;
	uint32_t system_time;
	uint32_t idle_time;
	uint32_t iowait_time;
	uint32_t irq_time;
	uint32_t softirq_time;

	file_fd = open("/proc/stat", O_RDONLY | O_SYNC);
	assert(file_fd >= 0);

	result = read(file_fd, buffer, 4096);
	assert(result >= 0);

	result = close(file_fd);
	assert(result >= 0);

	result = sscanf(buffer, "cpu %u %u %u %u %u %u %u",
			&user_time, &nice_time, &system_time, &idle_time,
			&iowait_time, &irq_time, &softirq_time);
	assert(result == 7);

	g_user_time    = user_time;
	g_nice_time    = nice_time;
	g_system_time  = system_time;
	g_idle_time    = idle_time;
	g_iowait_time  = iowait_time;
	g_irq_time     = irq_time;
	g_softirq_time = softirq_time;

	return;
}

