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

#ifndef __DE10_INIT_H__
#define __DE10_INIT_H__

/* LWH2F bridge */
#define LWH2F_BASE	(0xFF200000)
#define LWH2F_SPAN	(0x00200000)

/* FBR component */
#define FBR_BASE		(0x8000)
#define FBR_CNTL_REG		(0)
#define FBR_STAT_REG		(1)
#define FBR_INTR_REG		(2)
#define FBR_FRM_CNTR_REG	(3)
#define FBR_REP_CNTR_REG	(4)
#define FBR_FRM_INFO_REG	(5)
#define FBR_FRM_STRT_ADDR_REG	(6)
#define FBR_FRM_RDR_REG		(7)
#define FBR_MISC_REG		(8)

#define FBR_FRM_RDR_READY_MSK	(1 << 26)

/* Mandelbrot Frame Buffer */
#define DEFAULT_FRAME_BUFFER_BASE	(0x3F000000)
#define MANDELBROT_FRAME_BUFFER_BASE	(0x3F800000)
#define SPARE_0_FRAME_BUFFER_BASE	(0x3E000000)
#define SPARE_1_FRAME_BUFFER_BASE	(0x3E800000)
#define FRAME_BUFFER_SPAN		(0x00800000)

extern void de10_init(void);
void de10_release(void);
extern void set_frame_buffer_default(void);
extern void set_frame_buffer_mandelbrot(void);
extern uint32_t get_fb_width(void);
extern uint32_t get_fb_height(void);
extern void clear_screen_to_black(void);
extern void set_pixel_fb(uint32_t pixmap_width, int y_coord,
						int x_coord, uint32_t value);
extern void *get_g_lwh2f_map(void);
extern void copy_fb2user_buffer(uint32_t *dma_hw_results_buffer,
				uint32_t pixmap_width, uint32_t pixmap_height,
				uint32_t max_iter);
extern void *get_g_mandelbrot_fb_map(void);
extern void *get_g_spare_0_fb_map(void);
extern void *get_g_spare_1_fb_map(void);
extern void start_buffered_video(void);
extern void stop_buffered_video(void);
extern void *get_next_draw_buffer(void);

#endif // __DE10_INIT_H__

