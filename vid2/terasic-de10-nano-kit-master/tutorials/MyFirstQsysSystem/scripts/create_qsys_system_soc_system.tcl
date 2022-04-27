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

# qsys scripting (.tcl) file for soc_system
package require -exact qsys 16.0

create_system {soc_system}

set_project_property DEVICE_FAMILY {Cyclone V}
set_project_property DEVICE {5CSEBA6U23I7}
set_project_property HIDE_FROM_IP_CATALOG {false}

# Instances and instance parameters
# (disabled instances are intentionally culled)

#
# Manually reorder this list of component instances to reflect the order that
# you wish to see them appear in the Qsys GUI.
#
#-------------------------------------------------------------------------------

add_instance clk_0 clock_source
add_instance ocram_64k altera_avalon_onchip_memory2
add_instance default_16b altera_avalon_onchip_memory2
add_instance led_pio altera_avalon_pio
add_instance button_pio altera_avalon_pio
add_instance switch_pio altera_avalon_pio
add_instance system_id altera_avalon_sysid_qsys
add_instance master_0 altera_jtag_avalon_master

#-------------------------------------------------------------------------------

# add_instance button_pio altera_avalon_pio 17.0
set_instance_parameter_value button_pio {bitClearingEdgeCapReg} {0}
set_instance_parameter_value button_pio {bitModifyingOutReg} {0}
set_instance_parameter_value button_pio {captureEdge} {0}
set_instance_parameter_value button_pio {direction} {Input}
set_instance_parameter_value button_pio {edgeType} {RISING}
set_instance_parameter_value button_pio {generateIRQ} {0}
set_instance_parameter_value button_pio {irqType} {LEVEL}
set_instance_parameter_value button_pio {resetValue} {0.0}
set_instance_parameter_value button_pio {simDoTestBenchWiring} {0}
set_instance_parameter_value button_pio {simDrivenValue} {0.0}
set_instance_parameter_value button_pio {width} {1}

# add_instance clk_0 clock_source 17.0
set_instance_parameter_value clk_0 {clockFrequency} {50000000.0}
set_instance_parameter_value clk_0 {clockFrequencyKnown} {1}
set_instance_parameter_value clk_0 {resetSynchronousEdges} {NONE}

# add_instance default_16b altera_avalon_onchip_memory2 17.0
set_instance_parameter_value default_16b {allowInSystemMemoryContentEditor} {0}
set_instance_parameter_value default_16b {blockType} {AUTO}
set_instance_parameter_value default_16b {copyInitFile} {0}
set_instance_parameter_value default_16b {dataWidth} {32}
set_instance_parameter_value default_16b {dataWidth2} {32}
set_instance_parameter_value default_16b {dualPort} {0}
set_instance_parameter_value default_16b {ecc_enabled} {0}
set_instance_parameter_value default_16b {enPRInitMode} {0}
set_instance_parameter_value default_16b {enableDiffWidth} {0}
set_instance_parameter_value default_16b {initMemContent} {1}
set_instance_parameter_value default_16b {initializationFileName} {onchip_mem.hex}
set_instance_parameter_value default_16b {instanceID} {NONE}
set_instance_parameter_value default_16b {memorySize} {16.0}
set_instance_parameter_value default_16b {readDuringWriteMode} {DONT_CARE}
set_instance_parameter_value default_16b {resetrequest_enabled} {1}
set_instance_parameter_value default_16b {simAllowMRAMContentsFile} {0}
set_instance_parameter_value default_16b {simMemInitOnlyFilename} {0}
set_instance_parameter_value default_16b {singleClockOperation} {0}
set_instance_parameter_value default_16b {slave1Latency} {1}
set_instance_parameter_value default_16b {slave2Latency} {1}
set_instance_parameter_value default_16b {useNonDefaultInitFile} {0}
set_instance_parameter_value default_16b {useShallowMemBlocks} {0}
set_instance_parameter_value default_16b {writable} {1}

