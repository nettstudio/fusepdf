/*
#
# FusePDF - https://nettstudio.no
#
# Copyright (c) 2021 NettStudio AS. All rights reserved.
#
#
*/

#include "fusepdf.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("FusePDF");
    QApplication::setApplicationDisplayName("FusePDF");
    QApplication::setApplicationVersion(VERSION_APP);
    QApplication::setOrganizationName("NettStudio AS");
    QApplication::setOrganizationDomain("nettstudio.no");
    FusePDF w;
    w.show();
    return a.exec();
}
