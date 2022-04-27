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

// this routine zooms into a coordinate that is passed into it as the center
// point of the image
void do_fluid_zoom_in(double in_x, double in_y, double in_x_dim,
	int in_max_iter, void *pixel_buf_ptr, uint32_t pixmap_width,
	uint32_t pixmap_height) {

	int i, j;
	double x_dim, center_x, center_y, step_dim, last_step_dim;
	double f_cur_x_dim;
	double f_max_iter_step, f_cur_max_iters;
	int i_cur_max_iters;

	// set the initial center point, x dimension, and maximum iterations
	center_x = in_x;
	center_y = in_y;

	// set initial zoom and iterations
	f_cur_x_dim = 4.25;
	f_cur_max_iters = 25;

	// determine how many 2x zooms it takes to reach the final zoom point
	j = 1;
	while (f_cur_x_dim > in_x_dim) {
		f_cur_x_dim /= 1.1;
		j++;
	}

	// reset the zoom
	f_cur_x_dim = 4.25;
	f_max_iter_step = (in_max_iter - f_cur_max_iters) / j;

	// zoom in
	last_step_dim = 0;
	for(i = 0 ; i < j ; i++) {
		x_dim = f_cur_x_dim;

		// calculate the step dimension between each pixel
		step_dim = x_dim / pixmap_width;

		// make sure that we are taking a visible step,
		// if step_dim == last_step_dim, then we won't see this step
		if(step_dim == last_step_dim) {
			// increase the iterations for more resolution on the
			// next step
			f_cur_max_iters += f_max_iter_step;

			// zoom in 2x
			f_cur_x_dim /= 1.1;

			continue;
		}

		last_step_dim = step_dim;

		// convert f_cur_max_iters to an integer
		i_cur_max_iters = f_cur_max_iters;


		pixel_buf_ptr = get_next_draw_buffer();
		draw(center_x, center_y, x_dim, i_cur_max_iters, pixel_buf_ptr,
		pixmap_width, pixmap_height);

		if(get_g_draw_interrupted() == 1)
			return;

		// increase the iterations for more resolution on the next step
		f_cur_max_iters += f_max_iter_step;

		// zoom in 2x
		f_cur_x_dim /= 1.1;
	}

	// zoom out
	last_step_dim = 0;
	for(i = 0 ; i < j ; i++) {
		x_dim = f_cur_x_dim;

		// calculate the step dimension between each pixel
		step_dim = x_dim / pixmap_width;

		// make sure that we are taking a visible step,
		// if step_dim == last_step_dim, then we won't see this step
		if(step_dim == last_step_dim) {
			// increase the iterations for more resolution on the
			// next step
			f_cur_max_iters -= f_max_iter_step;

			// zoom in 2x
			f_cur_x_dim *= 1.1;

			continue;
		}

		last_step_dim = step_dim;

		// convert f_cur_max_iters to an integer
		i_cur_max_iters = f_cur_max_iters;

		pixel_buf_ptr = get_next_draw_buffer();
		draw(center_x, center_y, x_dim, i_cur_max_iters, pixel_buf_ptr,
						pixmap_width, pixmap_height);

		if(get_g_draw_interrupted() == 1)
		return;

		// increase the iterations for more resolution on the next step
		f_cur_max_iters -= f_max_iter_step;

		// zoom in 2x
		f_cur_x_dim *= 1.1;
	}
}

