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

#ifndef __MANDELBROT_HW_ENGINE_H__
#define __MANDELBROT_HW_ENGINE_H__

#define MANDELBROT_CONTROLLER_BASE_OFST 0x100000
#define MANDELBROT_DISTRIBUTOR_BASE_OFST 0x100040
#define MANDELBROT_COLORIZER_0_BASE_OFST 0x100060
#define MANDELBROT_COLORIZER_1_BASE_OFST 0x100064
#define MANDELBROT_COLORIZER_2_BASE_OFST 0x100068
#define MANDELBROT_COLORIZER_3_BASE_OFST 0x10006c
#define MANDELBROT_COLORIZER_4_BASE_OFST 0x100070
#define MANDELBROT_COLORIZER_5_BASE_OFST 0x100074
#define MANDELBROT_COLORIZER_6_BASE_OFST 0x100078
#define MANDELBROT_COLORIZER_7_BASE_OFST 0x10007c
#define MANDELBROT_COLORIZER_8_BASE_OFST 0x100080
#define MANDELBROT_COLORIZER_9_BASE_OFST 0x100084
#define MANDELBROT_COLORIZER_10_BASE_OFST 0x100088
#define MANDELBROT_COLORIZER_11_BASE_OFST 0x10008c
#define MANDELBROT_INTERRUPT_PIO_BASE_OFST 0x100090

// MANDELBROT_CONTROLLER REGISTER MAP
#define MANDEL_CI		(0)
#define MANDEL_CR		(1)
#define MANDEL_MAX_ITERS	(2)
#define MANDEL_STEP_DIM		(3)
#define MANDEL_LINE_COUNT_SHIFT	(4)
#define MANDEL_PIX_BUF_PTR	(5)
#define MANDEL_PIX_MAP_WIDTH	(6)
#define MANDEL_PIX_MAP_HEIGHT	(7)
#define MANDEL_CONTROL		(8)
#define MANDEL_STATUS		(9)

// MANDELBROT_CONTROLLER - CONTROL REGISTER BIT MAP
#define MANDEL_CONTROL_GO_MASK		(0x01)

// MANDELBROT_CONTROLLER - STATUS REGISTER BIT MAP
#define MANDEL_STATUS_GO_MASK		(0x01)
#define MANDEL_STATUS_READY_O_MASK	(0x02)
#define MANDEL_STATUS_READY_1_MASK	(0x04)

// MANDELBROT_DISTRIBUTOR REGISTER MAP
#define MANDEL_DIST_ENABLE		(0)
#define MANDEL_DIST_FLUSH		(1)
#define MANDEL_DIST_INT_ENABLE		(2)
#define MANDEL_DIST_INT_CLEAR		(3)
#define MANDEL_DIST_INT_PENDING		(4)
#define MANDEL_DIST_PORT_STATE		(5)
#define MANDEL_DIST_LATCH_STATE		(6)

// MANDELBROT_DISTRIBUTOR - ENABLE REGISTER BIT MAP
#define MANDEL_DIST_ENABLE_MASK		(0xFFF)

// MANDELBROT_DISTRIBUTOR - FLUSH REGISTER BIT MAP
#define MANDEL_DIST_FLUSH_MASK		(0x1)

// MANDELBROT_DISTRIBUTOR - INT_ENABLE REGISTER BIT MAP
#define MANDEL_DIST_INT_ENABLE_MASK		(0x1)

// MANDELBROT_DISTRIBUTOR - INT_CLEAR REGISTER BIT MAP
#define MANDEL_DIST_INT_CLEAR_MASK		(0x1)

// MANDELBROT_DISTRIBUTOR - INT_PENDING REGISTER BIT MAP
#define MANDEL_DIST_INT_PENDING_MASK		(0x1)

// MANDELBROT_DISTRIBUTOR - PORT_STATE REGISTER BIT MAP
#define MANDEL_DIST_PORT_STATE_OUT_VECT_READY_MASK		(0x0FFF)
#define MANDEL_DIST_PORT_STATE_IN_VECT_VALID_MASK		(0x1000)

// MANDELBROT_DISTRIBUTOR - LATCH_STATE REGISTER BIT MAP
#define MANDEL_DIST_LATCH_STATE_OUT_VECT_READY_MASK		(0x0FFF)
#define MANDEL_DIST_LATCH_STATE_IN_VECT_VALID_MASK		(0x1000)

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
);

void color_dma_evaluate_coordinate(
	int64_t  cr,
	int64_t  ci,
	uint32_t max_iter,
	int64_t  step_dim,
	uint8_t  line_count_shift,
	uint32_t pix_buf_ptr,
	uint16_t pix_map_width,
	uint16_t pix_map_height,
	uint32_t hw_engine_count
);

void draw_frame_dma_hw_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height,
			uint32_t hw_engine_count);

void draw_color_frame_dma_hw_mandelbrot(float in_x, float in_y, float in_x_dim,
			uint32_t in_max_iter, void *pixel_buf_ptr,
			uint32_t pixmap_width, uint32_t pixmap_height,
			uint32_t hw_engine_count);

uint32_t get_g_mandelbrot_hw_engine_count(void);

void set_g_mandelbrot_hw_engine_count(uint32_t new_count);

#endif // __MANDELBROT_HW_ENGINE_H__

