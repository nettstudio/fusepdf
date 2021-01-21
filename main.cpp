#include "fusepdf.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FusePDF w;
    w.show();
    return a.exec();
}
