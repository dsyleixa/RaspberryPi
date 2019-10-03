#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>

/*
 * GPIO setup (BCM numbering):
 * 23: Output (green LED + resistor) // switchable by widget buttons)
 * 24: Input  (default: GPIO24->switch->GND)
 * 25: Output (red LED + resistor)  // LED is lit while program runs
 *
 * Input 24 can be switched from INPUT_PULLUP (default) to INPUT_PULLDOWN
 * (beneath toolbar option Edit)
 * when  INPUT_PULLDOWN  is activated, the switch has to be re-wired
 * (GPIO24->switch->+3v3)
 *
*/



nWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iores = wiringPiSetupGpio();// init by BCM pin numbering
    if( iores == -1 )
        exit(1);

    pinMode(23, OUTPUT);
    digitalWrite(23, LOW); // init, default
    pin23 = LOW;
    ui->pin23Label->setText(QString("Low"));

    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH); // init, default

    pinMode(24, INPUT);
    pullUpDnControl(24, PUD_UP); // init, default


    //on_lowButton_clicked();// Slots can be called also within the program
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
    digitalWrite(23, LOW);
    digitalWrite(25, LOW);
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
    digitalWrite(23, LOW);
    digitalWrite(25, LOW);
    exit(EXIT_SUCCESS);
}


void MainWindow::on_actionQuit_triggered()
{
    //ui->statusBar->showMessage("File Quit menu activated", 2000);
    digitalWrite(23, LOW);
    digitalWrite(25, LOW);
    exit(EXIT_SUCCESS);
}


void MainWindow::on_actionGPIO24_pullup_triggered()
{
    ui->statusBar->showMessage("Test for GPIO pullup activated ", 1000);
    pullUpDnControl(24, PUD_UP);
}


void MainWindow::on_actionGPIO24_pulldown_triggered()
{
    ui->statusBar->showMessage("Test for GPIO pulldown activated ", 1000);
    pullUpDnControl(24, PUD_DOWN);
}
