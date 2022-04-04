#
# FusePDF - https://fusepdf.no
#
# Copyright (c) 2021, 2022 NettStudio AS <https://nettstudio.no>.
# All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#

unix:!mac {
    TARGET = fusepdf
} else {
    TARGET = FusePDF
}
VERSION = 2.2.0
TEMPLATE = app

QT += core gui widgets concurrent
CONFIG += c++11

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
isEmpty(PREFIX): PREFIX = /usr/local

CONFIG(FORCE_NORWEGIAN): DEFINES += FORCE_NORWEGIAN

DEFINES += VERSION_APP=\\\"$${VERSION}\\\"

QMAKE_TARGET_COMPANY = "NettStudio AS"
QMAKE_TARGET_PRODUCT = "FusePDF"
QMAKE_TARGET_DESCRIPTION = "Convert/Merge/Split PDF documents"
QMAKE_TARGET_COPYRIGHT = "NettStudio AS"

# convert -background transparent fusepdf.png -define icon:auto-resize=16,32,48,64,128,192,256 fusepdf.ico
win32: RC_ICONS += assets/fusepdf.ico

INCLUDEPATH += src
SOURCES += src/main.cpp src/fusepdf.cpp
HEADERS += src/fusepdf.h
FORMS += assets/fusepdf.ui
RESOURCES += fusepdf.qrc
INSTALLS += target docs desktop hicolor

desktop.path = $${PREFIX}/share/applications
desktop.files = assets/fusepdf.desktop
hicolor.path = $${PREFIX}/share/icons
hicolor.files = assets/hicolor
docs.path = $${PREFIX}/share/doc/$${TARGET}-$${VERSION}
docs.files = docs/README.pdf
target.path = $${PREFIX}/bin

exists(i18n/fusepdf.pri): include(i18n/fusepdf.pri)
