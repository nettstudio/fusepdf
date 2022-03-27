/*
#
# FusePDF - https://fusepdf.no
#
# Copyright (c) 2021, 2022 NettStudio AS <https://nettstudio.no>.
# All rights reserved.
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

ExportImageDialog::ExportImageDialog(QWidget *parent,
                                     Qt::WindowFlags f,
                                     QString suffix):
    QDialog(parent, f)
  , _type(nullptr)
  , _res(nullptr)
{
    setWindowTitle(tr("Export image"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QWidget *formatWidget = new QWidget(this);
    QHBoxLayout *formatLayout = new QHBoxLayout(formatWidget);

    QLabel *formatLabel = new QLabel(this);
    formatLabel->setText(tr("Format"));
    formatLayout->addWidget(formatLabel);

    _type = new QComboBox(this);
    _res = new QComboBox(this);

    _res->addItem(tr("72 dpi"), 72);
    _res->addItem(tr("96 dpi"), 96);
    _res->addItem(tr("120 dpi"), 120);
    _res->addItem(tr("300 dpi"), 300);
    _res->addItem(tr("600 dpi"), 600);
    _res->addItem(tr("1200 dpi"), 1200);
    _res->setCurrentIndex(3);

    if (suffix.toLower() == "png") {
        _type->addItem(tr("PNG 16"), exportPNGType16);
        _type->addItem(tr("PNG Gray"), exportPNGTypeGray);
    } else if (suffix.toLower() == "tif" || suffix.toLower() == "tiff") {
        _type->addItem(tr("TIFF RGB 24-bit"), exportTiffTypeRGB24);
        _type->addItem(tr("TIFF RGB 12-bit"), exportTiffTypeRGB12);
        _type->addItem(tr("TIFF RGB 48-bit"), exportTiffTypeRGB48);
        _type->addItem(tr("TIFF CMYK 32-bit"), exportTiffTypeCMYK32);
        _type->addItem(tr("TIFF CMYK 64-bit"), exportTiffTypeCMYK64);
        _type->addItem(tr("TIFF Gray"), exportTiffTypeGray);
    } else {
        _type->addItem(tr("PNG 16"), exportPNGType16);
        _type->addItem(tr("PNG Gray"), exportPNGTypeGray);
        _type->addItem(tr("TIFF RGB 24-bit"), exportTiffTypeRGB24);
        _type->addItem(tr("TIFF RGB 12-bit"), exportTiffTypeRGB12);
        _type->addItem(tr("TIFF RGB 48-bit"), exportTiffTypeRGB48);
        _type->addItem(tr("TIFF CMYK 32-bit"), exportTiffTypeCMYK32);
        _type->addItem(tr("TIFF CMYK 64-bit"), exportTiffTypeCMYK64);
        _type->addItem(tr("TIFF Gray"), exportTiffTypeGray);
    }

    QLabel *resLabel = new QLabel(this);
    resLabel->setText(tr("Resolution"));

    formatLayout->addWidget(_type);
    formatLayout->addWidget(resLabel);
    formatLayout->addWidget(_res);

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    QPushButton *buttonCancel = new QPushButton(this);
    QPushButton *buttonOk = new QPushButton(this);

    connect(buttonCancel, SIGNAL(clicked()),
            this, SLOT(handleButtonCancel()));
    connect(buttonOk, SIGNAL(clicked()),
            this, SLOT(handleButtonOk()));

    buttonCancel->setText(tr("Cancel"));
    buttonOk->setText(tr("Export"));

    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonCancel);
    buttonLayout->addWidget(buttonOk);

    layout->addWidget(formatWidget);
    layout->addWidget(buttonWidget);

    QTimer::singleShot(100, buttonOk, SLOT(setFocus()));
}

int ExportImageDialog::getImageType()
{
    return _type->currentData().toInt();
}

int ExportImageDialog::getImageRes()
{
    return _res->currentData().toInt();
}

void ExportImageDialog::handleButtonCancel()
{
    QDialog::reject();
}

void ExportImageDialog::handleButtonOk()
{
    QDialog::accept();
}

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
    setItemDelegate(new PageDelegate(this));
    setSpacing(10);

    for (int i = 1; i <= _pages; ++i) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(FUSEPDF_ICON_LOGO),
                                                    QString(),
                                                    this);
        item->setData(FUSEPDF_PAGE_ROLE, i);
        item->setData(FUSEPDF_CHECKED_ROLE, true);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }

    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(handleItemClicked(QListWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(handleContextMenu(QPoint)));
}

bool PagesListWidget::isModified() {
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (!item->data(FUSEPDF_CHECKED_ROLE).toBool()) { return true; }
    }
    return false;
}

QVector<int> PagesListWidget::getPagesState(bool state)
{
    QVector<int> result;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->data(FUSEPDF_CHECKED_ROLE).toBool() == state) {
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
    if (item->data(FUSEPDF_CHECKED_ROLE).toBool()) {
        item->setData(FUSEPDF_CHECKED_ROLE, false);
    } else {
        item->setData(FUSEPDF_CHECKED_ROLE, true);
    }
    emit changed();
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

    menu.addSeparator();
    QAction exportAction;
    exportAction.setText(tr("Export current page to image"));
    QAction exportSelectedAction;
    exportSelectedAction.setText(tr("Export selected pages to images"));

    connect(&exportAction, SIGNAL(triggered(bool)),
            this, SLOT(exportSelectedPage()));
    connect(&exportSelectedAction, SIGNAL(triggered(bool)),
            this, SLOT(exportSelectedPages()));

    menu.addAction(&exportAction);
    menu.addAction(&exportSelectedAction);

    menu.exec(viewport()->mapToGlobal(pos));
}

void PagesListWidget::selectAllPages()
{
    setCheckedState(true);
}

void PagesListWidget::selectNoPages()
{
    setCheckedState(false);
}

void PagesListWidget::setCheckedState(bool state)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        item->setData(FUSEPDF_CHECKED_ROLE, state);
    }
    emit changed();
}

void PagesListWidget::exportSelectedPage()
{
    QListWidgetItem *item = currentItem();
    if (!item) { return; }
    QString filename = _filename;
    int page = item->data(FUSEPDF_PAGE_ROLE).toInt();
    emit requestExportPage(filename, page);
}

void PagesListWidget::exportSelectedPages()
{
    emit requestExportPages(_filename,
                            getPagesState(true));
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

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(handleContextMenu(QPoint)));
}

void FilesTreeWidget::handleContextMenu(QPoint pos)
{
    QMenu menu;

    QAction removeSelectedAction;
    QAction clearAllAction;
    QAction addAction;

    removeSelectedAction.setText(tr("Remove"));
    clearAllAction.setText(tr("Clear"));
    addAction.setText(tr("Add"));

    connect(&removeSelectedAction, SIGNAL(triggered(bool)),
            this, SLOT(handleRemoveSelectedAction()));
    connect(&clearAllAction, SIGNAL(triggered(bool)),
            this, SLOT(handleClearAllAction()));
    connect(&addAction, SIGNAL(triggered(bool)),
            this, SLOT(handleAddAction()));

    menu.addAction(&addAction);
    if (selectedItems().size() > 0) { menu.addAction(&removeSelectedAction); }
    if (topLevelItemCount() > 0) { menu.addAction(&clearAllAction); }

    menu.exec(viewport()->mapToGlobal(pos));
}

void FilesTreeWidget::handleRemoveSelectedAction()
{
    emit removeSelected();
}

void FilesTreeWidget::handleClearAllAction()
{
    emit clearAll();
}

void FilesTreeWidget::handleAddAction()
{
    emit add();
}

void FilesTreeWidget::dropEvent(QDropEvent *e)
{
    emit changed();
    if (e->mimeData()->hasUrls()) { emit foundPDF(e->mimeData()->urls()); }
    else { QTreeWidget::dropEvent(e); }
}

FusePDF::FusePDF(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FusePDF)
    , _proc(nullptr)
    , _tabButton(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(FUSEPDF_ICON_LOGO));

    QFont font = this->font();
    if (font.pointSize() < 9) {
        font.setPointSize(9);
        setFont(font);
    }

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    // force fusion style on Windows/macOS,
    // on Linux/BSD we don't care, use whatever the system has defined.
    qApp->setStyle(QStyleFactory::create("fusion"));
#endif

    ui->presetLabel->setContentsMargins(6, 0, 6, 0);
    ui->compatLabel->setContentsMargins(6, 0, 6, 0);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->addWidget(spacer);

    ui->toolBar->addWidget(ui->presetLabel);
    ui->toolBar->addWidget(ui->preset);
    ui->toolBar->addWidget(ui->compatLabel);
    ui->toolBar->addWidget(ui->compat);

    QPalette previewPalette  = ui->preview->palette();
    previewPalette.setColor(QPalette::Base, hasDarkMode() ? QColor(30, 30, 30) : Qt::lightGray);
    ui->preview->setPalette(previewPalette);

    // don't set palette on macos as it breaks dark mode
#ifndef Q_OS_MAC
    if (hasDarkMode()) {
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(15,15,15));
        palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        palette.setColor(QPalette::Link, Qt::white);
        palette.setColor(QPalette::LinkVisited, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53,53,53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::Highlight, QColor(0,124,151));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        qApp->setPalette(palette);
    }
    QPalette mainPalette = qApp->palette();
    mainPalette.setColor(QPalette::Highlight, QColor(203, 9, 0)); // #cb0900
    mainPalette.setColor(QPalette::Link, hasDarkMode() ? Qt::white : QColor(203, 9, 0));
    qApp->setPalette(mainPalette);

    QPalette treePalette = ui->inputs->palette();
    treePalette.setColor(QPalette::Highlight, QColor(124, 124, 124)); // #7c7c7c
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
    showProgress(false);

    _proc = new QProcess(this);

    _tabButton = new QPushButton(this);
    _tabButton->setFlat(true);
    _tabButton->setIconSize(QSize(24, 24));
    _tabButton->setIcon(QIcon::fromTheme(HICOLOR_ICON_CLEAR,
                                         QIcon(FUSEPDF_ICON_CLEAR)));
    _tabButton->setToolTip(tr("Clear"));
    ui->tabs->setCornerWidget(_tabButton);
    populateUI();

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
    connect(ui->inputs, SIGNAL(changed()),
            this, SLOT(handleOutputPagesChanged()));
    connect(ui->inputs, SIGNAL(removeSelected()),
            this, SLOT(handleOutputRemoveSelected()));
    connect(ui->inputs, SIGNAL(clearAll()),
            this, SLOT(handleOutputClearAll()));
    connect(ui->inputs, SIGNAL(add()),
            this, SLOT(handleOutputAdd()));
    connect(this, SIGNAL(commandReady(QString,QString)),
            this, SLOT(runCommand(QString,QString)));
    connect(this, SIGNAL(statusMessage(QString,int)),
            statusBar(), SLOT(showMessage(QString,int)));
    connect(this, SIGNAL(exportDone(QString)),
            this, SLOT(handleExportDone(QString)));
    connect(_tabButton, SIGNAL(clicked(bool)),
            this, SLOT(handleTabButtonClicked(bool)));

    QTimer::singleShot(0, this, SLOT(loadSettings()));
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
    int totalPages = pagesToExport();
    qDebug() << "total pages to export" << totalPages;
    if (ui->inputs->topLevelItemCount() == 0 || totalPages < 1) {
        QMessageBox::warning(this,
                             tr("No documents/pages"),
                             tr("No documents/pages to merge, please add some documents or enable some pages before trying to save."));
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

void FusePDF::on_actionQuit_triggered()
{
    qApp->quit();
}

void FusePDF::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       QString("FusePDF"),
                       QString("<h2>FusePDF %1</h2>"
                               "<p>Developed by <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a> for NettStudio AS.</p>"
                               "<p>Copyright &copy; 2021, 2022 <a href='https://nettstudio.no'>NettStudio AS</a>. All rights reserved.</p>"
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
    if (filename.isEmpty()) { return QString(); }

    QString command = findGhost();
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(" -sDEVICE=pdfwrite");
    if (!ui->compat->currentData().toString().isEmpty() &&
        ui->compat->currentData().toString().toLower() != "default") {
        command.append(QString(" -dCompatibilityLevel=%1").arg(ui->compat->currentData().toString()));
    }
    if (!ui->preset->currentData().toString().isEmpty() &&
        ui->preset->currentData().toString().toLower() != "none")
    {
        command.append(QString(" -dPDFSETTINGS=/%1").arg(ui->preset->currentData().toString().toLower()));
    }
    command.append(" -dNOPAUSE -dBATCH -dDetectDuplicateImages -dCompressFonts=true");
    command.append(QString(" -sOutputFile=\"%1\"").arg(filename));
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        QString filename = ui->inputs->topLevelItem(i)->data(0, FUSEPDF_PATH_ROLE).toString();
        PagesListWidget *tab = getTab(filename);
        int enabledPages = tab? tab->getPagesState(true).size() : 0;
        bool modified = false;
        if (tab && tab->isModified() && enabledPages > 0) {
            modified = true;
            QVector<int> pages = tab->getPagesState(true);
            for (int i = 0; i < pages.count(); ++i) {
                QString extracted = extractPDF(filename, getChecksum(filename), pages.at(i));
                if (!extracted.isEmpty()) {
                    command.append(QString(" \"%1\"").arg(extracted));
                }
            }
        }
        if (enabledPages < 1) { continue; }
        if (!modified) {
            command.append(QString(" \"%1\"").arg(filename));
        }
    }

    QString title = ui->metaTitle->text();
    QString subject = ui->metaSubject->text();
    QString author = ui->metaAuthor->text();

    QString marks;
    marks.append("/Creator(FusePDF - https://fusepdf.no)");

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
                             QString("FusePDF %1").arg(tr("process is still active, please wait until done.")));
        return;
    }

    if (command.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Failed to make process"),
                             tr("Failed to make process, this should not happen, please contact support."));
        return;
    }
    _output = filename;
    qDebug() << command;
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
    QIcon docIcon(QIcon::fromTheme(HICOLOR_ICON_DOC,
                                   QIcon(FUSEPDF_ICON_DOC)));

    ui->actionOpen->setIcon(QIcon::fromTheme(HICOLOR_ICON_OPEN,
                                             QIcon(FUSEPDF_ICON_OPEN)));
    ui->actionQuit->setIcon(QIcon::fromTheme(HICOLOR_ICON_QUIT,
                                             QIcon(FUSEPDF_ICON_QUIT)));
    ui->actionSave->setIcon(QIcon::fromTheme(HICOLOR_ICON_SAVE,
                                             QIcon(FUSEPDF_ICON_SAVE)));


    QString versionString = tr("version");

    ui->compat->blockSignals(true);
    ui->compat->clear();
    ui->compat->addItem(docIcon, QString("PDF %1 1.0").arg(versionString), "1.0");
    ui->compat->addItem(docIcon, QString("PDF %1 1.1").arg(versionString), "1.1");
    ui->compat->addItem(docIcon, QString("PDF %1 1.2").arg(versionString), "1.2");
    ui->compat->addItem(docIcon, QString("PDF %1 1.3").arg(versionString), "1.3");
    ui->compat->addItem(docIcon, QString("PDF %1 1.4").arg(versionString), "1.4");
    ui->compat->addItem(docIcon, QString("PDF %1 1.5").arg(versionString), "1.5");
    ui->compat->addItem(docIcon, QString("PDF %1 1.6").arg(versionString), "1.6");
    ui->compat->addItem(docIcon, QString("PDF %1 1.7").arg(versionString), "1.7");
    ui->compat->blockSignals(false);

    ui->preset->blockSignals(true);
    ui->preset->clear();
    ui->preset->addItem(docIcon, tr("None"), "None");
    ui->preset->addItem(docIcon, tr("Default"), "Default");
    ui->preset->addItem(docIcon, tr("Prepress"), "Prepress");
    ui->preset->addItem(docIcon, tr("eBook"), "eBook");
    ui->preset->addItem(docIcon, tr("Screen"), "Screen");
    ui->preset->addItem(docIcon, tr("Printer"), "Printer");
    ui->preset->blockSignals(false);

    ui->preview->hide();
    ui->preview->setViewMode(QListView::IconMode);
    ui->preview->setIconSize(QSize(310, 310));
    ui->preview->setUniformItemSizes(true);
    ui->preview->setWrapping(true);
    ui->preview->setResizeMode(QListView::Adjust);
    ui->preview->setFrameShape(QFrame::NoFrame);
    ui->preview->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->preview->setItemDelegate(new PageDelegate(this));
    ui->preview->setSpacing(10);

    connect(this, SIGNAL(generatedOutputPreview(QStringList)),
            this, SLOT(showOutputPreview(QStringList)));
}

void FusePDF::loadSettings()
{
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

    loadOptions();

    if (getCachePath().isEmpty()) {
        QMessageBox::warning(this,
                             tr("Missing cache folder"),
                             tr("Unable to create the cache folder, please check your system permissions."));
        QTimer::singleShot(100, qApp, SLOT(quit()));
    }
    if (missingGhost()) {
        QTimer::singleShot(100, qApp, SLOT(quit()));
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

void FusePDF::clearInput(bool askFirst)
{
    if (ui->inputs->topLevelItemCount() < 1) { return; }
    if (askFirst) {
        int res = QMessageBox::question(this,
                                        tr("Clear?"),
                                        tr("Clear all documents?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes);
        if (res != QMessageBox::Yes) { return; }
    }
    ui->inputs->clear();
    for (int i = 0; i < ui->tabs->count(); ++i) {
        PagesListWidget *tab = qobject_cast<PagesListWidget*>(ui->tabs->widget(i));
        if (!tab) { continue; }
        tab->deleteLater();
    }
    ui->tabs->clear();
    ui->tabs->addTab(ui->tabInputs,
                     QIcon(FUSEPDF_ICON_MAIN),
                     tr("Output"));
    ui->cmd->clear();
    _output.clear();
    ui->preview->clear();
    ui->preview->hide();
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
    QString programFilesPath(qgetenv("PROGRAMFILES"));
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
    QMessageBox::aboutQt(this, QString("%1 Qt").arg(tr("About")));
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
                         QIcon(QIcon::fromTheme(HICOLOR_ICON_DOC, QIcon(FUSEPDF_ICON_DOC))),
                         info.fileName());
        QPalette pal = getTab(info.filePath())->palette();
        if (hasDarkMode()) { pal.setColor(QPalette::Base, QColor(30, 30, 30)); }
        else { pal.setColor(QPalette::Base, Qt::lightGray); }
        getTab(info.filePath())->setPalette(pal);
        connect(this, SIGNAL(foundPagePreview(QString,QString,QString,int)),
                getTab(info.filePath()), SLOT(setPageIcon(QString,QString,QString,int)));
        connect(getTab(info.filePath()), SIGNAL(requestExportPage(QString,int)),
                this, SLOT(handleExport(QString,int)));
        connect(getTab(info.filePath()), SIGNAL(requestExportPages(QString,QVector<int>)),
                this, SLOT(handleExports(QString,QVector<int>)));
        connect(getTab(info.filePath()), SIGNAL(changed()),
                this, SLOT(handleOutputPagesChanged()));
        QtConcurrent::run(this,
                          &FusePDF::getPagePreviews,
                          info.filePath(),
                          checksum,
                          pages);
        if (ui->actionOutput_preview->isChecked()) {
            QtConcurrent::run(this, &FusePDF::generateOutputPreview);
        }
    }
}

void FusePDF::on_inputs_itemDoubleClicked(QTreeWidgetItem *item,
                                          int column)
{
    Q_UNUSED(column)
    if (!item) { return; }
    QString filename = item->data(0, FUSEPDF_PATH_ROLE).toString();
    int index = getTabIndex(filename);
    if (index < 1) { return; }
    ui->tabs->setCurrentIndex(index);
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
    QSettings settings;
    settings.beginGroup("options");

    QString savedCompat = settings.value("compat", "1.5").toString();
    ui->compat->blockSignals(true);
    for (int i = 0; i < ui->compat->count(); i++) {
        QString value = ui->compat->itemData(i).toString();
        if (value == savedCompat) {
            ui->compat->setCurrentIndex(i);
            qDebug() << "set compat index" << i << savedCompat;
            break;
        }
    }
    ui->compat->blockSignals(false);

    QString savedPreset = settings.value("preset", "Default").toString();
    ui->preset->blockSignals(true);
    for (int i = 0; i < ui->preset->count(); i++) {
        QString value = ui->preset->itemData(i).toString();
        if (value == savedPreset) {
            ui->preset->setCurrentIndex(i);
            qDebug() << "set preset index" << i << savedPreset;
            break;
        }
    }
    ui->preset->blockSignals(false);

    ui->actionOutput_preview->setChecked(settings.value("outputPreview", true).toBool());
    ui->actionShow_log->setChecked(settings.value("showLog", false).toBool());
    ui->actionAuto_Sort->setChecked(settings.value("autoSort", false).toBool());
    ui->actionRemember_meta_author->setChecked(settings.value("metaAuthor", true).toBool());
    ui->actionRemember_meta_subject->setChecked(settings.value("metaSubject", true).toBool());
    ui->actionRemember_meta_title->setChecked(settings.value("metaTitle", true).toBool());
    _lastLoadDir = settings.value("lastLoadDir", "").toString();
    _lastSaveDir = settings.value("lastSaveDir", "").toString();
    _lastExportDir = settings.value("lastExportDir").toString();
    ui->actionOpen_saved_PDF->setChecked(settings.value("openSavedPDF", true).toBool());
    ui->actionShow_tooltips->setChecked(settings.value("showTooltips", true).toBool());
    showTooltips(settings.value("showTooltips", true).toBool());
    ui->cmd->setVisible(ui->actionShow_log->isChecked());
    ui->inputs->setSortingEnabled(ui->actionAuto_Sort->isChecked());

    if (ui->actionRemember_meta_author->isChecked()) {
        QString author = settings.value("metaAuthorText").toString();
        if (!author.isEmpty()) {
            ui->metaAuthor->blockSignals(true);
            ui->metaAuthor->setText(author);
            ui->metaAuthor->blockSignals(false);
        }
    }
    if (ui->actionRemember_meta_subject->isChecked()) {
        QString subject = settings.value("metaSubjectText").toString();
        if (!subject.isEmpty()) {
            ui->metaSubject->blockSignals(true);
            ui->metaSubject->setText(subject);
            ui->metaSubject->blockSignals(false);
        }
    }
    if (ui->actionRemember_meta_title->isChecked()) {
        QString title = settings.value("metaTitleText").toString();
        if (!title.isEmpty()) {
            ui->metaTitle->blockSignals(true);
            ui->metaTitle->setText(title);
            ui->metaTitle->blockSignals(false);
        }
    }

    settings.endGroup();
}

void FusePDF::saveOptions()
{
    QSettings settings;
    settings.beginGroup("options");
    settings.setValue("outputPreview", ui->actionOutput_preview->isChecked());
    settings.setValue("showLog", ui->actionShow_log->isChecked());
    settings.setValue("autoSort", ui->actionAuto_Sort->isChecked());
    settings.setValue("openSavedPDF", ui->actionOpen_saved_PDF->isChecked());
    settings.setValue("showTooltips", ui->actionShow_tooltips->isChecked());
    settings.setValue("metaAuthor", ui->actionRemember_meta_author->isChecked());
    settings.setValue("metaSubject", ui->actionRemember_meta_subject->isChecked());
    settings.setValue("metaTitle", ui->actionRemember_meta_title->isChecked());

    if (!_lastLoadDir.isEmpty()) { settings.setValue("lastLoadDir", _lastLoadDir); }
    if (!_lastSaveDir.isEmpty()) { settings.setValue("lastSaveDir", _lastSaveDir); }
    if (!_lastExportDir.isEmpty()) { settings.setValue("lastExportDir", _lastExportDir); }
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
        QString mac = QString(". %1 <a href=\"%2\">uoregon.edu</a>, %3 <a href=\"%4\">macports</a> %5 <a href=\"%6\">homebrew</a> %7 Ghostscript")
                      .arg(tr("On macOS you can find installers from"),
                           FUSEPDF_GS_MAC_URL,
                           tr("or use"),
                           FUSEPDF_GS_MACPORTS_URL,
                           tr("or"),
                           FUSEPDF_GS_HOMEBREW_URL,
                           tr("to install"));
        QMessageBox::warning(this,
                             QString("%1 Ghostscript").arg(tr("Missing")),
                             QString("%1 Ghostscript, %2 <a href=\"%4\">ghostscript.com</a> %3%5.")
                             .arg(tr("Unable to find"),
                                  tr("please download the latest Windows installer from"),
                                  tr("and install it before running this application again"),
                                  FUSEPDF_GS_URL,
                                  mac));
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
        errorMsg = tr("The process failed to start.");
        break;
    case QProcess::Crashed:
        errorMsg = tr("The process crashed.");
        break;
    default:
        errorMsg = tr("An unknown error occured.");
        break;
    }
    errorMsg.append(QString(" %1").arg(tr("See log (CTRL+L) for more information")));
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

    handleOutputPagesChanged();
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
    if (command.isEmpty()) { return result; }
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_COUNT).arg(filename));

    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    result = proc.readAll().replace("PageCount:", "").simplified().toInt();
    proc.close();

    qDebug() << command << result;

    return result;
}

bool FusePDF::isFileType(const QString &filename,
                         const QString &mime,
                         bool startsWith)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filename);
    return (startsWith? type.name().startsWith(mime) : type.name() == mime);
}

bool FusePDF::isPDF(const QString &filename)
{
    return isFileType(filename, "application/pdf");
}

bool FusePDF::isJPG(const QString &filename)
{
    return isFileType(filename, "image/jpeg");
}

bool FusePDF::isTIFF(const QString &filename)
{
    return isFileType(filename, "image/tiff");
}

bool FusePDF::isPNG(const QString &filename)
{
    return isFileType(filename, "image/png");
}

bool FusePDF::isImage(const QString &filename)
{
    return isFileType(filename, "image/", true);
}

QString FusePDF::ghostImageFormat(int type)
{
    QString format;
    switch (type) {
    case exportTiffTypeGray:
        format = "tiffgray";
        break;
    case exportTiffTypeRGB12:
        format = "tiff12nc";
        break;
    case exportTiffTypeRGB24:
        format = "tiff24nc";
        break;
    case exportTiffTypeRGB48:
        format = "tiff48nc";
        break;
    case exportTiffTypeCMYK32:
        format = "tiff32nc";
        break;
    case exportTiffTypeCMYK64:
        format = "tiff64nc";
        break;
    case exportPNGTypeGray:
        format = "pnggray";
        break;
    case exportPNGType16:
        format = "png16m";
        break;
    default: break;
    }
    return format;
}

const QString FusePDF::getCachePath()
{
    QString path = QDir::tempPath();
    path.append("/fusepdf");
    if (!QFile::exists(path)) {
        QDir dir(path);
        if (!dir.mkpath(path)) { return QString(); }
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

    QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache, checksum, QString::number(page));

    QString command = findGhost();
    if (command.isEmpty()) { return QString(); }
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_PREVIEW).arg(filename).arg(image).arg(page).arg(quality));

    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    proc.close();

    qDebug() << command << image;

    if (isJPG(image)) { return image; }
    return QString();
}

void FusePDF::getPagePreviews(const QString &filename,
                              const QString &checksum,
                              int pages)
{
    QString cache = getCachePath();
    if (!isPDF(filename) || pages < 1 || cache.isEmpty()) { return; }
    for (int i = 1; i <= pages; ++i) {
        QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache, checksum, QString::number(i));
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
                                  int page,
                                  bool force)
{
    emit statusMessage(tr("Extracting pages ..."), 1000);
    QString cache = getCachePath();
    QString command = findGhost();

    if (command.isEmpty() || cache.isEmpty()) { return QString(); }
    cache = QString(FUSEPDF_CACHE_PDF).arg(cache, checksum, QString::number(page));
    if (!force && QFile::exists(cache) && isPDF(cache)) { return cache; }
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_EXTRACT).arg(filename).arg(cache).arg(page));

    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    proc.close();

    qDebug() << command << cache;

    if (isPDF(cache)) { return cache; }
    return QString();
}

void FusePDF::showProgress(bool progress)
{
    ui->progressBar->setMaximum(progress?0:100);
    ui->progressBar->setValue(progress?0:100);
    ui->progressBar->setHidden(!progress);
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
    ui->metaTitleLabel->setToolTip(show?tr("Set custom document title"):QString());
    ui->metaTitle->setToolTip(show?ui->metaTitleLabel->toolTip():QString());
    ui->metaAuthorLabel->setToolTip(show?tr("Set custom document author"):QString());
    ui->metaAuthor->setToolTip(show?ui->metaAuthorLabel->toolTip():QString());
    ui->metaSubjectLabel->setToolTip(show?tr("Set custom document subject"):QString());
    ui->metaSubject->setToolTip(show?ui->metaSubjectLabel->toolTip():QString());
    ui->presetLabel->setToolTip(show?tr("Distiller presets\n\n"
                                        "- DEFAULT: selects output intended to be useful across a wide variety of uses, possibly at the expense of a larger output file.\n"
                                        "- PREPRESS: selects output similar to Acrobat Distiller \"Prepress Optimized\" setting.\n"
                                        "- EBOOK: selects medium-resolution output similar to the Acrobat Distiller \"eBook\" setting.\n"
                                        "- SCREEN: selects low-resolution output similar to the Acrobat Distiller \"Screen Optimized\" setting.\n"
                                        "- PRINTER: selects output similar to the Acrobat Distiller \"Print Optimized\" setting."):QString());
    ui->preset->setToolTip(show?ui->presetLabel->toolTip():QString());
    ui->compatLabel->setToolTip(show?tr("Select the PDF version this document should be compatible with."):QString());
    ui->compat->setToolTip(show?ui->compatLabel->toolTip():QString());
    /*ui->inputs->setToolTip(show?tr("Drag and drop PDF documents you want to merge here. You can re-arrange after adding them (if sorting is disabled).\n\n"
                                   "Note that the first document will define the paper size on the final output.\n\n"
                                   "You can remove a document with the DEL key."):QString());*/
}

