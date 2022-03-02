# FusePDF

[![GitHub](https://img.shields.io/github/license/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/blob/main/COPYING) [![C/C++ CI](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml) [![GitHub issues](https://img.shields.io/github/issues/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/issues)

FusePDF is a simple cross-platform application used for merging, splitting and exporting PDF pages and documents.

Copyright (c) 2021, 2022 [NettStudio AS](https://nettstudio.no). All rights reserved.

[![NettStudio](https://raw.githubusercontent.com/nettstudio/fusepdf/main/assets/nettstudio.png)](https://nettstudio.no)

## Build (on Linux/macOS/FreeBSD)

FusePDF requires Qt5+ (Core/Gui/Widgets/Concurrent) to build, and Ghostscript (``gs``) available during runtime.

## Installing dependencies

Instructions for installing the needed dependencies.

### Debian/Ubuntu (and compatible)

``sudo apt install ghostscript qt5-default qtbase5-dev qttools5-dev-tools``

### Fedora/CentOS/RHEL (and compatible)

``sudo yum install ghostscript qt5-qtbase-devel qt5-qttools``

### macports

``sudo port install ghostscript qt5-qtbase qt5-qttools``

### FreeBSD

``sudo pkg install ghostscript9-agpl-base qt5``

## Get source code

Getting the latest (``main``) source code from GitHub:

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
git submodule update -i
```

We also have stable branches (may contain fixes not available in releases):

 * ``v1.0`` *(unmaintained, kept as archive)*
 * ``v2.0`` *(maintained until ``v2.1`` is branched)*

```
git checkout vX.Y
```

Or download the latest stable tarball [release](https://github.com/nettstudio/fusepdf/releases/latest).


## Build source code

```
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
```

## Run executable

```
./fusepdf
```

## Package

```
make INSTALL_ROOT=<path_to_package_folder> install
```
