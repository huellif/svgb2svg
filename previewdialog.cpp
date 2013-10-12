#include "previewdialog.h"
#include "ui_previewdialog.h"
#include "QFileDialog"
#include <QApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QInputDialog>
#include <QFileInfo>

//#include <QDebug>


PreviewDialog::PreviewDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreviewDialog)
{
    ui->setupUi(this);
    createMyMenu();
    path = "";
    newpath = "";
    myDir = new QDir;
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
}

void PreviewDialog::createMyMenu()
{
    menu_ClearAction = new QAction(tr("Clear"), this);
    menuBar()->addAction(menu_ClearAction);
    connect(menu_ClearAction, SIGNAL(triggered()), this, SLOT(clearAction()));

    menu_SetAction = new QAction(tr("Choose a SVG(B)"), this);
    menuBar()->addAction(menu_SetAction);
    connect(menu_SetAction, SIGNAL(triggered()), this, SLOT(setAction()));
}

void PreviewDialog::setAction()
{
    newpath = QFileDialog::getOpenFileName(
        this,
        "Select a .svg to render to ",
        "",
        "Image Files (*.svg);;Image Files (*.svgb)");
    if (newpath == ""){
    }
    else{
        QFileInfo info(newpath);
        QString ext = info.suffix();        
            if (ext == "svgb"){
                myDir->remove("C://data/temp.svgb");
                myDir->remove("C://data/temp.svg");
                QFile::copy(newpath, "C://data/temp.svgb");
                QStringList files;
                files << "C://data/temp.svgb";
                decoder=new Decoder(files,this);
                decoder->start();
                ui->label->setText("wait ...");

            }
            else {
                path = newpath;
                ui->label->setText("Path: " + path);

                QSvgRenderer renderer(QString("" + path));
                bool valid = renderer.isValid();
                if (valid == false){
                    myDir->remove("C://data/temp.svgb");
                    myDir->remove("C://data/temp.svg");
                    ui->label->setText("Error, problem with " + path +  " file");
                }
                else{
                    int w = ui->label_2->width();
                    int h = ui->label_2->height();
                    if (w <= h){
                        h = w;
                    }
                    if (h <= w){
                        w = h;
                    }
                    QImage image(w, h, QImage::Format_ARGB32);
                    QPainter painter(&image);
                    renderer.render(&painter);
                    QPixmap pixmap(QPixmap::fromImage(image));
                    ui->label_2->setPixmap(pixmap);
                }
            }
    }
}

void PreviewDialog::clearAction()
{
    path = "";
    newpath = "";
    ui->label->setText("Path: empty");
    ui->label_2->clear();
}

void PreviewDialog::on_png_triggered()
{
    exportAction(".png");
}

void PreviewDialog::on_jpg_triggered()
{
    exportAction(".jpg");
}

void PreviewDialog::on_bmp_triggered()
{
    exportAction(".bmp");
}


void PreviewDialog::exportAction(QString end)
{       if (path == ""){
            setAction();
        }
        if (path == ""){
            }
        else {
                QSvgRenderer renderer(QString("" + path));
                bool valid = renderer.isValid();
                if (valid == false){
                    ui->label->setText("Error: can't render the .svg(b) successfully");
                }
                else{
                    int w = 360;
                    int h = 360;
                    bool ok;
                    int wq = QInputDialog::getInteger(this, "Export to" + end, "Set Width, for 360 press cancel", 360, 1, 2000, 1, &ok);
                    if (ok)
                        w = wq;
                    int hq = QInputDialog::getInteger(this, "Export to" + end, "Set Height, for 360 press cancel", 360, 1, 2000, 1, &ok);
                    if (ok)
                        h = hq;
                    QImage image(w, h, QImage::Format_ARGB32);
                    QPainter painter(&image);
                    renderer.render(&painter);
                    if (path == "C://data/temp.svg"){
                        myDir->remove(newpath + end);
                        image.save(newpath + end);
                        ui->label->setText("Saved " + newpath + end + " successfully");
                    }
                    else{
                        myDir->remove(path + end);
                        image.save(path + end);
                        ui->label->setText("Saved " + path + end + " successfully");
                    }

               }
        }
}

void PreviewDialog::ProgressBarIncNeed(int val)
{
}

void PreviewDialog::FileProcessed(const QString &result)
{
    if (result != "C://data/temp.svgb <font color='red'><b>ERROR! </b>This is not an S60 svgb file<br></font>"){
        QSvgRenderer renderer(QString("C://data/temp.svg"));
        bool valid = renderer.isValid();
        if (valid == false){
            myDir->remove("C://data/temp.svgb");
            myDir->remove("C://data/temp.svg");
            ui->label->setText("Error, problem with " + newpath +  " file decoding");
            ui->label_2->clear();
        }
        else {
            myDir->remove("C://data/temp.svgb");
            path = "C://data/temp.svg";
            ui->label->setText("Path: " + newpath);

            QSvgRenderer renderer(QString("" + path));
            int w = ui->label_2->width();
            int h = ui->label_2->height();
            if (w <= h){
                h = w;
            }
            if (h <= w){
                w = h;
            }
            QImage image(w, h, QImage::Format_ARGB32);
            QPainter painter(&image);
            renderer.render(&painter);
            QPixmap pixmap(QPixmap::fromImage(image));
            ui->label_2->setPixmap(pixmap);
        }
    }
    else {
        myDir->remove("C://data/temp.svgb");
        myDir->remove("C://data/temp.svg");
        ui->label->setText("Error, problem with " + newpath +  " file decoding");
        ui->label_2->clear();
    }
}

void PreviewDialog::FileProcessFinish()
{    
}
