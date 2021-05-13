// with friendly support from Heater, raspberrypi.org forums

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>


QBrush blueBrush(Qt::blue);
QBrush greenBrush(Qt::green);
QBrush redBrush(Qt::red);
QBrush blackBrush(Qt::black);
QBrush whiteBrush(Qt::white);
QPen outlinePen(Qt::black);

int blockSize = 4;

int tftheight=640,
    tftwidth=640;

int statusGliderEater[3] = {0} ; // let Gliders pass
int statusRBnew[3] = {1} ;

int updspeed=50;

//---------------------------------------------------------------------------
// preferences and settings
//---------------------------------------------------------------------------
// The blocks are blockSize * blockSize big
// 2...6 seems to be a good value for this





// The size of the GoL screen window

const int screenWidth = 500;   // <~~~~~~~~~~~~ adjust screen dimensions !
const int screenHeight= 500;
const int frame = 10;




// Make the board larger on either side to ensure that there's an invisible border of dead cells

int yvisrows = (screenHeight / blockSize);
int xviscols = (screenWidth / blockSize);

int yrows = yvisrows + 2*frame;
int xcols = xviscols + 2*frame;

#define centeryrow (yrows/2)-1
#define centerxcol (xcols/2)-1

// two boards, one for the current generation and one for calculating the next one
char board[screenHeight + 2*frame][screenWidth + 2*frame];
char tmpboard[screenHeight + 2*frame][screenWidth + 2*frame];




//---------------------------------------------------------------------------
// GoL functions
//---------------------------------------------------------------------------

uint32_t GenerationCnt=1;


// Count thy neighbours
int countNeighbours(int yrow, int xcol)
{
  int count = 0;
  for (int x = -1; x <= +1; x++)  {
    for (int y = -1; y <= +1; y++) {
      if ((board[yrow + y][xcol + x] == 1) && (x != 0 || y != 0))
        count++;
    }
  }
  return count;
}


//---------------------------------------------------------------------------
// Calculate the cells that will live and die for the next generation
void calculateGeneration()
{
  int aliveNeighbours = 0;

  // Clear the board for the next generation
  memset(tmpboard, 0, sizeof(tmpboard));

  for (int yrow = 1; yrow < (yrows-1); yrow++)  {
    for (int xcol = 1; xcol < (xcols-1); xcol++)   {
      aliveNeighbours = countNeighbours(yrow, xcol);

      // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
      if(aliveNeighbours < 2)
        tmpboard[yrow][xcol] = 0;

      // Any live cell with two or three live neighbours lives on to the next calculateGeneration
      if (aliveNeighbours >= 2  &&  aliveNeighbours <= 3 )
        tmpboard[yrow][xcol] = board[yrow][xcol];

      // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction
      if(aliveNeighbours == 3 && board[yrow][xcol]==0)
        tmpboard[yrow][xcol] = 1;

      // Any live cell with more than three live neighbours dies, as if by overcyrowding
      if(aliveNeighbours > 3)
        tmpboard[yrow][xcol] = 0;
    }
  }
  // Copy the new board to the old one
  memcpy(board, tmpboard, sizeof(tmpboard));
}



//---------------------------------------------------------------------------
// patterns
//---------------------------------------------------------------------------

int Eater1x, Eater1y, Eater2x, Eater2y;


//---------------------------------------------------------------------------
// This adds some random live cells to the board
void put_randomBoard(int seedChance)
{
  for (int yrow = 1; yrow < (yrows - 1); yrow++)
  {
    for (int xcol = 1; xcol < (xcols - 1); xcol++)
    {
      board[yrow][xcol] = !(rand() % seedChance);
    }
  }
}


//---------------------------------------------------------------------------

void put_Blinker3x1(int starty, int startx) {       //
 char sprite[1][5] = {  //
    {1,1,1}
  } ;

  for(int x=0; x<3; ++x) {
       board[starty+frame][startx+frame+x]=sprite[0][x] ;
  }


}

