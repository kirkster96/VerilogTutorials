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
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <poll.h>

#include "gpio_support/gpio_support.h"

struct gpio_instance_struct {
	const char *gpio_name;
	const char *label_str;
	const char *interrupt_str;
	const uint32_t gpio_offset;
	uint32_t gpio_number;
	int gpio_value_fd;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	uint32_t count;
};

static struct gpio_instance_struct g_gpio_instance[] = {
	{
		KEY0_GPIO_NAME,
		"gpio@0x100005000",
		"falling",
		0,
		0,
		0,
		0,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER,
		0,
	},
	{
		KEY1_GPIO_NAME,
		"gpio@0x100005000",
		"falling",
		1,
		0,
		0,
		0,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER,
		0,
	},
	{
		MANDELBROT_INT_GPIO_NAME,
		"gpio@0x100100090",
		"rising",
		0,
		0,
		0,
		0,
		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER,
		0,
	},
	{
		0
	},
};

static uint32_t g_gpio_support_init = 0;

static uint32_t poll_gpio_instance(struct gpio_instance_struct *instance);

static void * gpio_broadcast_thread(void *arg) {

	struct gpio_instance_struct *instance =
					(struct gpio_instance_struct *)arg;
	int result;

	while(1) {
		poll_gpio_instance(instance);

		result = pthread_mutex_lock(&instance->mutex);
		assert(result == 0);

		instance->count++;

		result = pthread_cond_broadcast(&instance->cond);
		assert(result == 0);

		result = pthread_mutex_unlock(&instance->mutex);
		assert(result == 0);
	}

	return(0);
}

