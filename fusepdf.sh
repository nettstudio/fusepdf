#!/bin/bash

CWD=`pwd`
BUILD_DIR=${BUILD_DIR:-"${CWD}/tmp"}
MXE=${MXE:-"${CWD}/mxe"}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.static}
BIT=${BIT:-64}
SNAPSHOT=${SNAPSHOT:-1}
STRIP=${MXE_TC}-strip
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d%H%M`}
VERSION=`cat $CWD/fusepdf.pro | sed '/VERSION =/!d' | awk '{print $3}'`
COMMIT=`git rev-parse --short HEAD`
TAG=${TAG:-${TIMESTAMP}-${COMMIT}}
ZIP="FusePDF-$VERSION-$TAG-portable.zip"
FOLDER="FusePDF-$VERSION-$TAG"

if [ "$SNAPSHOT" = 0 ]; then
    ZIP="FusePDF-$VERSION-portable.zip"
    FOLDER="FusePDF-$VERSION"
fi

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
mv $ZIP .. || exit 1
echo "DONE!"
