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

class FilesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FilesTreeWidget(QWidget *parent= nullptr):
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
    const QString findGhost();
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

    QByteArray toUtf16Hex(QString str)
    {
        // https://stackoverflow.com/a/38831604
        str.prepend(QChar::ByteOrderMark);
        // It is OK to use `fromRawData` since toHex copies it.
        return QByteArray::fromRawData(reinterpret_cast<const char*>(str.constData()),
                                       (str.size()+1)*2).toHex();
    }

private:
    Ui::FusePDF *ui;
    QString _output;
    QString _lastSaveDir;
    QString _lastLoadDir;
    QProcess *_proc;
};
#endif // FUSEPDF_H
