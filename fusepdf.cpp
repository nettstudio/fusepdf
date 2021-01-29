/*
#
# FusePDF - https://nettstudio.no
#
# Copyright (c) 2021 NettStudio AS. All rights reserved.
#
#
*/

#include "fusepdf.h"
#include "ui_fusepdf.h"

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/fusepdf.png"));
    qApp->setStyle(QStyleFactory::create("fusion"));

    QPalette mainPalette = qApp->palette();
    mainPalette.setColor(QPalette::Highlight, QColor(203,9,0)); // #cb0900
    mainPalette.setColor(QPalette::Link, QColor(203,9,0));
    qApp->setPalette(mainPalette);

    QPalette treePalette = ui->inputs->palette();
    treePalette.setColor(QPalette::Highlight, QColor(124,124,124)); // #7c7c7c
    ui->inputs->setPalette(treePalette);
    ui->inputs->header()->setVisible(true); // designer bug

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    ui->toolBar->addWidget(spacer);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(ui->presetWidget);
    //ui->toolBar->addSeparator();
    ui->toolBar->addWidget(ui->compatWidget);

    ui->metaTitleLabel->setToolTip(tr("Set document title"));
    ui->metaTitle->setToolTip(ui->metaTitleLabel->toolTip());
    ui->metaAuthorLabel->setToolTip(tr("Set document author"));
    ui->metaAuthor->setToolTip(ui->metaAuthorLabel->toolTip());
    ui->metaSubjectLabel->setToolTip(tr("Set document subject"));
    ui->metaSubject->setToolTip(ui->metaSubjectLabel->toolTip());
    ui->presetLabel->setToolTip(tr("Distiller presets\n\n"
                                   "- DEFAULT: selects output intended to be useful across a wide variety of uses, possibly at the expense of a larger output file.\n"
                                   "- PREPRESS: selects output similar to Acrobat Distiller \"Prepress Optimized\" (up to version X) setting.\n"
                                   "- EBOOK: selects medium-resolution output similar to the Acrobat Distiller (up to version X) \"eBook\" setting.\n"
                                   "- SCREEN: selects low-resolution output similar to the Acrobat Distiller (up to version X) \"Screen Optimized\" setting.\n"
                                   "- PRINTER: selects output similar to the Acrobat Distiller \"Print Optimized\" (up to version X) setting."));
    ui->preset->setToolTip(ui->presetLabel->toolTip());
    ui->compatLabel->setToolTip(tr("Select the PDF version this document should be compatible with."));
    ui->compat->setToolTip(ui->compatLabel->toolTip());
    //ui->dpiCheck->setToolTip(tr("Override resolution for pattern fills, for fonts that must be converted to bitmaps\n and any other rendering required (eg rendering transparent pages for output to PDF versions < 1.4). "));
    //ui->dpi->setToolTip(ui->dpiCheck->toolTip());
    ui->inputs->setToolTip(tr("Drag and drop PDF documents you want to merge here. You can re-arrange after adding them (if sorting is disabled).\n\n"
                              "Note that the first document will define the paper size on the final output.\n\n"
                              "You can remove a document with the DEL key."));

    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->inputs);
    connect(deleteShortcut, SIGNAL(activated()),
            this, SLOT(deleteDocumentItem()));

    ui->progressBar->setMaximumWidth(100);
    ui->statusBar->addPermanentWidget(ui->progressBar);

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
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("Open document(s)"),
                                                      !_lastLoadDir.isEmpty()?_lastLoadDir:QDir::homePath(),
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
    QString file = QFileDialog::getSaveFileName(this,
                                                tr("Save document"),
                                                !_lastSaveDir.isEmpty()?_lastSaveDir:QDir::homePath(),
                                                "*.pdf");
    if (file.isEmpty()) { return; }
    if (!file.endsWith(".pdf", Qt::CaseInsensitive)) { file.append(".pdf"); }
    runCommand(file);
}

