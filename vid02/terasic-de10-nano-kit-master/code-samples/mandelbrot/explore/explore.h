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

#ifndef __EXPLORE_H__
#define __EXPLORE_H__

/*
  Format of fixed point coordinate representation.

   BIT POS - FRACTIONAL WEIGHT
	31 - Sign
	30 - 1<<2    - 4
	29 - 1<<1    - 2
	28 - 1<<0    - 1
	27 - 1/1<<1  - 1/2
	26 - 1/1<<2  - 1/4
	25 - 1/1<<3  - 1/8
	24 - 1/1<<4  - 1/16
	23 - 1/1<<5  - 1/32
	22 - 1/1<<6  - 1/64
	21 - 1/1<<7  - 1/128
	20 - 1/1<<8  - 1/256
	19 - 1/1<<9  - 1/512
	18 - 1/1<<10 - 1/1024
	17 - 1/1<<11 - 1/2048
	16 - 1/1<<12 - 1/4096
	15 - 1/1<<13 - 1/8192
	14 - 1/1<<14 - 1/16384
	13 - 1/1<<15 - 1/32768
	12 - 1/1<<16 - 1/65536
	11 - 1/1<<17 - 1/131072
	10 - 1/1<<18 - 1/262144
	 9 - 1/1<<19 - 1/524288
	 8 - 1/1<<20 - 1/1048576
	 7 - 1/1<<21 - 1/2097152
	 6 - 1/1<<22 - 1/4194304
	 5 - 1/1<<23 - 1/8388608
	 4 - 1/1<<24 - 1/16777216
	 3 - 1/1<<25 - 1/33554432
	 2 - 1/1<<26 - 1/67108864
	 1 - 1/1<<27 - 1/134217728
	 0 - 1/1<<28 - 1/268435456
 */

#define EBANNER_WIDTH_AIR	(20)
#define COORD_STR_LEN		(32)
#define ZOOM_STR_LEN		(27)
#define CURSOR_WIDTH		(16)
#define CURSOR_HEIGHT		(16)

extern void do_explore(void);
extern void set_new_max_iters(int new_max_iters);
extern void set_new_zoom_direction(int new_zoom);
extern void menu_explore_start(void);
extern void menu_explore_return(void);
extern int check_exit_explore_mode(void);
extern void do_change_iterations(void);
extern void do_change_zoom_direction(void);
extern void about_explore_print(void);
extern void do_current_explore_print(void);
extern void explore_console_print(void);

#endif // __EXPLORE_H__

