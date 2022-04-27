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

LINUX_EXAMPLES_DIR="linux_examples"
ZIP_NAME="${LINUX_EXAMPLES_DIR:?}.zip"

# do not overwrite an existing archive
[ -f "${ZIP_NAME:?}" ] && {
	echo ""
	echo "ERROR: ZIP archive already exists"
	echo "'${ZIP_NAME:?}'"
	echo ""
	exit 1
}

#-------------------------------------------------------------------------------
# Remove any stale debris from prior builds
#-------------------------------------------------------------------------------
rm -rf ${LINUX_EXAMPLES_DIR:?} || { echo "ERROR" ; exit 1 ; }
mkdir ${LINUX_EXAMPLES_DIR:?} || { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Process the C examples directory
#-------------------------------------------------------------------------------
TAR_FROM_DIR=".."
TAR_DIR_NAME="c_examples"
TAR_NAME="${TAR_DIR_NAME:?}.tar"

ARCHIVE_FILES="
build_app.sh
build_devmem2.sh
build_devmem.sh
linux_blink_app.c
"

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
tar -xf "${TAR_NAME:?}" -C ${LINUX_EXAMPLES_DIR:?} \
|| { echo "ERROR" ; exit 1 ; }

rm -rf "${TAR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Process the sysfs examples directory
#-------------------------------------------------------------------------------
TAR_FROM_DIR=".."
TAR_DIR_NAME="sysfs_examples"
TAR_NAME="${TAR_DIR_NAME:?}.tar"

ARCHIVE_FILES="
read_button_pio_sysfs.sh
read_switch_pio_sysfs.sh
read_system_id_sysfs.sh
toggle_leds_sysfs.sh
"

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
tar -xf "${TAR_NAME:?}" -C ${LINUX_EXAMPLES_DIR:?} \
|| { echo "ERROR" ; exit 1 ; }

rm -rf "${TAR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Process the device tree overlay directory
#-------------------------------------------------------------------------------
TAR_FROM_DIR=".."
TAR_DIR_NAME="devicetree_overlay"
TAR_NAME="${TAR_DIR_NAME:?}.tar"

ARCHIVE_FILES="
soc_system.dtbo
soc_system.dtso
"

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
tar -xf "${TAR_NAME:?}" -C ${LINUX_EXAMPLES_DIR:?} \
|| { echo "ERROR" ; exit 1 ; }

rm -rf "${TAR_NAME:?}" || { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Create the final archive
#-------------------------------------------------------------------------------
# create the zip archive of the tar directory
zip -r "${ZIP_NAME:?}" "${LINUX_EXAMPLES_DIR:?}" || { echo "ERROR" ; exit 1 ; }

# remove the intermediates
rm -rf "${LINUX_EXAMPLES_DIR:?}" || { echo "ERROR" ; exit 1 ; }

exit 0

