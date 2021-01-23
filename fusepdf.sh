#!/bin/bash

CWD=`pwd`
BUILD_DIR=${BUILD_DIR:-"${CWD}/tmp"}
MXE=${MXE:-"${CWD}/mxe"}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.static}
BIT=${BIT:-64}
STRIP=${MXE_TC}-strip
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d%H%M`}
VERSION=`cat $CWD/fusepdf.pro | sed '/VERSION =/!d' | awk '{print $3}'`
COMMIT=`git rev-parse --short HEAD`
ZIP="FusePDF-$VERSION-$TIMESTAMP-$COMMIT-Windows-x$BIT"

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
mkdir $ZIP $ZIP/platforms $ZIP/opensource || exit 1
cp -a $CWD/legal/* $ZIP/opensource/ || exit 1
cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $ZIP/platforms/ || exit 1
cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5CoreNettStudio.dll,Qt5GuiNettStudio.dll,Qt5WidgetsNettStudio.dll} $ZIP/ || exit 1
cp release/FusePDF.exe $ZIP || exit 1
$STRIP -s $ZIP/*.exe $ZIP/*.dll $ZIP/*/*.dll || exit 1
zip -r -9 $ZIP.zip $ZIP || exit 1
mv $ZIP.zip .. || exit 1
echo "DONE!"
