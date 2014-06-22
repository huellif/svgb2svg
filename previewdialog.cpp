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
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
    delete decoder;
    delete menu_SetAction;
    delete menu_ClearAction;
    delete menu_ToggleAction;
}

void PreviewDialog::createMyMenu()
{
    menu_SetAction = new QAction("Choose a SVG(B)", this);
    menuBar()->addAction(menu_SetAction);
    connect(menu_SetAction, SIGNAL(triggered()), this, SLOT(setAction()));

    menu_ToggleAction = new QAction("Toggle background", this);
    menuBar()->addAction(menu_ToggleAction);
    connect(menu_ToggleAction, SIGNAL(triggered()), this, SLOT(toggleAction()));

    menu_ClearAction = new QAction("Clear", this);
    menuBar()->addAction(menu_ClearAction);
    connect(menu_ClearAction, SIGNAL(triggered()), this, SLOT(clearAction()));
}

void PreviewDialog::resizeEvent(QResizeEvent *)
{
    if(path.isEmpty()) return;
    QSvgRenderer renderer(path);
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

void PreviewDialog::setAction()
{
    newpath = QFileDialog::getOpenFileName(
                0,
                "Select a .svg(b) to render",
                QString(),
                "*.svgb *.svg");
    if (!newpath.isEmpty()){
        ui->menuSet_path->setEnabled(true);
        if (QFileInfo(newpath).suffix() == QLatin1String("svgb"))
        {
            QFile::remove("C://data/temp.svgb");
            QFile::remove("C://data/temp.svg");
            QFile::copy(newpath, "C://data/temp.svgb");
            QStringList files;
            files << "C://data/temp.svgb";
            decoder=new Decoder(files,this);
            decoder->start();
            ui->label->setText("wait ...");
        }
        else
        {
            path = newpath;
            ui->label->setText("Path: " + path);
            if (!QSvgRenderer (path).isValid()){
                QFile::remove("C://data/temp.svgb");
                QFile::remove("C://data/temp.svg");
                ui->label->setText("Error, problem with " + path +  " file");
            }
            else{
                resizeEvent(0);
            }
        }
    }
}

void PreviewDialog::clearAction()
{
    path.clear();
    newpath.clear();
    ui->label->setText("Path: empty");
    ui->label_2->clear();
    ui->menuSet_path->setDisabled(true);
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


void PreviewDialog::exportAction(const QString &end)
{       if (path.isEmpty()){
        setAction();
    }
    if (!path.isEmpty()){
        QSvgRenderer renderer(path);
        bool valid = renderer.isValid();
        if (valid == false){
            ui->label->setText("Error: can't render the .svg(b) successfully");
        }
        else{
            int w = 360;
            int h = 360;
            bool ok;
            int wq = QInputDialog::getInteger(0, "Export to" + end, "Set Width, for 360 press cancel", 360, 1, 2000, 1, &ok);
            if (ok)
                w = wq;
            int hq = QInputDialog::getInteger(0, "Export to" + end, "Set Height, for 360 press cancel", 360, 1, 2000, 1, &ok);
            if (ok)
                h = hq;
            QImage image(w, h, QImage::Format_ARGB32);
            QPainter painter(&image);
            renderer.render(&painter);
            if (path == "C://data/temp.svg"){
                QFile::remove(newpath + end);
                image.save(newpath + end);
                ui->label->setText("Saved " + newpath + end + " successfully");
            }
            else{
                QFile::remove(path + end);
                image.save(path + end);
                ui->label->setText("Saved " + path + end + " successfully");
            }

        }
    }
}

void PreviewDialog::toggleAction()
{
    QPalette Pal(palette());
    if(this->palette().color(QPalette::Background)==Qt::black)
    {
        Pal.setColor(QPalette::Background, Qt::white);
        QPalette palette =ui->label->palette();
        palette.setColor(ui->label->backgroundRole(), Qt::black);
        palette.setColor(ui->label->foregroundRole(), Qt::black);
        ui->label->setPalette(palette);
        ui->label_2->setPalette(palette);
    }
    else
    {
        Pal.setColor(QPalette::Background, Qt::black);
        QPalette palette =ui->label->palette();
        palette.setColor(ui->label->backgroundRole(), Qt::white);
        palette.setColor(ui->label->foregroundRole(), Qt::white);
        ui->label->setPalette(palette);
        ui->label_2->setPalette(palette);
    }
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    this->show();
}

void PreviewDialog::ProgressBarIncNeed(const int &val)
{
    Q_UNUSED(val);
}

void PreviewDialog::FileProcessed(const QString &result)
{
    if (result != QLatin1String("C://data/temp.svgb <font color='red'><b>ERROR! </b>This is not an S60 svgb file</font>")){
        QSvgRenderer renderer(QString("C://data/temp.svg"));
        if (!renderer.isValid()){
            QFile::remove("C://data/temp.svgb");
            QFile::remove("C://data/temp.svg");
            ui->label->setText("Error, problem with " + newpath +  " file decoding");
            ui->label_2->clear();
        }
        else {
            QFile::remove("C://data/temp.svgb");
            path = "C://data/temp.svg";
            ui->label->setText("Path: " + newpath);
            resizeEvent(0);
        }
    }
    else {
        QFile::remove("C://data/temp.svgb");
        QFile::remove("C://data/temp.svg");
        ui->label->setText("Error, problem with " + newpath +  " file decoding");
        ui->label_2->clear();
    }
}

void PreviewDialog::FileProcessFinish()
{
}
