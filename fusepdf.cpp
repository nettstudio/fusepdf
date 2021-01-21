#include "fusepdf.h"
#include "ui_fusepdf.h"

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
{
    ui->setupUi(this);
}

FusePDF::~FusePDF()
{
    delete ui;
}

