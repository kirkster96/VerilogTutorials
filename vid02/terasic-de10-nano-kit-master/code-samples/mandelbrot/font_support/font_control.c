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

#include "font_public.h"
#include "pix_map_routines/pix_map_base_routines.h"

#include <stdio.h>

uint32_t count_air_above(struct abc_font_struct *the_font) {

	int i;
	uint32_t min_count = 256;
	uint32_t row, col, next_alpha = 0;

	for(i = 33 ; i < 127 ; i++) {
		for(row = 0 ; row < the_font[i - 33].bounds_height ; row++) {
			for(col = 0 ; col < the_font[i - 33].bounds_width ;
									col++) {

				next_alpha =
					the_font[i - 33].char_alpha_map[col +
					(row * the_font[i - 33].bounds_width)];

				if(next_alpha)
					break;
			}
			if(next_alpha)
				break;
		}
		if(row < min_count)
			min_count = row;
	}
	return(min_count);
}

void remove_air_from_font(struct abc_font_struct *the_font, int min_air) {

	int i;

	for(i = 33 ; i < 127 ; i++) {
		the_font[i - 33].char_alpha_map =
			the_font[i - 33].char_alpha_map +
			(the_font[i - 33].bounds_width * (min_air >> 1));
		the_font[i - 33].bounds_height =
			the_font[i - 33].bounds_height - (min_air >> 1);
		the_font[i - 33].extents_height =
			the_font[i - 33].extents_height - (min_air >> 1);
	}
}

void draw_string_line(
	struct abc_font_struct *theFontStruct,	// pointer to the font structure
	uint32_t *theDrawBuffer,	// this is the buffer to draw the font into
	uint32_t theDrawBufferWidth,	// this is the pixel line width of the draw buffer
	uint32_t *theBlendBuffer,	// this is the background blend buffer 64X64 32-bit words fill it with the background color 32-bit pixels RGBA format
	uint32_t theTextColor,		// this is the color of the text we draw 32-bit word is this format [0BGR]
	char *the_string,		// pointer to the string to draw
	uint32_t under_line,		// the horizontal line number above which the text will draw
	uint32_t start_x,		// the vertical column number that text begins to draw
	uint32_t max_width		// maximum horizontal width to draw
) {

	uint32_t x_offset, y_offset, pix_offset;
	uint32_t cur_draw_width;
	uint8_t next_char;

	x_offset = start_x;
	y_offset = under_line;
	cur_draw_width = 0;

	while((next_char = *the_string++) != '\0') {
		if(next_char < 33) {
			if(next_char == ' ') {
				cur_draw_width +=
					theFontStruct['I' - 33].bounds_width;
				if(cur_draw_width >= max_width)
					break;

				pix_offset = x_offset +
					(theDrawBufferWidth *
					(y_offset -
					theFontStruct['I' - 33].bounds_height));
				copy_pix_map_32_to_32(
					(char *)theBlendBuffer,			// void *src_ptr,
					theFontStruct['I' - 33].bounds_width,	// long src_active_width,
					theFontStruct['I' - 33].bounds_height,	// long src_active_height,
					theFontStruct['I' - 33].bounds_width,	// long src_line_width,
					(char *)(theDrawBuffer + pix_offset),	// void *dest_ptr,
					theDrawBufferWidth			// long dest_line_width
				);

				x_offset +=
					theFontStruct['I' - 33].bounds_width;
			}
		} else {
			cur_draw_width +=
				theFontStruct[next_char - 33].bounds_width;
			if(cur_draw_width >= max_width)
				break;

			pix_offset = x_offset +
				(theDrawBufferWidth *
				(y_offset -
				theFontStruct[next_char - 33].bounds_height));

			blend_8_with_32_to_32(
				(char *)(theFontStruct[next_char - 33].
							char_alpha_map),	// void *alpha_ptr,
				theFontStruct[next_char - 33].bounds_width,	// long alpha_active_width,
				theFontStruct[next_char - 33].bounds_height,	// long alpha_active_height,
				theTextColor,					// long alpha_color,
				theFontStruct[next_char - 33].bounds_width,	// long alpha_line_width,
				(char *)theBlendBuffer,				// void *src_ptr,
				64,						// long src_line_width,
				(char *)(theDrawBuffer + pix_offset),		// void *dest_ptr,
				theDrawBufferWidth				// long dest_line_width
			);

			x_offset += theFontStruct[next_char - 33].bounds_width;
		}
	}
}

uint32_t get_string_line_width(
	struct abc_font_struct *theFontStruct,	// pointer to the font structure
	char *the_string			// pointer to the string to draw
) {
	uint32_t cur_draw_width;
	uint8_t next_char;

	cur_draw_width = 0;

	while((next_char = *the_string++) != '\0') {
		if(next_char < 33) {
			if(next_char == ' ') {
				cur_draw_width +=
					theFontStruct['I' - 33].bounds_width;
			}
		} else {
			cur_draw_width +=
				theFontStruct[next_char - 33].bounds_width;
		}
	}
	return(cur_draw_width);
}

