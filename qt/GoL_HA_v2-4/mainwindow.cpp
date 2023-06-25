// with friendly support from Heater, raspberrypi.org forums

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gol_defs.h"
#include "gol_tools.h"
#include <wiringPi.h>
#include <thread>


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

int updspeed=50, updspeedSav=50;
int userUSnew=updspeed;

uint32_t GenerationCnt=1;
static char  StepMode=false;


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

      // Any live cell with more than three live neighbours dies, as if by overcrowding
      if(aliveNeighbours > 3)
        tmpboard[yrow][xcol] = 0;
    }
  }
  // Copy the new board to the old one
  memcpy(board, tmpboard, sizeof(tmpboard));
}


//---------------------------------------------------------------------------
// thread func
void calculateGenerationThrFun(int starty, int stopy)
{
  int aliveNeighbours = 0;

  // Clear the board for the next generation
  // memset(tmpboard, 0, sizeof(tmpboard));

  for (int yrow = starty; yrow < (stopy); yrow++)  {
    for (int xcol = 1; xcol < (xcols-1); xcol++)   {

      int count = 0;
      for (int x = -1; x <= +1; x++)  {
          for (int y = -1; y <= +1; y++) {
            if ((board[yrow + y][xcol + x] == 1) && (x != 0 || y != 0))
              count++;
          }
      }
      aliveNeighbours = count;

      // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
      if(aliveNeighbours < 2)
        tmpboard[yrow][xcol] = 0;

      // Any live cell with two or three live neighbours lives on to the next calculateGeneration
      if (aliveNeighbours >= 2  &&  aliveNeighbours <= 3 )
        tmpboard[yrow][xcol] = board[yrow][xcol];

      // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction
      if(aliveNeighbours == 3 && board[yrow][xcol]==0)
        tmpboard[yrow][xcol] = 1;

      // Any live cell with more than three live neighbours dies, as if by overcrowding
      if(aliveNeighbours > 3)
        tmpboard[yrow][xcol] = 0;
    }
  }
  // Copy the new board to the old one
  // memcpy(board, tmpboard, sizeof(tmpboard));
}





//---------------------------------------------------------------------------
void put_XOR(int startx, int starty, char put_UI_Input=1) {

    static int EaterX[20], EaterY[20];
    static int stateGEater[20] = {0} ; // current state: 0=block, 1=let Gliders pass

    // absolute start positions of gliderguns
    int GGy1= starty + 1,   GGx1= startx + 39   + 9;      // A
    int GGy2= starty + 2,   GGx2= startx + 39*2 + 9;      // B
    int GGy3= starty +10 -10,   GGx3= startx + 39*2 +35 +20;    // Invert (y +1 dot offset opt.)
    int GGy4= starty +10 -10+1, GGx4= startx + 2  +20;           // output




    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    if(put_UI_Input) stateGEater[1]=userInputGlobal[1];
    if(put_UI_Input) stateGEater[2]=userInputGlobal[2];

    if(put_UI_Input) {
      //glidergun 1: INPUT A
      put_GliderGun( GGx1, GGy1 );
      //Gun Eater 1
      EaterXglob[1] = EaterX[1] =  GGx1 + 24 +3;
      EaterYglob[1] = EaterY[1] =  GGy1 + 10 +4;
      put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);  // Input A
    }

    if(put_UI_Input) {
      //glidergun 2: INPUT B
      put_GliderGunRev( GGx2, GGy2 );
      //Gun Eater 2
      EaterXglob[2] = EaterX[2] =  GGx2 +  5;
      EaterYglob[2] = EaterY[2] =  GGy2 + 14;
      put_GliderEaterRev( EaterX[2], EaterY[2], stateGEater[2]); // INPUT B
    }


    // glidergun 3: GUN INVERT A,B
    put_GliderGunRev( GGx3, GGy3 );
    // Gun Eater 3: INF delimiter
    //EaterX[3] =  GGx3 +10 -10;
    //EaterY[3] =  GGy3 +10 +10;
    //put_GliderEaterRev( EaterX[3], EaterY[3], 0);

    int GRx0=GGx2  -1 ;
    int GRy0=GGy2  +6 ;
    put_GliderReflxVert(GRx0,GRy0);


    // glidergun 4: output
    put_GliderGun( GGx4, GGy4 );
    // Gun Eater 4: INF delimiter
    EaterX[4] =  GGx4 + 27 +60;
    EaterY[4] =  GGy4 + 14 +60;
    //put_GliderEater( EaterX[4], EaterY[4], 0);


}



