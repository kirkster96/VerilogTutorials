#!/bin/sh

#
# Copyright (c) 2017 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#

[ -f devmem.c ] || {
	echo ""
	echo "Cannot locate the source file 'devmem.c'"
	echo ""
	echo "Try downloading the source file using these commands:"
	echo "git clone https://gfiber.googlesource.com/vendor/opensource/toolbox"
	echo "cp toolbox/devmem.c ."
	echo "rm -rf toolbox"
	echo ""
	exit 1
}

grep "devmem_main" devmem.c > /dev/null 2>&1 && {
	echo ""
	echo "Editing devmem.c to create main() function label."
	echo ""
	sed -i.orig "s/devmem_main/main/" devmem.c
}

# require CROSS_COMPILE on x86_64 development host
[ "x86_64" == "$(uname -m)" ] && {
	: ${CROSS_COMPILE:?}
}

${CROSS_COMPILE}gcc \
	-march=armv7-a \
	-mfloat-abi=hard \
	-mfpu=vfp3 \
	-mthumb-interwork \
	-mthumb \
	-O2 \
	-g \
	-feliminate-unused-debug-types \
	-std=gnu99 \
	-W \
	-Wall \
	-Werror \
	-Wc++-compat \
	-Wwrite-strings \
	-Wstrict-prototypes \
	-o devmem \
	devmem.c