void FusePDF::on_actionClear_triggered()
{
    clearInput();
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

const QString FusePDF::makeCommand(const QString &filename)
{
    if (filename.isEmpty()) { return QString(); }

    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(" -sDEVICE=pdfwrite");
    if (!ui->compat->currentText().isEmpty() && ui->compat->currentText() != "default") {
        command.append(QString(" -dCompatibilityLevel=%1").arg(ui->compat->currentText()));
    }
    command.append(QString(" -dPDFSETTINGS=/%1").arg(ui->preset->currentText().toLower()));
    command.append(" -dNOPAUSE -dBATCH -dDetectDuplicateImages -dCompressFonts=true");
    /*if (ui->dpiCheck->isChecked()) {
        command.append(QString(" -r%1").arg(ui->dpi->value()));
    }*/
    command.append(QString(" -sOutputFile=\"%1\"").arg(filename));
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        command.append(QString(" \"%1\"").arg(ui->inputs->topLevelItem(i)->text(1)));
    }

    QString title = ui->metaTitle->text();
    QString subject = ui->metaSubject->text();
    QString author = ui->metaAuthor->text();

    QString marks;
    marks.append("/Creator(FusePDF - nettstudio.no)");

    if (!title.isEmpty()) { marks.append(QString("/Title<%1>").arg(QString(toUtf16Hex(title)).toUpper())); }
    if (!subject.isEmpty()) { marks.append(QString("/Subject<%1>").arg(QString(toUtf16Hex(subject)).toUpper())); }
    if (!author.isEmpty()) { marks.append(QString("/Author<%1>").arg(QString(toUtf16Hex(author)).toUpper())); }

    command.append(QString(" -c \"[%1/DOCINFO pdfmark\"").arg(marks));

    qDebug() << command;
    return command;
}

void FusePDF::runCommand(const QString &filename)
{
    if (missingGhost()) { return; }

    if (ui->inputs->topLevelItemCount() == 0 || filename.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Unable to process"),
                             tr("Input and/or output is missing."));
        return;
    }
    if (hasFile(filename)) {
        QMessageBox::warning(this,
                             tr("Unable to save file"),
                             tr("Unable to save to file, the output file is found in input."));
        return;
    }
    if (QFile::exists(filename)) {
        int ret = QMessageBox::question(this,
                                        tr("File exists"),
                                        tr("File already exists, are you sure you want to overwrite it?"));
        if (ret != QMessageBox::Yes) { return; }
    }
    if (_proc->isOpen()) {
        QMessageBox::warning(this,
                             tr("Still active"),
                             tr("FusePDF process is still active, please wait until done."));
        return;
    }

    QString command = makeCommand(filename);
    if (command.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Failed to make process"),
                             tr("Failed to make process, this should not happen, please contact support."));
        return;
    }
    _output = filename;
    _proc->start(command);
}

void FusePDF::commandStarted()
{
    ui->cmd->clear();
    ui->actionSave->setDisabled(true);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
}

void FusePDF::commandFinished(int exitCode)
{
    ui->actionSave->setDisabled(false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    _proc->close();

    if (exitCode == 0) {
        QFileInfo fileInfo(_output);
        if (_lastSaveDir != fileInfo.absoluteDir().absolutePath()) {
            _lastSaveDir = fileInfo.absoluteDir().absolutePath();
        }
        if (ui->actionOpen_saved_PDF->isChecked()) {
            QDesktopServices::openUrl(_output);
        }
        return;
    }

    QMessageBox::warning(this,
                         tr("Failed to save PDF"),
                         tr("Failed to save PDF, see log (CTRL+L) for more information"));
}

void FusePDF::populateUI()
{
    ui->compat->clear();
    QIcon docIcon(":/icons/document.png");
    //ui->compat->addItem(docIcon, "default");
    ui->compat->addItem(docIcon, "1.0");
    ui->compat->addItem(docIcon, "1.1");
    ui->compat->addItem(docIcon, "1.2");
    ui->compat->addItem(docIcon, "1.3");
    ui->compat->addItem(docIcon, "1.4");
    ui->compat->addItem(docIcon, "1.5");
    ui->compat->addItem(docIcon, "1.6");
    ui->compat->addItem(docIcon, "1.7");

    ui->preset->clear();
    ui->preset->addItem(docIcon, "Default");
    ui->preset->addItem(docIcon, "Prepress");
    ui->preset->addItem(docIcon, "eBook");
    ui->preset->addItem(docIcon, "Screen");
    ui->preset->addItem(docIcon, "Printer");
}

void FusePDF::loadSettings()
{
    if (isNewVersion()) { on_actionAbout_triggered(); }
    if (!hasWindowState()) { setGeometry(100, 100, 700, 250); }

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
    ui->cmd->appendPlainText(log.simplified());
    ui->statusBar->showMessage(log.simplified(), 1000);
    qDebug() << log;
}

void FusePDF::clearInput()
{
    ui->inputs->clear();
    ui->cmd->clear();
    _output.clear();
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
    ui->cmd->setVisible(ui->actionShow_log->isChecked());
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
        item->setIcon(0, QIcon(":/icons/fusepdf-document.png"));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled|Qt::ItemNeverHasChildren);
        if (!info.absolutePath().isEmpty()) { _lastLoadDir = info.absolutePath(); }
    }
}

