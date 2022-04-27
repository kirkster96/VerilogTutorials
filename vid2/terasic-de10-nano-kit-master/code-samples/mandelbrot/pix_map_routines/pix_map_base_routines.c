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

#include "pix_map_base_routines.h"


void copy_pix_map(
			char *src_ptr,
			long src_pixel_size,
			long src_active_width,
			long src_active_height,
			long src_line_width,
			char *dest_ptr,
			long dest_pixel_size,
			long dest_line_width) {
	switch(src_pixel_size) {
	case(_16BPP) :
		switch(dest_pixel_size) {
		case(_16BPP) :
		copy_pix_map_16_to_16(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_24BPP) :
		copy_pix_map_16_to_24(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_32BPP) :
		copy_pix_map_16_to_32(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		}
		break;
	case(_24BPP) :
		switch(dest_pixel_size) {
		case(_16BPP) :
		copy_pix_map_24_to_16(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_24BPP) :
		copy_pix_map_24_to_24(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_32BPP) :
		copy_pix_map_24_to_32(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		}
		break;
	case(_32BPP) :
		switch(dest_pixel_size) {
		case(_16BPP) :
		copy_pix_map_32_to_16(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_24BPP) :
		copy_pix_map_32_to_24(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		case(_32BPP) :
		copy_pix_map_32_to_32(src_ptr, src_active_width,
					src_active_height, src_line_width,
					dest_ptr, dest_line_width);
		break;
		}
		break;
	}
}

void copy_pix_map_32_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for( src_col = 0 ; src_col < src_active_width ; src_col++ ) {
			*(unsigned long *)(dest_line_ptr) =
					*(unsigned long *)(src_line_ptr);
			src_line_ptr += 4;
			dest_line_ptr += 4;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_32_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for( src_col = 0 ; src_col < src_active_width ; src_col++ ) {
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 2;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_32_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	unsigned long next_long_pixel;
	unsigned short next_short_pixel;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for( src_col = 0 ; src_col < src_active_width ; src_col++) {
			next_long_pixel = *(unsigned long *)(src_line_ptr);
			src_line_ptr += 4;

			next_short_pixel =
				(((next_long_pixel >> 3) & 0x1f) << 11) |
				(((next_long_pixel >> 10) & 0x3f) << 5) |
				(((next_long_pixel >> 19) & 0x1f) << 0) ;

			*(unsigned short *)(dest_line_ptr) = next_short_pixel;
			dest_line_ptr += 2;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_24_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					*(unsigned char *)(src_line_ptr);
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) = 0xff;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_24_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			*(unsigned char *)(dest_line_ptr) =
					(*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_24_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width ) {

	int src_row, src_col;
	unsigned short next_short_pixel;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			next_short_pixel =
			((*(unsigned char *)(src_line_ptr)) & 0xf8) << 8;

			src_line_ptr += 1;
			next_short_pixel |=
			((*(unsigned char *)(src_line_ptr)) & 0xfc) << 3;

			src_line_ptr += 1;
			next_short_pixel |=
			((*(unsigned char *)(src_line_ptr)) & 0xf8) >> 3;
			src_line_ptr += 1;

			*(unsigned short *)(dest_line_ptr) = next_short_pixel;
			dest_line_ptr += 2;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_16_to_32(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	unsigned short next_short_pixel;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			next_short_pixel = *(unsigned short *)(src_line_ptr);
			src_line_ptr += 2;

			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel >> 8) & 0x000000f8;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel >> 3) & 0x000000fc;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel << 3) & 0x000000f8;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) = 0xff;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_16_to_24(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	unsigned short next_short_pixel;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			next_short_pixel = *(unsigned short *)(src_line_ptr);
			src_line_ptr += 2;

			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel >> 8) & 0x000000f8;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel >> 3) & 0x000000fc;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) =
					(next_short_pixel << 3) & 0x000000f8;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_16_to_16(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			*(unsigned short *)(dest_line_ptr) =
					*(unsigned short *)(src_line_ptr);
			src_line_ptr += 2;
			dest_line_ptr += 2;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void copy_pix_map_8_to_8(
				char *src_ptr,
				long src_active_width,
				long src_active_height,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width ) {

	int src_row, src_col;
	char *src_line_ptr, *dest_line_ptr;
	long src_increment, dest_increment;

	src_increment = src_line_width;
	dest_increment = dest_line_width;

	for(src_row = 0 ; src_row < src_active_height ; src_row++) {
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < src_active_width ; src_col++) {
			*(unsigned char *)(dest_line_ptr) =
					(*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			dest_line_ptr += 1;
		}
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_16_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width ) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				src_red = ((src_pixel >> 11) & 0x1f) << 3;
				src_green = ((src_pixel >> 5) & 0x3f) << 2;
				src_blue = ((src_pixel >> 0) & 0x1f) << 3;

				dest_red = ((next_alpha *
						(alpha_red - src_red))
						>> 8 ) + src_red;
				dest_green = ((next_alpha *
						(alpha_green - src_green))
						>> 8) + src_green;
				dest_blue = ((next_alpha *
						(alpha_blue - src_blue))
						>> 8) + src_blue;

				*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11) |
					(((dest_green >> 2) & 0x3f) << 5) |
					(((dest_blue >> 3) & 0x1f) << 0);
			} else {
				*(unsigned short *)(dest_line_ptr) = src_pixel;
			}

			alpha_line_ptr += 1;
			src_line_ptr += 2;
			dest_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_16_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);
			src_red = ((src_pixel >> 11) & 0x1f) << 3;
			src_green = ((src_pixel >> 5) & 0x3f) << 2;
			src_blue = ((src_pixel >> 0) & 0x1f) << 3;

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
			src_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_16_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);
			src_red = ( ( src_pixel >> 11 ) & 0x1f ) << 3;
			src_green = ( ( src_pixel >> 5 ) & 0x3f ) << 2;
			src_blue = ( ( src_pixel >> 0 ) & 0x1f ) << 3;

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
			src_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_24_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11) |
					(((dest_green >> 2) & 0x3f) << 5) |
					(((dest_blue >> 3) & 0x1f) << 0);
			} else {
				*(unsigned short *)(dest_line_ptr) =
					(((src_red >> 3) & 0x1f) << 11) |
					(((src_green >> 2) & 0x3f) << 5) |
					(((src_blue >> 3) & 0x1f) << 0);
			}

			alpha_line_ptr += 1;
			dest_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_24_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_24_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;

			next_alpha = (*(unsigned char *)(alpha_line_ptr));

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_32_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			next_alpha = (*(unsigned char *)(alpha_line_ptr));
			src_line_ptr += 1;

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11 ) |
					(((dest_green >> 2 ) & 0x3f) << 5 ) |
					(((dest_blue >> 3 ) & 0x1f) << 0 );
			} else {
				*(unsigned short *)(dest_line_ptr) =
					(((src_red >> 3) & 0x1f) << 11) |
					(((src_green >> 2) & 0x3f) << 5) |
					(((src_blue >> 3) & 0x1f) << 0);
			}

			alpha_line_ptr += 1;
			dest_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_32_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			next_alpha = (*(unsigned char *)(alpha_line_ptr));
			src_line_ptr += 1;

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_8_with_32_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_color,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_red = (alpha_color >> 0) & 0x000000ff;
	alpha_green = (alpha_color >> 8) & 0x000000ff;
	alpha_blue = (alpha_color >> 16) & 0x000000ff;

	alpha_increment = alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			next_alpha = (*(unsigned char *)(alpha_line_ptr));
			src_line_ptr += 1;

			if(next_alpha) {
				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) =src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			}
			alpha_line_ptr += 1;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_16_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				src_red = ( ( src_pixel >> 11 ) & 0x1f ) << 3;
				src_green = ( ( src_pixel >> 5 ) & 0x3f ) << 2;
				src_blue = ( ( src_pixel >> 0 ) & 0x1f ) << 3;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11) |
					(((dest_green >> 2) & 0x3f) << 5) |
					(((dest_blue >> 3) & 0x1f) << 0);
			} else {
				*(unsigned short *)(dest_line_ptr) = src_pixel;

				alpha_line_ptr += 4;
			}

			src_line_ptr += 2;
			dest_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_16_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);
			src_red = ( ( src_pixel >> 11 ) & 0x1f ) << 3;
			src_green = ( ( src_pixel >> 5 ) & 0x3f ) << 2;
			src_blue = ( ( src_pixel >> 0 ) & 0x1f ) << 3;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
			src_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_16_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;
	unsigned short src_pixel;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_pixel = *(unsigned short *)(src_line_ptr);
			src_red = ( ( src_pixel >> 11 ) & 0x1f ) << 3;
			src_green = ( ( src_pixel >> 5 ) & 0x3f ) << 2;
			src_blue = ( ( src_pixel >> 0 ) & 0x1f ) << 3;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
			src_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_24_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
	alpha_line_ptr = alpha_ptr;
	src_line_ptr = src_ptr;
	dest_line_ptr = dest_ptr;
	for( col = 0 ; col < alpha_active_width ; col++ )
	{
	src_red = (*(unsigned char *)(src_line_ptr));
	src_line_ptr += 1;
	src_green = (*(unsigned char *)(src_line_ptr));
	src_line_ptr += 1;
	src_blue = (*(unsigned char *)(src_line_ptr));
	src_line_ptr += 1;

	next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

	if(next_alpha) {
		alpha_red = (*(unsigned char *)(alpha_line_ptr));
		alpha_line_ptr += 1;
		alpha_green = (*(unsigned char *)(alpha_line_ptr));
		alpha_line_ptr += 1;
		alpha_blue = (*(unsigned char *)(alpha_line_ptr));
		alpha_line_ptr += 2;

		dest_red = ((next_alpha *
				(alpha_red - src_red)) >> 8) + src_red;
		dest_green = ((next_alpha *
				(alpha_green - src_green)) >> 8) + src_green;
		dest_blue = ((next_alpha *
				(alpha_blue - src_blue)) >> 8) + src_blue;

		*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11) |
					(((dest_green >> 2) & 0x3f) << 5) |
					(((dest_blue >> 3) & 0x1f) << 0);
	} else {
		*(unsigned short *)(dest_line_ptr) =
					(((src_red >> 3) & 0x1f) << 11) |
					(((src_green >> 2) & 0x3f) << 5) |
					(((src_blue >> 3) & 0x1f) << 0);
		alpha_line_ptr += 4;
	}

	dest_line_ptr += 2;
	}
	alpha_ptr += alpha_increment;
	src_ptr += src_increment;
	dest_ptr += dest_increment;
	}
}