# add_instance led_pio altera_avalon_pio 17.0
set_instance_parameter_value led_pio {bitClearingEdgeCapReg} {0}
set_instance_parameter_value led_pio {bitModifyingOutReg} {0}
set_instance_parameter_value led_pio {captureEdge} {0}
set_instance_parameter_value led_pio {direction} {Output}
set_instance_parameter_value led_pio {edgeType} {RISING}
set_instance_parameter_value led_pio {generateIRQ} {0}
set_instance_parameter_value led_pio {irqType} {LEVEL}
set_instance_parameter_value led_pio {resetValue} {0.0}
set_instance_parameter_value led_pio {simDoTestBenchWiring} {0}
set_instance_parameter_value led_pio {simDrivenValue} {0.0}
set_instance_parameter_value led_pio {width} {8}

# add_instance master_0 altera_jtag_avalon_master 17.0
set_instance_parameter_value master_0 {FAST_VER} {0}
set_instance_parameter_value master_0 {FIFO_DEPTHS} {2}
set_instance_parameter_value master_0 {PLI_PORT} {50000}
set_instance_parameter_value master_0 {USE_PLI} {0}

# add_instance ocram_64k altera_avalon_onchip_memory2 17.0
set_instance_parameter_value ocram_64k {allowInSystemMemoryContentEditor} {0}
set_instance_parameter_value ocram_64k {blockType} {AUTO}
set_instance_parameter_value ocram_64k {copyInitFile} {0}
set_instance_parameter_value ocram_64k {dataWidth} {64}
set_instance_parameter_value ocram_64k {dataWidth2} {32}
set_instance_parameter_value ocram_64k {dualPort} {0}
set_instance_parameter_value ocram_64k {ecc_enabled} {0}
set_instance_parameter_value ocram_64k {enPRInitMode} {0}
set_instance_parameter_value ocram_64k {enableDiffWidth} {0}
set_instance_parameter_value ocram_64k {initMemContent} {1}
set_instance_parameter_value ocram_64k {initializationFileName} {onchip_mem.hex}
set_instance_parameter_value ocram_64k {instanceID} {NONE}
set_instance_parameter_value ocram_64k {memorySize} {65536.0}
set_instance_parameter_value ocram_64k {readDuringWriteMode} {DONT_CARE}
set_instance_parameter_value ocram_64k {resetrequest_enabled} {1}
set_instance_parameter_value ocram_64k {simAllowMRAMContentsFile} {0}
set_instance_parameter_value ocram_64k {simMemInitOnlyFilename} {0}
set_instance_parameter_value ocram_64k {singleClockOperation} {0}
set_instance_parameter_value ocram_64k {slave1Latency} {1}
set_instance_parameter_value ocram_64k {slave2Latency} {1}
set_instance_parameter_value ocram_64k {useNonDefaultInitFile} {0}
set_instance_parameter_value ocram_64k {useShallowMemBlocks} {0}
set_instance_parameter_value ocram_64k {writable} {1}

# add_instance switch_pio altera_avalon_pio 17.0
set_instance_parameter_value switch_pio {bitClearingEdgeCapReg} {0}
set_instance_parameter_value switch_pio {bitModifyingOutReg} {0}
set_instance_parameter_value switch_pio {captureEdge} {0}
set_instance_parameter_value switch_pio {direction} {Input}
set_instance_parameter_value switch_pio {edgeType} {RISING}
set_instance_parameter_value switch_pio {generateIRQ} {0}
set_instance_parameter_value switch_pio {irqType} {LEVEL}
set_instance_parameter_value switch_pio {resetValue} {0.0}
set_instance_parameter_value switch_pio {simDoTestBenchWiring} {0}
set_instance_parameter_value switch_pio {simDrivenValue} {0.0}
set_instance_parameter_value switch_pio {width} {4}

# add_instance system_id altera_avalon_sysid_qsys 17.0
set_instance_parameter_value system_id {id} {-569319920}