void FusePDF::on_actionCheck_for_updates_triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(FUSEPDF_RELEASES_URL));
}

void FusePDF::on_actionReport_issue_triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(FUSEPDF_ISSUE_URL));
}

bool FusePDF::exportImage(const QString &filename,
                          const QString &image,
                          int page,
                          int type,
                          int res,
                          int alpha)
{
    if (!isPDF(filename) || image.isEmpty() || page < 1 || res < 72 || alpha < 1) {
        return false;
    }
    QString format = ghostImageFormat(type);
    QString command = findGhost();

    if (command.isEmpty() || format.isEmpty()) { return false; }
#ifdef Q_OS_WIN
    command = QString("\"%1\"").arg(findGhost());
#endif
    command.append(QString(FUSEPDF_GS_EXPORT).arg(filename).arg(image).arg(page).arg(format).arg(res));

    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    proc.close();

    qDebug() << command << image;

    if (isImage(image)) { return true; }
    return false;
}

void FusePDF::handleExport(const QString &filename, int page)
{
    QString image = QFileDialog::getSaveFileName(this,
                                                tr("Save Image"),
                                                !_lastExportDir.isEmpty()?_lastExportDir:QDir::homePath(),
                                                "*.tif *.tiff *.png");
    if (image.isEmpty()) { return; }
    ExportImageDialog dialog(this, Qt::WindowFlags(), QFileInfo(image).suffix());
    int d = dialog.exec();
    if (d != QDialog::Accepted) { return; }
    int type = dialog.getImageType();
    int res = dialog.getImageRes();
    QVector<int> pages;
    pages << page;

    showProgress(true);
    QtConcurrent::run(this,
                      &FusePDF::doExport,
                      filename,
                      image,
                      pages,
                      type,
                      res);
}

