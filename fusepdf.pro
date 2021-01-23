TARGET = FusePDF
VERSION = 1.0.0
TEMPLATE = app

QT += core gui widgets
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += VERSION_APP=\\\"$${VERSION}\\\"

QMAKE_TARGET_COMPANY = "NettStudio AS"
QMAKE_TARGET_PRODUCT = "FusePDF"
QMAKE_TARGET_DESCRIPTION = "Merge multiple PDF documents"
QMAKE_TARGET_COPYRIGHT = "NettStudio AS"
win32: RC_ICONS += icons/fusepdf.ico

SOURCES += main.cpp fusepdf.cpp
HEADERS += fusepdf.h
FORMS += fusepdf.ui
RESOURCES += fusepdf.qrc
OTHER_FILES += fusepdf.sh icons/mkico.sh README.md

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
