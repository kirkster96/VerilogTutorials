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
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <assert.h>

#include "mandelbrot_hw_engine/mandelbrot_hw_engine.h"
#include "de10_init/de10_init.h"
#include "gpio_support/gpio_support.h"

static uint32_t g_mandelbrot_hw_engine_count = 0;

static void *mandelbrot_hw_interrupt_thread(void *arg) {

	pthread_mutex_t *ready_mutex = (pthread_mutex_t *)arg;
	int result;
	void *instance;

	(void)arg;

	result = get_gpio_instance(MANDELBROT_INT_GPIO_NAME, &instance);
	assert(result == 0);

	wait_for_gpio_interrupt(instance, ready_mutex);
	return(NULL);
}

static void mandelbrot_hw_thread_cleanup(void *arg) {

	int result;
	pthread_t *thread_ptr = (pthread_t *)arg;
	volatile uint32_t *mandelbrot_distributor_ptr;
	uint32_t temp;

	mandelbrot_distributor_ptr = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_DISTRIBUTOR_BASE_OFST);

	// disable the interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_ENABLE] =
						~MANDEL_DIST_INT_ENABLE_MASK;

	// enable flush
	mandelbrot_distributor_ptr[MANDEL_DIST_FLUSH] = MANDEL_DIST_FLUSH_MASK;

	// cancel the interrupt thread
	result = pthread_cancel(*thread_ptr);
	assert(result == 0);

	// wait for mandelbrot distributor to reach idle state
	temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];
	while((temp & MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK) !=
		MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	while((temp & MANDEL_DIST_LATCH_STATE_IN_VECT_VALID_MASK) != 0)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	result = pthread_join(*thread_ptr, NULL);
	assert(result == 0);

	// clear any pending interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_CLEAR] =
						MANDEL_DIST_INT_CLEAR_MASK;
}

