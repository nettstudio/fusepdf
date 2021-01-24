#!/bin/bash
#
# FusePDF - https://nettstudio.no
#
# Copyright (c) 2021 NettStudio AS. All rights reserved.
#
#

CWD=`pwd`
BUILD_DIR=${BUILD_DIR:-"${CWD}/tmp"}
MXE=${MXE:-"${CWD}/mxe"}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.static}
BIT=${BIT:-64}
STRIP=${MXE_TC}-strip
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d%H%M`}
VERSION=`cat $CWD/fusepdf.pro | sed '/VERSION =/!d' | awk '{print $3}'`
COMMIT=`git rev-parse --short HEAD`
TAG=${TAG:--${TIMESTAMP}-${COMMIT}}
ZIP="FusePDF-$VERSION$TAG-x$BIT-portable.zip"
FOLDER="FusePDF-$VERSION$TAG"
INNO=${INNO:-$MXE/inno6/ISCC.exe}
WINE=${WINE:-wine}
SETUP=${SETUP:-1}

if [ ! -d "${MXE}" ]; then
    echo "Please setup MXE!"
    exit 1
fi
rm -rf "${BUILD_DIR}" || true
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}" || exit 1

export PATH="${MXE}/usr/bin:${MXE}/usr/${MXE_TC}/qt5/bin:${PATH}"
export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"

qmake CONFIG+=release ${CWD} || exit 1
make || exit 1
mkdir $FOLDER $FOLDER/platforms $FOLDER/opensource || exit 1
cp -a $CWD/legal/* $FOLDER/opensource/ || exit 1
cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $FOLDER/platforms/ || exit 1
cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5CoreNettStudio.dll,Qt5GuiNettStudio.dll,Qt5WidgetsNettStudio.dll} $FOLDER/ || exit 1
cp release/FusePDF.exe $FOLDER || exit 1
cp $CWD/LICENSE.TXT $FOLDER/ || exit 1
$STRIP -s $FOLDER/*.exe $FOLDER/*.dll $FOLDER/*/*.dll || exit 1
zip -r -9 $ZIP $FOLDER || exit 1
if [ "$SETUP" = 1 ]; then
    cd $CWD || exit 1
    cat fusepdf.iss | sed 's/__VERSION__/'"$VERSION$TAG"'/g;s/__PF__/commonpf'"$BIT"'/g;s/__BIT__/'"$BIT"'/g;' > setup.iss || exit 1
    $WINE $INNO setup.iss || exit 1
fi

echo "DONE!"
