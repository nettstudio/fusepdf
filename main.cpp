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