void FusePDF::handleExports(const QString &filename,
                            QVector<int> pages)
{
    QString image = QFileDialog::getSaveFileName(this,
                                                tr("Save Image"),
                                                !_lastExportDir.isEmpty()?_lastExportDir:QDir::homePath(),
                                                "*.tif *.tiff *.png");
    if (image.isEmpty()) { return; }
    ExportImageDialog dialog(this, Qt::WindowFlags(), QFileInfo(image).suffix());
    int d = dialog.exec();
    if (d != QDialog::Accepted) { return; }
    int type = dialog.getImageType();
    int res = dialog.getImageRes();

    showProgress(true);
    QtConcurrent::run(this,
                      &FusePDF::doExport,
                      filename,
                      image,
                      pages,
                      type,
                      res);
}

void FusePDF::doExport(const QString &filename,
                       const QString &image,
                       QVector<int> pages,
                       int type,
                       int res)
{
    QFileInfo info(image);
    for (int i = 0; i < pages.size(); ++i) {
        emit statusMessage(tr("Exporting image %1 from %2 ...")
                           .arg(pages.at(i))
                           .arg(QFileInfo(filename).fileName()),
                           1000);
        if (!exportImage(filename,
                         QString("%1/%2-%3.%4")
                         .arg(info.dir().absolutePath(),
                              info.baseName(),
                              QString::number(pages.at(i)),
                              info.suffix()),
                         pages.at(i),
                         type,
                         res))
        {
            emit exportDone(QString());
            return;
        }
    }
    if (pages.size() < 2) {
        emit exportDone(QString("%1/%2-%3.%4")
                        .arg(info.dir().absolutePath(),
                             info.baseName(),
                             QString::number(pages.at(0)),
                             info.suffix()));
    } else {
        emit exportDone(info.dir().absolutePath());
    }
}

