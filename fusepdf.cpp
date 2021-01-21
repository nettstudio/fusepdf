#include "fusepdf.h"
#include "ui_fusepdf.h"

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(Q_NULLPTR)
{
    ui->setupUi(this);
    _proc = new QProcess(this);
    connect(_proc, SIGNAL(finished(int)),
            this, SLOT(commandFinished(int)));
    connect(_proc, SIGNAL(started()),
            this, SLOT(commandStarted()));
    connect(ui->inputs, SIGNAL(addedItem()),
            this, SLOT(handleAddedItem()));
    loadSettings();
}

FusePDF::~FusePDF()
{
    saveSettings();
    delete ui;
}


void FusePDF::on_actionOpen_triggered()
{
    qDebug() << "on_actionOpen_triggered";
}

void FusePDF::on_actionSave_triggered()
{
    qDebug() << "on_actionSave_triggered";
}

void FusePDF::on_actionClear_triggered()
{
    qDebug() << "on_actionClear_triggered";
}

void FusePDF::on_actionQuit_triggered()
{
    qDebug() << "on_actionQuit_triggered";
}

void FusePDF::on_actionAbout_triggered()
{
    qDebug() << "on_actionAbout_triggered";
}

void FusePDF::on_paper_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_paper_currentIndexChanged" << arg1;
}

void FusePDF::on_preset_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_preset_currentIndexChanged" << arg1;
}

void FusePDF::on_compat_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_compat_currentIndexChanged" << arg1;
}

void FusePDF::on_dpi_valueChanged(int arg1)
{
    qDebug() << "on_dpi_valueChanged" << arg1;
}

void FusePDF::on_fileButton_clicked()
{
    qDebug() << "on_fileButton_clicked";
}

void FusePDF::on_clear_clicked()
{
    qDebug() << "on_clear_clicked";
}

void FusePDF::makeCommand()
{
    qDebug() << "makeCommand";
}

void FusePDF::runCommand()
{
    qDebug() << "runCommand";
}

void FusePDF::commandStarted()
{
    qDebug() << "commandStarted";
}

void FusePDF::commandFinished(int exitCode)
{
    Q_UNUSED(exitCode)
    qDebug() << "commandfinished" << exitCode;
}

void FusePDF::populateUI()
{
    qDebug() << "populateUI";
}

void FusePDF::loadSettings()
{
    qDebug() << "loadSettings";
}

void FusePDF::saveSettings()
{
    qDebug() << "saveSettings";
}

void FusePDF::on_dpiCheck_stateChanged(int arg1)
{
    qDebug() << "on_dpiCheck_stateChanged" << arg1;
}

void FusePDF::on_save_clicked()
{
    qDebug() << "on_save_clicked";
}

void FusePDF::handleAddedItem()
{
    qDebug() << "handleAddedItem";
}
