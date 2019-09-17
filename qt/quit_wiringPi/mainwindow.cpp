#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iores = wiringPiSetupGpio();// init by BCM pin numbering
    if( iores == -1 )
        exit(1);

    pinMode(23, OUTPUT);
    pinMode(24, INPUT);
    pullUpDnControl(24, PUD_UP);

    on_lowButton_clicked();// Slots can be called also within the program
    onUpdateTime();

    // Every time the Timer will reach its terminal count
    // invoke the "slot" onUpdateTime()
    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(100);// in msec
    // Go to the event loop...
}


MainWindow::~MainWindow() {
    updateTimer.stop();
    delete ui;
}


// Invoked every 100ms
void
MainWindow::onUpdateTime() {
    pin24 = digitalRead(24);
    ui->pin24Label->setText(pin24 ? QString("High") : QString("Low"));
}


void
MainWindow::on_highButton_clicked() {
    digitalWrite(23, HIGH);
    pin23 = HIGH;
    ui->pin23Label->setText(QString("High"));
}


void
MainWindow::on_lowButton_clicked() {
    digitalWrite(23, LOW);
    pin23 = LOW;
    ui->pin23Label->setText(QString("Low"));
}


void __attribute__((noreturn))
MainWindow::on_quitButton_clicked() {
    exit(EXIT_SUCCESS);
}
