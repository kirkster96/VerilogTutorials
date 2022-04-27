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

#ifndef __MENU_SUPPORT_H__
#define __MENU_SUPPORT_H__

struct menu_item_struct {
	const char *name;
	const char *selector_str;
	void (*action)(void);
};

struct menu_struct {
	const char *menu_title;
	struct abc_font_struct *menu_font;
	struct menu_item_struct *menu_items;
};

extern void init_menu_support(void);
extern void start_menu(void (**return_action)(void));
extern void release_menu_support(void);
extern uint32_t get_g_menu_init(void);
extern void set_g_current_menu(struct menu_struct *new_menu);
extern struct menu_struct *get_g_current_menu(void);

#endif // __MENU_SUPPORT_H__

