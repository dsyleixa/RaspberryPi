// ref: https://doc.qt.io/qt-5/qgraphicsview.html   
// with friendly support from Heater, raspberrypi.org forums

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene  = new QGraphicsScene(this);

    //--------------------------------------------------------------
    // Timer
    // QTimer updateTimer;  // see: mainwindow.h!
    // onUpdateTime();      // see: mainwindow.h!
    // Every time the Timer will reach its terminal count
    // invoke the "slot" onUpdateTime()
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(50);// in msec

 }


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    exit(EXIT_SUCCESS);
}




// Invoked every 50ms
void
MainWindow::onUpdateTime() {
    //--------------------------------------------------------------
    // Create a GraphicsScene and add it to the graphicsView
    // scene  = new QGraphicsScene(this);  // see: MainWindow constructor!

    scene->clear();

    ui->graphicsView->setScene(scene);
    int r,s,x,y;

    // Create a blue and green brushes and outline pen
    QBrush blueBrush(Qt::blue);
    QBrush greenBrush(Qt::green);
    QBrush redBrush(Qt::red);
    QPen outlinePen(Qt::black);    
    outlinePen.setWidth(2);

    // Add a blue rectangle to our graphics scene.
    x=rand()%200;
    y=rand()%200;
    rectangle = scene->addRect(0, 0, x, y, outlinePen, blueBrush);

    // Add a green elipse to our graphics scene.
    // addEllipse(x,y,w,h,pen,brush)
    ellipse = scene->addEllipse(50, 50, 300, 60, outlinePen, greenBrush);

    x=rand()%100;
    y=rand()%100;
    r=rand()%200;
    s=rand()%200;
    ellipse = scene->addEllipse(x, y, r, s, outlinePen, redBrush);
    ellipse->setPos(100, 100);

    // Add a some text to our graphics scene.
    text = scene->addText("Hi dsyleixa123!", QFont("Arial", 20) );
    text->setPos(80, 60);


    // Add a line
    line = scene->addLine(350, 0, 200, 200, outlinePen);
    line->setPos(0, 0);

    ui->graphicsView->centerOn(200,200);  // <<<< not perfectly focussing yet

}
