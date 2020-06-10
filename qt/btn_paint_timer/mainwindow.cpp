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
    // QTimer updateTimer;  // see: mainwindow.h
    // onUpdateTime(); // see: mainwindow.h
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
    close();
}




// Invoked every 50ms
void
MainWindow::onUpdateTime() {
    //--------------------------------------------------------------
    // Heater: Create a GraphicsScene and add it to the graphicsView    

    scene->clear();
    ui->graphicsView->setScene(scene);
    int r,s,x,y;

    // Heater: Create a blue and green brushes and outline pen
    QBrush blueBrush(Qt::blue);
    QBrush greenBrush(Qt::green);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    // Heater: Add a blue rectabgle to our graphics scene.
    x=rand()%200;
    y=rand()%200;
    rectangle = scene->addRect(0, 0, x, y, outlinePen, blueBrush);

    // Heater: Add a green elipse to our graphics scene.
    // addEllipse(x,y,w,h,pen,brush)
    ellipse = scene->addEllipse(50, 50, 300, 60, outlinePen, greenBrush);

    // Heater: Add a some text to our graphics scene.
    text = scene->addText("Hi dsyleixa123!", QFont("Arial", 20) );

    // Heater: Add a line
    line = scene->addLine(250, 0, 200, 200, outlinePen);
    line->setPos(0, 0);

}
