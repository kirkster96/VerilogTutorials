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

#include <stdint.h>

#include "de10_init/de10_init.h"

static uint32_t g_mandelbrot_sw_colorize = 0;

// map the mandelbrot iteration count into a pixel value and set the pixel
void set_pixel_value(uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, int y_coord, int x_coord,
			uint32_t value) {

	uint32_t *typed_ptr = (uint32_t *)pixel_buf_ptr;

	if(g_mandelbrot_sw_colorize == 0) {
		*(typed_ptr + (y_coord * pixmap_width) + x_coord) = value;
	} else {
		if(value == in_max_iter) {
			// it's in the set
			set_pixel_fb(pixmap_width, y_coord, x_coord, 0);
		} else {
			// it's out of the set
			set_pixel_fb(pixmap_width, y_coord, x_coord, value);
		}
	}
}

void set_g_mandelbrot_sw_colorize(uint32_t new_colorize_value) {
	g_mandelbrot_sw_colorize = new_colorize_value;
}

