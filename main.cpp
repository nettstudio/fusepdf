#include "fusepdf.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("FusePDF");
    QApplication::setApplicationDisplayName("FusePDF");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("NettStudio AS");
    QApplication::setOrganizationDomain("nettstudio.no");
    FusePDF w;
    w.show();
    return a.exec();
}
