#!/bin/bash
#
# Origin
# Copyright (C) 2021 Intel Corporation 
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information

# Summary of compile&build commands 

# Generate Qsys RTL files and .sopcinfo file
qsys-generate -syn top_qsys.qsys

# Build NiosII software and mem_init hex file
rm software_nios2/RFS_SENSOR_bsp/*.mk software_nios2/RFS_SENSOR_bsp/*.bsp
cd software_nios2/RFS_SENSOR
if [ -f Makefile ]; then
  rm Makefile
fi
./create-this-app
make mem_init_generate
  
# Compile FPGA design
cd ../..
quartus_sh --flow compile Module5_Sample_HW


# Generate RBF file
cd output_files
quartus_cpf -c -o bitstream_compression=on Module5_Sample_HW.sof Module5_Sample_HW.rbf


cd ..