static void init_gpio_instance(struct gpio_instance_struct *instance) {
	DIR *gpio_dir;
	const char *gpio_dir_path = "/sys/class/gpio";
	const char *gpiochip_str = "gpiochip";
	const char *gpio_label = instance->label_str;
	size_t gpiochip_str_len = strlen(gpiochip_str);
	int result;
	struct dirent *dir_entry;
	char path[PATH_MAX];
	int path_length;
	int file_fd;
	char buffer[PATH_MAX+1] = {0};
	char gpio_number_buffer[PATH_MAX+1] = {0};
	char *str_result = NULL;
	char *newline_ptr;
	pthread_attr_t attr;
	struct sched_param param;

	printf("Initializing GPIO instance: '%s'\n", instance->gpio_name);

	// open the sysfs gpio directory
	gpio_dir = opendir(gpio_dir_path);
	assert(gpio_dir != NULL);

	// find the gpio controller for the KEY0 push button 'gpio@0x100005000'
	while(1) {
		// read the next directory entry
		errno = 0;
		dir_entry = readdir(gpio_dir);
		if(dir_entry == NULL) {
			assert(errno == 0);
			break;
		}

		// check if this is a gpio controller entry
		result = strncmp(dir_entry->d_name, gpiochip_str,
				gpiochip_str_len);
		if(result != 0)
			continue;

		// open the gpio controller label file and read label value
		path_length = snprintf(path, PATH_MAX, "%s/%s/label",
				gpio_dir_path, dir_entry->d_name);
		assert(path_length >= 0);
		assert(path_length < PATH_MAX);

		file_fd = open(path, O_RDONLY | O_SYNC);
		assert(file_fd >= 0);

		result = read(file_fd, buffer, PATH_MAX);
		assert(result >= 0);
		assert(result < PATH_MAX);

		result = close(file_fd);
		assert(result >= 0);

		buffer[PATH_MAX] = 0;
		// test the gpio controller label value for our gpio controller
		str_result = strstr(buffer, gpio_label);
		if(str_result != NULL)
			break;
	}

	assert(str_result != NULL);

	// open the gpio controller base file and read base value
	path_length = snprintf(path, PATH_MAX, "%s/%s/base",
			gpio_dir_path, dir_entry->d_name);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	file_fd = open(path, O_RDONLY | O_SYNC);
	assert(file_fd >= 0);

	result = read(file_fd, gpio_number_buffer, PATH_MAX);
	assert(result >= 0);
	assert(result < PATH_MAX);

	result = close(file_fd);
	assert(result >= 0);

	gpio_number_buffer[PATH_MAX] = 0;

	// remove the newline at the end of the string
	newline_ptr = strchr(gpio_number_buffer,'\n');
	if(newline_ptr != NULL)
		memset(newline_ptr, '\0', 1);

	instance->gpio_number = strtoul(gpio_number_buffer, NULL, 10);

	instance->gpio_number += instance->gpio_offset;

	path_length = snprintf(gpio_number_buffer, PATH_MAX, "%d",
			instance->gpio_number);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	// open the gpio export file and write our gpio number
	path_length = snprintf(path, PATH_MAX, "%s/export",
			gpio_dir_path);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	file_fd = open(path, O_WRONLY | O_SYNC);
	assert(file_fd >= 0);

	result = write(file_fd, gpio_number_buffer, strlen(gpio_number_buffer));
	// NOTE: we don't bother checking for errors here because if this gpio
	// has already been exported this write will receive a device busy error
	// which is perfectly normal.

	result = close(file_fd);
	assert(result >= 0);

	// open the gpio edge file and write 'falling' to it
	path_length = snprintf(path, PATH_MAX, "%s/gpio%s/edge",
			gpio_dir_path, gpio_number_buffer);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	file_fd = open(path, O_WRONLY | O_SYNC);
	assert(file_fd >= 0);

	result = write(file_fd, instance->interrupt_str,
			strlen(instance->interrupt_str));
	assert(result >= 0);
	assert(result == (int)strlen(instance->interrupt_str));

	result = close(file_fd);
	assert(result >= 0);

	// open the gpio value file
	path_length = snprintf(path, PATH_MAX, "%s/gpio%s/value",
			gpio_dir_path, gpio_number_buffer);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	file_fd = open(path, O_RDONLY | O_SYNC);
	assert(file_fd >= 0);

	instance->gpio_value_fd = file_fd;

	instance->count = 0;

	result = pthread_cond_init(&instance->cond, NULL);
	assert(result == 0);

	result = pthread_mutex_init(&instance->mutex, NULL);
	assert(result == 0);

	result = pthread_attr_init(&attr);
	assert(result == 0);

	result = pthread_attr_setschedpolicy(&attr, SCHED_RR);
	assert(result == 0);

	result = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	assert(result == 0);

	param.sched_priority = 99;
	result = pthread_attr_setschedparam(&attr, &param);
	assert(result == 0);

	result = pthread_create(&instance->thread, &attr, gpio_broadcast_thread,
								instance);
	assert(result == 0);

	return;
}

static void release_gpio_instance(struct gpio_instance_struct *instance) {

	int path_length;
	char path[PATH_MAX];
	char gpio_number_buffer[PATH_MAX+1] = {0};
	int result;
	int file_fd;
	const char *gpio_dir_path = "/sys/class/gpio";

	printf("Releasing GPIO instance: '%s'\n", instance->gpio_name);

	// cancel the thread and destroy the mutex and condition variable
	result = pthread_cancel(instance->thread);
	assert(result == 0);

	result = pthread_join(instance->thread, NULL);
	assert(result == 0);

	result = pthread_cond_destroy(&instance->cond);
	assert(result == 0);

	result = pthread_mutex_destroy(&instance->mutex);
	assert(result == 0);

	// close the gpio value file fd
	result = close(instance->gpio_value_fd);
	assert(result == 0);

	// build the gpio number string
	path_length = snprintf(gpio_number_buffer, PATH_MAX, "%d",
			instance->gpio_number);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	// open the gpio unexport file and write our gpio number
	path_length = snprintf(path, PATH_MAX, "%s/unexport",
			gpio_dir_path);
	assert(path_length >= 0);
	assert(path_length < PATH_MAX);

	file_fd = open(path, O_WRONLY | O_SYNC);
	assert(file_fd >= 0);

	result = write(file_fd, gpio_number_buffer, strlen(gpio_number_buffer));
	assert(result >= 0);
	assert((size_t)(result) == strlen(gpio_number_buffer));

	result = close(file_fd);
	assert(result >= 0);
}