//---------------------------------------------------------------------------

void put_HA(int startx, int starty, char put_UI_Input=1) {
    static int EaterX[20], EaterY[20];
    static int stateGEater[20] = {0} ; // current state: 0=block, 1=let Gliders pass

    // absolute start positions of gliderguns
    int GGy1= starty + 1+60,       GGx1= startx + 39   +11;     // A
    int GGy2= starty + 2+60,       GGx2= startx + 39*6 -27;     // B

    //int GGy3= starty + 4,        GGx3= startx + 39*3  +5 ;   // B blockt exakt, A Chaos.
    //int GGy3= starty + 4,        GGx3= startx + 39*3  +6;    // B Doppelblock , A Chaos
    int GGy3= starty + 4,        GGx3= startx + 39*3  +7;    // B blockt exakt, A KurzFeuerwerk
    //int GGy3= starty + 5,        GGx3= startx + 39*3  +6;    // B Chaos, A exakt
    //int GGy3= starty + 6,        GGx3= startx + 39*3  +6;    // B , A Chaos
    //int GGy3= starty + 6,        GGx3= startx + 39*3  +5 ;   // B , A Chaos
    //int GGy3= starty + 4,        GGx3= startx + 39*3  +8;    // B blockt exakt, A Chaos
    //int GGy3= starty + 4,        GGx3= startx + 39*3  +9;    // B Chaos, A blockt exakt
    //int GGy3= starty + 5,        GGx3= startx + 39*3  +7;    // B blockt exakt, A Chaos
    //int GGy3= starty + 5,        GGx3= startx + 39*3  +8;    // B blockt exakt, A Chaos

    int GDx1, GDy1, GDx2, GDy2;   // duplicator    


    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    stateGEater[1]=stateGEaterglob[1]=userInputGlobal[1];
    stateGEater[2]=stateGEaterglob[2]=userInputGlobal[2];

    // glider duplicator for INPUT A
    GDx1=GGx1+34 -15;  // 34
    GDy1=GGy1+11 -15;  // 11
    put_GliderDuplic_LD_RUD(GDx1, GDy1);
    EaterX[11]=GDx1+24;
    EaterY[11]=GDy1+35;
    //put_GliderEaterDn(EaterX[11]+45,EaterY[11]-45,0);
    EaterX[12]=GDx1+19;
    EaterY[12]=GDy1+24;
    //put_GliderEater(EaterX[12]+45,EaterY[12]+45,0);


    // glider duplicator for INPUT B
    GDx2=GGx2-34 + 1;  //
    GDy2=GGy2+11 -15;  //
    put_GliderDuplic_RD_LUD(GDx2, GDy2);
    EaterX[13]=GDx2-68;  //
    EaterY[13]=GDy2+35 ;
    //put_GliderEaterDnRev(EaterX[13]+45,EaterY[13]-45,0);
    EaterX[14]=GDx2-63;  //
    EaterY[14]=GDy1+25;
    //put_GliderEaterRev(EaterX[14]+45,EaterY[14]+45,0);


    //put_GliderReflxVertUpR(GDx2-22,GDy2-13);
    put_GliderReflxVertUpL(GDx1+61,GDy1-13);

    // A&B stream for carry flag
    // upper A stream NW to SW
    put_GliderReflxHorizDnR_1(startx+67, starty + 4);  // 1.1.0 top left

    // glidergun 1: INPUT A
    put_GliderGun( GGx1, GGy1 );
    // Gun Eater 1        
    EaterXglob[1] = EaterX[1] =  GGx1 + 27;
    EaterYglob[1] = EaterY[1] =  GGy1 + 14;
    put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);  // Input A


    // glidergun 2: INPUT B
    put_GliderGunRev( GGx2, GGy2 );
    // Gun Eater 2
    EaterXglob[2] = EaterX[2] =  GGx2 +  5;
    EaterYglob[2] = EaterY[2] =  GGy2 + 14;
    put_GliderEaterRev( EaterX[2], EaterY[2], stateGEater[2]); // INPUT B

    // XOR pattern for A,B duplicator downstreams
    put_XOR(startx+75-15, starty+100+15, 0);

    put_GliderGunRev(GGx3,GGy3);  // INVERT A&B upper streams
    EaterX[3] =  GGx3 +  5 -30;
    EaterY[3] =  GGy3 + 14 +30;
    put_GliderEaterRev( EaterX[3], EaterY[3], 0); // INPUT B

}