void FusePDF::handleExportDone(const QString &path)
{
    qDebug() << "export done" << path;
    showProgress(false);
    if (path.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Failed to export"),
                             tr("Failed to export image(s)."));
        return;
    }
    if (_lastExportDir != QFileInfo(path).dir().absolutePath()) {
        _lastExportDir = QFileInfo(path).dir().absolutePath();
    }
    QDesktopServices::openUrl(QUrl::fromUserInput(path));
}

void FusePDF::on_preset_currentIndexChanged(int index)
{
    if (index >= ui->preset->count()) { return; }
    QString newPreset = ui->preset->itemData(index).toString();

    QSettings settings;
    settings.beginGroup("options");
    QString savedPreset = settings.value("preset").toString();
    if (newPreset != savedPreset && !newPreset.isEmpty()) {
        settings.setValue("preset", newPreset);
        qDebug() << "updated preset settings" << newPreset << "vs." << savedPreset;
    }
    settings.endGroup();
}

void FusePDF::on_compat_currentIndexChanged(int index)
{
    if (index >= ui->compat->count()) { return; }
    QString newCompat = ui->compat->itemData(index).toString();

    QSettings settings;
    settings.beginGroup("options");
    QString savedCompat = settings.value("compat").toString();
    if (newCompat != savedCompat && !newCompat.isEmpty()) {
        settings.setValue("compat", newCompat);
        qDebug() << "updated compat settings" << newCompat << "vs." << savedCompat;
    }
    settings.endGroup();
}

