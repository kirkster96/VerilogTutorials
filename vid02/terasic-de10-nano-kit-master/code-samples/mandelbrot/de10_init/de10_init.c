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

#include "de10_init/de10_init.h"
#include "pix_map_routines/pix_map_base_routines.h"
#include "do_fluid_movement/movement_functions.h"

static uint32_t g_de10_init = 0;
static int g_devmem_fd;
static void *g_lwh2f_map;
static void *g_mandelrot_fb_map;
static void *g_spare_0_fb_map;
static void *g_spare_1_fb_map;
static void *g_default_fb_map;
static volatile uint32_t *g_fbr_base_ptr;
static uint32_t g_fb_column_count;
static uint32_t g_fb_row_count;
static uint32_t g_optimized_color_table[16];
static void *g_current_display_buffer;
static void *g_current_draw_buffer;
static uint32_t g_buffered_video_flag;

void de10_init(void) {

	int fd;
	int result;
	char in_str[16];
	char *comma_ptr;

	assert(g_de10_init == 0);
	g_de10_init = 1;

	/* disable the frame buffer cursor blink so we can use it as an
	   offscreen buffer */
	fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY);
	assert(fd >= 0);

	result = write(fd, "0\n", 2);
	assert(result >= 0);

	close(fd);

	/* get the frame buffer size */
	fd = open("/sys/class/graphics/fb0/virtual_size", O_RDONLY);
	assert(fd >= 0);

	memset(in_str, 0x00, 16);
	result = read(fd, in_str, 15);
	assert(result >= 0);

	close(fd);

	comma_ptr = strchr(in_str, ',');
	assert(comma_ptr != NULL);
	*comma_ptr++ = '\0';
	g_fb_column_count = atoi(in_str);
	g_fb_row_count = atoi(comma_ptr);

	/* open the /dev/mem device */
	g_devmem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	assert(g_devmem_fd >= 0);

	/* mmap lwh2f bridge */
	g_lwh2f_map = mmap(NULL, LWH2F_SPAN,
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			LWH2F_BASE);
	assert(g_lwh2f_map != MAP_FAILED);

	/* calculate frame buffer reader peripheral base pointer */
	g_fbr_base_ptr = (uint32_t*)((uint32_t)g_lwh2f_map + FBR_BASE);

	/* mmap the mandelbrot frame buffer */
	g_mandelrot_fb_map = mmap(NULL, FRAME_BUFFER_SPAN,
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			MANDELBROT_FRAME_BUFFER_BASE);
	assert(g_mandelrot_fb_map != MAP_FAILED);

	/* mmap the default frame buffer */
	g_default_fb_map = mmap(NULL, FRAME_BUFFER_SPAN,
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			DEFAULT_FRAME_BUFFER_BASE);
	assert(g_default_fb_map != MAP_FAILED);

	/* mmap the spare_0 frame buffer */
	g_spare_0_fb_map = mmap(NULL, FRAME_BUFFER_SPAN,
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			SPARE_0_FRAME_BUFFER_BASE);
	assert(g_spare_0_fb_map != MAP_FAILED);

	/* mmap the spare_1 frame buffer */
	g_spare_1_fb_map = mmap(NULL, FRAME_BUFFER_SPAN,
			PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd,
			SPARE_1_FRAME_BUFFER_BASE);
	assert(g_spare_1_fb_map != MAP_FAILED);

	/* original color table from 2004 implementation */
	g_optimized_color_table[0]  = 0xCC0000;
	g_optimized_color_table[1]  = 0x00CC00;
	g_optimized_color_table[2]  = 0x0000CC;
	g_optimized_color_table[3]  = 0xFF0099;
	g_optimized_color_table[4]  = 0x00FF99;
	g_optimized_color_table[5]  = 0x9900FF;
	g_optimized_color_table[6]  = 0xFF9900;
	g_optimized_color_table[7]  = 0x99FF00;
	g_optimized_color_table[8]  = 0x0099FF;
	g_optimized_color_table[9]  = 0xCC0099;
	g_optimized_color_table[10] = 0x00CC99;
	g_optimized_color_table[11] = 0x9900CC;
	g_optimized_color_table[12] = 0xCC9900;
	g_optimized_color_table[13] = 0x99CC00;
	g_optimized_color_table[14] = 0x0099CC;
	g_optimized_color_table[15] = 0x990099;

	/* init the display variables */
	g_buffered_video_flag = 0;
	g_current_draw_buffer = g_mandelrot_fb_map;
	g_current_display_buffer = g_mandelrot_fb_map;
}

void de10_release(void) {

	int result;

	result = munmap(g_spare_1_fb_map, FRAME_BUFFER_SPAN);
	assert(result == 0);

	result = munmap(g_spare_0_fb_map, FRAME_BUFFER_SPAN);
	assert(result == 0);

	result = munmap(g_default_fb_map, FRAME_BUFFER_SPAN);
	assert(result == 0);

	result = munmap(g_mandelrot_fb_map, FRAME_BUFFER_SPAN);
	assert(result == 0);

	result = munmap(g_lwh2f_map, LWH2F_SPAN);
	assert(result == 0);

	result = close(g_devmem_fd);
	assert(result == 0);
}

void set_frame_buffer_default(void) {

	assert(g_de10_init == 1);
	g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] = DEFAULT_FRAME_BUFFER_BASE;
}

void set_frame_buffer_mandelbrot(void) {

	assert(g_de10_init == 1);
	g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] = MANDELBROT_FRAME_BUFFER_BASE;
}

