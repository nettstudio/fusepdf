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
#include <QVector>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QPixmap>
#include <QRegularExpression>

#define FUSEPDF_SITE_URL "https://fusepdf.no"
#define FUSEPDF_RELEASES_URL "https://github.com/nettstudio/fusepdf/releases"
#define FUSEPDF_ISSUE_URL "https://github.com/nettstudio/fusepdf/issues"
#define FUSEPDF_GS_URL "https://www.ghostscript.com/download/gsdnld.html"
#define FUSEPDF_GS_MAC_URL "https://pages.uoregon.edu/koch/"
#define FUSEPDF_GS_MACPORTS_URL "https://ports.macports.org/port/ghostscript/"
#define FUSEPDF_GS_HOMEBREW_URL "https://formulae.brew.sh/formula/ghostscript"

#define FUSEPDF_TOOLBOX_PREVIEW 0
#define FUSEPDF_TOOLBOX_OUTPUT 1

#define FUSEPDF_PATH_ROLE Qt::UserRole + 1
#define FUSEPDF_PAGES_ROLE Qt::UserRole + 2
#define FUSEPDF_PAGE_ROLE Qt::UserRole + 3
#define FUSEPDF_SELECTED_ROLE Qt::UserRole + 4
#define FUSEPDF_CHECKSUM_ROLE Qt::UserRole + 5
#define FUSEPDF_IMAGE_TYPE_ROLE Qt::UserRole + 6
#define FUSEPDF_IMAGE_RES_ROLE Qt::UserRole + 7
#define FUSEPDF_CHECKED_ROLE Qt::UserRole + 8
#define FUSEPDF_PDFA_ROLE Qt::userRole + 9

#define FUSEPDF_PAGE_ICON_SIZE 470
#define FUSEPDF_ICON_DEFAULT_SIZE 22

#define FUSEPDF_CACHE_JPEG "%1/%2-%3.jpg"
#define FUSEPDF_CACHE_PDF "%1/%2-%3.pdf"

#define FUSEPDF_ICON_MAIN ":/assets/fusepdf-document.png"
#define FUSEPDF_ICON_DOC ":/assets/document.png"
#define FUSEPDF_ICON_LOGO ":/assets/fusepdf.png"
#define FUSEPDF_ICON_CLEAR ":/assets/fusepdf-clear.png"
#define FUSEPDF_ICON_VALID ":/assets/fusepdf-valid.png"
#define FUSEPDF_ICON_DENIED ":/assets/fusepdf-denied.png"
#define FUSEPDF_ICON_GOFIRST ":/assets/fusepdf-gofirst.png"
#define FUSEPDF_ICON_OPEN ":/assets/fusepdf-open.png"
#define FUSEPDF_ICON_SAVE ":/assets/fusepdf-save.png"
#define FUSEPDF_ICON_QUIT ":/assets/fusepdf-quit.png"

#define HICOLOR_ICON_DOC "document"
#define HICOLOR_ICON_CLEAR "edit-clear"
#define HICOLOR_ICON_VALID "dialog-apply"
#define HICOLOR_ICON_DENIED "dialog-cancel"
#define HICOLOR_ICON_GOFIRST "go-first"
#define HICOLOR_ICON_OPEN "document-open"
#define HICOLOR_ICON_SAVE "document-save"
#define HICOLOR_ICON_QUIT "appliction-exit"

#define FUSEPDF_GS_PREVIEW " -q -sDEVICE=jpeg -o \"%2\" -dFirstPage=%3 -dLastPage=%3 -dJPEGQ=%4 -r72x72 \"%1\""
#define FUSEPDF_GS_EXPORT " -q -sDEVICE=%4 -o \"%2\" -dFirstPage=%3 -dLastPage=%3 -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -r%5x%5 \"%1\""
#define FUSEPDF_GS_COUNT " -q -dNODISPLAY -dNOSAFER -c \"/pdffile (%1) (r) file runpdfbegin (PageCount: ) print pdfpagecount = quit\""
#define FUSEPDF_GS_EXTRACT " -q -dNOPAUSE -dBATCH -sOutputFile=\"%2\" -dFirstPage=%3 -dLastPage=%3 -sDEVICE=pdfwrite \"%1\""
#define FUSEPDF_GS_INFO " -q -dNODISPLAY -dNOSAFER -sFile=\"%1\" -dDumpMediaSizes=false -dDumpFontsUsed=false \"%2\""

