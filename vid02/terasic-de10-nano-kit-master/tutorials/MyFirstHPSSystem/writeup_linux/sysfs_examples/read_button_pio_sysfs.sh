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

#
# validate the FPGA state
#
[ -f "/sys/class/fpga_manager/fpga0/state" ] || {
	echo ""
	echo "ERROR: FPGA state sysfs file not found"
	echo "'/sys/class/fpga_manager/fpga0/state'"
	exit 1
}

[ "operating" = "$(cat /sys/class/fpga_manager/fpga0/state)" ] || {
	echo ""
	echo "ERROR: FPGA state not in 'operating' mode"
	echo "'/sys/class/fpga_manager/fpga0/state'"
	echo "FPGA state: '$(cat /sys/class/fpga_manager/fpga0/state)'"
	exit 1
}

#
# validate the LWH2F bridge name
#
[ -f "/sys/class/fpga_bridge/br0/name" ] || {
	echo ""
	echo "ERROR: LWH2F bridge name sysfs file not found"
	echo "'/sys/class/fpga_bridge/br0/name'"
	exit 1
}

[ "lwhps2fpga" = "$(cat /sys/class/fpga_bridge/br0/name)" ] || {
	echo ""
	echo "ERROR: LWH2F bridge name not found for 'br0'"
	echo "'/sys/class/fpga_bridge/br0/name'"
	echo "br0 name: '$(cat /sys/class/fpga_bridge/br0/name)'"
	exit 1
}

#
# validate the LWH2F bridge state
#
[ -f "/sys/class/fpga_bridge/br0/state" ] || {
	echo ""
	echo "ERROR: LWH2F bridge state sysfs file not found"
	echo "'/sys/class/fpga_bridge/br0/state'"
	exit 1
}

[ "enabled" = "$(cat /sys/class/fpga_bridge/br0/state)" ] || {
	echo ""
	echo "ERROR: LWH2F bridge state not 'enabled'"
	echo "'/sys/class/fpga_bridge/br0/state'"
	echo "br0 state: '$(cat /sys/class/fpga_bridge/br0/state)'"
	exit 1
}

#
# locate the button_pio GPIO
#
GPIO_BASE_INDEX=0
for NEXT in /sys/class/gpio/gpiochip*
do
	[ "/soc/base-fpga-region/gpio@0x100010010" = "$(cat ${NEXT:?}/label)" ] && {
		[ "1" = "$(cat ${NEXT:?}/ngpio)" ] || {
			echo ""
			echo "ERROR: expected ngpio value of '1'"
			echo "got '$(cat ${NEXT:?}/ngpio)'"
			echo "'${NEXT:?}/ngpio'"
			exit 1
		}
		GPIO_BASE_INDEX=${NEXT#/sys/class/gpio/gpiochip}
		break
	}
done

[ ${GPIO_BASE_INDEX:?} -eq 0 ] && {
	echo ""
	echo "ERROR: could not find expected GPIO"
	echo ""
	exit 1
}

#
# check if this GPIO is already exported
#
[ -d /sys/class/gpio/gpio${GPIO_BASE_INDEX:?} ] && {
	echo ""
	echo "ERROR: GPIO is already exported"
	echo "'/sys/class/gpio/gpio${GPIO_BASE_INDEX:?}'"
	echo ""
	exit 1
}

#
# export the GPIO
#
echo ${GPIO_BASE_INDEX:?} > /sys/class/gpio/export

#
# verify that the GPIO is exported and the value file exists
#
[ -f /sys/class/gpio/gpio${GPIO_BASE_INDEX:?}/value ] || {
	echo ""
	echo "ERROR: GPIO 'value' file cannot be found"
	echo "'/sys/class/gpio/gpio${GPIO_BASE_INDEX:?}/value'"
	echo ""
	exit 1
}

#
# print the GPIO value
#
echo "BUTTON_PIO: '$(cat /sys/class/gpio/gpio${GPIO_BASE_INDEX:?}/value)'"

#
# unexport the GPIO
#
echo ${GPIO_BASE_INDEX:?} > /sys/class/gpio/unexport