void FusePDF::on_inputs_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (!item) { return; }
    QString file = item->text(1);
    if (!QFile::exists(file)) { return; }
    QDesktopServices::openUrl(QUrl::fromUserInput(file));
}

void FusePDF::on_actionAuto_Sort_triggered()
{
    ui->inputs->setSortingEnabled(ui->actionAuto_Sort->isChecked());
}

bool FusePDF::hasFile(const QString &file)
{
    if (file.isEmpty()) { return false; }
    QFileInfo fileInfo(file);
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        QFileInfo inputInfo(ui->inputs->topLevelItem(i)->text(1));
        if (inputInfo.absoluteFilePath() == fileInfo.absoluteFilePath()) { return  true; }
    }
    return false;
}

void FusePDF::loadOptions()
{
    populateUI();

    QSettings settings;
    settings.beginGroup("options");
    //ui->dpi->setValue(settings.value("dpi", 720).toInt());
    ui->compat->setCurrentText(settings.value("compat", "1.5").toString());
    ui->preset->setCurrentText(settings.value("preset", "Default").toString());
    ui->actionShow_log->setChecked(settings.value("showLog", false).toBool());
    ui->actionAuto_Sort->setChecked(settings.value("autoSort", false).toBool());
    ui->actionRemember_meta_author->setChecked(settings.value("metaAuthor", true).toBool());
    ui->actionRemember_meta_subject->setChecked(settings.value("metaSubject", true).toBool());
    ui->actionRemember_meta_title->setChecked(settings.value("metaTitle", true).toBool());
    _lastLoadDir = settings.value("lastLoadDir", "").toString();
    _lastSaveDir = settings.value("lastSaveDir", "").toString();
    //ui->dpiCheck->setChecked(settings.value("checkdpi", false).toBool());
    ui->actionOpen_saved_PDF->setChecked(settings.value("openSavedPDF", true).toBool());
    ui->cmd->setVisible(ui->actionShow_log->isChecked());
    ui->inputs->setSortingEnabled(ui->actionAuto_Sort->isChecked());
    if (ui->actionRemember_meta_author->isChecked()) {
        QString author = settings.value("metaAuthorText").toString();
        if (!author.isEmpty()) { ui->metaAuthor->setText(author); }
    }
    if (ui->actionRemember_meta_subject->isChecked()) {
        QString subject = settings.value("metaSubjectText").toString();
        if (!subject.isEmpty()) { ui->metaSubject->setText(subject); }
    }
    if (ui->actionRemember_meta_title->isChecked()) {
        QString title = settings.value("metaTitleText").toString();
        if (!title.isEmpty()) { ui->metaTitle->setText(title); }
    }
    settings.endGroup();

    missingGhost();
}

void FusePDF::saveOptions()
{
    QSettings settings;
    settings.beginGroup("options");
    //settings.setValue("dpi", ui->dpi->value());
    if (!ui->compat->currentText().isEmpty()) {
        settings.setValue("compat", ui->compat->currentText());
    }
    if (!ui->preset->currentText().isEmpty()) {
        settings.setValue("preset", ui->preset->currentText());
    }
    settings.setValue("showLog", ui->actionShow_log->isChecked());
    settings.setValue("autoSort", ui->actionAuto_Sort->isChecked());
    //settings.setValue("checkdpi", ui->dpiCheck->isChecked());
    settings.setValue("openSavedPDF", ui->actionOpen_saved_PDF->isChecked());
    settings.setValue("metaAuthor", ui->actionRemember_meta_author->isChecked());
    settings.setValue("metaSubject", ui->actionRemember_meta_subject->isChecked());
    settings.setValue("metaTitle", ui->actionRemember_meta_title->isChecked());
    if (ui->actionRemember_meta_author->isChecked()) {
        settings.setValue("metaAuthorText", ui->metaAuthor->text());
    }
    if (ui->actionRemember_meta_subject->isChecked()) {
        settings.setValue("metaSubjectText", ui->metaSubject->text());
    }
    if (ui->actionRemember_meta_title->isChecked()) {
        settings.setValue("metaTitleText", ui->metaTitle->text());
    }
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
    _proc->close();
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

void FusePDF::deleteDocumentItem()
{
    if (ui->inputs->topLevelItemCount() == 0 || ui->inputs->currentItem() == nullptr) { return; }
    delete ui->inputs->currentItem();
}
