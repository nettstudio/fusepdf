@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 %*
set PATH=C:\qt-5.12\bin;%PATH%

qmake.exe CONFIG+=release
nmake
