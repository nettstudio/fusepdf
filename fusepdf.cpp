#include "fusepdf.h"
#include "ui_fusepdf.h"

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(nullptr)
{
    ui->setupUi(this);
    qApp->setStyle(QStyleFactory::create("fusion"));
    setWindowIcon(QIcon(":/fusepdf.png"));

    _proc = new QProcess(this);

    connect(_proc, SIGNAL(finished(int)),
            this, SLOT(commandFinished(int)));
    connect(_proc, SIGNAL(started()),
            this, SLOT(commandStarted()));
    connect(_proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleProcOutput()));
    connect(_proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));
    connect(ui->inputs, SIGNAL(foundPDF(QList<QUrl>)),
            this, SLOT(handleFoundPDF(QList<QUrl>)));

    loadSettings();
}

FusePDF::~FusePDF()
{
    saveSettings();
    delete ui;
}


void FusePDF::on_actionOpen_triggered()
{
    QString dir = QDir::homePath();
    if (!_lastLoadDir.isEmpty()) { dir = _lastLoadDir; }
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Open document(s)"),
                                                      dir,
                                                      "*.pdf");
    if (files.size() > 0) {
        QList<QUrl> urls;
        for (int i = 0; i < files.size(); ++i) {
            urls.append(QUrl::fromUserInput(files.at(i)));
        }
        if (urls.size() > 0) { handleFoundPDF(urls); }
    }
}

void FusePDF::on_actionSave_triggered()
{
    runCommand();
}

void FusePDF::on_actionClear_triggered()
{
    clearAll();
}

void FusePDF::on_actionQuit_triggered()
{
    qApp->quit();
}

void FusePDF::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("FusePDF"),
                       tr("<h2>FusePDF %1</h2>"
                          "<p>Copyright &copy;2021 <a href='https://nettstudio.no'>NettStudio AS</a>. All rights reserved.</p>"
                          "<p style=\"font-size:small;\">Permission is hereby granted, free of charge, to any person obtaining a copy of FusePDF and associated documentation files from NettStudio AS, to use FusePDF for personal or commercial usage provided that the following conditions are met:</p>"
                          "<p style=\"font-size:small;\">1. You are not allowed to modify, merge, publish, (re-)distribute, sublicense, and/or sell copies of FusePDF.</p>"
                          "<p style=\"font-size:small;\">2. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.</p>"
                          "<p style=\"font-size:small;\">THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</p>"
                          ).arg(VERSION_APP));
}

void FusePDF::on_paper_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    makeCommand();
}

void FusePDF::on_preset_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    makeCommand();
}

void FusePDF::on_compat_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    makeCommand();
}

void FusePDF::on_dpi_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    makeCommand();
}

void FusePDF::on_dpiCheck_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    makeCommand();
}

void FusePDF::on_fileButton_clicked()
{
    QString dir = QDir::homePath();
    if (!ui->fileName->text().isEmpty()) {
        QFileInfo info(ui->fileName->text());
        dir = info.absolutePath();
    } else { if (!_lastSaveDir.isEmpty()) { dir = _lastSaveDir; } }

    QString file = QFileDialog::getSaveFileName(this, tr("Save PDF"), dir, "*.pdf");
    if (file.isEmpty()) { return; }
    if (!file.endsWith(".pdf")) { file.append(".pdf"); }

    ui->fileName->setText(file);
    QFileInfo saveFile(file);
    _lastSaveDir = saveFile.absolutePath();

    makeCommand();
}

void FusePDF::on_clear_clicked()
{
    clearAll();
}

void FusePDF::on_save_clicked()
{
    runCommand();
}

void FusePDF::makeCommand()
{
    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
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
    _cmd = command;

    ui->cmd->setPlainText(_cmd);
    qDebug() << _cmd;
}

void FusePDF::runCommand()
{
    qDebug() << "runCommand";
    if (missingGhost()) { return; }

    if (ui->inputs->topLevelItemCount() == 0 || ui->fileName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Unable to process"), tr("Input and/or output is missing."));
        return;
    }
    if (hasFile(ui->fileName->text())) {
        QMessageBox::warning(this, tr("Unable to save file"), tr("Unable to save to file, the output file is found in input."));
        return;
    }
    if (QFile::exists(ui->fileName->text())) {
        int ret = QMessageBox::question(this,
                                        tr("File exists"),
                                        tr("File already exists, are you sure you want to overwrite it?"));
        if (ret != QMessageBox::Yes) { return; }
    }
    if (_proc->isOpen()) {
        QMessageBox::warning(this, tr("Still active"), tr("FusePDF process is still active, please wait until done."));
        return;
    }

    makeCommand();
    _proc->start(_cmd);
}

