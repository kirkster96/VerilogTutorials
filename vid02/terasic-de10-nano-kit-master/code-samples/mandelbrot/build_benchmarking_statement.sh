#!/bin/bash

#
# to run, pass the required environment variables in on the command line
#
# CROSS_COMPILE="<cross-compiler-prefix>" \
# MANDEL_ELF="<path-to-Mandelbrot-ELF>" \
# MANDEL_SOF="<path-to-Mandelbrot-SOF>" \
# MANDEL_RBF="<path-to-Mandelbrot-RBF>" \
# ./build_benchmarking_statement.sh > benchmark.txt
#

if [ -z "${READELF}" ]; then
READELF=${CROSS_COMPILE:?}readelf
fi
if [ -z "${CC}" ]; then
CC=${CROSS_COMPILE:?}gcc
fi
: ${MANDEL_ELF:?}
: ${MANDEL_SOF:?}
: ${MANDEL_RBF:?}

echo "DE10-Nano Mandelbrot Benchmark Conditions"
echo ""
echo "The Mandelbrot application is performed separately by the"
echo "dual-core ARM Cortex-A9 CPU, and a Mandelbrot hardware"
echo "accelerator inside the FPGA. The sections below list the"
echo "versions of tools and settings used to generate both the"
echo "executable code (CPU) as well and the FPGA hardware"
echo "(Mandelbrot accelerator)."
echo ""
echo "+------------------------------------------------------------+"
echo "|        Mandelbrot Software Application (CPU)               |"
echo "+------------------------------------------------------------+"
echo ""
echo "Filename:"
echo "    $(basename ${MANDEL_ELF:?})"
echo ""
echo "MD5 sum:"
echo "    $(md5sum -b ${MANDEL_ELF:?} | sed -e 's/\([0-9a-fA-F]*\)\s.*/\1/')"
echo ""
echo "Compiler Version:"

${READELF:?} -p .comment ${MANDEL_ELF:?} | \
	sed -e '/\[.*\]/!d' -e 's/.*\[.*\]\s*\(.*\)/\1/;' | \
	sed -e 's/\(.*\)/    \1/'

echo ""
echo "Compiler Options:"
${READELF:?} --debug-dump=info --dwarf-depth=1 ${MANDEL_ELF:?} | \
	sed -e '/.*DW_AT_producer.*/!d' -e 's/.*:.*:\s*\(.*\)/\1/' | \
	head -n 1 | \
	sed -e 's/\(.*\)/    \1/'

echo ""
echo "Linked Libraries:"
for NEXT in $(${READELF:?} -d ${MANDEL_ELF:?} | \
	sed -e '/.*\(NEEDED\).*/!d' -e 's/.*\[\(.*\)\].*/\1/')
do
	readlink $(${CC:?} -print-file-name=${NEXT:?}) | \
	sed -e 's/\(.*\)/    \1/'
done

echo ""
echo "+------------------------------------------------------------+"
echo "|       Mandelbrot Accelerator Hardware (FPGA)               |"
echo "+------------------------------------------------------------+"
echo ""
echo "Filename:"
echo "    $(basename ${MANDEL_RBF:?})"
echo ""
echo "MD5 sum:"
echo "    $(md5sum -b ${MANDEL_RBF:?} | sed -e 's/\([0-9a-fA-F]*\)\s.*/\1/')"
echo ""
echo "Compiler Version:"

strings ${MANDEL_SOF:?} | \
	grep "Version" | \
	sed -e 's/\(.*\)/    \1/'

