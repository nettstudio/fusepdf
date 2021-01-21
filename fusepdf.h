#ifndef FUSEPDF_H
#define FUSEPDF_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FusePDF; }
QT_END_NAMESPACE

class FusePDF : public QMainWindow
{
    Q_OBJECT

public:
    FusePDF(QWidget *parent = nullptr);
    ~FusePDF();

private:
    Ui::FusePDF *ui;
};
#endif // FUSEPDF_H