# exported interfaces
add_interface button_pio conduit end
set_interface_property button_pio EXPORT_OF button_pio.external_connection
add_interface clk clock sink
set_interface_property clk EXPORT_OF clk_0.clk_in
add_interface led_pio conduit end
set_interface_property led_pio EXPORT_OF led_pio.external_connection
add_interface reset reset sink
set_interface_property reset EXPORT_OF clk_0.clk_in_reset
add_interface switch_pio conduit end
set_interface_property switch_pio EXPORT_OF switch_pio.external_connection

# connections and connection parameters
add_connection clk_0.clk button_pio.clk

add_connection clk_0.clk default_16b.clk1

add_connection clk_0.clk led_pio.clk

add_connection clk_0.clk master_0.clk

add_connection clk_0.clk ocram_64k.clk1

add_connection clk_0.clk switch_pio.clk

add_connection clk_0.clk system_id.clk

add_connection clk_0.clk_reset button_pio.reset

add_connection clk_0.clk_reset default_16b.reset1

add_connection clk_0.clk_reset led_pio.reset

add_connection clk_0.clk_reset master_0.clk_reset

add_connection clk_0.clk_reset ocram_64k.reset1

add_connection clk_0.clk_reset switch_pio.reset

add_connection clk_0.clk_reset system_id.reset

add_connection master_0.master button_pio.s1
set_connection_parameter_value master_0.master/button_pio.s1 arbitrationPriority {1}
set_connection_parameter_value master_0.master/button_pio.s1 baseAddress {0x00010010}
set_connection_parameter_value master_0.master/button_pio.s1 defaultConnection {0}

add_connection master_0.master default_16b.s1
set_connection_parameter_value master_0.master/default_16b.s1 arbitrationPriority {1}
set_connection_parameter_value master_0.master/default_16b.s1 baseAddress {0x0000}
set_connection_parameter_value master_0.master/default_16b.s1 defaultConnection {1}

add_connection master_0.master led_pio.s1
set_connection_parameter_value master_0.master/led_pio.s1 arbitrationPriority {1}
set_connection_parameter_value master_0.master/led_pio.s1 baseAddress {0x00010000}
set_connection_parameter_value master_0.master/led_pio.s1 defaultConnection {0}

add_connection master_0.master ocram_64k.s1
set_connection_parameter_value master_0.master/ocram_64k.s1 arbitrationPriority {1}
set_connection_parameter_value master_0.master/ocram_64k.s1 baseAddress {0x0000}
set_connection_parameter_value master_0.master/ocram_64k.s1 defaultConnection {0}

add_connection master_0.master switch_pio.s1
set_connection_parameter_value master_0.master/switch_pio.s1 arbitrationPriority {1}
set_connection_parameter_value master_0.master/switch_pio.s1 baseAddress {0x00010020}
set_connection_parameter_value master_0.master/switch_pio.s1 defaultConnection {0}

add_connection master_0.master system_id.control_slave
set_connection_parameter_value master_0.master/system_id.control_slave arbitrationPriority {1}
set_connection_parameter_value master_0.master/system_id.control_slave baseAddress {0x00010030}
set_connection_parameter_value master_0.master/system_id.control_slave defaultConnection {0}

add_connection master_0.master_reset button_pio.reset

add_connection master_0.master_reset default_16b.reset1

add_connection master_0.master_reset led_pio.reset

add_connection master_0.master_reset master_0.clk_reset

add_connection master_0.master_reset ocram_64k.reset1

add_connection master_0.master_reset switch_pio.reset

add_connection master_0.master_reset system_id.reset

# interconnect requirements
set_interconnect_requirement {$system} {qsys_mm.clockCrossingAdapter} {HANDSHAKE}
set_interconnect_requirement {$system} {qsys_mm.enableEccProtection} {FALSE}
set_interconnect_requirement {$system} {qsys_mm.insertDefaultSlave} {FALSE}
set_interconnect_requirement {$system} {qsys_mm.maxAdditionalLatency} {1}

save_system {soc_system.qsys}
