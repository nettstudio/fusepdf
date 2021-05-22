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

#ifndef FUSEPDF_H
#define FUSEPDF_H

#include <QMainWindow>
#include <QDropEvent>
#include <QTreeWidget>
#include <QMimeData>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <iostream>
#include <QTimer>
#include <QEvent>
#include <QList>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QStyleFactory>
#include <QSettings>
#include <QShortcut>
#include <QScrollBar>
#include <QHeaderView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QtConcurrent/QtConcurrentRun>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QCryptographicHash>

#define FUSEPDF_PATH_ROLE Qt::UserRole + 1
#define FUSEPDF_PAGES_ROLE Qt::UserRole + 2
#define FUSEPDF_PAGE_ROLE Qt::UserRole + 3
#define FUSEPDF_SELECTED_ROLE Qt::UserRole + 4
#define FUSEPDF_CHECKSUM_ROLE Qt::UserRole + 5
#define FUSEPDF_PAGE_ICON_SIZE 320
#define FUSEPDF_CACHE_JPEG "%1/%2-%3.jpg"
#define FUSEPDF_CACHE_PDF "%1/%2-%3.pdf"
#define FUSEPDF_ICON_MAIN ":/assets/fusepdf-document.png"
#define FUSEPDF_ICON_DOC ":/assets/document.png"
#define FUSEPDF_ICON_LOGO ":/assets/fusepdf.png"
#define FUSEPDF_GS_PREVIEW " -q -sDEVICE=jpeg -o \"%2\" -dFirstPage=%3 -dLastPage=%3 -dJPEGQ=%4 -r72x72 \"%1\""
#define FUSEPDF_GS_COUNT " -q -dNODISPLAY -dNOSAFER -c \"/pdffile (%1) (r) file runpdfbegin (PageCount: ) print pdfpagecount = quit\""

class PagesListWidget : public QListWidget
{
    Q_OBJECT

public:
    PagesListWidget(QWidget *parent = nullptr,
                    const QString &filename = QString(),
                    const QString &checksum = QString(),
                    int pages = 0):
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

        for (int i = 1; i <= _pages; ++i) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(":/assets/fusepdf.png"),
                                                        QString::number(i),
                                                        this);
            item->setData(FUSEPDF_PAGE_ROLE, i);
            item->setCheckState(Qt::Checked);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        }

        connect(this, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(handleItemClicked(QListWidgetItem*)));
    }
    const QString getFilename() {
        return _filename;
    }
    const QString getChecksum() {
        return _checksum;
    }
    int getPageCount() {
        return _pages;
    }

public slots:
    void setPageIcon(const QString &filename,
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

private slots:
    void handleItemClicked(QListWidgetItem *item)
    {
        if (!item) { return; }
        if (item->checkState() == Qt::Checked) {
            item->setCheckState(Qt::Unchecked);
        } else { item->setCheckState(Qt::Checked); }
    }

private:
    QString _filename;
    QString _checksum;
    int _pages;
};

class FilesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FilesTreeWidget(QWidget *parent = nullptr):
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

signals:
    void foundPDF(const QList<QUrl> &urls);

protected:
    void dragEnterEvent(QDragEnterEvent *e)
    {
        e->acceptProposedAction();
    }
    void dropEvent(QDropEvent *e)
    {
        if (e->mimeData()->hasUrls()) { emit foundPDF(e->mimeData()->urls()); }
        else { QTreeWidget::dropEvent(e); }
    }
};

QT_BEGIN_NAMESPACE
namespace Ui { class FusePDF; }
QT_END_NAMESPACE

class FusePDF : public QMainWindow
{
    Q_OBJECT

public:
    FusePDF(QWidget *parent = nullptr);
    ~FusePDF();

signals:
    void foundPagePreview(const QString &filename,
                          const QString &checksum,
                          const QString &image,
                          int page);

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionClear_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    const QString makeCommand(const QString &filename);
    void runCommand(const QString &filename);
    void commandStarted();
    void commandFinished(int exitCode);
    void populateUI();
    void loadSettings();
    void saveSettings();
    void handleProcOutput();
    void clearInput();
    static const QString findGhost();
    void on_actionShow_log_triggered();
    void on_actionAbout_Qt_triggered();
    bool hasWindowState();
    void handleFoundPDF(const QList<QUrl> &urls);
    void on_inputs_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionAuto_Sort_triggered();
    bool hasFile(const QString &file);
    void loadOptions();
    void saveOptions();
    bool isNewVersion();
    bool missingGhost();
    void handleProcessError(QProcess::ProcessError error);
    void deleteDocumentItem();
    QByteArray toUtf16Hex(QString str);
    int getPageCount(const QString &filename);
    static bool isPDF(const QString &filename);
    static bool isJPG(const QString &filename);
    static const QString getCachePath();
    PagesListWidget* getTab(const QString &filename);
    bool hasTab(const QString &filename);
    int getTabIndex(const QString &filename);
    static const QString getPagePreview(const QString &filename,
                                        const QString &checksum,
                                        int page,
                                        int quality = 50);
    void getPagePreviews(const QString &filename,
                         const QString &checksum,
                         int pages);
    const QString getChecksum(const QString &filename);

private:
    Ui::FusePDF *ui;
    QString _output;
    QString _lastSaveDir;
    QString _lastLoadDir;
    QProcess *_proc;
};
#endif // FUSEPDF_H
