#pragma once


char * srcfilepath = __FILE__ ;
char proname[50]="";

void extractproname(char *path) {
    int  l=strlen(path);
    char cstr[50], *p;

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



