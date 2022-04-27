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

#ifndef MOVEMENT_FUNCTIONS_H_
#define MOVEMENT_FUNCTIONS_H_

#define NUMBER_OF_COORDINATES	(13)
#define BANNER_WIDTH_AIR	(20)
#define FPS_STR_LEN		(11)
#define CPU_STR_LEN		(21)

extern pthread_mutex_t g_fps_mutex;
extern pthread_cond_t g_fps_cond;
extern uint32_t g_fps_count;

extern void do_fluid_continuous_demo(void);
extern void do_fluid_zoom_in(double in_x, double in_y, double in_x_dim,
	int in_max_iter, void *pixel_buf_ptr, uint32_t pixmap_width,
	uint32_t pixmap_height);
extern void do_fluid_move(double a_in_x, double a_in_y, double b_in_x, double b_in_y,
	void *pixel_buf_ptr, uint32_t pixmap_width, uint32_t pixmap_height);
extern void draw(double center_x, double center_y, double x_dim,
	int i_cur_max_iters, void *pixel_buf_ptr, uint32_t pixmap_width,
	uint32_t pixmap_height);

extern float calculate_cpu_load(void);
extern void init_cpu_load(void);

extern void set_g_draw_with_hw(uint32_t new_draw_with_hw);
extern uint32_t get_g_draw_with_hw(void);
extern void set_g_draw_interrupted(uint32_t new_draw_interrupt);
extern uint32_t get_g_draw_interrupted(void);
extern void set_g_latest_fps_value(float new_value);

extern char *get_g_crawl_algo_str_ptr(void);
extern char *get_g_crawl_hw_str_ptr(void);
extern char *get_g_crawl_sw_str_ptr(void);
extern void set_g_crawl_algo_str_ptr(char *new_str_ptr);
extern void set_g_crawl_hw_str_ptr(char *new_str_ptr);
extern void set_g_crawl_sw_str_ptr(char *new_str_ptr);
extern void set_g_sw_algorithm(void (*new_sw_algorithm)(float, float, float,
					uint32_t, void *, uint32_t, uint32_t));
extern uint32_t get_g_banner_height(void);
extern uint32_t get_g_banner_start_pixel(void);
extern uint32_t get_g_buffered_video_mode(void);
extern void set_g_buffered_video_mode(uint32_t new_mode);

#endif /*MOVEMENT_FUNCTIONS_H_*/

