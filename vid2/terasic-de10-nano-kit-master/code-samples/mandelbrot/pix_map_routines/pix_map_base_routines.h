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

#ifndef _PIX_MAP_BASE_ROUTINES_H_
#define _PIX_MAP_BASE_ROUTINES_H_

#define _8BPP  1
#define _16BPP 2
#define _24BPP 3
#define _32BPP 4

void copy_pix_map(
			char *src_base,
			long src_pixel_size,
			long src_active_width,
			long src_active_height,
			long src_line_width,
			char *dest_base,
			long dest_pixel_size,
			long dest_line_width);

void copy_pix_map_32_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_32_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_32_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_24_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_24_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_24_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_16_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_16_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_16_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void copy_pix_map_8_to_8(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_16_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_24_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_32_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_16_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_24_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_32_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_16_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_24_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_8_with_32_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_16_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_24_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_32_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_16_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_24_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_32_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_16_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_24_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void blend_32_with_32_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width);

void fill_pix_map_32(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color);

void fill_pix_map_24(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color);

void fill_pix_map_16(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color);

void fill_pix_map_8(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			char fill_alpha);

#endif //_PIX_MAP_BASE_ROUTINES_H_

