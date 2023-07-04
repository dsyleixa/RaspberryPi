// with friendly support from Heater, raspberrypi.org forums

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gol_defs.h"
#include "gol_tools.h"
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

void put_Duplic(int startx, int starty) {

    // absolute start positions of gliderguns
    int GGy1= starty + 1+60,   GGx1= startx + 39 -19+30;     // A
    int GGy2= starty + 1+60,   GGx2= startx + 39*4 -19;     // B
    int GGy3= starty +60+60,   GGx3= startx + 39*2 +36+30;  // Invert (y +1 dot offset opt.)
    int GGy4= starty +60+1+60, GGx4= startx + 1+30;
    int GDx1, GDy1, GDx2, GDy2;   // duplicator



    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    stateGEater[0]=0;
    stateGEater[1]=userGEater[1];
    stateGEater[2]=userGEater[2];
    stateGEater[3]=0;
    stateGEater[4]=0;
    stateGEater[5]=0;


    // glider duplicator for INPUT A
    GDx1=GGx1+34 -15;  // 34
    GDy1=GGy1+11 -15;  // 11
    put_GliderDuplic_LD_RUD(GDx1, GDy1);
    EaterX[11]=GDx1+54;
    EaterY[11]=GDy1+05;
    put_GliderEaterDn(EaterX[11]+45,EaterY[11]-45,0);
    EaterX[12]=GDx1+49;
    EaterY[12]=GDy1+54;
    put_GliderEater(EaterX[12]+45,EaterY[12]+45,0);

    // glider duplicator for INPUT B
    GDx2=GGx2+34 -15;  //
    GDy2=GGy2+11 -15;  //
    put_GliderDuplic_LD_RUD(GDx2, GDy2);
    EaterX[13]=GDx2+54;
    EaterY[13]=GDy2+5 ;
    put_GliderEaterDn(EaterX[13]+45,EaterY[13]-45,0);
    EaterX[14]=GDx2+49;
    EaterY[14]=GDy1+54;
    put_GliderEater(EaterX[14]+45,EaterY[14]+45,0);


    // glidergun 1: INPUT A
    put_GliderGun( GGx1, GGy1 );
    // Gun Eater 1        
    EaterX[1] =  GGx1 + 27;
    EaterY[1] =  GGy1 + 14;
    put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);  // Input A


    // glidergun 2: INPUT B
    put_GliderGun( GGx2, GGy2 );
    // Gun Eater 2
    EaterX[2] =  GGx2 + 27;
    EaterY[2] =  GGy2 + 14;
    put_GliderEater( EaterX[2], EaterY[2], stateGEater[2]); // INPUT B


}

//---------------------------------------------------------------------------

void ResetCircuit() {
    memset(board, 0, sizeof(board));
    memset(tmpboard, 0, sizeof(tmpboard));

    put_Duplic( 0, 0 );
}

//---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    extractproname(srcfilepath);
    this->setWindowTitle((QString)proname);

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


      synctick1=(GenerationCnt-8+30)%30;  // sync-in for quick runtime input changes
      synctick2=(GenerationCnt-8+30)%30;  // 30=GGG_period, 8=GE_offset

      if(stateGEater[1]!=userGEater[1] && sync1)  {
          sync1=0;
      }
      if(synctick1==16  && !sync1) {
             stateGEater[1]=userGEater[1];
             put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);
             sync1=1;
      }
      //ResetCircuit();


      if(stateGEater[2]!=userGEater[2] && sync2)  {
          sync2=0;
      }
      if(synctick2==16  && !sync2) {
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

      QGraphicsSimpleTextItem* text0 = scene->addSimpleText("Glider Duplicator", QFont("Arial", 14) );
      text0->setBrush(Qt::red);
      textposX[0] =(EaterX[12]-50)*blockSize ;
      textposY[0] =(EaterY[12])   *blockSize-4;
      text0->setPos(textposX[0], textposY[0]);

      QGraphicsSimpleTextItem* text1 = scene->addSimpleText("Glider Duplicator", QFont("Arial", 14) );
      text1->setBrush(Qt::red);
      textposX[1] =(EaterX[14]-50)*blockSize ;
      textposY[1] =(EaterY[14])   *blockSize-4;
      text1->setPos(textposX[1], textposY[0]);

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
