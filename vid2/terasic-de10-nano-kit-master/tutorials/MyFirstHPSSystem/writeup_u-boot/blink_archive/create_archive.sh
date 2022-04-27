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

TAR_FROM_DIR="../.."
TAR_DIR_NAME="blink"
TAR_NAME="${TAR_DIR_NAME:?}.tar"
ZIP_NAME="${TAR_DIR_NAME:?}.zip"

ARCHIVE_FILES="
output_files/blink.rbf
qsys_headers/hps_0_arm_a9_0.h
"

# do not overwrite an existing archive
[ -f "${ZIP_NAME:?}" ] && {
	echo ""
	echo "ERROR: ZIP archive already exists"
	echo "'${ZIP_NAME:?}'"
	echo ""
	exit 1
}

# make sure all source files are available
TAR_FILES=""
for FILE in ${ARCHIVE_FILES:?}
do
	FILE_PATH="${TAR_FROM_DIR:?}/${TAR_DIR_NAME:?}/${FILE:?}"
	[ -f "${FILE_PATH:?}" ] || {
		echo ""
		echo "ERROR: file not found"
		echo "'${FILE_PATH:?}'"
		echo ""
		exit 1
	}
	TAR_FILES="${TAR_FILES} ${FILE_PATH#${TAR_FROM_DIR}/}"
done

# create a tar archive to easily collect the source files and retain source tree
# structure
tar -cf "${TAR_NAME:?}" -C "${TAR_FROM_DIR:?}" ${TAR_FILES:?} \
|| { echo "ERROR" ; exit 1 ; }

# extract the tar archive in local directory
tar -xf "${TAR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

# create the zip archive of the tar directory
zip -r "${ZIP_NAME:?}" "${TAR_DIR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

# remove the intermediates
rm -rf "${TAR_DIR_NAME:?}" "${TAR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

#rename the zip archive
cp "${ZIP_NAME:?}" blink_for_uboot.zip

exit 0

