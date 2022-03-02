#!/bin/bash
#
# Make FusePDF docs
#

#
# apt-get install autoconf automake autopoint bash bison bzip2 flex g++ g++-multilib gettext git gperf intltool libc6-dev-i386 libgdk-pixbuf2.0-dev libltdl-dev libssl-dev libtool-bin libxml-parser-perl lzip make openssl p7zip-full patch perl python ruby sed unzip wget xz-utils
# sudo apt-get install texlive-xetex
# https://github.com/jgm/pandoc/releases/download/2.7.2/pandoc-2.7.2-linux.tar.gz
#

cat tools/header.md > tmp.md
cat docs/index.md >> tmp.md

./tools/md2pdf.sh tmp.md docs/manual.pdf

rm tmp.md