void blend_32_with_24_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_24_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_32_to_16(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 2;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned short *)(dest_line_ptr) =
					(((dest_red >> 3) & 0x1f) << 11 ) |
					(((dest_green >> 2) & 0x3f) << 5) |
					(((dest_blue >> 3) & 0x1f) << 0);
			} else {
				*(unsigned short *)(dest_line_ptr) =
					(((src_red >> 3) & 0x1f) << 11) |
					(((src_green >> 2) & 0x3f) << 5) |
					(((src_blue >> 3) & 0x1f) << 0);
				alpha_line_ptr += 4;
			}

			dest_line_ptr += 2;
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_32_to_24(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 2;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
			}
			else
			{
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void blend_32_with_32_to_32(
				char *alpha_ptr,
				long alpha_active_width,
				long alpha_active_height,
				long alpha_line_width,
				char *src_ptr,
				long src_line_width,
				char *dest_ptr,
				long dest_line_width) {

	int row, col;
	char *alpha_line_ptr, *src_line_ptr, *dest_line_ptr;
	long alpha_increment, src_increment, dest_increment;
	unsigned char alpha_red, alpha_green, alpha_blue;
	unsigned char src_red, src_green, src_blue;
	unsigned char dest_red, dest_green, dest_blue;
	unsigned char next_alpha;

	alpha_increment = alpha_line_width + alpha_line_width +
					alpha_line_width + alpha_line_width;
	src_increment = src_line_width + src_line_width + src_line_width +
								src_line_width;
	dest_increment = dest_line_width + dest_line_width + dest_line_width +
								dest_line_width;

	for(row = 0 ; row < alpha_active_height ; row++) {
		alpha_line_ptr = alpha_ptr;
		src_line_ptr = src_ptr;
		dest_line_ptr = dest_ptr;
		for(col = 0 ; col < alpha_active_width ; col++) {
			src_red = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_green = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 1;
			src_blue = (*(unsigned char *)(src_line_ptr));
			src_line_ptr += 2;

			next_alpha = (*(unsigned char *)(alpha_line_ptr + 3));

			if(next_alpha) {
				alpha_red =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_green =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 1;
				alpha_blue =
					(*(unsigned char *)(alpha_line_ptr));
				alpha_line_ptr += 2;

				dest_red = ((next_alpha *
					(alpha_red - src_red)) >> 8) +
					src_red;
				dest_green = ((next_alpha *
					(alpha_green - src_green)) >> 8) +
					src_green;
				dest_blue = ((next_alpha *
					(alpha_blue - src_blue)) >> 8) +
					src_blue;

				*(unsigned char *)(dest_line_ptr) = dest_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = dest_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;
			} else {
				*(unsigned char *)(dest_line_ptr) = src_red;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_green;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = src_blue;
				dest_line_ptr += 1;
				*(unsigned char *)(dest_line_ptr) = 0xff;
				dest_line_ptr += 1;

				alpha_line_ptr += 4;
			}
		}
		alpha_ptr += alpha_increment;
		src_ptr += src_increment;
		dest_ptr += dest_increment;
	}
}

void fill_pix_map_32(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color) {

	int src_row, src_col;
	char *dest_line_ptr;
	long dest_increment;

	dest_increment = dest_line_width + dest_line_width + dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < dest_active_height ; src_row++) {
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < dest_active_width ; src_col++) {
			*(unsigned long *)(dest_line_ptr) = fill_color;
			dest_line_ptr += 4;
		}
		dest_ptr += dest_increment;
	}
}

