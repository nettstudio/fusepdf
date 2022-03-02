#!/bin/bash
#
# Make FusePDF docs
#

#
# sudo apt-get install texlive-xetex
# https://github.com/jgm/pandoc/releases/download/2.7.2/pandoc-2.7.2-linux.tar.gz
#

cat tools/header.md > tmp.md
cat docs/index.md >> tmp.md

echo "## GNU General Public License v3.0" >> tmp.md
echo "" >> tmp.md
cat COPYING >> tmp.md

./tools/md2pdf.sh tmp.md docs/fuse.pdf

rm tmp.md
