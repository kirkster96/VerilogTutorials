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

#-------------------------------------------------------------------------------
# Change into the directory of this script
#-------------------------------------------------------------------------------
cd $(dirname "${0:?}")

./clean_tex.sh || { echo "ERROR" ; exit 1 ; }

for TEX_FILE in *.tex
do
	[ -f "${TEX_FILE:?}" ] && {

		echo "Processing: '${TEX_FILE:?}'"
		pdflatex -halt-on-error -shell-escape "${TEX_FILE:?}" \
		> /dev/null || { echo "ERROR" ; exit 1 ; }

		while grep "Rerun to get cross-references right" \
			"${TEX_FILE%.tex}.log" > /dev/null 2>&1
		do
			echo "Reprocessing: '${TEX_FILE:?}'"
			pdflatex -halt-on-error -shell-escape "${TEX_FILE:?}" \
			> /dev/null || { echo "ERROR" ; exit 1 ; }
		done

		echo "Completed Processing: '${TEX_FILE:?}'"
	}
done

exit 0

