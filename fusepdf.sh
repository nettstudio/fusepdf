#!/bin/bash
# convert -background transparent fusepdf.png -define icon:auto-resize=16,32,48,64,128,192,256 fusepdf.ico

CWD=`pwd`
BUILD_DIR=${BUILD_DIR:-"${CWD}/tmp"}
MXE=${MXE:-"${CWD}/mxe"}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.static}
STRIP=${MXE_TC}-strip
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d%H%M`}
COMMIT=`git rev-parse --short HEAD`
ZIP="FusePDF-$TIMESTAMP-$COMMIT-win64.7z"

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
$STRIP -s release/FusePDF.exe || exit 1
cd release || exit 1
7za -m0=lzma2 -mx=9 a $ZIP FusePDF.exe || exit 1
mv $ZIP .. || exit 1
echo "DONE!"
#scp ${CWD}/$ZIP gir:/home/media/Cryon/
#rm -rf $BUILD_DIR $CWD/Cryon.exe