uint32_t get_fb_width(void) {

	assert(g_de10_init == 1);
	return(g_fb_column_count);
}

uint32_t get_fb_height(void) {

	assert(g_de10_init == 1);
	return(g_fb_row_count);
}

void clear_screen_to_black(void) {

	uint32_t i;
	uint32_t j;
	volatile uint32_t *typed_ptr;

	assert(g_de10_init == 1);

	typed_ptr = (uint32_t *)g_mandelrot_fb_map;
	for(i = 0 ; i < g_fb_row_count ; i++) {
		for(j = 0 ; j < g_fb_column_count ; j++) {
			*(typed_ptr + (i * g_fb_column_count) + j) = 0x00000000;
		}
	}

	typed_ptr = (uint32_t *)g_spare_0_fb_map;
	for(i = 0 ; i < g_fb_row_count ; i++) {
		for(j = 0 ; j < g_fb_column_count ; j++) {
			*(typed_ptr + (i * g_fb_column_count) + j) = 0x00000000;
		}
	}

	typed_ptr = (uint32_t *)g_spare_1_fb_map;
	for(i = 0 ; i < g_fb_row_count ; i++) {
		for(j = 0 ; j < g_fb_column_count ; j++) {
			*(typed_ptr + (i * g_fb_column_count) + j) = 0x00000000;
		}
	}
}

void set_pixel_fb(uint32_t pixmap_width, int y_coord,
					int x_coord, uint32_t value) {

	volatile uint32_t *typed_ptr = (uint32_t *)g_current_draw_buffer;

	assert(g_de10_init == 1);

	if(value == 0x00) {
		*(typed_ptr + (y_coord * pixmap_width) + x_coord) = 0x00000000;
	} else {
		*(typed_ptr + (y_coord * pixmap_width) + x_coord) =
					g_optimized_color_table[value & 0x0F];
	}
}

void *get_g_lwh2f_map(void) {

	assert(g_de10_init == 1);

	return(g_lwh2f_map);
}

void *get_g_mandelbrot_fb_map(void) {

	assert(g_de10_init == 1);

	return(g_mandelrot_fb_map);
}

void *get_g_spare_0_fb_map(void) {

	assert(g_de10_init == 1);

	return(g_spare_0_fb_map);
}

void *get_g_spare_1_fb_map(void) {

	assert(g_de10_init == 1);

	return(g_spare_1_fb_map);
}

void start_buffered_video(void) {

	// the frame buffer should be the mandelbrot frame buffer to begin with
	if(g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] !=
						MANDELBROT_FRAME_BUFFER_BASE) {
		assert(0);
	}

	// init the current display and draw buffer pointers
	g_current_display_buffer = g_mandelrot_fb_map;
	g_current_draw_buffer = g_spare_0_fb_map;

	// wait for the next frame ready
	while((g_fbr_base_ptr[FBR_FRM_RDR_REG] & FBR_FRM_RDR_READY_MSK) !=
							FBR_FRM_RDR_READY_MSK);
	
	// write the FBR start address init frame sync
	g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] = MANDELBROT_FRAME_BUFFER_BASE;

	g_buffered_video_flag = 1;
}

void stop_buffered_video(void) {

	assert(g_buffered_video_flag == 1);

	// copy the current display buffer to the mandelbrot buffer
	copy_pix_map_32_to_32(
		(char *)g_current_display_buffer,	// char *src_ptr,
		g_fb_column_count,			// long src_active_width,
		g_fb_row_count,				// long src_active_height,
		g_fb_column_count,			// long src_line_width,
		(char *)g_mandelrot_fb_map,		// char *dest_ptr,
		g_fb_column_count			// long dest_line_width
	);

	// wait for the next frame ready
	while((g_fbr_base_ptr[FBR_FRM_RDR_REG] & FBR_FRM_RDR_READY_MSK) !=
							FBR_FRM_RDR_READY_MSK);
	
	// write the FBR start address
	g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] = MANDELBROT_FRAME_BUFFER_BASE;

	g_buffered_video_flag = 0;
	g_current_draw_buffer = g_mandelrot_fb_map;
	g_current_display_buffer = g_mandelrot_fb_map;
}

void *get_next_draw_buffer(void) {
	
	if(g_buffered_video_flag == 0)
		return(g_mandelrot_fb_map);

	// if the next frame is ready
	if((g_fbr_base_ptr[FBR_FRM_RDR_REG] & FBR_FRM_RDR_READY_MSK) !=
							FBR_FRM_RDR_READY_MSK) {
		return(g_current_draw_buffer);
	}

	// sequence the buffers
	g_current_display_buffer = g_current_draw_buffer;
	if(g_current_draw_buffer == g_mandelrot_fb_map) {

		g_current_draw_buffer = g_spare_0_fb_map;

		// write the FBR start address
		g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] =
						MANDELBROT_FRAME_BUFFER_BASE;

	} else if(g_current_draw_buffer == g_spare_0_fb_map) {

		g_current_draw_buffer = g_spare_1_fb_map;

		// write the FBR start address
		g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] =
						SPARE_0_FRAME_BUFFER_BASE;

	} else if(g_current_draw_buffer == g_spare_1_fb_map) {

		g_current_draw_buffer = g_mandelrot_fb_map;

		// write the FBR start address
		g_fbr_base_ptr[FBR_FRM_STRT_ADDR_REG] =
						SPARE_1_FRAME_BUFFER_BASE;
	} else {
		assert(0);
	}

	return(g_current_draw_buffer);
}

