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
int EaterX[10], EaterY[10];   // absolute coordinates


// variables for program control
int stateGEater[10] = {1} ; // current state: 0=block, 1=let Gliders pass
int userGEater[10] = {1} ;  // optional arbitrary user setting






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
// GliderGuns plus eaters
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
void put_GliderEater(int starty, int startx, char V) {
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
void put_GliderEaterRev(int starty, int startx, char V) {
  int x,y;

  char sprite[6][6] = {  //
  {0 ,0 ,0 ,0 ,0 ,0 },
  {0 ,0 ,0 ,1 ,1 ,0 },
  {0 ,0 ,0 ,0 ,1 ,0 },
  {0 ,1 ,1 ,1 ,V ,0 },
  {0 ,1 ,0 ,0 ,0 ,0 },
  {0 ,0 ,0 ,0 ,0 ,0 },
  } ;

  for(x=0; x<6; ++x) {
    for(y=0; y<6; ++y) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------

void ResetCircuit() {
    memset(board, 0, sizeof(board));
    memset(tmpboard, 0, sizeof(tmpboard));


    // absolute start positions of gliderguns
    int GGy1= 1, GGx1=0;           // A
    int GGy2= 1, GGx2=1+(37+2)*1;  // B
    int GGy3= 1, GGx3=1+(37+2)*2;  // Invert
    int GGy4= 1, GGx4=  (37+2)*3;  // Invert
    int GGy5= 1, GGx5=  (37+2)*4;  // Clock

    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    stateGEater[0]=0;
    stateGEater[1]=userGEater[1];
    stateGEater[2]=userGEater[2];
    stateGEater[3]=0;
    stateGEater[4]=0;
    stateGEater[5]=0;


    // glidergun 1: INPUT A
    put_GliderGun( GGy1, GGx1 );
    // Gun Eater 1    
    EaterY[1] =  GGy1 +  9 +4;
    EaterX[1] =  GGx1 + 23 +4;
    put_GliderEater( EaterY[1], EaterX[1], stateGEater[1]);  // Input A


    // glidergun 2: INPUT B
    put_GliderGun( GGy2, GGx2 );
    // Gun Eater 2
    EaterY[2] =  GGy2 +  9 +4;
    EaterX[2] =  GGx2 + 23 +4;
    put_GliderEater( EaterY[2], EaterX[2], stateGEater[2]); // INPUT B

    put_GliderEater( EaterY[2]+16, EaterX[2]+16, 0); // INF delimiter


    // glidergun 3: GUN INVERT B
    put_GliderGunRev( GGy3, GGx3 );
    // Gun Eater 3: INF delimiter
    EaterY[3] =  GGy3 + 9 +4*10;
    EaterX[3] =  GGx3 + 9 -4*10;
    put_GliderEaterRev( EaterY[3], EaterX[3], 0);


    // glidergun 4:  NAND output stream
    put_GliderGunRev( GGy4, GGx4 );
    // Gun Eater 0: NAND output INF delimiter
    EaterY[0] =  GGy4 + 9  +8*10;
    EaterX[0] =  GGx4 + 9  -8*10;
    put_GliderEaterRev( EaterY[0], EaterX[0], stateGEater[0]);


/*
    // glidergun 5: const CLOCK  // parallel to NAND output stream
    put_GliderGunRev( GGy5, GGx5 );
    // Gun Eater 5
    EaterY[5] =  GGy5 + 9  +9*10;
    EaterX[5] =  GGx5 + 9  -9*10;
    put_GliderEaterRev( EaterY[5], EaterX[5], stateGEater[5]);
*/

}



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
         updateTimer.start(1003-10*updspeed);
      }      
      ui->labelUpdspeed->setText("Speed: "+QString::number(userUSnew));


      if(blockSize!=userBlsize) {
         blockSize=userBlsize;
      }
      ui->labelBlocksize->setText("Blocksize: "+QString::number(userBlsize));

      if(stateGEater[1]!=userGEater[1])  {
          stateGEater[1]=userGEater[1];
          ResetCircuit();
      }

      if(stateGEater[2]!=userGEater[2]) {
          stateGEater[2]=userGEater[2];
          ResetCircuit();
      }

      ui->labelOut1->setText(QString::number(userGEater[1]));
      ui->labelOut2->setText(QString::number(userGEater[2]));

      // GoL: calculate next Generation
      calculateGeneration();


      // clear and redraw scene
      //ui->graphicsView->setScene(scene);
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
      GenerationCnt++;


      QGraphicsSimpleTextItem* text = scene->addSimpleText("    NAND\nresult stream", QFont("Arial", 14) );
      text->setBrush(Qt::red);
      //text->setDefaultTextColor(Qt::red);
      text->setPos(EaterX[0]*blockSize+8*blockSize-1, EaterY[0]*blockSize-3*blockSize+2);

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
    GenerationCnt=0;
}

void MainWindow::on_SliderBlocksize_sliderMoved(int position)
{
    userBlsize = position;
}


void MainWindow::on_SliderBlocksize_valueChanged(int value)
{
     userBlsize=value;
}
