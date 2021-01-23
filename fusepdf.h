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
    void on_paper_currentIndexChanged(const QString &arg1);
    void on_preset_currentIndexChanged(const QString &arg1);
    void on_compat_currentIndexChanged(const QString &arg1);
    void on_dpi_valueChanged(int arg1);
    void on_dpiCheck_stateChanged(int arg1);
    void on_fileButton_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void makeCommand();
    void runCommand();
    void commandStarted();
    void commandFinished(int exitCode);
    void populateUI();
    void loadSettings();
    void saveSettings();
    void handleProcOutput();
    void clearAll();
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

private:
    Ui::FusePDF *ui;
    QString _cmd;
    QProcess *_proc;
    QString _lastSaveDir;
    QString _lastLoadDir;
};
#endif // FUSEPDF_H