void FusePDF::commandStarted()
{
    qDebug() << "commandStarted";

    ui->save->setDisabled(true);
    ui->actionSave->setDisabled(true);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
}

void FusePDF::commandFinished(int exitCode)
{
    qDebug() << "commandfinished" << exitCode;

    ui->save->setDisabled(false);
    ui->actionSave->setDisabled(false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    _proc->close();

    if (exitCode == 0) {
        QDesktopServices::openUrl(ui->fileName->text());
        return;
    }

    QMessageBox::warning(this,
                         tr("Failed to save PDF"),
                         tr("Failed to save PDF, see log (CTRL+L) for more information"));
}

void FusePDF::populateUI()
{
    ui->compat->clear();
    ui->compat->addItem("1.0");
    ui->compat->addItem("1.1");
    ui->compat->addItem("1.2");
    ui->compat->addItem("1.3");
    ui->compat->addItem("1.4");
    ui->compat->addItem("1.5");
    ui->compat->addItem("1.6");
    ui->compat->addItem("1.7");

    ui->paper->clear();
    ui->paper->addItem("default");
    ui->paper->addItem("letter");
    ui->paper->addItem("legal");
    ui->paper->addItem("a1");
    ui->paper->addItem("a2");
    ui->paper->addItem("a3");
    ui->paper->addItem("a4");

    ui->preset->clear();
    ui->preset->addItem("default");
    ui->preset->addItem("prepress");
    ui->preset->addItem("ebook");
    ui->preset->addItem("screen");
    ui->preset->addItem("printer");
}

void FusePDF::loadSettings()
{
    if (isNewVersion()) { on_actionAbout_triggered(); }
    if (!hasWindowState()) { setGeometry(100, 100, 700, 300); }

    QSettings settings;
    settings.beginGroup("ui");
    QByteArray lastGeo = settings.value("geometry").toByteArray();
    if (!lastGeo.isNull()) {
        restoreGeometry(lastGeo);
        QApplication::processEvents();
    }
    QByteArray lastState = settings.value("state").toByteArray();
    if (!lastState.isNull()) {
        restoreState(lastState);
        QApplication::processEvents();
    }
    bool wasMax = settings.value("maximized", false).toBool();
    if (wasMax) { showMaximized(); }
    settings.endGroup();

    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    loadOptions();
}

void FusePDF::saveSettings()
{
    saveOptions();

    QSettings settings;
    settings.beginGroup("ui");
    settings.setValue("state", saveState());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
    settings.sync();
}

void FusePDF::handleProcOutput()
{
    QString log;
    log.append(_proc->readAllStandardError());
    log.append(_proc->readAllStandardOutput());
    ui->cmd->appendPlainText(log);
    qDebug() << log;
}

void FusePDF::clearAll()
{
    ui->inputs->clear();
    ui->cmd->clear();
    ui->fileName->clear();
    _cmd.clear();
    loadOptions();
}

const QString FusePDF::findGhost()
{
#ifdef Q_OS_WIN
    QString programFilesPath(getenv("PROGRAMFILES"));
    QDirIterator it(programFilesPath + "/gs", QStringList() << "*.*", QDir::Dirs/*, QDirIterator::Subdirectories*/);
    while (it.hasNext()) {
        QString folder = it.next();
        QString bin64 = folder + "/bin/gswin64c.exe";
        if (QFile::exists(bin64)) { return bin64; }
        QString bin32 = folder + "/bin/gswin32c.exe";
        if (QFile::exists(bin32)) { return bin32; }
    }
    return QString();
#endif
    return QString("gs");
}

void FusePDF::on_actionShow_log_triggered()
{
    ui->logBox->setVisible(ui->actionShow_log->isChecked());
}

void FusePDF::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

bool FusePDF::hasWindowState()
{
    return false;
}

void FusePDF::handleFoundPDF(const QList<QUrl> &urls)
{
    for (int i=0;i< urls.size();++i) {
        const QFileInfo info(urls.at(i).toLocalFile());
        if (!info.isFile()) { continue; }
        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(info.absoluteFilePath());
        if (type.name() != "application/pdf") { continue; }
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->inputs);
        item->setText(0, info.fileName());
        item->setText(1, info.filePath());
        item->setIcon(0, QIcon(":/fusepdf-notext.png"));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled|Qt::ItemNeverHasChildren);

        if (ui->fileName->text().isEmpty()) {
            QString outputFile = info.absolutePath() + "/FusePDF-output.pdf";
            QFileInfo outputInfo(outputFile);
            if (!QFile::exists(outputInfo.absoluteFilePath())) {
                ui->fileName->setText(outputInfo.absoluteFilePath());
            }
        }
        if (!info.absolutePath().isEmpty()) { _lastLoadDir = info.absolutePath(); }
    }
    makeCommand();
}