void frame_dma_evaluate_coordinate(
	int64_t  cr,
	int64_t  ci,
	uint32_t max_iter,
	int64_t  step_dim,
	uint8_t  line_count_shift,
	uint32_t pix_buf_ptr,
	uint16_t pix_map_width,
	uint16_t pix_map_height,
	uint32_t hw_engine_count
) {

	int result;
	volatile uint32_t *mandelbrot_colorizer_ptr;
	volatile uint32_t *mandelbrot_controller_ptr;
	volatile uint32_t *mandelbrot_distributor_ptr;
	uint32_t temp;
	pthread_t hw_interrupt_thread;
	pthread_mutex_t ready_mutex;

	mandelbrot_colorizer_ptr  = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_COLORIZER_0_BASE_OFST);

	mandelbrot_controller_ptr = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_CONTROLLER_BASE_OFST);

	mandelbrot_distributor_ptr = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_DISTRIBUTOR_BASE_OFST);

	// wait for mandelbrot controller to reach idle state
	temp = mandelbrot_controller_ptr[MANDEL_STATUS];
	while((temp & MANDEL_STATUS_GO_MASK) != 0)
		temp = mandelbrot_controller_ptr[MANDEL_STATUS];

	while((temp & (MANDEL_STATUS_READY_1_MASK | MANDEL_STATUS_READY_O_MASK))
		!= (MANDEL_STATUS_READY_1_MASK | MANDEL_STATUS_READY_O_MASK))
			temp = mandelbrot_controller_ptr[MANDEL_STATUS];

	// wait for mandelbrot distributor to reach idle state
	temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];
	while((temp & MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK) !=
		MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	while((temp & MANDEL_DIST_LATCH_STATE_IN_VECT_VALID_MASK) != 0)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	// ensure flush is disabled
	mandelbrot_distributor_ptr[MANDEL_DIST_FLUSH] = ~MANDEL_DIST_FLUSH_MASK;

	// clear any pending interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_CLEAR] =
						MANDEL_DIST_INT_CLEAR_MASK;

	// enable the interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_ENABLE] =
						MANDEL_DIST_INT_ENABLE_MASK;

	// enable the requested hardware channels
	temp = ((1 << hw_engine_count) - 1) & MANDEL_DIST_ENABLE_MASK;
	mandelbrot_distributor_ptr[MANDEL_DIST_ENABLE] = temp;

	// configure the mandelbrot colorizer cores to disable colorization
	mandelbrot_colorizer_ptr[0] = 0;
	mandelbrot_colorizer_ptr[1] = 0;
	mandelbrot_colorizer_ptr[2] = 0;
	mandelbrot_colorizer_ptr[3] = 0;
	mandelbrot_colorizer_ptr[4] = 0;
	mandelbrot_colorizer_ptr[5] = 0;
	mandelbrot_colorizer_ptr[6] = 0;
	mandelbrot_colorizer_ptr[7] = 0;
	mandelbrot_colorizer_ptr[8] = 0;
	mandelbrot_colorizer_ptr[9] = 0;
	mandelbrot_colorizer_ptr[10] = 0;
	mandelbrot_colorizer_ptr[11] = 0;

	// configure the mandelbrot controller for the next coordinate
	mandelbrot_controller_ptr[MANDEL_PIX_MAP_HEIGHT] =
						(uint32_t)(pix_map_height);
	mandelbrot_controller_ptr[MANDEL_PIX_MAP_WIDTH] =
						(uint32_t)(pix_map_width);
	mandelbrot_controller_ptr[MANDEL_PIX_BUF_PTR] = pix_buf_ptr;
	mandelbrot_controller_ptr[MANDEL_LINE_COUNT_SHIFT] =
						(uint32_t)(line_count_shift);
	mandelbrot_controller_ptr[MANDEL_STEP_DIM] =
				(uint32_t)((step_dim >> 28) & 0xFFFFFFFF);
	mandelbrot_controller_ptr[MANDEL_MAX_ITERS] = max_iter;
	mandelbrot_controller_ptr[MANDEL_CR] =
					(uint32_t)((cr >> 28) & 0xFFFFFFFF);
	mandelbrot_controller_ptr[MANDEL_CI] =
					(uint32_t)((ci >> 28) & 0xFFFFFFFF);

	// start the interrupt thread
	result = pthread_mutex_init(&ready_mutex, NULL);
	assert(result == 0);

	result = pthread_mutex_lock(&ready_mutex);
	assert(result == 0);

	result = pthread_create(&hw_interrupt_thread, NULL,
					mandelbrot_hw_interrupt_thread,
								&ready_mutex);
	assert(result == 0);

	pthread_cleanup_push(mandelbrot_hw_thread_cleanup,
							&hw_interrupt_thread);

	result = pthread_mutex_lock(&ready_mutex);
	assert(result == 0);

	result = pthread_mutex_unlock(&ready_mutex);
	assert(result == 0);

	result = pthread_mutex_destroy(&ready_mutex);
	assert(result == 0);

	// start the mandelbrot controller
	mandelbrot_controller_ptr[MANDEL_CONTROL] = MANDEL_CONTROL_GO_MASK;

	// wait for the interrupt
	result = pthread_join(hw_interrupt_thread, NULL);
	assert(result == 0);

	pthread_cleanup_pop(0);

	// clear the interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_CLEAR] =
						MANDEL_DIST_INT_CLEAR_MASK;
}

