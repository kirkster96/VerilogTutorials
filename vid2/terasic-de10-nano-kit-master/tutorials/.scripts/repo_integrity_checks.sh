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

GIT_FILES_FILE="repo_integrity_git_files.txt"
GIT_TEXT_FILES_FILE="repo_integrity_git_text_files.txt"
GIT_README_FILES_FILE="repo_integrity_git_readme_files.txt"
CONTAIN_COPYRIGHT_FILES="repo_integrity_contain_copyright.txt"
ALL_IMAGE_FILES="repo_integrity_all_image_files.txt"
REAL_IMAGE_FILES="repo_integrity_real_image_files.txt"
IMAGE_LICENSE_FILES="repo_integrity_image_license_files.txt"

# change into the directory containing this script
cd $(dirname ${0:?})

# change into the top level directory of the git repo
cd $(git rev-parse --show-toplevel)

# get list of all files in git repo in tutorials path
git ls-tree -r --name-only HEAD tutorials > "${GIT_FILES_FILE:?}"

# filter out all non-image files
grep \
	-e ".png" -e ".fodg" \
	"${GIT_FILES_FILE:?}" \
	| sort > "${ALL_IMAGE_FILES:?}"

# filter only real image files
grep \
	-v \
	-e "LICENSE."\
	"${ALL_IMAGE_FILES:?}" \
	> "${REAL_IMAGE_FILES:?}"

# filter only image license files
grep \
	-e "LICENSE."\
	"${ALL_IMAGE_FILES:?}" \
	| sed -e "s/LICENSE\.//" > "${IMAGE_LICENSE_FILES:?}"

# output diff between real image files and image license files
echo ""
echo "diff between real image list and image license list (should be none):"
diff "${IMAGE_LICENSE_FILES:?}" "${REAL_IMAGE_FILES:?}"
echo ""

# filter out all image, readme and howto files
grep \
	-v \
	-e ".png" -e ".fodg" \
	-e "README" -e "HOWTO" "${GIT_FILES_FILE:?}" \
	> "${GIT_TEXT_FILES_FILE:?}"

# filter out readme and howto files
grep \
	-e "README" -e "HOWTO" "${GIT_FILES_FILE:?}" \
	> "${GIT_README_FILES_FILE:?}"

# search text files for Intel copyright statement
grep \
	-l \
	-P "Copyright \(c\) 201[67] Intel Corporation" \
	$(echo $(cat "${GIT_TEXT_FILES_FILE:?}")) \
	> "${CONTAIN_COPYRIGHT_FILES:?}"

# output the files without Intel copyright statement
echo ""
echo "These files DO NOT contain Intel copyright statements:"
diff "${CONTAIN_COPYRIGHT_FILES:?}" "${GIT_TEXT_FILES_FILE:?}"
echo ""

# output the files with Intel copyright statement not 2017
echo ""
echo "These files DO NOT contain 2017 copyright statements:"
grep \
	-L \
	-P "Copyright \(c\) 2017 Intel Corporation" \
	$(echo $(cat "${CONTAIN_COPYRIGHT_FILES:?}"))
echo ""

# search text files for trailing whitespace
echo ""
echo "These files contain trailing whitespace:"
grep -l -P "[\t ]$" $(echo $(cat "${GIT_TEXT_FILES_FILE:?}"))
grep -l -P "[\t ]$" $(echo $(cat "${GIT_README_FILES_FILE:?}"))
echo ""

# search text files for non-ascii characters
echo ""
echo "These files contain non-ascii characters:"
grep -l -P "[^\t\n -~]" $(echo $(cat "${GIT_TEXT_FILES_FILE:?}"))
grep -l -P "[^\t\n -~]" $(echo $(cat "${GIT_README_FILES_FILE:?}"))
echo ""

# remove the temporary working files
rm \
	"${GIT_FILES_FILE:?}" \
	"${GIT_TEXT_FILES_FILE:?}" \
	"${GIT_README_FILES_FILE:?}" \
	"${CONTAIN_COPYRIGHT_FILES:?}" \
	"${ALL_IMAGE_FILES:?}" \
	"${REAL_IMAGE_FILES:?}" \
	"${IMAGE_LICENSE_FILES:?}"