enum exportImageType {
    exportImageTypeUndefined,
    exportTiffTypeGray,
    exportTiffTypeRGB12,
    exportTiffTypeRGB24,
    exportTiffTypeRGB48,
    exportTiffTypeCMYK32,
    exportTiffTypeCMYK64,
    exportPNGTypeGray,
    exportPNGType16
};

class ExportImageDialog : public QDialog
{
    Q_OBJECT

public:
    ExportImageDialog(QWidget *parent = nullptr,
                      Qt::WindowFlags f = Qt::WindowFlags(),
                      QString suffix = QString());
    int getImageType();
    int getImageRes();

private slots:
    void handleButtonCancel();
    void handleButtonOk();

private:
    QComboBox *_type;
    QComboBox *_res;
};

class PageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyledItemDelegate::paint(painter, option, index);
        painter->save();

        int dim = FUSEPDF_ICON_DEFAULT_SIZE;
        int x = option.rect.x() + ((option.rect.width()/2) - (dim/2));
        int y = option.rect.y() + (option.rect.height() - dim) - (dim/2);

        if (index.data(FUSEPDF_CHECKED_ROLE).isValid()) {
            QPixmap pix;
            pix.load(index.data(FUSEPDF_CHECKED_ROLE).toBool()? FUSEPDF_ICON_VALID : FUSEPDF_ICON_DENIED);
            painter->drawPixmap(QRect(x, y, dim, dim), pix);
        }

        painter->restore();
    }
};

class PagesListWidget : public QListWidget
{
    Q_OBJECT

public:
    PagesListWidget(QWidget *parent = nullptr,
                    const QString &filename = QString(),
                    const QString &checksum = QString(),
                    int pages = 0);
    const QString getFilename() { return _filename; }
    const QString getChecksum() { return _checksum; }
    int getPageCount() { return _pages; }
    bool isModified();
    QVector<int> getPagesState(bool state);

signals:
    void requestExportPage(const QString &filename,
                           int page);
    void requestExportPages(const QString &filename,
                            QVector<int> pages);
    void changed();

public slots:
    void setPageIcon(const QString &filename,
                     const QString &checksum,
                     const QString &image,
                     int page);

private slots:
    void handleItemClicked(QListWidgetItem *item);
    void handleContextMenu(QPoint pos);
    void selectAllPages();
    void selectNoPages();
    void setCheckedState(bool state);
    void exportSelectedPage();
    void exportSelectedPages();

private:
    QString _filename;
    QString _checksum;
    int _pages;
};

class FilesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FilesTreeWidget(QWidget *parent = nullptr);

signals:
    void foundPDF(const QList<QUrl> &urls);
    void changed();
    void removeSelected();
    void clearAll();
    void add();

private slots:
    void handleContextMenu(QPoint pos);
    void handleRemoveSelectedAction();
    void handleClearAllAction();
    void handleAddAction();
    void delayedChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *e) { e->acceptProposedAction(); }
    void dropEvent(QDropEvent *e);
};

QT_BEGIN_NAMESPACE
namespace Ui { class FusePDF; }
QT_END_NAMESPACE

class FusePDF : public QMainWindow
{
    Q_OBJECT

public:
    struct pdfInfo {
        QString filename;
        QString checksum;
        QString info;
    };