void FusePDF::on_inputs_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (!item) { return; }
    QDesktopServices::openUrl(QUrl::fromUserInput(item->text(1)));
}

void FusePDF::on_actionAuto_Sort_triggered()
{
    ui->inputs->setSortingEnabled(ui->actionAuto_Sort->isChecked());
}

bool FusePDF::hasFile(const QString &file)
{
    if (!file.isEmpty()) { return false; }
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        // TODO: use FileInfo!
        if (ui->inputs->topLevelItem(i)->text(1) == file) { return  true; }
    }
    return false;
}

void FusePDF::loadOptions()
{
    populateUI();

    QSettings settings;
    settings.beginGroup("options");
    ui->paper->setCurrentText(settings.value("paper", "default").toString());
    ui->dpi->setValue(settings.value("dpi", 150).toInt());
    ui->compat->setCurrentText(settings.value("compat", "1.3").toString());
    ui->preset->setCurrentText(settings.value("preset", "default").toString());
    ui->actionShow_log->setChecked(settings.value("showLog", false).toBool());
    ui->actionAuto_Sort->setChecked(settings.value("autoSort", false).toBool());
    _lastLoadDir = settings.value("lastLoadDir", "").toString();
    _lastSaveDir = settings.value("lastSaveDir", "").toString();
    ui->dpiCheck->setChecked(settings.value("checkdpi", false).toBool());
    settings.endGroup();

    ui->logBox->setVisible(ui->actionShow_log->isChecked());
    ui->inputs->setSortingEnabled(ui->actionAuto_Sort->isChecked());

    missingGhost();
}

void FusePDF::saveOptions()
{
    QSettings settings;
    settings.beginGroup("options");
    if (!ui->paper->currentText().isEmpty()) {
        settings.setValue("paper", ui->paper->currentText());
    }
    settings.setValue("dpi", ui->dpi->value());
    if (!ui->compat->currentText().isEmpty()) {
        settings.setValue("compat", ui->compat->currentText());
    }
    if (!ui->preset->currentText().isEmpty()) {
        settings.setValue("preset", ui->preset->currentText());
    }
    settings.setValue("showLog", ui->actionShow_log->isChecked());
    settings.setValue("autoSort", ui->actionAuto_Sort->isChecked());
    settings.setValue("checkdpi", ui->dpiCheck->isChecked());
    if (!_lastLoadDir.isEmpty()) {
        settings.setValue("lastLoadDir", _lastLoadDir);
    }
    if (!_lastSaveDir.isEmpty()) {
        settings.setValue("lastSaveDir", _lastSaveDir);
    }
    settings.endGroup();
}

bool FusePDF::isNewVersion()
{
    QSettings settings;
    if (settings.value("version") != VERSION_APP) {
        settings.setValue("version", VERSION_APP);
        return true;
    }
    return false;
}

bool FusePDF::missingGhost()
{
    if (findGhost().isEmpty()) {
        QMessageBox::warning(this,
                             tr("Missing Ghostscript"),
                             tr("Unable to find Ghostscript, please download the latest installer from <a href='https://www.ghostscript.com/download/gsdnld.html'>www.ghostscript.com</a> and install it before running FusePDF again."));
        return true;
    }
    return false;
}

void FusePDF::handleProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = tr("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program (Ghostscript).");
        break;
    case QProcess::Crashed:
        errorMsg = tr("The process crashed some time after starting successfully.");
        break;
    default:
        errorMsg = tr("An unknown error occured. For example, the process may not be running, or it may have closed its input channel.");
        break;
    }
    errorMsg.append(" See log (CTRL+L) for more information");
    QMessageBox::warning(this, tr("Process failed"), errorMsg);
}
