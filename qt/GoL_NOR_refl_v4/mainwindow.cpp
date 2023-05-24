// with friendly support from Heater, raspberrypi.org forums

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>


QBrush blueBrush(Qt::blue);
QBrush greenBrush(Qt::green);
QBrush redBrush(Qt::red);
QBrush blackBrush(Qt::black);
QBrush whiteBrush(Qt::white);
QBrush transpBrush(Qt::transparent);
QPen   outlinePen(Qt::black);
QPen   RedPen(Qt::red);



//---------------------------------------------------------------------------
// preferences and settings
//---------------------------------------------------------------------------
// The blocks are blockSize * blockSize big
// 2...6 seems to be a good value for this

int blockSize = 4;
int userBlsize = 4;


// The size of the GoL screen window

const int GOLscrWidth = 1000;   // <~~~~~~~~~~~~ adjust screen dimensions !
const int GOLscrHeight= 1000;
const int frame = 10;




// Make the board larger on either side to ensure that there's an invisible border of dead cells

int yvisrows = (GOLscrHeight / blockSize);
int xviscols = (GOLscrWidth / blockSize);

int yrows = yvisrows + 2*frame;
int xcols = xviscols + 2*frame;

#define centeryrow (yrows/2)-1
#define centerxcol (xcols/2)-1

// two boards, one for the current generation and one for calculating the next one
char board[GOLscrHeight + 2*frame][GOLscrWidth + 2*frame];
char tmpboard[GOLscrHeight + 2*frame][GOLscrWidth + 2*frame];




//---------------------------------------------------------------------------
// GoL functions
//---------------------------------------------------------------------------


// update speed for calcularting generations and drawing

int updspeed=50;
int userUSnew=updspeed;

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
// GoL coordinates and patterns
//---------------------------------------------------------------------------

// coordinates
int EaterX[10], EaterY[10];
int textposX[10], textposY[10];


// variables for program control
int stateGEater[10] = {1} ; // current state: 0=block, 1=let Gliders pass
int userGEater[10] = {1} ;  // optional arbitrary user setting






