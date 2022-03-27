@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 %*
set PATH=C:\qt-5.12\bin;%ProgramFiles%\7-Zip;%PATH%
set FUSEPDF=2.1.0
REM set GS=9.55.0

@RD /S /Q build
mkdir build
cd build

qmake.exe CONFIG+=release ..
nmake

mkdir FusePDF-%FUSEPDF%-x64
COPY release\FusePDF.exe FusePDF-%FUSEPDF%-x64\
COPY ..\docs\README.pdf FusePDF-%FUSEPDF%-x64\

if exist ..\tools\gs%GS%.zip (
    7z x ..\tools\gs%GS%.zip
    MOVE gs%GS% FusePDF-%FUSEPDF%-x64\gs
)

7z a -mx9 FusePDF-%FUSEPDF%-x64.zip FusePDF-%FUSEPDF%-x64
