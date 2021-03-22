#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <ads1115.h>
#include <pthread.h>
#include <unistd.h>




/*
* ver 0.1
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


int analog0=0, analog1=0, analog2=0, analog3=0;
int pinstate[40];



void msleep(uint32_t ms) {
    usleep(ms*1000);
}



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
    pinstate[18] = LOW; digitalWrite(18, 0);
    pinstate[23] = LOW; digitalWrite(23, 0);
    pinstate[24] = LOW; digitalWrite(24, 0);
    // input switch
    pinstate[6]  = LOW; digitalWrite(6 , 0);
    pinstate[16] = LOW; digitalWrite(16, 0);
    pinstate[20] = LOW; digitalWrite(20, 0);
    pinstate[21] = LOW; digitalWrite(21, 0);
    pinstate[25] = LOW; digitalWrite(25, 0);
}

void cleanup();


volatile bool TASKS_ACTIVE = true;
pthread_t thread0;




void* loop(void*)
{
    while(TASKS_ACTIVE )  {  // blink loop
        pinstate[23]=LOW;
        digitalWrite(23, pinstate[23]);
        //delay(500);
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        msleep(500);
        pinstate[23]=HIGH;
        digitalWrite(23, pinstate[23]);        
        //delay(500);
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        msleep(500);
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

    ui->pin25Label->setText(QString::number(pinstate[25]));


    scene0 = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene0);
    scene1= new QGraphicsScene(this);
    ui->graphicsView_1->setScene(scene1);

    // Create brushes and outline pen
    QBrush blueBrush(Qt::blue);
    QBrush greenBrush(Qt::green);
    QBrush redBrush(Qt::red);
    QBrush whiteBrush(Qt::white);
    QPen outlinePen(Qt::black);

    outlinePen.setWidth(2);

    rectangle = scene0->addRect(0, 0, 120, 60, outlinePen, whiteBrush);

    QPainterPath* arcp = new QPainterPath();
    arcp->moveTo(60, 110);
    arcp->arcTo(10, 10, 100, 60, 0, 180);
    scene0->addPath(*arcp);
    line = scene0->addLine(0, 0, 100, 0, outlinePen);
    line->setPos(10, 50);

    text = scene0->addText("ADC0", QFont("Arial", 8) );

    
// next gauge

    rectangle = scene1->addRect(0, 0, 120, 60, outlinePen, whiteBrush);



    pthread_create(&thread0, NULL, loop, NULL);

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
    int newval= (val*newmax)/oldmax;
    if(newval>newmax) newval=newmax;
    return newval;
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

    // read pins only

    pinstate[6] = digitalRead(6);
    ui->pin6Label->setText(QString::number(pinstate[6]));
    Qwriteln1("pinstate[6]="+QString::number(pinstate[6]));

    pinstate[16] = digitalRead(16);
    ui->pin16Label->setText(QString::number(pinstate[16]));
    Qwriteln1("pinstate[16]="+QString::number(pinstate[16]));

    pinstate[20] = digitalRead(20);
    ui->pin20Label->setText(QString::number(pinstate[20]));
    Qwriteln1("pinstate[20]="+QString::number(pinstate[20]));

    pinstate[21] = digitalRead(21);
    ui->pin21Label->setText(QString::number(pinstate[21]));
    Qwriteln1("pinstate[21]="+QString::number(pinstate[21]));

    // read GPIO pin and switch related LED pin

    pinstate[25] = digitalRead(25);
    ui->pin25Label->setText(QString::number(pinstate[25]));
    Qwriteln1("pinstate[25]="+QString::number(pinstate[25]));
    pinstate[24]=pinstate[25];
    if(pinstate[24]) digitalWrite(24, HIGH); else digitalWrite(24, LOW);

    // output 23 auto-program-triggered, outputs 18+24 manually

    ui->pin18Label->setText(QString::number(pinstate[18]));
    ui->label_p1->setText(QString::number(pinstate[23]));
    Qwriteln1("pinstate[18]="+QString::number(pinstate[18]));
    Qwriteln1("pinstate[23]="+QString::number(pinstate[23]));
    Qwriteln1("pinstate[24]="+QString::number(pinstate[24]));

    //analog0 = analogRead(PINBASE + 0);          // debug
    //analog1 = analogRead(PINBASE + 1);          // debug
    analog0 = adc16To10bit(PINBASE, 0, 26243);  // adjust to actual potentiometer max
    analog1 = adc16To10bit(PINBASE, 1, 26243);
    analog2 = adc16To10bit(PINBASE, 2, 26243);
    analog3 = adc16To10bit(PINBASE, 3, 26243);

    ui->label_ads1115A0->setText(QString::number(analog0));
    ui->label_ads1115A1->setText(QString::number(analog1));
    ui->label_ads1115A2->setText(QString::number(analog2));
    ui->label_ads1115A3->setText(QString::number(analog3));
}


// read GUI button and switch related LED pin

void MainWindow::on_highButton_clicked() {
    pinstate[18] = HIGH;
    digitalWrite(18, pinstate[18]);
}
void MainWindow::on_lowButton_clicked() {
    pinstate[18] = LOW;
    digitalWrite(18, pinstate[18]);
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
