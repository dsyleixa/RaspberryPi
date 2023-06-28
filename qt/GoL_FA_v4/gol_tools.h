#pragma once


char srcfilepath[100] = __FILE__ ;
char proname[100]="";

void extractproname(char *path) {
    int  l=strlen(path);
    char cstr[100], *p;

    strncpy(cstr, path, l);
    p=strrchr(cstr, '/');
    p[0]=0;
    p=strrchr(cstr, '/');
    strncpy(proname, p+1, strlen(p+1));
}


/*  

add in mainwindow.cpp:

#include "gol_tools.h"

// MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    extractproname(srcfilepath);
    this->setWindowTitle((QString)proname);    
    
*/