int get_gpio_instance(const char *name, void **gpio_instance) {

	int result;
	uint32_t instance = 0;

	assert(g_gpio_support_init == 1);

	while(g_gpio_instance[instance].gpio_name != NULL) {
		result = strncmp(name, g_gpio_instance[instance].gpio_name,
				strlen(g_gpio_instance[instance].gpio_name));
		if(result == 0) {
			*gpio_instance = &g_gpio_instance[instance];
			return(0);
		}
		instance++;
	}

	return(-1);
}

static int validate_gpio_instance_ptr(
				struct gpio_instance_struct *instance_ptr) {

	uint32_t instance = 0;

	while(g_gpio_instance[instance].gpio_name != NULL) {
		if(instance_ptr == &g_gpio_instance[instance])
			return(0);
		instance++;
	}

	return(-1);
}

static uint32_t poll_gpio_instance(struct gpio_instance_struct *instance) {

	int result;
	char buffer[PATH_MAX+1] = {0};
	struct pollfd pollfd_struct;

	// we first need to read the file before we can poll it, otherwise poll
	// will not block
	result = (int)lseek(instance->gpio_value_fd, 0, SEEK_SET);
	assert(result == 0);

	result = read(instance->gpio_value_fd, buffer, PATH_MAX);
	assert(result >= 0);
	assert(result < PATH_MAX);

	buffer[PATH_MAX] = 0;

	pollfd_struct.fd = instance->gpio_value_fd;
	pollfd_struct.events = POLLPRI | POLLERR;
	pollfd_struct.revents = 0;

	result = poll(&pollfd_struct, 1, -1);
	assert(result == 1);

	result = (int)lseek(instance->gpio_value_fd, 0, SEEK_SET);
	assert(result == 0);

	result = read(instance->gpio_value_fd, buffer, PATH_MAX);
	assert(result == 2);

	if(buffer[0] == '0')
		return(0);
	else if(buffer[0] == '1')
		return(1);
	else
		assert(0);

	return(0);
}

uint32_t read_gpio_instance(void *instance_ptr) {

	struct gpio_instance_struct *instance =
				(struct gpio_instance_struct *)instance_ptr;
	int result;
	char buffer[PATH_MAX+1] = {0};

	assert(g_gpio_support_init == 1);

	result = validate_gpio_instance_ptr(instance);
	assert(result >= 0);

	result = (int)lseek(instance->gpio_value_fd, 0, SEEK_SET);
	assert(result == 0);

	result = read(instance->gpio_value_fd, buffer, PATH_MAX);
	assert(result == 2);

	if(buffer[0] == '0')
		return(0);
	else if(buffer[0] == '1')
		return(1);
	else
		assert(0);

	return(0);
}

static void thread_cleanup_cancel(void *arg) {
	pthread_t *thread = (pthread_t *)arg;
	int result;

	result = pthread_cancel(*thread);
	assert(result == 0);

	result = pthread_join(*thread, NULL);
	assert(result == 0);
}

static void thread_cleanup_mutex_unlock(void *arg) {
	pthread_mutex_t *mutex = (pthread_mutex_t *)arg;
	int result;

	result = pthread_mutex_unlock(mutex);
	assert(result == 0);
}

void wait_for_gpio_interrupt(void *instance_ptr, pthread_mutex_t *ready_mutex) {

	struct gpio_instance_struct *instance =
				(struct gpio_instance_struct *)instance_ptr;
	int result;
	uint32_t my_count;

	assert(g_gpio_support_init == 1);

	result = validate_gpio_instance_ptr(instance);
	assert(result >= 0);

	result = pthread_mutex_lock(&instance->mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock, &instance->mutex);

	my_count = instance->count;

	if(ready_mutex != NULL) {
		result = pthread_mutex_unlock(ready_mutex);
		assert(result == 0);
	}

	while(my_count == instance->count) {
		result = pthread_cond_wait(&instance->cond,
					&instance->mutex);
		assert(result == 0);
	}

	pthread_cleanup_pop(1);
}

