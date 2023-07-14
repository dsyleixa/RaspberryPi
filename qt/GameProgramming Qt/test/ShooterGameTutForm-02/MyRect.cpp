#include "MyRect.h"
#include <QKeyEvent>

void MyRect::keyPressEvent(QKeyEvent *event){
    // qDebug() << "MyRect knows this kepress"

    if (event->key() == Qt::Key_Left){
        setPos(x()-10,y());
    }
    else if (event->key() == Qt::Key_Right){
        setPos(x()+10,y());
    }
    else if (event->key() == Qt::Key_Up){
        setPos(x(),y()-10);
    }
    else if (event->key() == Qt::Key_Down){
        setPos(x(),y()+10);
    }
}
