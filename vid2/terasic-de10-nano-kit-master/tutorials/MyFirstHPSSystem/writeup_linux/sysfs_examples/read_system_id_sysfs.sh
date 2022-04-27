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
# verify that system_id device sysfs files are defined
#
[ -f "/sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/id" ] || {
	echo ""
	echo "ERROR: system_id 'id' sysfs file not found"
	echo "'/sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/id'"
	exit 1
}

[ -f "/sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/timestamp" ] || {
	echo ""
	echo "ERROR: system_id 'timestamp' sysfs file not found"
	echo "'/sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/timestamp'"
	exit 1
}

echo "System ID ID: '$(cat /sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/id)'"
echo "System ID TS: '$(cat /sys/devices/platform/soc/soc:base-fpga-region/ff210030.sysid/sysid/timestamp)'"