static pthread_mutex_t g_get_input_key_mutex;
static pthread_cond_t g_get_input_key_cond;
static void *g_get_input_key_instance;

static void * get_input_key_thread(void *arg) {
	struct gpio_instance_struct *instance =
					(struct gpio_instance_struct *)arg;
	int result;

	while(1) {
		wait_for_gpio_interrupt(instance, NULL);

		result = pthread_mutex_lock(&g_get_input_key_mutex);
		assert(result == 0);

		pthread_cleanup_push(thread_cleanup_mutex_unlock,
							&g_get_input_key_mutex);

		if(g_get_input_key_instance == NULL)
			g_get_input_key_instance = arg;

		result = pthread_cond_broadcast(&g_get_input_key_cond);
		assert(result == 0);

		pthread_cleanup_pop(1);
	}

	return(0);
}


int get_input_key(void) {

	int result;
	void *key0_instance;
	void *key1_instance;
	pthread_attr_t attr;
	struct sched_param param;
	pthread_t key0_thread;
	pthread_t key1_thread;

	assert(g_gpio_support_init == 1);

	g_get_input_key_instance = NULL;

	result = get_gpio_instance(KEY0_GPIO_NAME, &key0_instance);
	assert(result >= 0);

	result = get_gpio_instance(KEY1_GPIO_NAME, &key1_instance);
	assert(result >= 0);

	result = pthread_cond_init(&g_get_input_key_cond, NULL);
	assert(result == 0);

	result = pthread_mutex_init(&g_get_input_key_mutex, NULL);
	assert(result == 0);

	result = pthread_attr_init(&attr);
	assert(result == 0);

	result = pthread_attr_setschedpolicy(&attr, SCHED_RR);
	assert(result == 0);

	result = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	assert(result == 0);

	param.sched_priority = 99;
	result = pthread_attr_setschedparam(&attr, &param);
	assert(result == 0);

	result = pthread_create(&key0_thread, &attr, get_input_key_thread,
								key0_instance);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_cancel, &key0_thread);

	result = pthread_create(&key1_thread, &attr, get_input_key_thread,
								key1_instance);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_cancel, &key1_thread);

	result = pthread_mutex_lock(&g_get_input_key_mutex);
	assert(result == 0);

	pthread_cleanup_push(thread_cleanup_mutex_unlock,
							&g_get_input_key_mutex);

	while(g_get_input_key_instance == NULL) {
		result = pthread_cond_wait(&g_get_input_key_cond,
					&g_get_input_key_mutex);
		assert(result == 0);
	}

	pthread_cleanup_pop(1);	// g_get_input_key_mutex
	pthread_cleanup_pop(1);	// key1_thread
	pthread_cleanup_pop(1); // key0_thread

	result = pthread_cond_destroy(&g_get_input_key_cond);
	assert(result == 0);

	result = pthread_mutex_destroy(&g_get_input_key_mutex);
	assert(result == 0);

	if( g_get_input_key_instance == key0_instance)
		return(0);
	else
		return(1);
}

void init_gpio_support(void) {

	uint32_t instance = 0;

	assert(g_gpio_support_init == 0);
	g_gpio_support_init = 1;

	while(g_gpio_instance[instance].gpio_name != NULL)
		init_gpio_instance(&g_gpio_instance[instance++]);
}

void release_gpio_support(void) {

	uint32_t instance = 0;

	assert(g_gpio_support_init == 1);

	while(g_gpio_instance[instance].gpio_name != NULL)
		release_gpio_instance(&g_gpio_instance[instance++]);
}

int key1_still_down(int milliseconds) {

	int result;
	void *key1_instance;
	uint32_t key1_state;
	int i;

	assert(g_gpio_support_init == 1);

	result = get_gpio_instance(KEY1_GPIO_NAME, &key1_instance);
	assert(result >= 0);

	for(i = 0 ; i < milliseconds ; i++) {
		key1_state = read_gpio_instance(key1_instance);
		if(key1_state == 1)
			return(0);
		usleep(1000);
	}

	key1_state = read_gpio_instance(key1_instance);
	if(key1_state == 1)
		return(0);

	return(1);
}

