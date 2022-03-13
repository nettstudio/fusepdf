# Introduction

FusePDF is an simple cross-platform and open source application used for merging, splitting and extracting PDF pages and documents.

![Screenshot of main interface](https://github.com/nettstudio/fusepdf/raw/main/assets/fusepdf-screenshot.png "Screenshot of main interface")

## Features

* Drag-and-drop any PDF document
* Sort documents
* Preview pages
* Exclude/Include pages
* Export to images or PDF
* PDF presets
* PDF compatibility up to 1.7
* Change or add PDF meta data (properties)

# ChangeLog

## 2.1.0

Released : TBA

  * Added support for i18n
  * Added Norwegian translation
  * Added documentation
  * Updated code base for Qt 5.15 (fix warnings/deprecated)
  * Export image bug fix (check format compatibility before export)
  * Windows release updated to Ghostscript 9.55
  * Windows release updated to Qt 5.12.12
  * Windows release now uses MSVC 2019

## 2.0.0

Released : Jun 2, 2021.

  * Improved UI/UX
  * Preview pages
  * Include/Exclude pages
  * Bug fixes
  * More options
  * Improved macOS support
  * Export page(s) to image(s)

## 1.2.0

Released : Apr 13, 2021.

  * Improved Linux support
  * Change license to GPLv3
  * Improved UI

## 1.1.0

Released : Jan 26, 2021.

  * Support for meta/properties data
  * More options
  * Improved UX

## 1.0.0

Released : Jan 24, 2021.

  * Initial release

# Open Source

## Introduction

FusePDF requires Qt5 (Core/Gui/Widgets/Concurrent) to build, and Ghostscript (``gs``) available during runtime.

## Dependencies

Instructions for installing the needed dependencies on various platforms.

### Debian/Ubuntu (and compatible)

``sudo apt install ghostscript qt5-default qtbase5-dev qttools5-dev-tools``

### Fedora/CentOS/RHEL (and compatible)

``sudo yum install ghostscript qt5-qtbase-devel qt5-qttools``

### macports

``sudo port install ghostscript qt5-qtbase qt5-qttools``

### FreeBSD

``sudo pkg install ghostscript9-agpl-base qt5``

## Download

Download the latest (``main``) source code from GitHub:

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
git submodule update -i
```

We also have stable branches (may contain fixes not available in releases):

 * ``v1.0``
 * ``v2.0``

```
git checkout vX.Y
```

Or download the latest stable tarball [release](https://github.com/nettstudio/fusepdf/releases/latest).

## Building

In the FusePDF source folder run the following commands:

```
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
```

## Package

If you want to package the build (highly recommended) run the following command (remember to replace the output folder):

```
make INSTALL_ROOT=<path_to_package_folder> install
```

# License

## FusePDF

Copyright &copy; 2021, 2022 [NettStudio AS](https://nettstudio.no). All rights reserved.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

Translations (i18n) are Creative Commons Zero v1.0 Universal.

The Creative Commons CC0 Public Domain Dedication waives copyright interest in a work you've created and dedicates it to the world-wide public domain. Use CC0 to opt out of copyright entirely and ensure your work has the widest reach. As with the Unlicense and typical software licenses, CC0 disclaims warranties. CC0 is very similar to the Unlicense.

## Qt

Copyright &copy; 2020 The Qt Company Ltd and other contributors.

Qt and the Qt logo are trademarks of The Qt Company Ltd.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

## GPL Ghostscript

Copyright &copy; 2000-2021 Artifex Software, Inc.  All rights reserved.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

