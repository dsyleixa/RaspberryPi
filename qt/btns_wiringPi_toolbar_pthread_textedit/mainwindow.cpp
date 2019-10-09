#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <pthread.h>

/*
* GPIO setup (BCM numbering):
* 23: Output (green LED + resistor) // switchable by widget buttons)
* 24: Input  (default: GPIO24->switch->GND)
* 25: Output (red LED + resistor)  // LED is blinking while program runs
*
* Input 24 can be switched from INPUT_PULLUP (default) to INPUT_PULLDOWN
* (beyond toolbar option Edit)
* when  INPUT_PULLDOWN  is activated, the switch has to be re-wired
* (GPIO24->switch->+3v3)
*
*/

#define  Qwrite1(str)   ui->setTextEdit1->insertPlainText(str)
#define  Qwriteln1(str) ui->plainTextEdit1->appendPlainText(str)



int pinstate23=LOW, pinstate24=LOW, pinstate25=LOW;

void GPIOreset() {
    pinstate23 = LOW;
    digitalWrite(23, pinstate23);

    pinstate24 = LOW;
    digitalWrite(24, pinstate24);

    pinstate25 = LOW;
    digitalWrite(25, pinstate25);
}


void cleanup();


volatile bool TASKS_ACTIVE = true;
pthread_t thread0;



void* loop(void*)
{
    while(TASKS_ACTIVE )  {  // blink loop
        pinstate25=LOW;
        digitalWrite(25, pinstate25);
        delay(500);
        pinstate25=HIGH;
        digitalWrite(25, pinstate25);
        delay(500);
    }
    return NULL;  //
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iores = wiringPiSetupGpio();// init by BCM pin numbering
    if( iores == -1 )
        exit(1);

    ui->plainTextEdit1->setStyleSheet("(color:black)");
    //ui->plainTextEdit1->setMaximumBlockCount(2000); // set in form editor

    pinMode(23, OUTPUT);
    pinMode(25, OUTPUT);
    GPIOreset();

    pinMode(24, INPUT);
    pullUpDnControl(24, PUD_UP); // init, default

    ui->pin23Label->setText(QString::number(pinstate23));

    pthread_create(&thread0, NULL, loop, NULL);

    //on_lowButton_clicked();// Slots can be called also within the program
    onUpdateTime();

    // Every time the Timer will reach its terminal count
    // invoke the "slot" onUpdateTime()
    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(50);// in msec
    // Go to the event loop...
}


MainWindow::~MainWindow() {
    updateTimer.stop();
    cleanup();
    delete ui;
}



void cleanup() {
    TASKS_ACTIVE = false;
    pthread_join(thread0, NULL);
    GPIOreset();
}




// Invoked every 50ms
void
MainWindow::onUpdateTime() {

    pinstate24 = digitalRead(24);
    ui->pin24Label->setText(QString::number(pinstate24));
    Qwriteln1("pinstate24="+QString::number(pinstate24));

    ui->label_3->setText(QString::number(pinstate25));
    Qwriteln1("pinstate25="+QString::number(pinstate25));

    Qwriteln1("pinstate23="+QString::number(pinstate23));
}


void
MainWindow::on_highButton_clicked() {    
    pinstate23 = HIGH;
    digitalWrite(23, pinstate23);
    ui->pin23Label->setText(QString::number(pinstate23));
    Qwriteln1("pinstate23="+QString::number(pinstate23));
}


void
MainWindow::on_lowButton_clicked() {
    pinstate23 = LOW;
    digitalWrite(23, pinstate23);
    ui->pin23Label->setText(QString::number(pinstate23));
    Qwriteln1(QString::number(pinstate23));
    Qwriteln1("pinstate23="+QString::number(pinstate23));
}


void __attribute__((noreturn))
MainWindow::on_quitButton_clicked() {
    updateTimer.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}


void
MainWindow::on_actionQuit_triggered()
{
    //ui->statusBar->showMessage("File Quit menu activated", 2000);
    updateTimer.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}


void
MainWindow::on_actionGPIO24_pullup_triggered()
{
    ui->statusBar->showMessage("Test for GPIO pullup activated ", 1000);
    pullUpDnControl(24, PUD_UP);
}


void
MainWindow::on_actionGPIO24_pulldown_triggered()
{
    ui->statusBar->showMessage("Test for GPIO pulldown activated ", 1000);
    pullUpDnControl(24, PUD_DOWN);
}