//---------------------------------------------------------------------------
void put_Clock(int startx, int starty) {    //
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
void put_Glider(int startx, int starty) {    //

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
void put_GliderUp(int startx, int starty) {    //

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
// GliderGuns plus eaters
//---------------------------------------------------------------------------

void put_GliderGun(int startx, int starty) {  // Gosper Glider Gun, period=30
  // period=30
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
void put_GliderGunRev(int startx, int starty) {  // Gosper Glider Gun, period=30

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
void put_GliderEater(int startx, int starty, char V) {
  int x,y;

  char sprite[4][5] = {  //
  {1 ,1 ,0 ,0 },
  {1 ,0 ,0 ,0 },
  {V ,1 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  } ;

  for(x=0; x<5; ++x) {
    for(y=0; y<4; ++y) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderEaterRev(int startx, int starty, char V) {
  // GG offset:
  //  EaterX[0] =  GGx + 10  -8*10;
  //  EaterY[0] =  GGy + 10  +8*10;


  int x,y;

  char sprite[4][5] = {  //
  {0 ,0 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  {1 ,1 ,1 ,V },
  {1 ,0 ,0 ,0 },
  } ;

  for(x=0; x<5; ++x) {
    for(y=0; y<4; ++y) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderReflxVert(int startx, int starty, char V) {
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream
  // GG offset:
  //  dX[0] =  GGx + 5  (-i*10);
  //  dY[0] =  GGy + 1  (+i*10);

  int x,y, xdim=11, ydim=22;

  char sprite[ydim][xdim] = {  //
  {0,0,1,1,0,0,0,0,0,1,1},
  {0,0,0,1,0,0,0,0,0,1,1},
  {0,0,0,1,0,1,0,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0},
  {0,0,0,0,0,0,0,1,0,1,0},
  {0,0,0,0,0,0,0,0,1,0,0},

  {0,0,0,0,0,0,0,0,0,0,0},

  {1,1,1,0,0,0,0,0,0,0,0},
  {1,1,1,0,0,0,0,0,1,0,0},
  {0,1,0,0,0,0,0,1,0,1,0},
  {0,1,0,0,0,0,1,0,0,0,1},
  {0,1,0,0,0,0,1,0,1,0,1},
  {1,0,1,0,0,0,0,0,0,0,0},

  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},

  {1,0,1,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,0,1,0,1,0,1},
  {0,1,0,0,0,0,1,0,0,0,1},
  {0,1,0,0,0,0,0,1,0,1,0},
  {1,1,1,0,0,0,0,0,1,0,0},
  {1,1,1,0,0,0,0,0,0,0,0}
  } ;

  for(x=0; x<xdim; ++x) {
    for(y=0; y<ydim; ++y) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------

void put_NOR(int startx, int starty) {

    // absolute start positions of gliderguns
    int GGy1= starty + 1, GGx1= startx ;                // A
    int GGy2= starty + 1, GGx2= startx + 1 + (37+2)*1;  // B
    int GGy3= starty +30, GGx3= startx + 1 + (37+2)*2 +30;  // Invert A, B 1 pont offset opt.  <<<<<
    int GRy4, GRx4;

    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    stateGEater[0]=0;
    stateGEater[1]=userGEater[1];
    stateGEater[2]=userGEater[2];
    stateGEater[3]=0;
    stateGEater[4]=0;


    // glidergun 1: INPUT A
    put_GliderGun( GGx1, GGy1 );
    // Gun Eater 1
    EaterX[1] =  GGx1 + 24 +4;
    EaterY[1] =  GGy1 + 10 +4;

    put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);   // Input A
    put_GliderEater( EaterX[1]+7*10, EaterY[1]+7*10, 0);      // INF delimiter


    // glidergun 2: INPUT B
    put_GliderGun( GGx2, GGy2 );
    // Gun Eater 2
    EaterX[2] =  GGx2 + 24 +4;
    EaterY[2] =  GGy2 + 10 +4;

    put_GliderEater( EaterX[2], EaterY[2], stateGEater[2]); // INPUT B
    //put_GliderEater( EaterY[2]+16, EaterX[2]+16, 0);      // INF delimiter


    // glidergun 3:  NOR output stream
    // Gun Eater 0: NOR output INF delimiter
    GRx4=GGx3 + 10  -8*10   -5;
    GRy4=GGy3 + 10  +8*10   -9;

    //EaterX[3] =  GRx4 +40;
    //EaterY[3] =  GRy4 +40;
    put_GliderGunRev( GGx3, GGy3 );



    // Glider reflector
    // put_GliderReflxVert(EaterX[0]-5, EaterY[0]-9, stateGEater[0]);
    put_GliderReflxVert(GRx4, GRy4, 0);
    EaterX[4] =  GRx4 +40+7;
    EaterY[4] =  GRy4 +40;
    //put_GliderEater( GRx4+40+7, GRy4+40, 0);   // INF delimiter
    put_GliderEater( EaterX[4], EaterY[4], 0);   // INF delimiter


}

//---------------------------------------------------------------------------

void ResetCircuit() {
    memset(board, 0, sizeof(board));
    memset(tmpboard, 0, sizeof(tmpboard));

    put_NOR( 0, 0 );
}

//---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene  = new QGraphicsScene(this);

      //rectangle = scene->addRect( 0, 0, GOLscrWidth+3, GOLscrHeight+3, outlinePen, blueBrush);

      ResetCircuit();

      //---------------------------------------------------------------------------
      // Draw all the cells
      //---------------------------------------------------------------------------

      ui->graphicsView->setScene(scene);
      scene->clear();

      outlinePen.setWidth(1);

      // paint border
      rectangle = scene->addRect( 1, 1, GOLscrWidth+frame, GOLscrHeight+frame, outlinePen, transpBrush);

      // paint GoL screen
      for (int yrow=frame; yrow <(yrows-frame); yrow++) {
        for (int xcol=frame; xcol<(xcols-frame); xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol])
            rectangle = scene->addRect( (xcol-frame+1)*blockSize, (yrow-frame+1)*blockSize,
                                        blockSize, blockSize, outlinePen, blueBrush);
        }        
      }



    //--------------------------------------------------------------
    // Timer
    // QTimer updateTimer;  // see: mainwindow.h!
    // onUpdateTime();      // see: mainwindow.h!


    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(1003-10*updspeed);// screen refresh in msec

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
      // display controls

      if(updspeed!=userUSnew) {
         updspeed=userUSnew;
         if(updspeed>0) updateTimer.start(1003-10*updspeed);
      }      
      ui->labelUpdspeed->setText("Speed: "+QString::number(userUSnew));


      if(blockSize!=userBlsize) {
         blockSize=userBlsize;
      }
      ui->labelBlocksize->setText("Blocksize: "+QString::number(userBlsize));


      static int synctick1=0, synctick2=0, sync1=1, sync2=1;

      synctick1=(GenerationCnt-7+30)%30;  // sync-in for quick runtime input changes
      synctick2=(GenerationCnt-7+30)%30;  // 30=GGG_period, 7=GE_offset

      if(stateGEater[1]!=userGEater[1] && sync1)  {
          sync1=0;
      }
      if(synctick1==19  && !sync1) {
          stateGEater[1]=userGEater[1];
          put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);
          sync1=1;
      }
      //ResetCircuit();


      if(stateGEater[2]!=userGEater[2] && sync2)  {
          sync2=0;
      }
      if(synctick2==19  && !sync2) {
          stateGEater[2]=userGEater[2];
          put_GliderEater( EaterX[2], EaterY[2], stateGEater[2]);
          sync2=1;
      }
      //ResetCircuit();




      ui->labelOut1->setText(QString::number(userGEater[1]));
      ui->labelOut2->setText(QString::number(userGEater[2]));

      // GoL: calculate next Generation
      if(updspeed>0) calculateGeneration();


      // clear and redraw scene
      scene->clear();

      // draw GoL screen border
      //rectangle = scene->addRect( 1, 1, GOLscrWidth+frame, GOLscrHeight+frame, outlinePen, transpBrush);

      // draw GoL screen dots
      for (int yrow=frame; yrow <(yrows-frame); yrow++) {
        for (int xcol=frame; xcol<(xcols-frame); xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol])
            rectangle = scene->addRect( (xcol-frame+1)*blockSize, (yrow-frame+1)*blockSize,
                                        blockSize, blockSize,  outlinePen, blackBrush);
        }
      }

      // generation monitor
      ui->labelGen->setText("gen="+QString::number(GenerationCnt));
      if(updspeed>0) GenerationCnt++;


      QGraphicsSimpleTextItem* text = scene->addSimpleText("    NOR\nresult stream \n   reflect", QFont("Arial", 14) );
      text->setBrush(Qt::red);
      //textposX[0] = EaterX[3]*blockSize+8*blockSize-10;
      //textposY[0] = EaterY[3]*blockSize+80;
      textposX[0] = (EaterX[4]-20)*blockSize -20;
      textposY[0] =  EaterY[4]*blockSize;
      text->setPos(textposX[0], textposY[0]);


}



void MainWindow::on_SliderUpdateSpeed_sliderMoved(int position)
{
    userUSnew=position;
}


void MainWindow::on_SliderUpdateSpeed_valueChanged(int value)
{
    userUSnew=value;
}


void MainWindow::on_checkBox1_clicked(bool checked)
{
    if (checked) userGEater[1]=1;
    else userGEater[1]=0;

}

void MainWindow::on_checkBox2_clicked(bool checked)
{
    if (checked) userGEater[2]=1;
    else userGEater[2]=0;
}


void MainWindow::on_btnGenReset_clicked()
{
    ResetCircuit();
    GenerationCnt=1;
}

void MainWindow::on_SliderBlocksize_sliderMoved(int position)
{
    userBlsize = position;
}


void MainWindow::on_SliderBlocksize_valueChanged(int value)
{
     userBlsize=value;
}
