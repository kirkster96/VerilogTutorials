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

#ifndef __GPIO_SUPPORT_H__
#define __GPIO_SUPPORT_H__

#define KEY0_GPIO_NAME			"key0"
#define KEY1_GPIO_NAME			"key1"
#define MANDELBROT_INT_GPIO_NAME	"mandelbrot_int"

extern void init_gpio_support(void);
extern void release_gpio_support(void);
extern uint32_t read_gpio_instance(void *instance_ptr);
extern int get_gpio_instance(const char *name, void **gpio_instance);
extern void wait_for_gpio_interrupt(void *instance_ptr, pthread_mutex_t *ready_mutex);
extern int get_input_key(void);
extern int key1_still_down(int milliseconds);

#endif // __GPIO_SUPPORT_H__

