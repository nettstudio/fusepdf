@echo off

REM ### BUILD FUSEPDF WINDOWS X64 RELEASE
REM ### COPYRIGHT 2022 NETTSTUDIO AS

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 %*

set CWD=%cd%
set TOOLS=%CWD%\tools
set FUSEPDF=2.1.0
set QT_MAJOR=5
set QT_MINOR=12
set QT_PATCH=12
set QT_VERSION=%QT_MAJOR%.%QT_MINOR%.%QT_PATCH%
set QT_DIR=%TOOLS%\qt
set GS=9550

set PATH=%QT_DIR%\bin;%ProgramFiles%\7-Zip;%PATH%

set GS_EXE=gs%GS%w64.exe
set GS_URL=https://github.com/ArtifexSoftware/ghostpdl-downloads/releases/download/gs%GS%/%GS_EXE%

set QTBASE=qtbase-everywhere-src-%QT_VERSION%
set QTTOOLS=qttools-everywhere-src-%QT_VERSION%
set QT_URL=https://mirrors.dotsrc.org/qtproject/archive/qt/%QT_MAJOR%.%QT_MINOR%/%QT_VERSION%/submodules
set QTBASE_URL=%QT_URL%/%QTBASE%.zip
set QTTOOLS_URL=%QT_URL%/%QTTOOLS%.zip

@RD /S /Q build
mkdir build

if not exist "%QT_DIR%\bin\qmake.exe" (
    if not exist "%TOOLS%\%QTBASE%.zip" (
        curl --url %QTBASE_URL% -o "%TOOLS%\%QTBASE%.zip"
    )
    cd "%CWD%\build"
    7z x "%TOOLS%\%QTBASE%.zip"
    cd "%QTBASE%"
    configure.bat -static -release -prefix "%QT_DIR%" -platform win32-msvc2019 -opensource -confirm-license -nomake examples -nomake tests -static-runtime -opengl desktop -optimize-size -no-pch -no-glib -no-dbus -no-sse4.2 -no-avx -no-avx2 -no-avx512 -strip -no-openssl -no-gif -no-ico
    nmake
    nmake install
)

if not exist "%QT_DIR%\bin\lrelease.exe" (
    if not exist "%TOOLS%\%QTTOOLS%.zip" (
        curl --url %QTTOOLS_URL% -o "%TOOLS%\%QTTOOLS%.zip"
    )
    cd "%CWD%\build"
    7z x "%TOOLS%\%QTTOOLS%.zip"
    cd "%QTTOOLS%"
    qmake.exe
    nmake
    nmake install
)

cd "%CWD%\build"
qmake.exe CONFIG+=release ..
nmake

mkdir FusePDF-%FUSEPDF%-x64
COPY release\FusePDF.exe FusePDF-%FUSEPDF%-x64\
COPY %CWD%\docs\README.pdf FusePDF-%FUSEPDF%-x64\

REM if not exist "%TOOLS%\%GS_EXE%" (
REM     curl --url %GS_URL% -o "%TOOLS%\%GS_EXE%"
REM )

if exist "%TOOLS%\gs.zip" (
    7z x "%TOOLS%\gs.zip"
    MOVE gs FusePDF-%FUSEPDF%-x64\
)

cd "%CWD%\build"
7z a -mx9 FusePDF-%FUSEPDF%-x64.zip FusePDF-%FUSEPDF%-x64