    FusePDF(QWidget *parent = nullptr);
    ~FusePDF();

signals:
    void foundPagePreview(const QString &filename,
                          const QString &checksum,
                          const QString &image,
                          int page);
    void commandReady(const QString &filename,
                      const QString &command);
    void statusMessage(const QString &message,
                       int timeout);
    void exportDone(const QString &path);
    void generatedOutputPreview(const QStringList &images);
    void foundPdfInfo(const FusePDF::pdfInfo &pdf);

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void prepCommand(const QString &filename);
    const QString makeCommand(const QString &filename,
                              int pdfa = 0);
    void runCommand(const QString &filename,
                    const QString &command);
    void commandStarted();
    void commandFinished(int exitCode);
    void populateUI();
    void loadSettings();
    void saveSettings();
    void handleProcOutput();
    void clearInput(bool askFirst = false);
    static const QString findGhost(bool pathOnly = false);
    static const QString findGhostPdfInfo();
    static const QString findGhostPdfa(const QString &title = QString());
    void on_actionShow_log_triggered();
    void on_actionAbout_Qt_triggered();
    void handleFoundPDF(const QList<QUrl> &urls);
    void on_inputs_itemDoubleClicked(QTreeWidgetItem *item,
                                     int column);
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
    static bool isFileType(const QString &filename,
                           const QString &mime,
                           bool startsWith = false);
    static bool isPDF(const QString &filename);
    static bool isJPG(const QString &filename);
    static bool isTIFF(const QString &filename);
    static bool isPNG(const QString &filename);
    static bool isImage(const QString &filename);
    static QString ghostImageFormat(int type);
    static const QString getCachePath();
    PagesListWidget* getTab(const QString &filename);
    bool hasTab(const QString &filename);
    int getTabIndex(const QString &filename);
    const QString getPagePreview(const QString &filename,
                                 const QString &checksum,
                                 int page,
                                 int quality = 90);
    void getPagePreviews(const QString &filename,
                         const QString &checksum,
                         int pages);
    const QString getChecksum(const QString &filename);
    const QString extractPDF(const QString &filename,
                             const QString &checksum,
                             int page,
                             bool force = true);
    void showProgress(bool progress);
    void on_actionOpen_cache_folder_triggered();
    void on_actionShow_tooltips_triggered();
    void showTooltips(bool show);
    void on_actionCheck_for_updates_triggered();
    void on_actionReport_issue_triggered();
    bool exportImage(const QString &filename,
                    const QString &image,
                    int page,
                    int type,
                    int res,
                    int alpha = 4);
    void handleExport(const QString &filename,
                      int page);
    void handleExports(const QString &filename,
                       QVector<int> pages);
    void doExport(const QString &filename,
                  const QString &image,
                  QVector<int> pages,
                  int type,
                  int res);
    void handleExportDone(const QString &path);
    void on_preset_currentIndexChanged(int index);
    void on_compat_currentIndexChanged(int index);
    void on_metaTitle_textChanged(const QString &arg1);
    void on_metaSubject_textChanged(const QString &arg1);
    void on_metaAuthor_textChanged(const QString &arg1);
    void on_tabs_currentChanged(int index);
    void handleTabButtonClicked(bool checked);
    bool hasDarkMode();
    void generateOutputPreview();
    void handleOutputPagesChanged();
    void showOutputPreview(const QStringList &images);
    void handleOutputRemoveSelected();
    void handleOutputClearAll();
    void handleOutputAdd();
    void on_actionDocumentation_triggered();
    void on_actionOutput_preview_triggered();
    int pagesToExport();
    void getPdfInfo(const QString &filename,
                    const QString &checksum);
    void handleFoundPdfInfo(const FusePDF::pdfInfo &pdf);
    static const QString stripMarks(QString s);
    const QString getCacheSize();
    void on_actionClear_cache_triggered();

private:
    Ui::FusePDF *ui;
    QString _output;
    QString _lastSaveDir;
    QString _lastLoadDir;
    QString _lastExportDir;
    QProcess *_proc;
    QPushButton *_tabButton;
};
#endif // FUSEPDF_H
