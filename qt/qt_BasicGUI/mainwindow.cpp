#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui->setupUi(this);
    /*
    QPushButton * pushButton1 = new QPushButton();
    pushButton1->disconnect();
    */
    ui->pushButton1->setFocusPolicy(Qt::NoFocus);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton1_clicked()
{
    close();
    //QCoreApplication::quit();
}
