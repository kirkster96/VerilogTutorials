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

set_global_assignment -name VERILOG_FILE blink.v
set_global_assignment -name SDC_FILE blink.sdc

# Pin & Location Assignments
# ==========================
set_location_assignment PIN_W15 -to LED
set_location_assignment PIN_V11 -to clk

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
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LED
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to clk

set_instance_assignment -name CURRENT_STRENGTH_NEW 16MA -to LED

set_instance_assignment -name SLEW_RATE 1 -to LED

project_close
qexit -success
