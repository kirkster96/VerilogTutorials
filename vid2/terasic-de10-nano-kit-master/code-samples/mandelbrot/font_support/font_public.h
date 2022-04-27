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

#ifndef FONT_CONTROL_H_
#define FONT_CONTROL_H_

#include <stdint.h>

// font related structures

struct abc_font_struct {
  unsigned long extents_width;
  unsigned long extents_height;
  unsigned long extents_ascent;
  unsigned long extents_descent;
  unsigned long bounds_width;
  unsigned long bounds_height;
  unsigned char *char_alpha_map;
  unsigned long reserved;
};

// Public entry points

extern uint32_t count_air_above(struct abc_font_struct *the_font);

extern void remove_air_from_font(struct abc_font_struct *the_font, int min_air);

extern void draw_string_line(
	struct abc_font_struct *theFontStruct,	// pointer to the font structure
	uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
	uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
	uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
	uint32_t theTextColor,		// this is the color of the text we draw 32-bit word is this format [0BGR]
	char *the_string,		// pointer to the string to draw
	uint32_t under_line,		// the horizontal line number above which the text will draw
	uint32_t start_x,		// the vertical column number that text begins to draw
	uint32_t max_width		// maximum horizontal width to draw
);

extern uint32_t get_string_line_width(
	struct abc_font_struct *theFontStruct,	// pointer to the font structure
	char *the_string			// pointer to the string to draw
);

#endif /*FONT_CONTROL_H_*/