void color_frame_dma_evaluate_coordinate(
	int64_t  cr,
	int64_t  ci,
	uint32_t max_iter,
	int64_t  step_dim,
	uint8_t  line_count_shift,
	uint32_t pix_buf_ptr,
	uint16_t pix_map_width,
	uint16_t pix_map_height,
	uint32_t hw_engine_count
) {

	int result;
	volatile uint32_t *mandelbrot_colorizer_ptr;
	volatile uint32_t *mandelbrot_controller_ptr;
	volatile uint32_t *mandelbrot_distributor_ptr;
	uint32_t temp;
	pthread_t hw_interrupt_thread;
	pthread_mutex_t ready_mutex;

	mandelbrot_colorizer_ptr  = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_COLORIZER_0_BASE_OFST);

	mandelbrot_controller_ptr = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_CONTROLLER_BASE_OFST);

	mandelbrot_distributor_ptr = (uint32_t *)((uint32_t)get_g_lwh2f_map()
					+ MANDELBROT_DISTRIBUTOR_BASE_OFST);

	// wait for mandelbrot controller to reach idle state
	temp = mandelbrot_controller_ptr[MANDEL_STATUS];
	while((temp & MANDEL_STATUS_GO_MASK) != 0)
		temp = mandelbrot_controller_ptr[MANDEL_STATUS];

	while((temp & (MANDEL_STATUS_READY_1_MASK | MANDEL_STATUS_READY_O_MASK))
		!= (MANDEL_STATUS_READY_1_MASK | MANDEL_STATUS_READY_O_MASK))
			temp = mandelbrot_controller_ptr[MANDEL_STATUS];

	// wait for mandelbrot distributor to reach idle state
	temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];
	while((temp & MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK) !=
		MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	while((temp & MANDEL_DIST_LATCH_STATE_IN_VECT_VALID_MASK) != 0)
		temp = mandelbrot_distributor_ptr[MANDEL_DIST_PORT_STATE];

	// ensure flush is disabled
	mandelbrot_distributor_ptr[MANDEL_DIST_FLUSH] = ~MANDEL_DIST_FLUSH_MASK;

	// clear any pending interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_CLEAR] =
						MANDEL_DIST_INT_CLEAR_MASK;

	// enable the interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_ENABLE] =
						MANDEL_DIST_INT_ENABLE_MASK;

	// enable the requested hardware channels
	temp = ((1 << hw_engine_count) - 1) & MANDEL_DIST_ENABLE_MASK;
	mandelbrot_distributor_ptr[MANDEL_DIST_ENABLE] = temp;

	// configure the mandelbrot colorizer cores to disable colorization
	mandelbrot_colorizer_ptr[0] = max_iter;
	mandelbrot_colorizer_ptr[1] = max_iter;
	mandelbrot_colorizer_ptr[2] = max_iter;
	mandelbrot_colorizer_ptr[3] = max_iter;
	mandelbrot_colorizer_ptr[4] = max_iter;
	mandelbrot_colorizer_ptr[5] = max_iter;
	mandelbrot_colorizer_ptr[6] = max_iter;
	mandelbrot_colorizer_ptr[7] = max_iter;
	mandelbrot_colorizer_ptr[8] = max_iter;
	mandelbrot_colorizer_ptr[9] = max_iter;
	mandelbrot_colorizer_ptr[10] = max_iter;
	mandelbrot_colorizer_ptr[11] = max_iter;

	// configure the mandelbrot controller for the next coordinate
	mandelbrot_controller_ptr[MANDEL_PIX_MAP_HEIGHT] =
						(uint32_t)(pix_map_height);
	mandelbrot_controller_ptr[MANDEL_PIX_MAP_WIDTH] =
						(uint32_t)(pix_map_width);
	mandelbrot_controller_ptr[MANDEL_PIX_BUF_PTR] = pix_buf_ptr;
	mandelbrot_controller_ptr[MANDEL_LINE_COUNT_SHIFT] =
						(uint32_t)(line_count_shift);
	mandelbrot_controller_ptr[MANDEL_STEP_DIM] =
				(uint32_t)((step_dim >> 28) & 0xFFFFFFFF);
	mandelbrot_controller_ptr[MANDEL_MAX_ITERS] = max_iter;
	mandelbrot_controller_ptr[MANDEL_CR] =
					(uint32_t)((cr >> 28) & 0xFFFFFFFF);
	mandelbrot_controller_ptr[MANDEL_CI] =
					(uint32_t)((ci >> 28) & 0xFFFFFFFF);

	// start the interrupt thread
	result = pthread_mutex_init(&ready_mutex, NULL);
	assert(result == 0);

	result = pthread_mutex_lock(&ready_mutex);
	assert(result == 0);

	result = pthread_create(&hw_interrupt_thread, NULL,
				mandelbrot_hw_interrupt_thread, &ready_mutex);
	assert(result == 0);

	pthread_cleanup_push(mandelbrot_hw_thread_cleanup,
							&hw_interrupt_thread);

	result = pthread_mutex_lock(&ready_mutex);
	assert(result == 0);

	result = pthread_mutex_unlock(&ready_mutex);
	assert(result == 0);

	result = pthread_mutex_destroy(&ready_mutex);
	assert(result == 0);

	// start the mandelbrot controller
	mandelbrot_controller_ptr[MANDEL_CONTROL] = MANDEL_CONTROL_GO_MASK;

	// wait for the interrupt
	result = pthread_join(hw_interrupt_thread, NULL);
	assert(result == 0);

	pthread_cleanup_pop(0);

	// clear the interrupt
	mandelbrot_distributor_ptr[MANDEL_DIST_INT_CLEAR] =
						MANDEL_DIST_INT_CLEAR_MASK;
}

