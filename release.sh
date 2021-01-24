#!/bin/bash
#
# FusePDF - https://nettstudio.no
#
# Copyright (c) 2021 NettStudio AS. All rights reserved.
#
#

CWD=`pwd`
export TAG="-release"
export SETUP=1

if [ ! -d "releases" ]; then
    mkdir -p releases
fi

BIT=64 ./fusepdf.sh || exit 1
mv $CWD/tmp/{*-portable.zip,*-setup.exe} $CWD/releases/ || exit 1

MXE_TC=i686-w64-mingw32.static BIT=32 ./fusepdf.sh || exit 1
mv $CWD/tmp/{*-portable.zip,*-setup.exe} $CWD/releases/ || exit 1
