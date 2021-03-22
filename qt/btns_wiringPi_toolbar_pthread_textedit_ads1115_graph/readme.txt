/*
* GPIO setup (BCM numbering):
*   pinMode(18, OUTPUT);  // triggered by program (blink)
    pinMode(23, OUTPUT);  // triggered by GUI buttons (on/off)
    pinMode(24, OUTPUT);  // triggered by GPIO switch (on/off)

    pinMode(6 , INPUT);  pullUpDnControl(6,  PUD_UP); // init, default
    pinMode(16, INPUT);  pullUpDnControl(16, PUD_UP); // init, default
    pinMode(20, INPUT);  pullUpDnControl(20, PUD_UP); // init, default
    pinMode(21, INPUT);  pullUpDnControl(21, PUD_UP); // init, default
    pinMode(25, INPUT);  pullUpDnControl(25, PUD_UP); // init, default
*
* Input 25 can be switched from INPUT_PULLUP (default) to INPUT_PULLDOWN
* (beneath toolbar option Edit)
* when  INPUT_PULLDOWN  is activated, the switch has to be re-wired
* (GPIO25->switch->+3v3)
*
* ads1115 at i2c-1
* #include <wiringPi.h> 
* #include <ads1115.h>
* show up ads1115 A0 reading in widget label
*
* ui->plainTextEdit1->setMaximumBlockCount(2000); // set in form editor
*
*
* .pro file:
  INCLUDEPATH += /usr/local/include
  LIBS += -L"/usr/local/lib"
  LIBS += -lwiringPi
  LIBS += -lpthread
*/