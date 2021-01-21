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
#include <QProcessEnvironment>
#include<QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

class FilesTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FilesTreeWidget(QWidget *parent= nullptr):
        QTreeWidget(parent)
    {
        setAcceptDrops(true);
        setDropIndicatorShown(true);
        setColumnCount(1);
    }

signals:
    void addedItem();

protected:
    bool dropMimeData(QTreeWidgetItem *parent, int /*index*/, const QMimeData *data, Qt::DropAction /*action*/)
    {
        qDebug() << data->urls();
        for(const QUrl url: data->urls()) {
            const QFileInfo info( url.toLocalFile());
            if (info.isFile()){
                QMimeDatabase db;
                QMimeType type = db.mimeTypeForFile(info.absoluteFilePath());
                if (type.name() != "application/pdf") {
                    qDebug() << "unsupported format" << info.fileName() << type.name();
                    continue;
                }
                QTreeWidgetItem *item;
                if (parent){
                    item = new QTreeWidgetItem(parent);
                    parent->setExpanded(true);
                }
                else { item = new QTreeWidgetItem(this); }
                item->setText(0, info.fileName());
                item->setText(1, info.filePath());
                emit addedItem();
            }
        }
        return true;
    }

    QStringList mimeTypes () const
    {
        return QStringList()<<"text/uri-list";
    }

    Qt::DropActions supportedDropActions () const
    {
        return Qt::CopyAction;
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
    void handleAddedItem();
    void handleProcOutput();
    void clearAll();

private:
    Ui::FusePDF *ui;
    QString _cmd;
    QProcess *_proc;
};
#endif // FUSEPDF_H
