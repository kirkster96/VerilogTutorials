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
#include <math.h>

#include "movement_functions.h"
#include "de10_init/de10_init.h"

// this routine traverses from one coordinate to another
void do_fluid_move(double a_in_x, double a_in_y, double b_in_x, double b_in_y,
	void *pixel_buf_ptr, uint32_t pixmap_width, uint32_t pixmap_height) {

	double x_dim, center_x, center_y;
	int i, j, i_cur_max_iters;
	double f_cur_x_dim, f_cur_max_iters;
	double distance, x_len, y_len,next_x,next_y;

	// compute the x distance
	if(a_in_x > b_in_x) {
		x_len = -1 * fabs(a_in_x - b_in_x);
	} else {
		x_len = fabs(b_in_x - a_in_x);
	}

	// compute the y distance
	if(a_in_y > b_in_y) {
		y_len = -1 * fabs(a_in_y - b_in_y);
	} else {
		y_len = fabs(b_in_y - a_in_y);
	}

	// compute the straight line distance
	distance = sqrt(pow(x_len,2) + pow(y_len, 2));

	j=1;
	if(distance > 0.05) {
		while(distance > 0.05) {
			distance -= 0.05;
			j++;
		}

		y_len /= j;
		x_len /= j;

		next_x = a_in_x;
		next_y = a_in_y;
	} else {
		y_len = 0;
		x_len = 0;

		next_x = b_in_x;
		next_y = b_in_y;
	}

	// set the initial center point, x dimension, and maximum iterations
	center_x = next_x;
	center_y = next_y;

	// set initial zoom and iterations
	f_cur_x_dim = 4.25;
	f_cur_max_iters = 25;

	// move it
	for(i = 0 ; i < j ; i++) {
		//x_dim = cur_x_dim;
		x_dim = f_cur_x_dim;

		// convert f_cur_max_iters to an integer
		i_cur_max_iters = f_cur_max_iters;

		pixel_buf_ptr = get_next_draw_buffer();
		draw(center_x, center_y, x_dim, i_cur_max_iters, pixel_buf_ptr,
						pixmap_width, pixmap_height);

		if(get_g_draw_interrupted() == 1)
		return;

		// adjust to the next center point
		next_x += x_len;
		next_y += y_len;

		// set the next center point
		center_x = next_x;
		center_y = next_y;
	}
}

