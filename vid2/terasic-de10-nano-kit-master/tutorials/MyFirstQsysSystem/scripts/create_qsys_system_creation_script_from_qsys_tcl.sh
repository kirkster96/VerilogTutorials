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
# This script is designed to take the qsys.tcl file exported from Qsys 16.0 for
# the complete Qsys system and modify it so we can reorder the add_instance
# commands so that the system displays in the Qsys GUI with the desired order.
#
# To create the qsys.tcl script, run qsys-edit version 16.0, open your existing
# Qsys system and choose File->"Export system as qsys script".
#
# Pass the name of the <qsys.tcl> file to be processed into this script, the
# name of the output file will be create_qsys_system_<qsys.tcl>.
#
#------------------------------------------------------------------------------

[ ${#} -eq 1 ] || {
	echo ""
	echo "USAGE: ${0} <qsys.tcl file>"
	echo ""
	exit 1
}

[ -f "${1:?}" ] || {
	echo "ERROR: Input file does not exists..."
	echo "'${1:?}'"
	realpath "${1:?}"
	exit 1
}

OUTPUT_FILE="create_qsys_system_$(basename ${1:?})"

set -x

# look for some indications that this is a qsys.tcl file
grep "create_system" ${1:?} || { set +x; echo "ERROR"; exit 1; }
grep "save_system" ${1:?} || { set +x; echo "ERROR"; exit 1; }

cat "${1:?}" | \
sed -n -e "1,/add_instance/ p" | \
sed -e "/add_instance/ d" \
> ${OUTPUT_FILE:?} || { set +x; echo "ERROR"; exit 1; }

cat << EOF >> ${OUTPUT_FILE:?} || { set +x; echo "ERROR"; exit 1; }

#
# Manually reorder this list of component instances to reflect the order that
# you wish to see them appear in the Qsys GUI.
#
#-------------------------------------------------------------------------------

EOF

cat "${1:?}" | \
sed -n -e "/add_instance/ p" >> ${OUTPUT_FILE:?} || { set +x; echo "ERROR"; exit 1; }

cat << EOF >> ${OUTPUT_FILE:?} || { set +x; echo "ERROR"; exit 1; }

#-------------------------------------------------------------------------------

EOF

cat "${1:?}" | \
sed -n -e "/add_instance/,$ p" | \
sed -e "s/add_instance/# add_instance/" >> ${OUTPUT_FILE:?} || { set +x; echo "ERROR"; exit 1; }

set +x

echo "Script completed successfully..."

exit 0

