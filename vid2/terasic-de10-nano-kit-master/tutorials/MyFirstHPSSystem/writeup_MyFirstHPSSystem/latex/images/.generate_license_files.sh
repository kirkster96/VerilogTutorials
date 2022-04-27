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

INTEL_IMAGES="
./01-open-qsys.png
./02-hps-search.png
./03-fpga-int-general.png
./04-fpga-int-axi-bridges.png
./05-fpga-int-f2s-bridge.png
./06-fpga-int-resets.png
./07-fpga-int-dma.png
./08-fpga-int-interrupts.png
./09-periph-emac.png
./10-periph-nand.png
./11-periph-qspi.png
./12-periph-sdmmc.png
./13-periph-usb.png
./14-periph-spi.png
./15-periph-uart.png
./16-periph-i2c.png
./17-periph-can.png
./18-periph-trace.png
./19-periph-mux.png
./20-hps-clks-in-external.png
./21-hps-clks-in-fpga2hps.png
./22-hps-clks-in-peripheral-fpga.png
./23-hps-clks-out-clk-srcs.png
./24-hps-clks-out-main-pll.png
./25-hps-clks-out-periph-pll.png
./26-hps-clks-out-h2f-user.png
./27-sdram-phy.png
./28-sdram-param.png
./29-sdram-param-init.png
./30-sdram-timing.png
./31-sdram-board.png
./32-sdram-board-sig-int.png
./33-sdram-board-skews.png
./34-hps-instance.png
./35-hps-system.png
./36-quartus-tcl-scripts-menu.png
./37-run-hps-pin-assignments.png
./38-run-hps-sdram-pin-assignments.png
./39-start-analysis-synthesis-button.png
./40-cpf-menu.png
./41-convert-programming-files.png
./hps_block_diagram.png
"

TERASIC_IMAGES="
"

for NEXT_FILE in ${INTEL_IMAGES:?}
do
	FILE="${NEXT_FILE##*/}"

	[ -f "${FILE:?}" ] || {
		echo ""
		echo "ERROR: file not found"
		echo "'${FILE:?}'"
		echo ""
		exit 1
	}

	[ -f "LICENSE.${FILE:?}" ] && {
		echo ""
		echo "ERROR: file already exists"
		echo "'LICENSE.${FILE:?}'"
		echo ""
		exit 1
	}

	(
		echo "Copyright (c) 2017 Intel Corporation"
		echo "All rights reserved."
	) > "LICENSE.${FILE:?}"
done

for NEXT_FILE in ${TERASIC_IMAGES:?}
do
	FILE="${NEXT_FILE##*/}"

	[ -f "${FILE:?}" ] || {
		echo ""
		echo "ERROR: file not found"
		echo "'${FILE:?}'"
		echo ""
		exit 1
	}

	[ -f "LICENSE.${FILE:?}" ] && {
		echo ""
		echo "ERROR: file already exists"
		echo "'LICENSE.${FILE:?}'"
		echo ""
		exit 1
	}

	(
		echo "Copyright (c) 2017 Terasic Technologies Inc."
		echo "All rights reserved."
	) > "LICENSE.${FILE:?}"
done

echo "All license files created..."

declare -a IMAGE_ARRAY=($(echo [^L]*.png))
declare -a LICENSE_ARRAY=($(echo LICENSE.*.png))

[ "${#IMAGE_ARRAY[*]}" -eq "${#LICENSE_ARRAY[*]}" ] || {
	echo ""
	echo "ERROR: license file count does not equal image file count"
	echo "License files: ${#LICENSE_ARRAY[*]}"
	echo "  Image files: ${#IMAGE_ARRAY[*]}"
	echo ""
	exit 1
}

for INDEX in $(seq 0 $(expr ${#IMAGE_ARRAY[*]} - 1))
do
	[ "LICENSE.${IMAGE_ARRAY[${INDEX:?}]}" == \
	  "${LICENSE_ARRAY[${INDEX:?}]}" ] || {
		echo ""
		echo "ERROR: image file and license file mismatch"
		echo "License file: ${LICENSE_ARRAY[${INDEX:?}]}"
		echo "  Image file: ${IMAGE_ARRAY[${INDEX:?}]}"
		echo ""
		exit 1
	}
done

echo "All image files appear to have a license file..."

exit 0

