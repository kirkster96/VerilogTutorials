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

#
# This script builds a font extraction script that uses GIMP to extract the font
# alpha channel into a usable C source format.  A script-fu script is passed
# into GIMP to render the output.
#
# The contents of this file are based on the Google Font 'Cousine'.
#
# The Google Font, 'Cousine', carries the following copyright:
#	Digitized data copyright (c) 2010-2012 Google Corporation.
#
# The Google Font 'Cousine' is licensed under the Apache License, Version 2.0:
#	https://github.com/google/fonts/raw/master/apache/cousine/LICENSE.txt
#
# For reference these URLs may be useful:
#	https://github.com/google/fonts/tree/master/apache/cousine
#	https://fonts.google.com/specimen/Cousine
#

EXTRACT_SCRIPT_NAME="extract_cousine.sh"
SCRIPTFU_SCRIPT="script-fu-extract_font_alpha.scm"
GIMP_CMD="gimp-2.8"
HOME_GIMP_SETTINGS_DIR="${HOME:?}/.gimp-2.8"
GIMP_FONTS_DIR="${HOME_GIMP_SETTINGS_DIR:?}/fonts"
GIMP_SCRIPTS_DIR="${HOME_GIMP_SETTINGS_DIR:?}/scripts"
GOOGLE_FONT_COUSINE_BASE_URL="https://github.com/google/fonts/raw/master/apache/cousine"
COUSINE_FONT_LIST_ARRAY=(
	"Cousine-Bold.ttf"
	"Cousine-BoldItalic.ttf"
	"Cousine-Italic.ttf"
	"Cousine-Regular.ttf"
)
FONT_FAMILY="Cousine"
declare -a FONT_STYLE_ARRAY
FONT_STYLE_ARRAY=(
	""
	"Bold"
	"Italic"
	"Bold Italic"
)
declare -a FONT_SIZE_ARRAY
FONT_SIZE_ARRAY=( 9 12 14 16 18 20 30 40 )

#
# change into the directory of this script
#
cd $(dirname ${0:?})
EXTRACTS_DIR="$(pwd)/font_extracts"

#
# verify that extract script does not exist
#
[ -f "${EXTRACT_SCRIPT_NAME:?}" ] && {
	echo ""
	echo "ERROR: extract script already exists"
	echo "'${EXTRACT_SCRIPT_NAME:?}'"
	echo ""
	exit 1
}

#
# verify that the script-fu script exists
#
[ -f "${SCRIPTFU_SCRIPT:?}" ] || {
	echo ""
	echo "ERROR: cannot locate script-fu script"
	echo "'${SCRIPTFU_SCRIPT:?}'"
	echo ""
	exit 1
}

#
#
#
[ -d "${EXTRACTS_DIR:?}" ] || {

	echo "Creating font extraction directory:"
	echo "'${EXTRACTS_DIR:?}'"

	mkdir "${EXTRACTS_DIR:?}"
}

#
# verify that the gimp settings directory exists in the users home directory
#
[ -d "${HOME_GIMP_SETTINGS_DIR:?}" ] || {
	echo ""
	echo "ERROR: cannot locate the gimp settings directory in your home directory"
	echo "'${HOME_GIMP_SETTINGS_DIR:?}'"
	echo ""
	echo "Do you have GIMP installed and have you run it once to establish a settings"
	echo "directory?"
	echo ""
	exit 1
}

#
# verify that the gimp settings directory contains a fonts directory
#
[ -d "${GIMP_FONTS_DIR:?}" ] || {
	echo ""
	echo "ERROR: cannot locate the gimp fonts settings directory"
	echo "'${GIMP_FONTS_DIR:?}'"
	echo ""
	echo "Do you have GIMP installed and have you run it once to establish a settings"
	echo "directory?"
	echo ""
	exit 1
}

#
# verify that the fonts we want are downloaded into the gimp fonts directory
#
for NEXT_FONT in "${COUSINE_FONT_LIST_ARRAY[@]:?}"
do
	[ -f "${GIMP_FONTS_DIR:?}/${NEXT_FONT:?}" ] || {

		echo "Downloading font: '${NEXT_FONT:?}'"

		#
		# set terminal text to green
		#
		echo -ne "\x1b[32m"

		wget \
		--directory-prefix="${GIMP_FONTS_DIR:?}" \
		"${GOOGLE_FONT_COUSINE_BASE_URL:?}/${NEXT_FONT:?}"

		#
		# set terminal text to default
		#
		echo -ne "\x1b[39m"
	}
done

#
# verify that the gimp settings directory contains a scripts directory
#
[ -d "${GIMP_SCRIPTS_DIR:?}" ] || {
	echo ""
	echo "ERROR: cannot locate the gimp scripts settings directory"
	echo "'${GIMP_SCRIPTS_DIR:?}'"
	echo ""
	echo "Do you have GIMP installed and have you run it once to establish a settings"
	echo "directory?"
	echo ""
	exit 1
}

#
# verify that the script-fu script is linked in the gimp scripts directory
#
[ -f "${GIMP_SCRIPTS_DIR:?}/${SCRIPTFU_SCRIPT:?}" ] || {
	echo ""
	echo "Creating link to script-fu script in gimp scripts setting directory"
	echo ""

	ln -s "$(pwd)/${SCRIPTFU_SCRIPT:?}" "${GIMP_SCRIPTS_DIR:?}/"
}

#
# generate font extraction script
#
echo "#!/bin/bash" > "${EXTRACT_SCRIPT_NAME:?}"

for NEXT_STYLE in "${FONT_STYLE_ARRAY[@]:?}"
do
	for NEXT_SIZE in "${FONT_SIZE_ARRAY[@]:?}"
	do
		echo "${GIMP_CMD:?} -i -c -b \\" >> "${EXTRACT_SCRIPT_NAME:?}"

		echo -n "'(${SCRIPTFU_SCRIPT%.scm} " \
			>> "${EXTRACT_SCRIPT_NAME:?}"

		[ -z "${NEXT_STYLE}" ] && {
			echo -n "\"${FONT_FAMILY:?}\" " \
				>> "${EXTRACT_SCRIPT_NAME:?}"
		} || {
			echo -n "\"${FONT_FAMILY:?} ${NEXT_STYLE:?}\" " \
				>> "${EXTRACT_SCRIPT_NAME:?}"
		}

		echo -n "${NEXT_SIZE:?} " >> "${EXTRACT_SCRIPT_NAME:?}"

		echo -n "\"${EXTRACTS_DIR:?}\") " >> "${EXTRACT_SCRIPT_NAME:?}"

		echo "(gimp-quit 0)'" >> "${EXTRACT_SCRIPT_NAME:?}"
	done
done

chmod +x "${EXTRACT_SCRIPT_NAME:?}"

echo "The font extraction script '${EXTRACT_SCRIPT_NAME:?}' has been generated."
echo "Execute that script to extract the font alpha channel into C source format."

