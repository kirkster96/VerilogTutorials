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

#-------------------------------------------------------------------------------
# Change into the directory of this script
#-------------------------------------------------------------------------------
cd $(dirname "${0:?}")

#-------------------------------------------------------------------------------
# Verify that the repo is completely clean
#-------------------------------------------------------------------------------
[ -z "$(git status -s)" ] || {

	echo ""
	echo "ERROR: Git repo is not clean, it contains staged or untracked files"
	echo ""
	exit 1
}

#-------------------------------------------------------------------------------
# Verify tool requirements
#-------------------------------------------------------------------------------
type -P \
	quartus_sh \
	quartus_cpf \
	quartus_map \
	sopc-create-header-files \
	qsys-generate \
	qsys-script \
	${SOCEDS_DEST_ROOT:?}/host_tools/gnu/dtc/dtc\
	pdflatex \
	grep \
	tar \
	zip \
> /dev/null || {
	echo ""
	echo "ERROR: could not locate all required tools in environment"
	echo ""
	exit 1
}

#-------------------------------------------------------------------------------
# Display the tools environment
#-------------------------------------------------------------------------------
cat << EOF

This example was tested with this version of the Quartus Prime software:

Quartus Prime Shell
Version 17.0.0 Build 595 04/25/2017 SJ Standard Edition
Copyright (C) 2017  Intel Corporation. All rights reserved.

If your tools are not from this version you may experience build issues related
to those version differences.

Your tool version appears to be:

EOF

quartus_sh --version || { echo "ERROR" ; exit 1 ; }

echo

#
# setup a build log directory
#
BUILD_LOGS="$(pwd)/build_logs"

[ -d "${BUILD_LOGS:?}" ] || {
	mkdir -p "${BUILD_LOGS:?}"
}

#-------------------------------------------------------------------------------
# Build the example designs in parallel
#-------------------------------------------------------------------------------
EXIT_VALUE=0

./MyFirstFPGA/build_example.sh \
> "${BUILD_LOGS:?}"/MyFirstFPGA_log.txt 2>&1 &
MyFirstFPGA_PID="$!"
echo "Build starting: MyFirstFPGA"

./MyFirstQsysSystem/build_example.sh \
> "${BUILD_LOGS:?}"/MyFirstQsysSystem_log.txt 2>&1 &
MyFirstQsysSystem_PID="$!"
echo "Build starting: MyFirstQsysSystem"

./MyFirstHPSSystem/build_example.sh \
> "${BUILD_LOGS:?}"/MyFirstHPSSystem_log.txt 2>&1 &
MyFirstHPSSystem_PID="$!"
echo "Build starting: MyFirstHPSSystem"

wait ${MyFirstFPGA_PID:?} \
|| { EXIT_VALUE=1; echo "ERROR: MyFirstFPGA";}
echo "Build finished: MyFirstFPGA"

wait ${MyFirstQsysSystem_PID:?} \
|| { EXIT_VALUE=1; echo "ERROR: MyFirstQsysSystem";}
echo "Build finished: MyFirstQsysSystem"

wait ${MyFirstHPSSystem_PID:?} \
|| { EXIT_VALUE=1; echo "ERROR: MyFirstHPSSystem";}
echo "Build finished: MyFirstHPSSystem"

[ "${EXIT_VALUE}" -ne "0" ] && { echo "Build Failure"; exit ${EXIT_VALUE}; }

#-------------------------------------------------------------------------------
# Build release artifacts
#-------------------------------------------------------------------------------
echo "Building release artifacts..."

./MyFirstQsysSystem/writeup_MyFirstQsysSystem/blink_archive/create_archive.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstQsysSystem/writeup_InteractSystemConsole/blink_archive/\
create_archive.sh || { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_MyFirstHPSSystem/blink_archive/create_archive.sh \
|| { echo "ERROR" ; exit 1 ; }

MyFirstHPSSystem/writeup_u-boot/blink_archive/create_archive.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_linux/devicetree_overlay/build.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_linux/linux_examples_archive/create_archive.sh \
|| { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Build PDF files
#-------------------------------------------------------------------------------
echo "Building PDF files..."

./MyFirstFPGA/writeup_MyFirstFPGA/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstQsysSystem/writeup_MyFirstQsysSystem/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstQsysSystem/writeup_InteractSystemConsole/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_MyFirstHPSSystem/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_u-boot/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

./MyFirstHPSSystem/writeup_linux/latex/build_tex.sh \
|| { echo "ERROR" ; exit 1 ; }

#-------------------------------------------------------------------------------
# Coalesce release artifacts
#-------------------------------------------------------------------------------
echo "Coalescing release artifacts..."

[ -d "release_material" ] && {
	echo ""
	echo "ERROR: directory already exists"
	echo "'release_material'"
	echo ""
	exit 1
}

mkdir release_material

RELEASE_FILES="
./MyFirstHPSSystem/writeup_linux/linux_examples_archive/linux_examples.zip
./MyFirstHPSSystem/writeup_MyFirstHPSSystem/blink_archive/blink_for_MyFirstHPSSystem.zip
./MyFirstHPSSystem/writeup_u-boot/blink_archive/blink_for_uboot.zip
./MyFirstQsysSystem/writeup_InteractSystemConsole/blink_archive/blink_for_InteractSystemConsole.zip
./MyFirstQsysSystem/writeup_MyFirstQsysSystem/blink_archive/blink_for_MyFirstQsysSystem.zip
./MyFirstFPGA/writeup_MyFirstFPGA/latex/writeup_MyFirstFPGA.pdf
./MyFirstHPSSystem/writeup_linux/latex/writeup_linux.pdf
./MyFirstHPSSystem/writeup_MyFirstHPSSystem/latex/writeup_MyFirstHPSSystem.pdf
./MyFirstHPSSystem/writeup_u-boot/latex/writeup_u-boot.pdf
./MyFirstQsysSystem/writeup_InteractSystemConsole/latex/writeup_InteractSystemConsole.pdf
./MyFirstQsysSystem/writeup_MyFirstQsysSystem/latex/writeup_MyFirstQsysSystem.pdf
"

ERROR_STATUS=0
for FILE in ${RELEASE_FILES}
do
	[ -f "${FILE:?}" ] && {
		cp "${FILE:?}" release_material/ || {
			echo ""
			echo "ERROR: while copying file"
			echo "'${FILE:?}'"
			echo ""
			ERROR_STATUS=1
		}
	} || {
		echo ""
		echo "ERROR: file not found"
		echo "'${FILE:?}'"
		echo ""
		ERROR_STATUS=1
	}
done

[ "${ERROR_STATUS:?}" -ne "0" ] && {
	echo ""
	echo "Exiting build due to errors..."
	echo ""
	exit 1
}

echo "Build completed successfully..."

exit 0

