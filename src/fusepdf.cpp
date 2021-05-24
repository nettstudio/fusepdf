/*
#
# FusePDF - https://nettstudio.no
#
# Copyright (c) 2021 NettStudio AS. All rights reserved.
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
#include "ui_fusepdf.h"

PagesListWidget::PagesListWidget(QWidget *parent,
                                 const QString &filename,
                                 const QString &checksum,
                                 int pages):
    QListWidget(parent)
  , _filename(filename)
  , _checksum(checksum)
  , _pages(pages)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(FUSEPDF_PAGE_ICON_SIZE, FUSEPDF_PAGE_ICON_SIZE));
    setUniformItemSizes(true);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setFrameShape(QFrame::NoFrame);
    setContextMenuPolicy(Qt::CustomContextMenu);

    for (int i = 1; i <= _pages; ++i) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(FUSEPDF_ICON_LOGO),
                                                    QString::number(i),
                                                    this);
        item->setData(FUSEPDF_PAGE_ROLE, i);
        item->setCheckState(Qt::Checked);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    }

    connect(this, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(handleItemClicked(QListWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(handleContextMenu(QPoint)));
}

bool PagesListWidget::isModified() {
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->checkState() == Qt::Unchecked) { return true; }
    }
    return false;
}

QVector<int> PagesListWidget::getPagesState(Qt::CheckState state)
{
    QVector<int> result;
    if (state == Qt::PartiallyChecked) { return result; }
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->checkState() == state) {
            result.append(item->data(FUSEPDF_PAGE_ROLE).toInt());
        }
    }
    return result;
}

void PagesListWidget::setPageIcon(const QString &filename,
                                  const QString &checksum,
                                  const QString &image,
                                  int page)
{
    Q_UNUSED(checksum)
    if (filename != _filename || page > _pages || page < 1) { return; }
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->data(FUSEPDF_PAGE_ROLE).toInt() != page) { continue; }
        QPixmap pix(FUSEPDF_PAGE_ICON_SIZE, FUSEPDF_PAGE_ICON_SIZE);
        pix.fill(QColor(Qt::transparent));
        QPainter p(&pix);
        QPixmap ppix = QPixmap::fromImage(QImage(image)).scaledToHeight(FUSEPDF_PAGE_ICON_SIZE,
                                                                        Qt::SmoothTransformation);
        ppix = ppix.copy(0, 0, FUSEPDF_PAGE_ICON_SIZE, FUSEPDF_PAGE_ICON_SIZE);
        QPainter pp(&ppix);
        QPainterPath ppath;
        ppath.addRect(0, 0, ppix.width(), ppix.height());
        QPen ppen(Qt::black, 2);
        pp.setPen(ppen);
        pp.drawPath(ppath);
        p.drawPixmap((pix.width()/2)-(ppix.width()/2), 0, ppix);
        item->setIcon(pix);
        break;
    }
}

void PagesListWidget::handleItemClicked(QListWidgetItem *item)
{
    if (!item) { return; }
    if (item->checkState() == Qt::Checked) {
        item->setCheckState(Qt::Unchecked);
    } else { item->setCheckState(Qt::Checked); }
}

void PagesListWidget::handleContextMenu(QPoint pos)
{
    QMenu menu;
    QAction selectAllAction;
    QAction selectNoneAction;
    selectAllAction.setText(tr("Select all"));
    selectNoneAction.setText(tr("Select none"));
    connect(&selectAllAction, SIGNAL(triggered(bool)),
            this, SLOT(selectAllPages()));
    connect(&selectNoneAction, SIGNAL(triggered(bool)),
            this, SLOT(selectNoPages()));
    menu.addAction(&selectAllAction);
    menu.addAction(&selectNoneAction);
    menu.exec(viewport()->mapToGlobal(pos));
}

void PagesListWidget::selectAllPages()
{
    setCheckedState(Qt::Checked);
}

void PagesListWidget::selectNoPages()
{
    setCheckedState(Qt::Unchecked);
}

void PagesListWidget::setCheckedState(Qt::CheckState state)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        item->setCheckState(state);
    }
}

FilesTreeWidget::FilesTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    setSortingEnabled(false);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(false);
    setIconSize(QSize(32, 32));
}

void FilesTreeWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) { emit foundPDF(e->mimeData()->urls()); }
    else { QTreeWidget::dropEvent(e); }
}

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(FUSEPDF_ICON_LOGO));


    qApp->setStyle(QStyleFactory::create("fusion"));
    // don't set palette on macos as it breaks dark mode
#ifndef Q_OS_MAC
    QPalette mainPalette = qApp->palette();
    mainPalette.setColor(QPalette::Highlight, QColor(203,9,0)); // #cb0900
    mainPalette.setColor(QPalette::Link, QColor(203,9,0));
    qApp->setPalette(mainPalette);

    QPalette treePalette = ui->inputs->palette();
    treePalette.setColor(QPalette::Highlight, QColor(124,124,124)); // #7c7c7c
    ui->inputs->setPalette(treePalette);
#endif
    ui->inputs->header()->setVisible(true); // bypass designer bug

    ui->inputs->header()->setStretchLastSection(false);
    ui->inputs->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->inputs->header()->setSectionResizeMode(1, QHeaderView::Fixed);



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
    connect(this, SIGNAL(commandReady(QString,QString)),
            this, SLOT(runCommand(QString,QString)));
    connect(this, SIGNAL(statusMessage(QString,int)),
            statusBar(), SLOT(showMessage(QString,int)));

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
    if (ui->inputs->topLevelItemCount() == 0) {
        QMessageBox::warning(this,
                             tr("No documents"),
                             tr("No documents to merge, please add some documents before trying to save."));
        return;
    }
    QString file = QFileDialog::getSaveFileName(this,
                                                tr("Save document"),
                                                !_lastSaveDir.isEmpty()?_lastSaveDir:QDir::homePath(),
                                                "*.pdf");
    if (file.isEmpty()) { return; }
    if (!file.endsWith(".pdf", Qt::CaseInsensitive)) { file.append(".pdf"); }

    showProgress(true);
    QtConcurrent::run(this, &FusePDF::prepCommand, file);
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
                          "<p style=\"font-size:small;\">This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.</p>"
                          "<p style=\"font-size:small;\">This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.</p>"
                          "<p style=\"font-size:small;\">You should have received a copy of the GNU General Public License along with this program.  If not, see <a href=\"https://www.gnu.org/licenses\">www.gnu.org/licenses</a>.</p>"
                          "<p style=\"text-align:left;\"><a href=\"https://nettstudio.no\"><img src=\":/assets/nettstudio.png\"></a></p>"
                          ).arg(VERSION_APP));
}

void FusePDF::prepCommand(const QString &filename)
{
    QString command = makeCommand(filename);
    if (!command.isEmpty()) { emit commandReady(filename, command); }
    else {
        showProgress(false);
        QMessageBox::warning(this,
                             tr("Failed to make process"),
                             tr("Failed to make process, this should not happen, please contact support."));
    }
}

const QString FusePDF::makeCommand(const QString &filename)
{
    qDebug() << "makeCommand" << filename;
    if (filename.isEmpty()) { return QString(); }

    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(" -sDEVICE=pdfwrite");
    if (!ui->compat->currentText().isEmpty() && ui->compat->currentText().toLower() != "default") {
        command.append(QString(" -dCompatibilityLevel=%1").arg(ui->compat->currentText()));
    }
    if (!ui->preset->currentText().isEmpty() && ui->preset->currentText().toLower() != "none") {
        command.append(QString(" -dPDFSETTINGS=/%1").arg(ui->preset->currentText().toLower()));
    }
    command.append(" -dNOPAUSE -dBATCH -dDetectDuplicateImages -dCompressFonts=true");
    command.append(QString(" -sOutputFile=\"%1\"").arg(filename));
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        QString filename = ui->inputs->topLevelItem(i)->data(0, FUSEPDF_PATH_ROLE).toString();
        PagesListWidget *tab = getTab(filename);
        bool modified = false;
        if (tab && tab->isModified() && tab->getPagesState(Qt::Checked).size() > 0) {
            modified = true;
            QVector<int> pages = tab->getPagesState(Qt::Checked);
            for (int i = 0; i < pages.count(); ++i) {
                QString extracted = extractPDF(filename, getChecksum(filename), pages.at(i));
                if (!extracted.isEmpty()) {
                    command.append(QString(" \"%1\"").arg(extracted));
                }
            }
        }
        if (!modified) {
            command.append(QString(" \"%1\"").arg(filename));
        }
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

void FusePDF::runCommand(const QString &filename,
                         const QString &command)
{
    showProgress(false);
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
    showProgress(true);
}

void FusePDF::commandFinished(int exitCode)
{
    ui->actionSave->setDisabled(false);
    showProgress(false);
    _proc->close();

    if (exitCode == 0) {
        QFileInfo fileInfo(_output);
        if (_lastSaveDir != fileInfo.absoluteDir().absolutePath()) {
            _lastSaveDir = fileInfo.absoluteDir().absolutePath();
        }
        if (ui->actionOpen_saved_PDF->isChecked()) {
            QDesktopServices::openUrl(QUrl::fromUserInput(_output));
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
    QIcon docIcon(FUSEPDF_ICON_DOC);
    ui->compat->addItem(docIcon, "1.0");
    ui->compat->addItem(docIcon, "1.1");
    ui->compat->addItem(docIcon, "1.2");
    ui->compat->addItem(docIcon, "1.3");
    ui->compat->addItem(docIcon, "1.4");
    ui->compat->addItem(docIcon, "1.5");
    ui->compat->addItem(docIcon, "1.6");
    ui->compat->addItem(docIcon, "1.7");

    ui->preset->clear();
    ui->preset->addItem(docIcon, "None");
    ui->preset->addItem(docIcon, "Default");
    ui->preset->addItem(docIcon, "Prepress");
    ui->preset->addItem(docIcon, "eBook");
    ui->preset->addItem(docIcon, "Screen");
    ui->preset->addItem(docIcon, "Printer");
}

void FusePDF::loadSettings()
{
    if (isNewVersion()) { on_actionAbout_triggered(); }
    //if (!hasWindowState()) { setGeometry(100, 100, 700, 250); } // ???

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

    if (getCachePath().isEmpty()) {
        QMessageBox::warning(this,
                             tr("Missing cache folder"),
                             tr("Unable to create the cache folder, please check your system permissions."));
    }
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
    for (int i = 0; i < ui->tabs->count(); ++i) {
        PagesListWidget *tab = qobject_cast<PagesListWidget*>(ui->tabs->widget(i));
        if (!tab) { continue; }
        tab->deleteLater();
    }
    ui->tabs->clear();
    ui->tabs->addTab(ui->tabInputs, QIcon(FUSEPDF_ICON_MAIN), tr("Documents"));
    ui->cmd->clear();
    _output.clear();
}

const QString FusePDF::findGhost()
{
#ifdef Q_OS_WIN
    QString appDir = QString("%1/gs").arg(qApp->applicationDirPath());
    if (QFile::exists(appDir)) {
        QString bin64 = appDir + "/bin/gswin64c.exe";
        if (QFile::exists(bin64)) { return bin64; }
        QString bin32 = appDir + "/bin/gswin32c.exe";
        if (QFile::exists(bin32)) { return bin32; }
    }
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
#ifdef Q_OS_MAC
    QStringList gs;
    gs << "/opt/local/bin/gs" << "/usr/local/bin/gs";
    for (int i = 0; i < gs.size(); ++i) { if (QFile::exists(gs.at(i))) { return gs.at(i); } }
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
        if (!isPDF(info.filePath()) || hasFile(info.filePath())) { continue; }
        int pages = getPageCount(info.filePath());
        QString checksum = getChecksum(info.filePath());
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->inputs);
        item->setData(0, FUSEPDF_PATH_ROLE, info.filePath());
        item->setData(0, FUSEPDF_PAGES_ROLE, pages);
        item->setData(0, FUSEPDF_CHECKSUM_ROLE, checksum);
        item->setText(0, info.fileName());
        item->setText(1, QString::number(pages));
        item->setIcon(0, QIcon(FUSEPDF_ICON_MAIN));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
        if (!info.absolutePath().isEmpty()) { _lastLoadDir = info.absolutePath(); }
        if (hasTab(info.filePath())) { continue; }
        ui->tabs->addTab(new PagesListWidget(this, info.filePath(), checksum, pages),
                         QIcon(FUSEPDF_ICON_DOC),
                         info.fileName());
        connect(this, SIGNAL(foundPagePreview(QString,QString,QString,int)),
                getTab(info.filePath()), SLOT(setPageIcon(QString,QString,QString,int)));
        QtConcurrent::run(this,
                          &FusePDF::getPagePreviews,
                          info.filePath(),
                          checksum,
                          pages);
    }
}

void FusePDF::on_inputs_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (!item) { return; }
    QString filename = item->data(0, FUSEPDF_PATH_ROLE).toString();
    //int pages = item->data(0, FUSEPDF_PAGES_ROLE).toInt();
    if (!QFile::exists(filename)) { return; }
    QDesktopServices::openUrl(QUrl::fromUserInput(filename));
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
        QFileInfo inputInfo(ui->inputs->topLevelItem(i)->data(0, FUSEPDF_PATH_ROLE).toString());
        if (inputInfo.absoluteFilePath() == fileInfo.absoluteFilePath()) { return  true; }
    }
    return false;
}

void FusePDF::loadOptions()
{
    populateUI();

    QSettings settings;
    settings.beginGroup("options");
    ui->compat->setCurrentText(settings.value("compat", "1.5").toString());
    ui->preset->setCurrentText(settings.value("preset", "Default").toString());
    ui->actionShow_log->setChecked(settings.value("showLog", false).toBool());
    ui->actionAuto_Sort->setChecked(settings.value("autoSort", false).toBool());
    ui->actionRemember_meta_author->setChecked(settings.value("metaAuthor", true).toBool());
    ui->actionRemember_meta_subject->setChecked(settings.value("metaSubject", true).toBool());
    ui->actionRemember_meta_title->setChecked(settings.value("metaTitle", true).toBool());
    _lastLoadDir = settings.value("lastLoadDir", "").toString();
    _lastSaveDir = settings.value("lastSaveDir", "").toString();
    ui->actionOpen_saved_PDF->setChecked(settings.value("openSavedPDF", true).toBool());
    ui->actionShow_tooltips->setChecked(settings.value("showTooltips", true).toBool());
    showTooltips(settings.value("showTooltips", true).toBool());
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
    if (!ui->compat->currentText().isEmpty()) {
        settings.setValue("compat", ui->compat->currentText());
    }
    if (!ui->preset->currentText().isEmpty()) {
        settings.setValue("preset", ui->preset->currentText());
    }
    settings.setValue("showLog", ui->actionShow_log->isChecked());
    settings.setValue("autoSort", ui->actionAuto_Sort->isChecked());
    settings.setValue("openSavedPDF", ui->actionOpen_saved_PDF->isChecked());
    settings.setValue("showTooltips", ui->actionShow_tooltips->isChecked());
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
    QString filename = ui->inputs->currentItem()->data(0, FUSEPDF_PATH_ROLE).toString();
    delete ui->inputs->currentItem();

    int index = getTabIndex(filename);
    if (index < 0) { return; }
    PagesListWidget *tab = getTab(filename);
    if (!tab) { return; }
    ui->tabs->removeTab(index);
    tab->deleteLater();
}

QByteArray FusePDF::toUtf16Hex(QString str)
{
    // https://stackoverflow.com/a/38831604
    str.prepend(QChar::ByteOrderMark);
    return QByteArray::fromRawData(reinterpret_cast<const char*>(str.constData()),
                                   (str.size()+1)*2).toHex();
}

int FusePDF::getPageCount(const QString &filename)
{
    int result = 0;
    if (!isPDF(filename)) { return result; }
    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_COUNT).arg(filename));
    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    result = proc.readAll().replace("PageCount:", "").simplified().toInt();
    proc.close();
    return result;
}

bool FusePDF::isPDF(const QString &filename)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filename);
    return (type.name() == "application/pdf");
}

bool FusePDF::isJPG(const QString &filename)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filename);
    return (type.name() == "image/jpeg");
}

const QString FusePDF::getCachePath()
{
    QString path = QDir::tempPath();
    path.append("/fusepdf");
    if (!QFile::exists(path)) {
        QDir dir(path);
        if (!dir.mkpath(path)) { return "";}
    }
    return path;
}

PagesListWidget *FusePDF::getTab(const QString &filename)
{
    if (filename.isEmpty()) { return nullptr; }
    for (int i = 0; i < ui->tabs->count(); ++i) {
        PagesListWidget *tab = qobject_cast<PagesListWidget*>(ui->tabs->widget(i));
        if (tab && tab->getFilename() == filename) { return tab; }
    }
    return nullptr;
}

bool FusePDF::hasTab(const QString &filename)
{
    PagesListWidget *tab = getTab(filename);
    if (tab && tab->getFilename() == filename) { return true; }
    return false;
}

int FusePDF::getTabIndex(const QString &filename)
{
    int result = -1;
    if (filename.isEmpty()) { return  result; }
    for (int i = 0; i < ui->tabs->count(); ++i) {
        PagesListWidget *tab = qobject_cast<PagesListWidget*>(ui->tabs->widget(i));
        if (tab && tab->getFilename() == filename) { return i; }
    }
    return result;
}

const QString FusePDF::getPagePreview(const QString &filename,
                                      const QString &checksum,
                                      int page,
                                      int quality)
{
    emit statusMessage(tr("Generating preview %1 for %2 ...").arg(page).arg(QFileInfo(filename).fileName()), 1000);
    QString cache = getCachePath();
    if (!isPDF(filename) || cache.isEmpty()) { return  QString(); }
    QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache).arg(checksum).arg(page);
    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_PREVIEW).arg(filename).arg(image).arg(page).arg(quality));
    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    proc.close();
    if (isJPG(image)) { return image; }
    return QString();
}

void FusePDF::getPagePreviews(const QString &filename, const QString &checksum, int pages)
{
    QString cache = getCachePath();
    if (!isPDF(filename) || pages < 1 || cache.isEmpty()) { return; }
    for (int i = 1; i <= pages; ++i) {
        QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache).arg(checksum).arg(i);
        if (!QFile::exists(image)) {
            image = getPagePreview(filename, checksum, i);
        }
        if (!image.isEmpty()) { emit foundPagePreview(filename, checksum, image, i); }
    }
}

const QString FusePDF::getChecksum(const QString &filename)
{
    if (!isPDF(filename)) { return QString(); }
    QFile file(filename);
    QString result;
    if (file.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        if (hash.addData(&file)) { result = hash.result().toHex(); }
        file.close();
    }
    return result;
}

const QString FusePDF::extractPDF(const QString &filename,
                                  const QString &checksum,
                                  int page)
{
    emit statusMessage(tr("Extracting page %1 from %2 ...").arg(page).arg(QFileInfo(filename).fileName()), 1000);
    QString cache = getCachePath();
    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    if (cache.isEmpty()) { return QString(); }
    cache = QString(FUSEPDF_CACHE_PDF).arg(cache).arg(checksum).arg(page);
    command.append(QString(FUSEPDF_GS_EXTRACT).arg(filename).arg(cache).arg(page));
    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    proc.close();
    if (isPDF(cache)) { return cache; }
    return QString();
}

void FusePDF::showProgress(bool progress)
{
    ui->progressBar->setMaximum(progress?0:100);
    ui->progressBar->setValue(progress?0:100);
}

void FusePDF::on_actionOpen_cache_folder_triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(getCachePath()));
}

void FusePDF::on_actionShow_tooltips_triggered()
{
    showTooltips(ui->actionShow_tooltips->isChecked());
}

void FusePDF::showTooltips(bool show)
{
    qDebug() << "show tooltips" << show;
    ui->metaTitleLabel->setToolTip(show?tr("Set document title"):QString());
    ui->metaTitle->setToolTip(show?ui->metaTitleLabel->toolTip():QString());
    ui->metaAuthorLabel->setToolTip(show?tr("Set document author"):QString());
    ui->metaAuthor->setToolTip(show?ui->metaAuthorLabel->toolTip():QString());
    ui->metaSubjectLabel->setToolTip(show?tr("Set document subject"):QString());
    ui->metaSubject->setToolTip(show?ui->metaSubjectLabel->toolTip():QString());
    ui->presetLabel->setToolTip(show?tr("Distiller presets\n\n"
                                        "- DEFAULT: selects output intended to be useful across a wide variety of uses, possibly at the expense of a larger output file.\n"
                                        "- PREPRESS: selects output similar to Acrobat Distiller \"Prepress Optimized\" (up to version X) setting.\n"
                                        "- EBOOK: selects medium-resolution output similar to the Acrobat Distiller (up to version X) \"eBook\" setting.\n"
                                        "- SCREEN: selects low-resolution output similar to the Acrobat Distiller (up to version X) \"Screen Optimized\" setting.\n"
                                        "- PRINTER: selects output similar to the Acrobat Distiller \"Print Optimized\" (up to version X) setting."):QString());
    ui->preset->setToolTip(show?ui->presetLabel->toolTip():QString());
    ui->compatLabel->setToolTip(show?tr("Select the PDF version this document should be compatible with."):QString());
    ui->compat->setToolTip(show?ui->compatLabel->toolTip():QString());
    ui->inputs->setToolTip(show?tr("Drag and drop PDF documents you want to merge here. You can re-arrange after adding them (if sorting is disabled).\n\n"
                                   "Note that the first document will define the paper size on the final output.\n\n"
                                   "You can remove a document with the DEL key."):QString());
}
