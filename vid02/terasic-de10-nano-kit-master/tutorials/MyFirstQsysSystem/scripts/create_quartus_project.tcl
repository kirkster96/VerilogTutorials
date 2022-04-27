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
set PROJECT_NAME "blink"

if [project_exists $PROJECT_NAME] {
        post_message -type error "project already exists..."
        post_message -type error "'$PROJECT_NAME'"
        qexit -error
}

project_new $PROJECT_NAME

# Project-Wide Assignments
# ========================
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files

set_global_assignment -name QIP_FILE soc_system/synthesis/soc_system.qip
set_global_assignment -name VERILOG_FILE blink.v
set_global_assignment -name SDC_FILE blink.sdc

# Pin & Location Assignments
# ==========================
set_location_assignment PIN_V11  -to clk

set_location_assignment PIN_AA23 -to leds[7]
set_location_assignment PIN_Y16  -to leds[6]
set_location_assignment PIN_AE26 -to leds[5]
set_location_assignment PIN_AF26 -to leds[4]
set_location_assignment PIN_V15  -to leds[3]
set_location_assignment PIN_V16  -to leds[2]
set_location_assignment PIN_AA24 -to leds[1]
set_location_assignment PIN_W15  -to leds[0]

set_location_assignment PIN_AH16 -to push_button[1]
set_location_assignment PIN_AH17 -to push_button[0]

set_location_assignment PIN_W20  -to switch[3]
set_location_assignment PIN_W21  -to switch[2]
set_location_assignment PIN_W24  -to switch[1]
set_location_assignment PIN_Y24  -to switch[0]

# Classic Timing Assignments
# ==========================
set_global_assignment -name MIN_CORE_JUNCTION_TEMP "-40"
set_global_assignment -name MAX_CORE_JUNCTION_TEMP 100

# Analysis & Synthesis Assignments
# ================================
set_global_assignment -name FAMILY "Cyclone V"
set_global_assignment -name TOP_LEVEL_ENTITY $PROJECT_NAME

# Fitter Assignments
# ==================
set_global_assignment -name DEVICE 5CSEBA6U23I7
set_global_assignment -name ERROR_CHECK_FREQUENCY_DIVISOR 256

# Fitter Assignments
# ==================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to clk
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to leds[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to push_button[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to push_button[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to switch[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to switch[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to switch[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to switch[0]

set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[7]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[6]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[5]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[4]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[3]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[2]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[1]
set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to leds[0]

set_instance_assignment -name SLEW_RATE 1 -to leds[7]
set_instance_assignment -name SLEW_RATE 1 -to leds[6]
set_instance_assignment -name SLEW_RATE 1 -to leds[5]
set_instance_assignment -name SLEW_RATE 1 -to leds[4]
set_instance_assignment -name SLEW_RATE 1 -to leds[3]
set_instance_assignment -name SLEW_RATE 1 -to leds[2]
set_instance_assignment -name SLEW_RATE 1 -to leds[1]
set_instance_assignment -name SLEW_RATE 1 -to leds[0]

project_close
qexit -success
