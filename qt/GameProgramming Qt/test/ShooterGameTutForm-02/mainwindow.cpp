// mainwindow.cpp


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include "MyRect.h"       // rect: keyPressEvent=> move
#include <QGraphicsView>
#include <QKeyEvent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui->setupUi(this);

    scene  = new QGraphicsScene(this);

    MyRect * rect = new MyRect();  // custom rect derived from QGraphicsItem
    rect->setRect(0,0, 100,100);
    scene->addItem(rect);

    rect->setFlag(QGraphicsItem::ItemIsFocusable); // can respond to events
    rect->setFocus(); // do respond!

    ui->graphicsView->setScene(scene);
}


//...