void draw_frame_dma_hw_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height,
			uint32_t hw_engine_count) {

	uint32_t max_iter;
	int64_t current_x;
	int64_t current_y;
	int64_t x_dim;
	int64_t center_x;
	int64_t center_y;
	int64_t step_dim;
	uint32_t *dma_buffer_ptr = (uint32_t *)pixel_buf_ptr;

	uint32_t line_count_shift = 4;

	(void)pixel_buf_ptr;

	// set the initial center point, x dimension, and maximum iterations
	center_x = (int64_t)(in_x * 0x0100000000000000LL);
	center_y = (int64_t)(in_y * 0x0100000000000000LL);
	x_dim = (int64_t)(in_x_dim * 0x0100000000000000LL);
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;
	step_dim &= 0xFFFFFFFFF0000000;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	current_x &= 0xFFFFFFFFF0000000;
	current_y = center_y + ((pixmap_height / 2) * step_dim);
	current_y &= 0xFFFFFFFFF0000000;

	//process the image

	// evaluate this coordinate
	frame_dma_evaluate_coordinate(
		current_x & 0xFFFFFFFFF0000000,
		current_y & 0xFFFFFFFFF0000000,
		max_iter,
		step_dim,
		line_count_shift,
		(uint32_t)(dma_buffer_ptr),
		(uint16_t)(pixmap_width & 0x0000FFFF),
		(uint16_t)(pixmap_height & 0x0000FFFF),
		hw_engine_count
	);
}


void draw_color_frame_dma_hw_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height,
			uint32_t hw_engine_count) {

	uint32_t max_iter;
	int64_t current_x;
	int64_t current_y;
	int64_t x_dim;
	int64_t center_x;
	int64_t center_y;
	int64_t step_dim;
	uint32_t *dma_buffer_ptr = (uint32_t *)pixel_buf_ptr;

	uint32_t line_count_shift = 4;

	(void)pixel_buf_ptr;

	// set the initial center point, x dimension, and maximum iterations
	center_x = (int64_t)(in_x * 0x0100000000000000LL);
	center_y = (int64_t)(in_y * 0x0100000000000000LL);
	x_dim = (int64_t)(in_x_dim * 0x0100000000000000LL);
	max_iter = in_max_iter;

	// calculate the step dimension between each pixel
	step_dim = x_dim / pixmap_width;
	step_dim &= 0xFFFFFFFFF0000000;

	// calculate the top left pixel value to start with
	current_x = center_x - (x_dim / 2);
	current_x &= 0xFFFFFFFFF0000000;
	current_y = center_y + ((pixmap_height / 2) * step_dim);
	current_y &= 0xFFFFFFFFF0000000;

	//process the image

	// evaluate this coordinate
	color_frame_dma_evaluate_coordinate(
		current_x & 0xFFFFFFFFF0000000,
		current_y & 0xFFFFFFFFF0000000,
		max_iter,
		step_dim,
		line_count_shift,
		(uint32_t)(dma_buffer_ptr),
		(uint16_t)(pixmap_width & 0x0000FFFF),
		(uint16_t)(pixmap_height & 0x0000FFFF),
		hw_engine_count
	);
}

uint32_t get_g_mandelbrot_hw_engine_count(void) {
	return(g_mandelbrot_hw_engine_count);
}

void set_g_mandelbrot_hw_engine_count(uint32_t new_count) {
	g_mandelbrot_hw_engine_count = new_count;
}

