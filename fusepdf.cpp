#include "fusepdf.h"
#include "ui_fusepdf.h"

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(nullptr)
{
    ui->setupUi(this);
    _proc = new QProcess(this);
    connect(_proc, SIGNAL(finished(int)),
            this, SLOT(commandFinished(int)));
    connect(_proc, SIGNAL(started()),
            this, SLOT(commandStarted()));
    connect(_proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleProcOutput()));
    connect(ui->inputs, SIGNAL(addedItem()),
            this, SLOT(handleAddedItem()));
    setWindowIcon(QIcon(":/fusepdf.png"));
    if (findGhost().isEmpty()) {
        QMessageBox::warning(this, tr("Missing Ghostscript"), tr("Unable to find Ghostscript, please download the latest x86_64 installer from https://www.ghostscript.com/download/gsdnld.html"));
        QTimer::singleShot(100, qApp, SLOT(quit()));
    }
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
    on_fileButton_clicked();
}

void FusePDF::on_actionClear_triggered()
{
    qDebug() << "on_actionClear_triggered";
    clearAll();
}

void FusePDF::on_actionQuit_triggered()
{
    qDebug() << "on_actionQuit_triggered";
    qApp->quit();
}

void FusePDF::on_actionAbout_triggered()
{
    qDebug() << "on_actionAbout_triggered";
}

void FusePDF::on_paper_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_paper_currentIndexChanged" << arg1;
    makeCommand();
}

void FusePDF::on_preset_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_preset_currentIndexChanged" << arg1;
    makeCommand();
}

void FusePDF::on_compat_currentIndexChanged(const QString &arg1)
{
    qDebug() << "on_compat_currentIndexChanged" << arg1;
    makeCommand();
}

void FusePDF::on_dpi_valueChanged(int arg1)
{
    qDebug() << "on_dpi_valueChanged" << arg1;
    makeCommand();
}

void FusePDF::on_dpiCheck_stateChanged(int arg1)
{
    qDebug() << "on_dpiCheck_stateChanged" << arg1;
    makeCommand();
}

void FusePDF::on_fileButton_clicked()
{
    qDebug() << "on_fileButton_clicked";
    QString file = QFileDialog::getSaveFileName(this,
                                                tr("Save PDF"),
                                                QDir::homePath(),
                                                 "*.pdf");
    if (file.isEmpty()) {
        qDebug() << "no save file";
        return;
    }
    if (!file.endsWith(".pdf")) { file.append(".pdf"); }
    ui->fileName->setText(file);
    makeCommand();
}

void FusePDF::on_clear_clicked()
{
    qDebug() << "on_clear_clicked";
    clearAll();
}

void FusePDF::on_save_clicked()
{
    qDebug() << "on_save_clicked";
    runCommand();
}

void FusePDF::makeCommand()
{
    qDebug() << "makeCommand";
    QString command = findGhost();
    command.append(" -sDEVICE=pdfwrite");
    command.append(QString(" -dCompatibilityLevel=%1").arg(ui->compat->currentText()));
    command.append(QString(" -dPDFSETTINGS=/%1").arg(ui->preset->currentText()));
    if (!ui->paper->currentText().isEmpty() && ui->paper->currentText() != "default") {
        command.append(QString(" -sPAPERSIZE=%1").arg(ui->paper->currentText()));
    }
    command.append(" -dNOPAUSE -dBATCH -dDetectDuplicateImages -dCompressFonts=true");
    if (ui->dpiCheck->isChecked()) {
        command.append(QString(" -r%1").arg(ui->dpi->value()));
    }
    command.append(QString(" -sOutputFile=\"%1\"").arg(ui->fileName->text()));
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        command.append(QString(" \"%1\"").arg(ui->inputs->topLevelItem(i)->text(1)));
    }
    qDebug() << command;
    _cmd = command;
    ui->cmd->setPlainText(_cmd);
}

void FusePDF::runCommand()
{
    qDebug() << "runCommand";
    if (_proc->isOpen()) {
        qDebug() << "proc still running?";
        return;
    }
    makeCommand();
    _proc->start(_cmd);
}

void FusePDF::commandStarted()
{
    qDebug() << "commandStarted";
    ui->save->setDisabled(true);
    ui->progressBar->setMaximum(0);
}

void FusePDF::commandFinished(int exitCode)
{
    Q_UNUSED(exitCode)
    qDebug() << "commandfinished" << exitCode;
    ui->save->setDisabled(false);
    ui->progressBar->setMaximum(100);
    _proc->close();
    if (exitCode == 0) {
        QDesktopServices::openUrl(ui->fileName->text());
        return;
    }
    QMessageBox::warning(this,
                         tr("Failed to save PDF"),
                         tr("Failed to save PDF, see log for more information"));
}

void FusePDF::populateUI()
{
    qDebug() << "populateUI";
    ui->compat->addItem("1.0");
    ui->compat->addItem("1.1");
    ui->compat->addItem("1.2");
    ui->compat->addItem("1.3");
    ui->compat->addItem("1.4");
    ui->compat->addItem("1.5");
    ui->compat->addItem("1.6");
    ui->compat->addItem("1.7");
    ui->compat->setCurrentText("1.3");

    ui->paper->addItem("default");
    ui->paper->addItem("letter");
    ui->paper->addItem("legal");
    ui->paper->addItem("a1");
    ui->paper->addItem("a2");
    ui->paper->addItem("a3");
    ui->paper->addItem("a4");

    ui->preset->addItem("default");
    ui->preset->addItem("prepress");
    ui->preset->addItem("ebook");
    ui->preset->addItem("screen");
    ui->preset->addItem("printer");
}

void FusePDF::loadSettings()
{
    qDebug() << "loadSettings";
    populateUI();
    ui->logBox->setVisible(ui->actionShow_log->isChecked());
}

void FusePDF::saveSettings()
{
    qDebug() << "saveSettings";
}

void FusePDF::handleAddedItem()
{
    qDebug() << "handleAddedItem";
    makeCommand();
}

void FusePDF::handleProcOutput()
{
    qDebug() << "handleProcOutput";
    QString log;
    log.append(_proc->readAllStandardError());
    log.append(_proc->readAllStandardOutput());
    qDebug() << log;
    ui->cmd->appendPlainText(log);
}

void FusePDF::clearAll()
{
    qDebug() << "clearAll";
    ui->inputs->clear();
    ui->compat->clear();
    ui->cmd->clear();
    ui->dpiCheck->setChecked(false);
    ui->dpi->setValue(150);
    ui->fileName->clear();
    ui->paper->clear();
    ui->preset->clear();
    _cmd.clear();
    loadSettings();
}

const QString FusePDF::findGhost()
{
#ifdef Q_OS_WIN
    QString programFilesPath(getenv("PROGRAMFILES"));
    QDirIterator it(programFilesPath + "/gs", QStringList() << "*.*", QDir::Dirs/*, QDirIterator::Subdirectories*/);
    while (it.hasNext()) {
        QString folder = it.next();
        QString bin = folder + "/bin/gswin64c.exe";
        if (QFile::exists(bin)) { return bin; }
    }
    return QString();
#endif
    return QString("gs");
}

void FusePDF::on_actionShow_log_triggered()
{
    qDebug() << "on_actionShow_log_triggered";
    ui->logBox->setVisible(ui->actionShow_log->isChecked());
}
