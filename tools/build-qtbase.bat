@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 %*

configure.bat -static -release -prefix C:\qt-5.12 -platform win32-msvc2019 -opensource -confirm-license -nomake examples -nomake tests -static-runtime -opengl desktop -optimize-size -no-pch -no-glib -no-dbus -no-sse4.2 -no-avx -no-avx2 -no-avx512 -strip -no-openssl -no-gif -no-ico 
nmake
nmake install