void put_VA(int startx, int starty) {
    int HA1x, HA1y, HA2x, HA2y;
    HA1x=0;
    HA1y=0;
    put_HA( HA1x, HA1y, 1 );

    // upper A stream SW to SE
    put_GliderReflxVertDnR_1(HA1x+8 , HA1y + 75 );
    // upper A stream SE to SW
    put_GliderReflxVertDnL_1(HA1x+48, HA1y + 101);
    // upper A stream SW to SE
    put_GliderReflxVertDnR_1(HA1x+8 , HA1y +75 +52 );


}

//---------------------------------------------------------------------------

void ResetCircuit() {
    memset(board, 0, sizeof(board));
    memset(tmpboard, 0, sizeof(tmpboard));

    put_VA( 0, 0 );
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

      //outlinePen.setWidth(2);

      // paint border
      //rectangle = scene->addRect( 2, 2, GOLscrWidth*blockSize, GOLscrHeight*blockSize, outlinePen, transpBrush);

      // paint GoL screen     
      /*
      for (int yrow=frame; yrow < GOLscrHeight; yrow++) {
        for (int xcol=frame; xcol < GOLscrWidth; xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol] )
              rectangle = scene->addRect( xcol*blockSize,  yrow*blockSize ,
                                          blockSize, blockSize, outlinePen, blueBrush);
        }
      }
      */



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

      if(stateGEaterglob[1]!=userInputGlobal[1] && sync1)  {
          sync1=0;
      }
      if(synctick1==16  && !sync1) {
             stateGEaterglob[1]=userInputGlobal[1];
             put_GliderEater( EaterXglob[1], EaterYglob[1], stateGEaterglob[1]);
             sync1=1;
      }
      //ResetCircuit();


      if(stateGEaterglob[2]!=userInputGlobal[2] && sync2)  {
          sync2=0;
      }
      if(synctick2==16  && !sync2) {
             stateGEaterglob[2]=userInputGlobal[2];
             put_GliderEaterRev( EaterXglob[2], EaterYglob[2], stateGEaterglob[2]);
             sync2=1;
      }
      //ResetCircuit();

      ui->labelOut1->setText(QString::number(userInputGlobal[1]));
      ui->labelOut2->setText(QString::number(userInputGlobal[2]));


      // GoL: calculate next Generation
      if(updspeed>0) {

         // Clear the temp board for the next generation
         memset(tmpboard, 0, sizeof(tmpboard));

         const int par=50; // array parts, parallel threads
         int yn[par+1];

         yn[0]=1;
         for (int32_t i=1; i<=par; i++) { yn[i]= (i*yrows)/par; }

         //calculate next Generations in threads
         std::thread* cthr[par];
         // launch all threads
         for (int i = 0; i < par; i++)
             cthr[i] = new std::thread(calculateGenerationThrFun, yn[i], yn[i+1]);
         // join threads when calcs finished
         for (int i = 0; i < par; i++) cthr[i]->join();

         // Copy the new board to the old one
         memcpy(board, tmpboard, sizeof(tmpboard));
      }

      // clear and redraw scene
      scene->clear();
      outlinePen.setWidth(2);

      // draw GoL screen border
      rectangle = scene->addRect( 0, 0, GOLscrWidth*blockSize+2, GOLscrHeight*blockSize+2, outlinePen, transpBrush);

      // draw GoL screen dots
      for (int yrow=frame; yrow < frame+GOLscrHeight; yrow++) {
        for (int xcol=frame; xcol < frame+GOLscrWidth; xcol++)  {
          // Draw all the "live" cells.
          if (board[yrow][xcol] )
              rectangle = scene->addRect( (xcol-frame)*blockSize,  (yrow-frame)*blockSize ,
                                      blockSize, blockSize, outlinePen, blueBrush);
        }
      }


      // generation monitor
      ui->labelGen->setText("gen="+QString::number(GenerationCnt));
      if(updspeed>0) GenerationCnt++;

      /*
      textposX[0] =(EaterX[12]-2 )*blockSize ;     // A duplicator
      textposY[0] =(EaterY[12]+22)*blockSize-4;

      textposX[1] =(EaterX[14]+80)*blockSize ;     // B duplicator
      textposY[1] =(EaterY[14]+22)*blockSize-4;

      textposX[2] =(EaterX[1]+8)*blockSize ;       // A
      textposY[2] =(EaterY[1]-17)*blockSize-8;

      textposX[3] =(EaterX[2]+6)*blockSize ;       // B
      textposY[3] =(EaterY[2]-17)*blockSize-8;

      textposX[4] =(EaterX[3]-60)*blockSize ;      // A&B top
      textposY[4] =(EaterY[3]-25)*blockSize-4;

      textposX[5] =(EaterX[4]-50)*blockSize ;      // A XOR B
      textposY[5] =(EaterY[4]+5)*blockSize;

      textposX[6] =(EaterX[4]-100)*blockSize ;     // A&B bottom
      textposY[6] =(EaterY[4]-25)*blockSize-4;




      QGraphicsSimpleTextItem* text0 = scene->addSimpleText("   Glider \nDuplicator", QFont("Arial", 14) );
      text0->setBrush(Qt::red);
      text0->setPos(textposX[0], textposY[0]);

      QGraphicsSimpleTextItem* text1 = scene->addSimpleText("   Glider \nDuplicator", QFont("Arial", 14) );
      text1->setBrush(Qt::red);
      text1->setPos(textposX[1], textposY[1]);

      QGraphicsSimpleTextItem* text2 = scene->addSimpleText("A", QFont("Arial", 14) );
      text2->setBrush(Qt::red);
      text2->setPos(textposX[2], textposY[2]);

      QGraphicsSimpleTextItem* text3 = scene->addSimpleText("B", QFont("Arial", 14) );
      text3->setBrush(Qt::red);
      text3->setPos(textposX[3], textposY[3]);

      QGraphicsSimpleTextItem* text4 = scene->addSimpleText("A AND B \n   carry", QFont("Arial", 14) );
      text4->setBrush(Qt::red);
      text4->setPos(textposX[4], textposY[4]);

      QGraphicsSimpleTextItem* text5 = scene->addSimpleText("A XOR B \n   sum", QFont("Arial", 14) );
      text5->setBrush(Qt::red);
      text5->setPos(textposX[5], textposY[5]);

      QGraphicsSimpleTextItem* text6 = scene->addSimpleText("A AND B \n  carry", QFont("Arial", 14) );
      text6->setBrush(Qt::red);
      text6->setPos(textposX[6], textposY[6]);

      */

      if(StepMode) {
          StepMode=false;
          userUSnew = 0;
      }

}