void FusePDF::on_metaTitle_textChanged(const QString &arg1)
{
    QSettings settings;
    settings.beginGroup("options");
    if (ui->actionRemember_meta_title->isChecked()) {
        settings.setValue("metaTitleText", arg1);
    }
    settings.endGroup();
}

void FusePDF::on_metaSubject_textChanged(const QString &arg1)
{
    QSettings settings;
    settings.beginGroup("options");
    if (ui->actionRemember_meta_subject->isChecked()) {
        settings.setValue("metaSubjectText", arg1);
    }
    settings.endGroup();
}

void FusePDF::on_metaAuthor_textChanged(const QString &arg1)
{
    QSettings settings;
    settings.beginGroup("options");
    if (ui->actionRemember_meta_author->isChecked()) {
        settings.setValue("metaAuthorText", arg1);
    }
    settings.endGroup();
}

void FusePDF::on_tabs_currentChanged(int index)
{
    if (index == 0) {
        _tabButton->setIcon(QIcon(QIcon::fromTheme(HICOLOR_ICON_CLEAR,
                                                   QIcon(FUSEPDF_ICON_CLEAR))));
        _tabButton->setToolTip(tr("Clear"));
    } else {
        _tabButton->setIcon(QIcon(QIcon::fromTheme(HICOLOR_ICON_GOFIRST,
                                                   QIcon(FUSEPDF_ICON_GOFIRST))));
        _tabButton->setToolTip(tr("Show documents"));
    }
}

