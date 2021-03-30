#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <ads1115.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>




/*
* ver 0.6b
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
int maxADC = 1023;


int analog0=0, analog1=0, analog2=0, analog3=0;
int pinstate[40];

//-------------------------------------------------------------------------------
// QGraphics
//-------------------------------------------------------------------------------

// Create brushes and outline pen
QBrush blueBrush(Qt::blue);
QBrush greenBrush(Qt::green);
QBrush redBrush(Qt::red);
QBrush whiteBrush(Qt::white);
QBrush transpBrush(Qt::transparent);
QPen outlinePen(Qt::black);
QPen whitePen(Qt::white);
QPen redPen(Qt::red);


// circle lookup tables
float uCircleXY[362][2];      // unit circle (math convention)
float myCircleXY[362][2];     // custom circle (qt conventions)
float myCircleXY_sm[362][2];  // smaller
float myCircleXY_lg[362][2];  // larger


// gauge clock faces
float offsX=10, offsY=55, radius=50;

// calc unit circle (math convention)
void initCircleXY() {
    for(int i=0;i<=361; i++) {
       uCircleXY[i][0]= cosf(i*M_PI/180);
       uCircleXY[i][1]= sinf(i*M_PI/180);
    }
}

// calc custom circle (qt conventions)
void initCustomCircleXY(float offsetX, float offsetY, float radius, float sm, float lg) {
    float x=0, y=0;

    for(int i=0; i<=361; i++) {
       x = radius*uCircleXY[i][0]+offsetX+radius;
       y =-radius*uCircleXY[i][1]+offsetY;
       myCircleXY[i][0]=x;
       myCircleXY[i][1]=y;

       x = (radius+sm)*uCircleXY[i][0]+offsetX+radius;
       y =-(radius+sm)*uCircleXY[i][1]+offsetY;
       myCircleXY_sm[i][0]=x;
       myCircleXY_sm[i][1]=y;

       x = (radius+lg)*uCircleXY[i][0]+offsetX+radius;
       y =-(radius+lg)*uCircleXY[i][1]+offsetY;
       myCircleXY_lg[i][0]=x;
       myCircleXY_lg[i][1]=y;
    }
}


//-------------------------------------------------------------------------------
// GPIOs
//-------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------
// map values
long  map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
}


//-------------------------------------------------------------------------------
// ADC map
int32_t read_adc16To10bit(int pinbase, int channel, int actmax16) {
    int32_t adc = analogRead(pinbase + channel);

    adc = map(adc, 0, actmax16, 0, maxADC);
    if(adc > maxADC) adc=maxADC;
    if(adc < 0)      adc=0;

    return adc;
}


//-------------------------------------------------------------------------------
// thread, timing
//-------------------------------------------------------------------------------

volatile bool TASKS_ACTIVE = true;
pthread_t thread0;


void msleep(uint32_t ms) {
    usleep(ms*1000);
}


//-------------------------------------------------------------------------------
// GPIO cleanup
void cleanup() {
    TASKS_ACTIVE = false;
    pthread_join(thread0, NULL);
    GPIOreset();
}



//-------------------------------------------------------------------------------
// pthread loops
//-------------------------------------------------------------------------------

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



//-------------------------------------------------------------------------------
// main()
//-------------------------------------------------------------------------------

//  constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iores = wiringPiSetupGpio();// init by BCM pin numbering
    if( iores == -1 )
        exit(1);

    GPIOsetup();
    GPIOreset();
    ads1115Setup ( PINBASE, ADS_ADDR );

    
    ui->plainTextEdit1->setStyleSheet("(color:black)");
    //ui->plainTextEdit1->setMaximumBlockCount(2000); // set in form editor

    // calculate  circle helper tables
    initCircleXY();  // radius=1
    initCustomCircleXY(offsX, offsY, radius, -2, 4); // custom pos and radius

    outlinePen.setWidth(2);
    redPen.setWidth(3);

    scene0 = new QGraphicsScene(this);
    ui->graphicsView_0->setScene(scene0);

    scene1 = new QGraphicsScene(this);
    ui->graphicsView_1->setScene(scene1);

    scene2 = new QGraphicsScene(this);
    ui->graphicsView_2->setScene(scene2);

    scene3 = new QGraphicsScene(this);
    ui->graphicsView_3->setScene(scene3);

    // Gauge Clock Faces
    drawGauge(scene0);
    drawGauge(scene1);
    drawGauge(scene2);
    drawGauge(scene3);


    // Gauge variant pointer needles and text

    pline0 = scene0->addLine(offsX+radius, offsY, offsX+3, offsY, redPen);
    pline1 = scene1->addLine(offsX+radius, offsY, offsX+3, offsY, redPen);
    pline2 = scene2->addLine(offsX+radius, offsY, offsX+3, offsY, redPen);
    pline3 = scene3->addLine(offsX+radius, offsY, offsX+3, offsY, redPen);
    pline0->setTransformOriginPoint(offsX+radius, offsY);
    pline1->setTransformOriginPoint(offsX+radius, offsY);
    pline2->setTransformOriginPoint(offsX+radius, offsY);
    pline3->setTransformOriginPoint(offsX+radius, offsY);

    text0 = scene0->addText("A0=", QFont("Arial", 16) );
    text1 = scene1->addText("A1=", QFont("Arial", 16) );
    text2 = scene2->addText("A2=", QFont("Arial", 16) );
    text3 = scene3->addText("A3=", QFont("Arial", 16) );
    text0->setPos(offsX+5, offsY-30);
    text1->setPos(offsX+5, offsY-30);
    text2->setPos(offsX+5, offsY-30);
    text3->setPos(offsX+5, offsY-30);


     // threads, timers

    pthread_create(&thread0, NULL, loop, NULL);

    onUpdateTime1();               // invoke the "slot" onUpdateTime1(): quick
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime1()));
    updateTimer.start(20);       // Start the timer (in msec)

    onUpdateTime2();               // invoke the "slot" onUpdateTime2(): slow
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime2()));
    updateTimer.start(100);       // Start the timer (in msec)
}


//-------------------------------------------------------------------------------
//  destructor
MainWindow::~MainWindow() {
    updateTimer.stop();
    cleanup();
    delete ui;
}


//-------------------------------------------------------------------------------
//  paint     // Gauge clock face

void MainWindow::drawGauge(QGraphicsScene *scene) {
    scene->clear();
    rectangle = scene->addRect(0, 0, 120, 60, outlinePen, whiteBrush);
    for(int i=0; i<180; i++) {
       line = scene->addLine(myCircleXY[i][0], myCircleXY[i][1], myCircleXY[i+1][0], myCircleXY[i+1][1], outlinePen);
    }
    // line = scene->addLine(offsX, offsY+2, offsX+(2*radius), offsY+2, outlinePen);
    line = scene->addLine(myCircleXY_sm[0][0], myCircleXY_sm[0][1], myCircleXY_lg[0][0], myCircleXY_lg[0][1], outlinePen);
    line = scene->addLine(myCircleXY_sm[90][0], myCircleXY_sm[90][1], myCircleXY_lg[90][0], myCircleXY_lg[90][1], outlinePen);
    line = scene->addLine(myCircleXY_sm[180][0], myCircleXY_sm[180][1], myCircleXY_lg[180][0], myCircleXY_lg[180][1], outlinePen);
}


//-------------------------------------------------------------------------------
//  read GUI buttons and switch related LED pin

void MainWindow::on_highButton_clicked() {
    pinstate[18] = HIGH;
    digitalWrite(18, pinstate[18]);
}

void MainWindow::on_lowButton_clicked() {
    pinstate[18] = LOW;
    digitalWrite(18, pinstate[18]);
}



//-------------------------------------------------------------------------------
//  GPIO set pullUp/Down
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



//-------------------------------------------------------------------------------
//  update time loops, invoked every ...ms
//-------------------------------------------------------------------------------
void MainWindow::onUpdateTime1() {  // quick

    // read input GPIOs
    pinstate[ 6] = digitalRead(6);
    pinstate[16] = digitalRead(16);
    pinstate[20] = digitalRead(20);
    pinstate[21] = digitalRead(21);
    pinstate[25] = digitalRead(25);

    // write output GPIOs
    // output GPIO 23 auto-program-triggered (pthread loop),
    // output GPIO 18 switch by ui HIGH+LOW button event slots
    // output GPIO 24 by pin 25 GPIO switch

    // write switch25-related LED output GPIO 24
    pinstate[24]=pinstate[25];
    if(pinstate[24]) digitalWrite(24, HIGH); else digitalWrite(24, LOW);

    // read i2c device(s) (ADC)
    //analog0 = analogRead(PINBASE + 0);             // debug
    //analog1 = analogRead(PINBASE + 1);             // debug
    analog0 = read_adc16To10bit(PINBASE, 0, 26243);  // adjust to actual poti max
    analog1 = read_adc16To10bit(PINBASE, 1, 26243);
    analog2 = read_adc16To10bit(PINBASE, 2, 26243);
    analog3 = read_adc16To10bit(PINBASE, 3, 26243);
}


//-------------------------------------------------------------------------------
void MainWindow::onUpdateTime2() {  // slow
    double val;

    // show up values on dashboard

    ui->pin6Label->setText(QString::number(pinstate[6]));
    Qwriteln1("pinstate[6]="+QString::number(pinstate[6]));

    ui->pin16Label->setText(QString::number(pinstate[16]));
    Qwriteln1("pinstate[16]="+QString::number(pinstate[16]));

    ui->pin20Label->setText(QString::number(pinstate[20]));
    Qwriteln1("pinstate[20]="+QString::number(pinstate[20]));

    ui->pin21Label->setText(QString::number(pinstate[21]));
    Qwriteln1("pinstate[21]="+QString::number(pinstate[21]));

    ui->pin25Label->setText(QString::number(pinstate[25]));
    Qwriteln1("pinstate[25]="+QString::number(pinstate[25]));    

    ui->pin18Label->setText(QString::number(pinstate[18]));
    ui->label_p1->setText(QString::number(pinstate[23]));

    Qwriteln1("pinstate[18]="+QString::number(pinstate[18]));
    Qwriteln1("pinstate[23]="+QString::number(pinstate[23]));
    Qwriteln1("pinstate[24]="+QString::number(pinstate[24]));

    // debug, test
    ui->label_ads1115A0->setText(QString::number(analog0));
    ui->label_ads1115A1->setText(QString::number(analog1));
    ui->label_ads1115A2->setText(QString::number(analog2));
    ui->label_ads1115A3->setText(QString::number(analog3));

    // Gauge pointer needles updates
    //Gauge 0
    val=(analog0)*180.0/maxADC;    
    pline0->setRotation(val);
    text0->setPlainText("A0="+QString::number(analog0));

    //Gauge 1
    val=(analog1)*180.0/maxADC;
    pline1->setRotation(val);
    text1->setPlainText("A1="+QString::number(analog1));

    //Gauge 2
    val=(analog2)*180.0/maxADC;
    pline2->setRotation(val);
    text2->setPlainText("A2="+QString::number(analog2));

    //Gauge 3
    val=(analog3)*180.0/maxADC;
    pline3->setRotation(val);
    text3->setPlainText("A3="+QString::number(analog3));
}




//-------------------------------------------------------------------------------
//  Quit
//-------------------------------------------------------------------------------

void __attribute__((noreturn))
MainWindow::on_quitButton_clicked() {
    updateTimer.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------

void MainWindow::on_actionQuit_triggered()
{
    //ui->statusBar->showMessage("File Quit menu activated", 2000);
    updateTimer.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}




