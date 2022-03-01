#!/bin/bash

cat tools/header.md > tmp.md
cat docs/index.md >> tmp.md

./md2pdf.sh tmp.md Manual.pdf

rm tmp.md
