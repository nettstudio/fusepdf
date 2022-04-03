# Merge PDF documents

FusePDF is an simple cross-platform and open source application used for converting, merging and splitting PDF documents.

![Screenshot of main interface](https://github.com/nettstudio/fusepdf/raw/main/docs/fusepdf-screenshot-01.png "Screenshot of main interface")

* Drag-and-drop any PDF document
* Preview input and output pages
* Exclude/Include pages
* Export to images (TIFF/PNG)
* Modify meta data (properties)
* Supports common distiller presets

## Supported PDF versions

FusePDF supports the most common Portable Document Format (PDF) versions and standards.

* PDF 1.0 (Acrobat 1.0)
* PDF 1.1 (Acrobat 2.0)
* PDF 1.2 (Acrobat 3.0)
* PDF 1.3 (Acrobat 4.0)
* PDF 1.4 (Acrobat 5.0)
* PDF 1.5 (Acrobat 6.0)
* PDF 1.6 (Acrobat 7.0)
* PDF 1.7 (Acrobat 8.0 / ISO 32000-1)
* PDF 2.0 (ISO 32000-2)

PDF/A is an ISO-standardized version of the Portable Document Format (PDF) specialized for use in the archiving and long-term preservation of electronic documents.

* PDF/A-1 (ISO 19005-1)
* PDF/A-2 (ISO 19005-2)
* PDF/A-3 (ISO 19005-3)

# Basic usage

For the most basic usage we assume you have a couple of PDF documents you want to merge into one.

You can add PDF documents in various ways, either by drag-and-drop or with the ``Open`` action in the toolbar, right-click and ``Add`` or in the ``File`` menu (``Ctrl+O``). You can select multiple documents with the open dialog, and of course drag-and-drop multiple documents.

![Screenshot of main interface with document added](https://github.com/nettstudio/fusepdf/raw/main/docs/fusepdf-screenshot-02.png "Screenshot of main interface with document added")

You can move documents up and down, and remove with the ``DEL`` key or right-click ``Remove``.

That's it, if you are satisfied click ``Save`` (or ``Ctrl+S``) and you are done. As default the generated PDF will open in your default PDF viewer.

# Excluding pages

Sometimes you may want to exclude some pages from a document.

Double-click the document or select the tab with the document you want to modify.

![Screenshot of document interface](https://github.com/nettstudio/fusepdf/raw/main/docs/fusepdf-screenshot-03.png "Screenshot of document interface")

Enabled pages are marked as green. If you want to exclude a page just double-click it. You can also select all or none through the context menu (right-click).

# Issues

Some common issues users may experience.

## Missing Ghostscript

![Screenshot of missing Ghostscript](https://github.com/nettstudio/fusepdf/raw/main/docs/fusepdf-screenshot-08.png "Screenshot of missing Ghostscript")

If you see this dialog your installation is broken, or you are using a version of FusePDF shipped without Ghostscript. On Windows follow the instructions or download a FusePDF version with Ghostscript included. On macOS/Linux/BSD and others make sure Ghostscript (``gs``) is installed and available in path, check your package manager for more information.

For macOS [Richard Koch](https://pages.uoregon.edu/koch/) hosts Ghostscript binaries that will work with FusePDF. Else use [macports](https://ports.macports.org/port/ghostscript/) or [homebrew](https://formulae.brew.sh/formula/ghostscript) to install Ghostscript.

# Roadmap

See https://github.com/nettstudio/fusepdf/issues.

# ChangeLog

## 2.2.0

Released : ETA Apr 2022.

* Preview update bug fix
* Updated UI
* Set document properties as tooltip
* Can now save to PDF 2.0 (ISO 32000-2)
* Can now save to PDF/A
  * PDF/A-1 (ISO 19005-1)
  * PDF/A-2 (ISO 19005-2)
  * PDF/A-3 (ISO 19005-3)

## 2.1.0

Released : Apr 1, 2022.

  * Check total pages before export (bug fix)
  * Context menu in output
  * Real time output preview (default on, can be disabled)
  * UI changes
  * Code changes
  * Support dark mode on Windows (can be disabled)
  * Added support for translations (i18n)
  * Added basic documentation (not complete)
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

FusePDF targets Windows as it's primary platform, but since it's open source it can be built and run on most platforms available today. We check compatibility with macOS and Linux and rely on community feedback for more exotic platform. If Qt5 and Ghostscript works on your platform, FusePDF will too.

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

Developed by Ole-André Rodlie for NettStudio AS.

Copyright &copy; 2021, 2022 [NettStudio AS](https://nettstudio.no). All rights reserved.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

Translations (i18n) are Creative Commons Zero v1.0 Universal.

The Creative Commons CC0 Public Domain Dedication waives copyright interest in a work you've created and dedicates it to the world-wide public domain. Use CC0 to opt out of copyright entirely and ensure your work has the widest reach. As with the Unlicense and typical software licenses, CC0 disclaims warranties. CC0 is very similar to the Unlicense.

## Qt

Source code:

 * [qtbase](https://mirrors.dotsrc.org/qtproject/archive/qt/5.12/5.12.12/submodules/qtbase-everywhere-src-5.12.12.zip) 5.12.12
 * [qttools](https://mirrors.dotsrc.org/qtproject/archive/qt/5.12/5.12.12/submodules/qttools-everywhere-src-5.12.12.zip) 5.12.12

Copyright &copy; 2020 The Qt Company Ltd and other contributors.

Qt and the Qt logo are trademarks of The Qt Company Ltd.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

## GPL Ghostscript

Source code:

 * [Ghostscript](https://github.com/ArtifexSoftware/ghostpdl-downloads/releases/download/gs9550/ghostscript-9.55.0.tar.xz) 9.55

Copyright &copy; 2000-2021 Artifex Software, Inc.  All rights reserved.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