void MainWindow::on_SliderUpdateSpeed_sliderMoved(int position)
{
    userUSnew=position;
}


void MainWindow::on_SliderUpdateSpeed_valueChanged(int value)
{
    userUSnew=value;
    if(value!=0) updspeedSav = value;  //  <<<< neu!
}


void MainWindow::on_checkBox1_clicked(bool checked)
{
    if (checked) userInputGlobal[1]=1;
    else userInputGlobal[1]=0;

}

void MainWindow::on_checkBox2_clicked(bool checked)
{
    if (checked) userInputGlobal[2]=1;
    else userInputGlobal[2]=0;
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

void MainWindow::on_pushButton_2_clicked()   // pause
{
    if(updspeed!=0) updspeedSav = updspeed;
    userUSnew = 0;
    StepMode=false;
    ui->labelUpdspeed->setText("Speed: "+QString::number(userUSnew));
}

void MainWindow::on_pushButton_3_clicked()   // play
{
    if(updspeedSav!=0) updspeed = updspeedSav;
    if(updspeed!=0) userUSnew = updspeed;
    StepMode=false;
    ui->labelUpdspeed->setText("Speed: "+QString::number(userUSnew));
}


void MainWindow::on_pushButton_4_clicked()   // 1 step
{
    if(updspeedSav!=0) updspeed = updspeedSav;
    userUSnew = updspeed;
    StepMode=true;
    ui->labelUpdspeed->setText("Speed: "+QString::number(userUSnew));
}
