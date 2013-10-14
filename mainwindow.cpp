#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include "previewdialog.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMyMenu();
     backAction = new QAction( tr("Exit"), this );
     backAction->setSoftKeyRole( QAction::NegativeSoftKey );
     connect(backAction, SIGNAL(triggered()), this, SLOT(killer()));
     addAction( backAction );
     open = false;
     del = new QDir;
}

MainWindow::~MainWindow()
{
	delete ui;
	delete decoder;
	delete menu_exitAction;
	delete menu_VersionAction;
	delete menu_ClearAction;
	delete p;
	delete del;
	delete backAction;
}

void MainWindow::killer()
{

    del->remove("C://data/temp.svgb");
    del->remove("C://data/temp.svg");
    if (open == true){
        p->deleteLater();
        open = false;
        backAction->setIconText("Exit");
    }    
    else {
        close();
    }

}


void MainWindow::createMyMenu()
{
    menu_ClearAction = new QAction(tr("Show a SVG(B)"), this);
    menuBar()->addAction(menu_ClearAction);
    connect(menu_ClearAction, SIGNAL(triggered()), this, SLOT(OpenNewWindow()));


    menu_ClearAction = new QAction(tr("Clear"), this);
    menuBar()->addAction(menu_ClearAction);
    connect(menu_ClearAction, SIGNAL(triggered()), this, SLOT(ClearAction()));

    menu_VersionAction = new QAction(tr("About"), this);
    menuBar()->addAction(menu_VersionAction);
    connect(menu_VersionAction, SIGNAL(triggered()), this, SLOT(VersionAction()));

    menu_exitAction = new QAction(tr("Exit"), this);
    menuBar()->addAction(menu_exitAction);
    connect(menu_exitAction, SIGNAL(triggered()), this, SLOT(close()));

}

void MainWindow::FileProcessed(const QString& result){
    ui->plainTextEdit->appendHtml(result);
}

void MainWindow::ProgressBarIncNeed(int val){
    ui->progressBar->setValue(val);
}

void MainWindow::FileProcessFinish(){
   ui->progressBar->setValue(100);
}

void MainWindow::VersionAction()
{
    QMessageBox::about(this,"About",tr("Tiny open-source SVGB to SVG converter written on Qt.\n"\
                       "Authors: Ilya Averyanov and Anton Mihailov\n"\
                       "Based on Svgb decoder by Slava Monich\n"\
                       "Updated and changed by Fabian Hüllmantel\n"\
                       "Thanks AJ___92 for your help\n"\
                       "Version %1").arg(APP_VERSION));}

void MainWindow::ClearAction()
{
    ui->plainTextEdit->clear();
    ui->progressBar->setValue(0);
}


void MainWindow::on_folder_clicked()
{
    QString path = QFileDialog::getExistingDirectory (this, tr("Select a folder to encode"));
    if (path == ""){}
    else{
        QDir dir(path);
        QStringList list;
        QStringList nameFilter;
        nameFilter << "*.svgb";
        QFileInfoList fiList( dir.entryInfoList(nameFilter, QDir::Files, QDir::Name ) );
        if (fiList.empty()){
            ui->plainTextEdit->appendHtml(path + "<font color='red'> ERROR! This folder doesn't contain a .svgb file!</font>");
        }
        else{
            foreach( const QFileInfo & fi, fiList ) {
                list << fi.absoluteFilePath();
            }
            decoder=new Decoder(list,this);
            decoder->start();
        }
    }
}

void MainWindow::on_file_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                    this,
                    "Select a file to decode",
                    "",
                    "Svgb (*.svgb)");
    if (files[0] != ""){
            decoder=new Decoder(files,this);
            decoder->start();
    }
}



void MainWindow::OpenNewWindow()
{
    p = new PreviewDialog();
    p->showMaximized();
    open = true;
    backAction->setIconText("Back");
}
