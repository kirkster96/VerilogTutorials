#!/bin/bash
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

# change into the directory containing this script
cd $(dirname ${0:?})

# change into the top level directory of the git repo tutorials path
cd $(git rev-parse --show-toplevel)/tutorials

# remove all PDF files
find -name "*.pdf" -exec rm \{\} \;

# build all PDF files
for NEXT in $(find -name "*.tex" | grep -v "common/latex")
do
	pushd $(dirname ${NEXT:?}) > /dev/null
	./build_tex.sh
	popd > /dev/null
done

# list the PDF files
find -name "*.pdf" -exec ls -l \{\} \;

