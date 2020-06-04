#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Heater: Create a GraphicsScene and add it to the graphicsView
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Heater: Create a blue and green brushes and outline pen
    QBrush blueBrush(Qt::blue);
    QBrush greenBrush(Qt::green);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    // Heater: Add a blue rectabgle to our graphics scene.
    rectangle = scene->addRect(0, 0, 200, 100, outlinePen, blueBrush);

    // Heater: Add a green elipse to our graphics scene.
    // addEllipse(x,y,w,h,pen,brush)
    ellipse = scene->addEllipse(50, 50, 300, 60, outlinePen, greenBrush);

    // Heater: Add a some text to our graphics scene.
    text = scene->addText("Hi dsyleixa123!", QFont("Arial", 20) );

    // Heater: Add a line
    line = scene->addLine(250, 0, 200, 200, outlinePen);
    //line->setPos(0, 0);
 }

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    close();
}