void FusePDF::handleTabButtonClicked(bool checked)
{
    Q_UNUSED(checked)
    int index = ui->tabs->currentIndex();
    if (index == 0) {
        clearInput(true);
    } else if (index > 0) {
        ui->tabs->setCurrentIndex(0);
    }
}

bool FusePDF::hasDarkMode()
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    if (settings.value("AppsUseLightTheme") == 0) { return true; }
#endif
    return false;
}

void FusePDF::generateOutputPreview()
{
    QString cache = getCachePath();
    if (cache.isEmpty()) { return; }
    QStringList docs, images;
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        QString filename = ui->inputs->topLevelItem(i)->data(0, FUSEPDF_PATH_ROLE).toString();
        PagesListWidget *tab = getTab(filename);
        bool modified = false;
        int enabledPages = tab? tab->getPagesState(true).size() : 0;
        if (tab && tab->isModified() && enabledPages > 0) {
            modified = true;
            QVector<int> pages = tab->getPagesState(true);
            for (int i = 0; i < pages.count(); ++i) {
                QString extracted = extractPDF(filename, getChecksum(filename), pages.at(i), false);
                if (!extracted.isEmpty()) {
                    docs << extracted;
                    QString checksum = getChecksum(filename);
                    QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache, checksum, QString::number(pages.at(i)));
                    if (!QFile::exists(image)) { image = getPagePreview(filename, checksum, i); }
                    if (!image.isEmpty()) { images << image; }
                }
            }
        }
        if (enabledPages < 1) { continue; }
        if (!modified) {
            docs << filename;
            int pages = getPageCount(filename);
            QString checksum = getChecksum(filename);
            for (int i = 1; i <= pages; ++i) {
                QString image = QString(FUSEPDF_CACHE_JPEG).arg(cache, checksum, QString::number(i));
                if (!QFile::exists(image)) { image = getPagePreview(filename, checksum, i); }
                if (!image.isEmpty() && !images.contains(image)) { images << image; }
            }
        }
    }
    if (images.size() > 0) { emit generatedOutputPreview(images); }
}

