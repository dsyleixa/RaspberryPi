#pragma once


char * srcfilepath = __FILE__ ;
char proname[50]="";

void extractproname(char *path) {
    int  l=strlen(path);
    char cstr[50], *p;

    for(int i=0; i<l-3; i++ ) {
        cstr[i]=path[i+3];
    }
    cstr[l-3]=0;
    p=strrchr(cstr,'/');
    p[0]=0;
    strncpy(proname,cstr,strlen(cstr) );
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



