#!/bin/bash

cat header.md > tmp.md
cat index.md >> tmp.md

./md2pdf.sh tmp.md fusepdf.pdf

rm tmp.md