void FusePDF::handleOutputPagesChanged()
{
    if (!ui->actionOutput_preview->isChecked()) { return; }
    int totalPages = pagesToExport();
    if (totalPages < 1) {
        ui->preview->clear();
        ui->preview->hide();
        return;
    }
    QtConcurrent::run(this, &FusePDF::generateOutputPreview);
}

void FusePDF::showOutputPreview(const QStringList &images)
{
    if (!ui->actionOutput_preview->isChecked()) { return; }
    if (images.size() < 1) { return; }
    ui->preview->clear();
    ui->preview->show();
    for (int i = 0; i < images.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(FUSEPDF_ICON_LOGO),
                                                    QString(),
                                                    ui->preview);
        item->setFlags(Qt::ItemIsEnabled);
        QPixmap pix(FUSEPDF_PAGE_ICON_SIZE, FUSEPDF_PAGE_ICON_SIZE);
        pix.fill(QColor(Qt::transparent));
        QPainter p(&pix);
        QPixmap ppix = QPixmap::fromImage(QImage(images.at(i))).scaledToHeight(FUSEPDF_PAGE_ICON_SIZE,
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
    }
}

void FusePDF::handleOutputRemoveSelected()
{
    deleteDocumentItem();
}

void FusePDF::handleOutputClearAll()
{
    clearInput(true);
}