//---------------------------------------------------------------------------
void put_Block2x2(int starty, int startx) {       //

  char sprite[2][2] = {  //
    {1,1},
    {1,1}
  } ;

  for(int x=0; x<2; ++x) {
    for(int y=0; y<2; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}



//---------------------------------------------------------------------------
void put_Bar5x1(int starty, int startx) {       //

  char sprite[1][5] = {  //
    {1,1,1,1,1}
  } ;

  for(int x=0; x<5; ++x) {
       board[starty+frame][startx+frame+x]=sprite[0][x] ;
  }


}


//---------------------------------------------------------------------------
void put_Clock(int starty, int startx) {    //
  int x,y;

  char sprite[4][4] = {  //
    {0,0,1,0},
    {1,1,0,0},
    {0,0,1,1},
    {0,1,0,0},
  } ;

  for(x=0; x<4; ++x) {
    for(y=0; y<4; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------

void put_F_Pentomino(int starty, int startx) {    //  == R-Pentomino
  int x,y;

  char sprite[3][3] = {  //
  {0,1,1},
  {1,1,0},
  {0,1,0},
  } ;

  for(x=0; x<3; ++x) {
    for(y=0; y<3; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_Pi_Heptomino(int starty, int startx) {    //

  int x,y;

  char sprite[3][5] = {  //
  {0,0,1,0,0},
  {0,1,0,1,0},
  {1,1,0,1,1}
  } ;

  for(x=0; x<5; ++x) {
    for(y=0; y<3; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_23334M(int starty, int startx) {    //
  int x,y;

  char sprite[8][5] = {  //
  {0,0,1,0,0},
  {1,1,0,0,0},
  {0,1,0,0,0},
  {1,0,0,1,0},
  {0,0,0,0,1},
  {0,1,0,0,1},
  {0,0,1,0,1},
  {0,1,0,0,0},
  } ;

  for(x=0; x<5; ++x) {
    for(y=0; y<8; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}



//---------------------------------------------------------------------------
void put_Glider(int starty, int startx) {    //

  int x,y;

  char sprite[3][3] = {  //
  {0,1,0,},
  {0,0,1,},
  {1,1,1,}
  } ;

  for(x=0; x<3; ++x) {
    for(y=0; y<3; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderUp(int starty, int startx) {    //

  int x,y;

  char sprite[3][3] = {  //
  {0,1,1,},
  {1,0,1,},
  {0,0,1,}
  } ;

  for(x=0; x<3; ++x) {
    for(y=0; y<3; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_LWSpaceship(int starty, int startx) {    //

  int x,y;

  char sprite[4][5] = {  //
  {1,0,0,1,0},
  {0,0,0,0,1},
  {1,0,0,0,1},
  {0,1,1,1,1},

  } ;

  for(x=0; x<5; ++x) {
    for(y=0; y<4; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_HWSpaceship(int starty, int startx) {    //

  int x,y;

  char sprite[5][7] = {  //
  {0,0,1,1,0,0,0},
  {0,0,0,0,0,1,0},
  {0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1},
  {0,1,1,1,1,1,1},

  } ;

  for(x=0; x<7; ++x) {
    for(y=0; y<5; ++y) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderGun(int starty, int startx) {  // Gosper Glider Gun, period=30

  int x,y;

  char sprite[9][37] = {  //
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  } ;

  for(x=0; x<37; ++x) {   // NXT screen (0,0) is bottom left, not top left  !
    for(y=0; y<9; ++y) {

      board[starty+frame+y][startx+frame+x] = sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderGunRev(int starty, int startx) {  // Gosper Glider Gun, period=30

  int x,y;

  char sprite[9][37] = {  //
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  } ;

  for(x=0; x<37; ++x) {   // NXT screen (0,0) is bottom left, not top left  !
    for(y=0; y<9; ++y) {

      board[starty+frame+y][startx+frame+x] = sprite[y][37-x] ;
    }
  }
}



//---------------------------------------------------------------------------
void put_GliderEater(int starty, int startx, int V) {
  int x,y;

  char sprite[6][6] = {  //
  {0 ,0 ,0 ,0 ,0 ,0 },
  {0 ,1 ,1 ,0 ,0 ,0 },
  {0 ,1 ,0 ,0 ,0 ,0 },
  {0 ,V ,1 ,1 ,1 ,0 },
  {0 ,0 ,0 ,0 ,1 ,0 },
  {0 ,0 ,0 ,0 ,0 ,0 },
  } ;

  for(x=0; x<6; ++x) {
    for(y=0; y<6; ++y) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
bool get_GliderEater(int starty, int startx, int ID) {

   bool isdestroyed= !board[starty+frame+0][startx+frame+0]
                  &&  board[starty+frame+0][startx+frame+2]
                  && !board[starty+frame+1][startx+frame+0]
                  &&  board[starty+frame+1][startx+frame+2]
                  &&  board[starty+frame+2][startx+frame+1]
                  && !board[starty+frame+2][startx+frame+2];

   if(isdestroyed) {
       /*
       Serial.println( "ID " +(String)ID + " Input=FALSE =>  Output=FALSE "
                  + " gen=" +(String)GenerationCnt);
       */
   }
   return isdestroyed;
}


void ResetGate() {
    memset(board, 0, sizeof(board));
    memset(tmpboard, 0, sizeof(tmpboard));


    int y1=10, x1=0, y2=10, x2=37+10;  // start pos of gliderguns

    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    statusGliderEater[1]=statusRBnew[1];
    statusGliderEater[2]=statusRBnew[2];

    // glidergun 1
    put_GliderGun( y1, x1 );
    // Gun Eater 1
    int deltaXY=1;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<
    Eater1y = 9 +y1 +deltaXY;
    Eater1x =23 +x1 +deltaXY;

    put_GliderEater( Eater1y, Eater1x, statusGliderEater[1]);

    // glidergun 2
    put_GliderGun( y2, x2 );
    // Gun Eater 2
    deltaXY=1;
    Eater2y = 9 +y2 +deltaXY;
    Eater2x =23 +x2 +deltaXY;
    put_GliderEater( Eater2y, Eater2x, statusGliderEater[2]);

}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene  = new QGraphicsScene(this);

    tftheight=640;
    tftwidth =640;


      rectangle = scene->addRect( 0, 0, screenWidth+3, screenHeight+3, outlinePen, blueBrush);

      ResetGate();

      //---------------------------------------------------------------------------
      // Draw all the cells
      //---------------------------------------------------------------------------
      scene->clear();
      ui->graphicsView->setScene(scene);
      outlinePen.setWidth(1);

      // paint border
      rectangle = scene->addRect( 1, 1, screenWidth+frame, screenHeight+frame, outlinePen, whiteBrush);

      // paint GoL screen
      for (int yrow=frame; yrow <(yrows-frame); yrow++) {
        for (int xcol=frame; xcol<(xcols-frame); xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol])
            rectangle = scene->addRect( (xcol-frame+1)*blockSize, (yrow-frame+1)*blockSize,
                                        blockSize, blockSize, outlinePen, blueBrush);
        }
      }

      ui->graphicsView->centerOn(screenWidth, screenHeight/2);  // <<<< not perfectly focussing yet

    //--------------------------------------------------------------
    // Timer
    // QTimer updateTimer;  // see: mainwindow.h!
    // onUpdateTime();      // see: mainwindow.h!
    // Every time the Timer will reach its terminal count
    // invoke the "slot" onUpdateTime()

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(305-3*updspeed);// screen refresh in msec

 }


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    close();
}




// screen refresh
void
MainWindow::onUpdateTime() {

      //--------------------------------------------------------------
      // Create a GraphicsScene and add it to the graphicsView
      // scene  = new QGraphicsScene(this);  // see: MainWindow constructor!

      ui->labelUpdspeed->setText(QString::number(updspeed));

      scene->clear();

      ui->labelOut1->setText(QString::number(statusRBnew[1]));
      ui->labelOut2->setText(QString::number(statusRBnew[2]));

      if(statusGliderEater[1]!=statusRBnew[1] || statusGliderEater[2]!=statusRBnew[2]) {
         ResetGate();
         statusGliderEater[1]=statusRBnew[1];
         statusGliderEater[2]=statusRBnew[2];
         GenerationCnt=0;
      }

      calculateGeneration();

      // paint border
      rectangle = scene->addRect( 1, 1, screenWidth+frame, screenHeight+frame, outlinePen, whiteBrush);

      // paint GoL screen
      for (int yrow=frame; yrow <(yrows-frame); yrow++) {
        for (int xcol=frame; xcol<(xcols-frame); xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol])
            rectangle = scene->addRect( (xcol-frame+1)*blockSize, (yrow-frame+1)*blockSize,
                                        blockSize, blockSize, outlinePen, blueBrush);
        }
      }

      // Add text to our graphics scene.
      //text = scene->addText("Hi dsyleixa123!", QFont("Arial", 20) );
      //text->setPos(80, 60);

      // generation monitor
      text = scene->addText("Generation "+QString::number(GenerationCnt), QFont("Arial", 14) );
      text->setPos(0, tftheight-100);

      GenerationCnt++;
      ui->graphicsView->centerOn(screenWidth, screenHeight/2);  // <<<< not perfectly focussing yet

      updateTimer.start(303-3*updspeed);

}



void MainWindow::on_SliderUpdateSpeed_sliderMoved(int position)
{
    updspeed=position;
}


void MainWindow::on_checkBox1_clicked(bool checked)
{
    if (checked) statusRBnew[1]=1;
    else statusRBnew[1]=0;
}

void MainWindow::on_checkBox2_clicked(bool checked)
{
    if (checked) statusRBnew[2]=1;
    else statusRBnew[2]=0;
}
