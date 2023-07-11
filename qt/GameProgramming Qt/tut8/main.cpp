#include <QApplication>
#include "Game.h"
#include <stdlib.h>

/*
Tutorial Topics:
-QGraphicsPixmapItem, QPixmap, QImage
*/

Game * game;

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    game = new Game();
    game->show();

    srand( time(0) );
    return a.exec();
}
