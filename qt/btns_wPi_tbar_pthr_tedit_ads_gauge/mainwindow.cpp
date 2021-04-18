#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <ads1115.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>



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


int adcraw[8],
    analog[8];
float anaf[8];
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

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
}


//-------------------------------------------------------------------------------
// ADC map
int32_t adc16To10bit(int32_t adc) {
    int32_t ADSMAX16 = 26280;
    adc = map(adc, 0, ADSMAX16, 0, 1023);
    if(adc > 1023) adc=1023;
    if(adc < 0)      adc=0;
    return adc;
}

float adcToPerc(float adc) {
    float ADSMAX16 = 26280.0;
    adc = adc*100.0/ADSMAX16;
    if(adc > 100.0)  adc=100.0;
    if(adc < 0)      adc=0.0;
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
    ads1115Setup(PINBASE, ADS_ADDR);

    

    // widgets
    // calculate  circle helper tables
    initCircleXY();  // radius=1
    initCustomCircleXY(offsX, offsY, radius, -2, 4); // custom pos and radius

    outlinePen.setWidth(2);
    redPen.setWidth(3);

    //ui->plainTextEdit1->setStyleSheet("(color:black)");
    //ui->plainTextEdit1->setMaximumBlockCount(2000); // set in form editor

    scene0 = new QGraphicsScene(this);
    ui->graphicsView_0->setScene(scene0);

    scene1 = new QGraphicsScene(this);
    ui->graphicsView_1->setScene(scene1);

    scene2 = new QGraphicsScene(this);
    ui->graphicsView_2->setScene(scene2);

    scene3 = new QGraphicsScene(this);
    ui->graphicsView_3->setScene(scene3);

    // Gauge Clock Faces
    drawGauge(scene0, &pline0, &text0);
    drawGauge(scene1, &pline1, &text1);
    drawGauge(scene2, &pline2, &text2);
    drawGauge(scene3, &pline3, &text3);


    // threads, timers
    pthread_create(&thread0, NULL, loop, NULL);

    onUpdateTime1();               // invoke the "slot" onUpdateTime1(): quick
    connect(&Timer1, SIGNAL(timeout()), this, SLOT(onUpdateTime1()));
    Timer1.start(10);       // Start the timer (in msec)

    onUpdateTime2();               // invoke the "slot" onUpdateTime2(): slow
    connect(&Timer2, SIGNAL(timeout()), this, SLOT(onUpdateTime2()));
    Timer2.start(50);       // Start the timer (in msec)
}


//-------------------------------------------------------------------------------
//  destructor
MainWindow::~MainWindow() {
    Timer1.stop();
    Timer2.stop();
    cleanup();
    delete ui;
}


//-------------------------------------------------------------------------------
//  paint     // Gauge clock face

void MainWindow::drawGauge(QGraphicsScene *scene, QGraphicsLineItem **pline, QGraphicsTextItem **text) {
    scene->clear();
    rectangle = scene->addRect(0, 0, 120, 60, outlinePen, whiteBrush);

    for(int i=0; i<180; i++) {
       line = scene->addLine(myCircleXY[i][0], myCircleXY[i][1], myCircleXY[i+1][0], myCircleXY[i+1][1], outlinePen);
    }
    for (int m=0; m<=10; m++) {
        int w=18*m;
        line = scene->addLine(myCircleXY_sm[w][0], myCircleXY_sm[w][1], myCircleXY_lg[w][0], myCircleXY_lg[w][1], outlinePen);
    }

    QGraphicsEllipseItem* circle=scene->addEllipse(offsX+radius-3, offsY-3, 6, 6, redPen, redBrush);

    QGraphicsLineItem* plinetemp = scene->addLine(offsX+radius, offsY, offsX+3, offsY, redPen);
    plinetemp->setTransformOriginPoint(offsX+radius, offsY);
    *pline = plinetemp;

    QGraphicsTextItem* texttemp = scene->addText("", QFont("Arial", 14) );
    texttemp->setPos(offsX+5, offsY-30);
    *text = texttemp;
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
    for (int i=0; i<4; i++) {
       adcraw[i] = analogRead(PINBASE + i);
       analog[i] = adc16To10bit(adcraw[i]);  // adjust to 10 bit
       anaf[i]   = adcToPerc(adcraw[i]);     // adjust to Percent
    }

}


//-------------------------------------------------------------------------------
void MainWindow::onUpdateTime2() {  // slow
    double val;
    char buf[10];

    // show up values on dashboard
    // inputs
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

    // outputs
    ui->label_p0->setText(QString::number(pinstate[18]));
    ui->label_p1->setText(QString::number(pinstate[23]));
    ui->label_p2->setText(QString::number(pinstate[24]));

    Qwriteln1("pinstate[18]="+QString::number(pinstate[18]));
    Qwriteln1("pinstate[23]="+QString::number(pinstate[23]));
    Qwriteln1("pinstate[24]="+QString::number(pinstate[24]));

    // debug, test
    ui->label_ads1115A0->setText( QString::number(adcraw[0]) );
    ui->label_ads1115A1->setText( QString::number(adcraw[1]) );
    ui->label_ads1115A2->setText( QString::number(adcraw[2]) );
    ui->label_ads1115A3->setText( QString::number(adcraw[3]) );

    // Gauge pointer needles updates
    //Gauge 0
    val=(anaf[0])*180.0/100;
    pline0->setRotation(val);
    text0->setPlainText( "A0="+QString::number(anaf[0], 'f', 1) );

    //Gauge 1
    val=(anaf[1])*180.0/100;
    pline1->setRotation(val);
    text1->setPlainText( "A1="+QString::number(anaf[1], 'f', 1) );

    //Gauge 2
    val=(anaf[2])*180.0/100;
    pline2->setRotation(val);
    text2->setPlainText( "A2="+QString::number(anaf[2], 'f', 1) );

    //Gauge 3
    val=(anaf[3])*180.0/100;
    pline3->setRotation(val);
    text3->setPlainText( "A3="+QString::number(anaf[3], 'f', 1) );
}




//-------------------------------------------------------------------------------
//  Quit
//-------------------------------------------------------------------------------

void __attribute__((noreturn))
MainWindow::on_quitButton_clicked() {
    Timer1.stop();
    Timer2.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------

void MainWindow::on_actionQuit_triggered()
{
    //ui->statusBar->showMessage("File Quit menu activated", 2000);
    Timer1.stop();
    Timer2.stop();
    cleanup();
    exit(EXIT_SUCCESS);
}




