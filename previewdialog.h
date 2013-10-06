#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QMainWindow>
#include "svgb_decoder.h"
#include <QDir>

namespace Ui {
class PreviewDialog;
}

class PreviewDialog : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PreviewDialog(QWidget *parent = 0);
    ~PreviewDialog();
    void createMyMenu();    

    
private:
    Ui::PreviewDialog *ui;
    Decoder *decoder;
    QAction* menu_SetAction;
    QAction* menu_ClearAction;
    QString path;
    QString newpath;
    QDir *myDir;

private slots:
    void setAction();
    void clearAction();
    void on_png_triggered();
    void on_jpg_triggered();
    void on_bmp_triggered();
    void exportAction(QString end);

    void ProgressBarIncNeed(int val);
    void FileProcessed(const QString& result);
    void FileProcessFinish();
};

#endif // PREVIEWDIALOG_H
