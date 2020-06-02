#include "mainwindow.h"
#include "ui_mainwindow.h"

void cleanup();


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    cleanup();
    delete ui;
}

void MainWindow::on_pushButton1_clicked()
{
    cleanup();
    delete ui;
}


void cleanup() {
    // to do:
    // TASKS_ACTIVE = false;
    // pthread_join(thread0, NULL);
    // GPIOreset();
}
