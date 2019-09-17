#include "mainwindow.h"
#include <QApplication>
#include <wiringPi.h>

volatile static char pin23, pin24;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    int iores = wiringPiSetupGpio();      // init by BCM pin numbering
        if( iores == -1 ) return 1;

    pinMode(23, OUTPUT);
    pinMode(24, INPUT);
    pullUpDnControl(24, PUD_UP);

    w.show();

    return a.exec();
}
