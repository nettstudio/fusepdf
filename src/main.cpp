/*
#
# FusePDF - https://fusepdf.no
#
# Copyright (c) 2021 NettStudio AS <https://nettstudio.no>.
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
*/

#include "fusepdf.h"

#include <QDebug>
#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("FusePDF");
    QApplication::setApplicationDisplayName("FusePDF");
    QApplication::setApplicationVersion(VERSION_APP);
    QApplication::setOrganizationName("NettStudio AS");
    QApplication::setOrganizationDomain("nettstudio.no");

#ifdef FORCE_NORWEGIAN
    QLocale curLocale(QLocale("no"));
    QLocale::setDefault(curLocale);
#endif

    QTranslator i18n;
    i18n.load(QLocale(),
              QLatin1String("fusepdf"),
              QLatin1String("_"),
              QLatin1String(":/i18n"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    qDebug() << "Locale" << QLocale().name() << i18n.filePath();
#endif
    a.installTranslator(&i18n);

    FusePDF w;
    w.show();
    return a.exec();
}
