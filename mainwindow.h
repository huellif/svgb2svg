/*
* Copyright (C) 2010-2011 Ilya Averyanov a1ien.n3t
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "svgb_decoder.h"
#include "previewdialog.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createMyMenu();
    bool open;

private slots:
    void ProgressBarIncNeed(int val);
    void FileProcessed(const QString& result);
    void FileProcessFinish();
    void VersionAction();
    void ClearAction();
    void on_folder_clicked();
    void on_file_clicked();
    void killer();
    void OpenNewWindow();

private:
    Ui::MainWindow *ui;
    Decoder *decoder;
    QAction* menu_exitAction;
    QAction* menu_VersionAction;
    QAction* menu_ClearAction;
    PreviewDialog *p;


};

#endif // MAINWINDOW_H