void FusePDF::handleOutputAdd()
{
    on_actionOpen_triggered();
}


void FusePDF::on_actionDocumentation_triggered()
{
    QString url = "https://fusepdf.no";
    QString readme = QString("%1/README.pdf").arg(qApp->applicationDirPath());
    if (QFile::exists(readme)) { url = readme; }
    QDesktopServices::openUrl(QUrl::fromUserInput(url));
}


void FusePDF::on_actionOutput_preview_triggered()
{
    if (ui->actionOutput_preview->isChecked()) {
        ui->preview->clear();
        handleOutputPagesChanged();
    } else {
        ui->preview->clear();
        ui->preview->hide();
    }
}

int FusePDF::pagesToExport()
{
    int result = 0;
    for (int i = 0; i < ui->inputs->topLevelItemCount(); ++i) {
        QString filename = ui->inputs->topLevelItem(i)->data(0, FUSEPDF_PATH_ROLE).toString();
        PagesListWidget *tab = getTab(filename);
        bool modified = false;
        int enabledPages = tab? tab->getPagesState(true).size() : 0;
        if (tab && tab->isModified() && enabledPages > 0) {
            modified = true;
            QVector<int> pages = tab->getPagesState(true);
            for (int i = 0; i < pages.count(); ++i) { result++; }
        }
        if (enabledPages < 1) { continue; }
        if (!modified) {
            result += ui->inputs->topLevelItem(i)->text(1).toInt();
        }
    }
    return result;
}
