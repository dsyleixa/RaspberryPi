#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <ads1115.h>
#include <pthread.h>




/*
* ver 0.8
*
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

#define  PINBASE  120
#define  ADS_ADDR 0x48


int pinstate18=LOW, pinstate23=LOW, pinstate24=LOW;
int pinstate6=LOW, pinstate16=LOW, pinstate20=LOW, pinstate21=LOW, pinstate25=LOW;
int analog0=0, analog1=0, analog2=0, analog3=0;




void GPIOsetup() {
    pinMode(18, OUTPUT);  // triggered by program (blink)
    pinMode(23, OUTPUT);  // triggered by GUI buttons (on/off)
    pinMode(24, OUTPUT);  // yet unused

    pinMode(6 , INPUT);  pullUpDnControl(6,  PUD_UP); // init, default
    pinMode(16, INPUT);  pullUpDnControl(16, PUD_UP); // init, default
    pinMode(20, INPUT);  pullUpDnControl(20, PUD_UP); // init, default
    pinMode(21, INPUT);  pullUpDnControl(21, PUD_UP); // init, default
    pinMode(25, INPUT);  pullUpDnControl(25, PUD_UP); // init, default
}

void GPIOreset() {
    // outputs
    pinstate18 = LOW; digitalWrite(18, 0);
    pinstate23 = LOW; digitalWrite(23, 0);
    pinstate24 = LOW; digitalWrite(24, 0);
    // input switch
    pinstate6  = LOW; digitalWrite(6 , 0);
    pinstate16 = LOW; digitalWrite(16, 0);
    pinstate20 = LOW; digitalWrite(20, 0);
    pinstate21 = LOW; digitalWrite(21, 0);
    pinstate25 = LOW; digitalWrite(25, 0);
}

void cleanup();


volatile bool TASKS_ACTIVE = true;
pthread_t thread0;



void* loop(void*)
{
    while(TASKS_ACTIVE )  {  // blink loop
        pinstate23=LOW;
        digitalWrite(23, pinstate23);
        delay(500);
        pinstate23=HIGH;
        digitalWrite(23, pinstate23);
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

    GPIOsetup();
    GPIOreset();

    pullUpDnControl(8, PUD_UP); // init, default

    ads1115Setup ( PINBASE, ADS_ADDR );

    ui->pin25Label->setText(QString::number(pinstate25));

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

int32_t map(int32_t val, int32_t oldmax, int32_t newmax) {
    if (oldmax==0) return 0;
    else  return (val*newmax)/oldmax;
}


int32_t adc16To10bit(int pinbase, int channel, int actmax) {
    int32_t adc = analogRead(pinbase + channel);

    adc = map(adc, actmax, 1023); 
    if(adc > 1023) adc=1023;
	if(adc < 0)    adc=0;	

	return adc;
}


// Invoked every 50ms
void
MainWindow::onUpdateTime() {

    pinstate6 = digitalRead(6);
    ui->pin6Label->setText(QString::number(pinstate6));
    Qwriteln1("pinstate6="+QString::number(pinstate21));

    pinstate16 = digitalRead(16);
    ui->pin16Label->setText(QString::number(pinstate16));
    Qwriteln1("pinstate16="+QString::number(pinstate16));

    pinstate20 = digitalRead(20);
    ui->pin20Label->setText(QString::number(pinstate20));
    Qwriteln1("pinstate20="+QString::number(pinstate20));

    pinstate21 = digitalRead(21);
    ui->pin21Label->setText(QString::number(pinstate21));
    Qwriteln1("pinstate21="+QString::number(pinstate21));

    pinstate25 = digitalRead(25);
    ui->pin25Label->setText(QString::number(pinstate25));
    Qwriteln1("pinstate25="+QString::number(pinstate25));

    // program-triggered
    ui->pin18Label->setText(QString::number(pinstate18));
    ui->label_p1->setText(QString::number(pinstate23));

    Qwriteln1("pinstate18="+QString::number(pinstate18));
    Qwriteln1("pinstate23="+QString::number(pinstate23));
    Qwriteln1("pinstate25="+QString::number(pinstate25));

    analog0 = adc16To10bit(PINBASE, 0, 26340);  // adjust for actual potentiometer max
    //analog0 = analogRead(PINBASE + 0);
    analog1 = adc16To10bit(PINBASE, 1, 26340);
    analog2 = adc16To10bit(PINBASE, 2, 26340);
    analog3 = adc16To10bit(PINBASE, 3, 26340);

    ui->label_ads1115A0->setText(QString::number(analog0));
    ui->label_ads1115A1->setText(QString::number(analog1));
    ui->label_ads1115A2->setText(QString::number(analog2));
    ui->label_ads1115A3->setText(QString::number(analog3));
}


void
MainWindow::on_highButton_clicked() {    
    pinstate18 = HIGH;
    digitalWrite(18, pinstate18);
}


void
MainWindow::on_lowButton_clicked() {
    pinstate18 = LOW;
    digitalWrite(18, pinstate23);
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





/*

void MainWindow::on_actionGPIO25_PUP_triggered()
{
    ui->statusBar->showMessage("Test for GPIO25 pullup activated ", 1000);
    pullUpDnControl(25, PUD_UP);
}

void MainWindow::on_actionGPIO25_PDN_triggered()
{
    ui->statusBar->showMessage("Test for GPIO25 pulldown activated ", 1000);
    pullUpDnControl(25, PUD_DOWN);
}

*/

void MainWindow::on_actionGPIO25_PUP_triggered()
{
    ui->statusBar->showMessage("Test for GPIO25 pullup activated ", 1000);
    pullUpDnControl(25, PUD_UP);
}

void MainWindow::on_actionGPIO25_PDN_triggered()
{
    ui->statusBar->showMessage("Test for GPIO25 pulldown activated ", 1000);
    pullUpDnControl(25, PUD_DOWN);
}