void fill_pix_map_24(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color) {

	int src_row, src_col;
	char *dest_line_ptr;
	long dest_increment;
	unsigned char fill_red, fill_green, fill_blue;

	fill_red = ( fill_color >> 0 ) & 0x000000ff;
	fill_green = ( fill_color >> 8 ) & 0x000000ff;
	fill_blue = ( fill_color >> 16 ) & 0x000000ff;

	dest_increment = dest_line_width + dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < dest_active_height ; src_row++) {
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < dest_active_width ; src_col++) {
			*(unsigned char *)(dest_line_ptr) = fill_red;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) = fill_green;
			dest_line_ptr += 1;
			*(unsigned char *)(dest_line_ptr) = fill_blue;
			dest_line_ptr += 1;
		}
		dest_ptr += dest_increment;
	}
}

void fill_pix_map_16(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			long fill_color) {

	int src_row, src_col;
	char *dest_line_ptr;
	long dest_increment;
	unsigned short fill_16_color;

	fill_16_color = (((fill_color >> 3) & 0x1f) << 11) |
				(((fill_color >> 10) & 0x3f) << 5) |
				(((fill_color >> 19) & 0x1f) << 0) ;

	dest_increment = dest_line_width + dest_line_width;

	for(src_row = 0 ; src_row < dest_active_height ; src_row++) {
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < dest_active_width ; src_col++) {
			*(unsigned short *)(dest_line_ptr) = fill_16_color;
			dest_line_ptr += 2;
		}
		dest_ptr += dest_increment;
	}
}

void fill_pix_map_8(
			char *dest_ptr,
			long dest_active_width,
			long dest_active_height,
			long dest_line_width,
			char fill_alpha) {

	int src_row, src_col;
	char *dest_line_ptr;
	long dest_increment;

	dest_increment = dest_line_width;

	for(src_row = 0 ; src_row < dest_active_height ; src_row++) {
		dest_line_ptr = dest_ptr;
		for(src_col = 0 ; src_col < dest_active_width ; src_col++) {
			*(unsigned char *)(dest_line_ptr) = fill_alpha;
			dest_line_ptr += 1;
		}
		dest_ptr += dest_increment;
	}
}

