// with friendly support from members of raspberrypi.org forums

// change log
//HA_v3-1 XOR_R, XOR_L
//FA_v0   shift HA1->+60 ri, Cin_man.
//FA_v1   test: AxorB duplicator, XOR_L
//FA_v3   HA2 to AxorB-sum1, HA and/sum top rflx outsourced, scene labels adjusted
//FA_v4   +GGG Cin_man
//FA_v5   HA2+FA complete, Layout too close
//FA_v6   HA2+FA complete, Layout wider, HA-A upstream better




#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gol_defs.h"
#include "gol_tools.h"
#include <wiringPi.h>
#include <thread>
#include <mutex>

std::mutex mtx;


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
void put_XOR_L(int startx, int starty, char put_UI_Input=1) {

    static int EaterX[20], EaterY[20];
    static int stateGEater[20] = {0} ; // current state: 0=block, 1=let Gliders pass

    // absolute start positions of gliderguns
    int GGy1= starty + 1,   GGx1= startx + 39   + 9;      // A
    int GGy2= starty + 2,   GGx2= startx + 39*2 + 9;      // B
    int GGy3= starty +10 -10,   GGx3= startx + 39*2 +35 +20;    // Invert (y +1 dot offset opt.)
    int GGy4= starty +10 -10+1, GGx4= startx + 2  +20;           // output




    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    if(put_UI_Input) {
        if(put_UI_Input) stateGEater[1]=userInputGlobal[1];
        if(put_UI_Input) stateGEater[2]=userInputGlobal[2];
    }

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


    //put_GliderReflxVert(GRx0,GRy0);  // for XOR_R: old ok

    //test
    //put_GliderReflxVertDnL(GRx0+8,GRy0-1);     // 1 for XOR_L: A zu hoch crash B hoch ok
    put_GliderReflxVertDnL(GRx0+8,GRy0-1-2);   // 2 for XOR_L: A okFW,  B hoch ok!

    put_GliderEaterRev(GRx0+8-40+5, GRy0-1-2+40-5, 0);  // Axor stream INF blocker



    // glidergun 4: output
    put_GliderGun( GGx4, GGy4 );
    // Gun Eater 4: INF delimiter
    EaterX[4] =  GGx4 + 27 +60;
    EaterY[4] =  GGy4 + 14 +60;
    //put_GliderEater( EaterX[4], EaterY[4], 0);

}


//---------------------------------------------------------------------------
void put_XOR_R(int startx, int starty, char put_UI_Input=1) {  // orig.

    static int EaterX[20], EaterY[20];
    static int stateGEater[20] = {0} ; // current state: 0=block, 1=let Gliders pass

    // absolute start positions of gliderguns
    int GGy1= starty + 1,   GGx1= startx + 39   + 9;      // A
    int GGy2= starty + 2,   GGx2= startx + 39*2 + 9;      // B
    int GGy3= starty +10 -10,   GGx3= startx + 39*2 +35 +20;    // Invert (y +1 dot offset opt.)
    int GGy4= starty +10 -10+1, GGx4= startx + 2  +20;           // output




    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    if(put_UI_Input) {
        if(put_UI_Input) stateGEater[1]=userInputGlobal[1];
        if(put_UI_Input) stateGEater[2]=userInputGlobal[2];
    }


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


    put_GliderReflxVert(GRx0,GRy0);  // XOR_R: old ok

    //test
    //put_GliderReflxVertDnL(GRx0+8,GRy0-1);     // 1 A zu hoch crash B hoch ok
    //put_GliderReflxVertDnL(GRx0+8,GRy0-1-2);   // 2 A okFW,  B hoch ok!

    put_GliderEaterRev(GRx0+8-40, GRy0-1-2+40, 0); // Rflx A to AxorB

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
    int GGx1= startx + 39   +11,   GGy1= starty + 1+60    ;     // A
    int GGx2= startx + 39*6 -27, GGy2= starty + 2+60      ;     // B

    //int GGx3= startx + 39*3  +7, GGy3= starty + 4 ;      // A RiesenFeuerwerk, B blockt exakt
    int GGx3= startx + 39*3  +10,  GGy3= starty + 6 ;      // x+=10, y+=6 ok!

    int GDx1, GDy1, GDx2, GDy2;   // duplicator


    // 0: GliderEater solid (active) - 1: GliderEater vanishes (inactive)
    if(put_UI_Input) {
        stateGEater[1]=stateGEaterglob[1]=userInputGlobal[1];
        stateGEater[2]=stateGEaterglob[2]=userInputGlobal[2];
    }


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
    // outsourced
    //put_GliderReflxHorizDnR_1(startx+67, starty + 4);  // 1.1.0 top left

    // glidergun 1: INPUT A
    if(put_UI_Input) {
        put_GliderGun( GGx1, GGy1 );
        // Gun Eater 1
        EaterXglob[1] = EaterX[1] =  GGx1 + 27;
        EaterYglob[1] = EaterY[1] =  GGy1 + 14;
        put_GliderEater( EaterX[1], EaterY[1], stateGEater[1]);  // Input A
    }

    // glidergun 2: INPUT B
    if(put_UI_Input) {
        put_GliderGunRev( GGx2, GGy2 );
        // Gun Eater 2
        EaterXglob[2] = EaterX[2] =  GGx2 +  5;
        EaterYglob[2] = EaterY[2] =  GGy2 + 14;
        put_GliderEaterRev( EaterX[2], EaterY[2], stateGEater[2]); // INPUT B
    }



    // XOR pattern for A,B duplicator downstreams
    //put_XOR_R(startx+75-15-30 , starty+100+15+30, 0); // ok
    //test
    //put_XOR_L(startx+75-15-30  +42,    starty+100+15+30, 0);    // 1 A crash B hoch ok
    put_XOR_L(startx+75-15-30  +42,    starty+100+15+30 +2, 0);   // 2 A okFW  B ok  !!!


    put_GliderGunRev(GGx3,GGy3);  // INVERT A&B upper streams
    EaterX[3] =  GGx3 +  5 -30;
    EaterY[3] =  GGy3 + 14 +30;
    put_GliderEaterRev( EaterX[3], EaterY[3], 0); // INPUT B

}


//---------------------------------------------------------------------------
void put_VA(int startx, int starty) {
    static int EaterX[20], EaterY[20];
    static int stateGEater[20] = {0} ; // current state: 0=block, 1=let Gliders pass
    const  int xoffs=90;
    /*
    int GGx1= startx + 39   + 9, GGy1= starty + 1    ;      // A
    int GGx2= startx + 39*2 + 9, GGy2= starty + 2,   ;      // B
    */

    int GGx3= startx+63,  GGy3= starty+1 +220;           // gui INPUT 3: Cin_man.

    int GGx4= startx+4,           GGy4= starty+2 +120-2;        // GGG  to HA2-carry
    int GGx5= startx+120-17,      GGy5= starty+3 +120-2;        // GGG NOT to HA2-carry
    int GGx6= GGx4+60+26,         GGy6= GGy4+60+6;              // GGG4 vwert Rflx

    put_HA(startx+xoffs, starty, 1);
    //put_GliderReflxVertDnL(153+xoffs-15, 227-15 );  // for refl XOR dn NW-SW // OK


    // A&B stream for carry flag
    // upper A stream NW to SW
    put_GliderReflxHorizDnR_1(startx+67+xoffs, starty + 4);  // 1.1.0 top left

    // HA2 to (AxorB)
    put_HA(startx+13, starty+160, 0);


    // glidergun 1: INPUT Cin_man.
    put_GliderGun( GGx3, GGy3 );
    // Gun Eater 1
    stateGEater[3]=stateGEaterglob[3]=userInputGlobal[3];
    EaterXglob[3] = EaterX[3] =  GGx3 + 27;
    EaterYglob[3] = EaterY[3] =  GGy3 + 14;
    put_GliderEater( EaterX[3], EaterY[3], stateGEater[3]);  // Input Cin_man.


    // GGG to HA2-carry
    put_GliderGun( GGx4, GGy4 );
    //put_GliderEater( GGx4+27+50, GGy4+14+50, 0);  // test
    //put_GliderReflxVertDnL( GGx4+27+50+4-10+15, GGy4+14+50-3-10+15);  // test
    //put_GliderReflxVertDnL( GGx6, GGy6);          // HA2_carry2 vert rflx  // test
    put_GliderReflxVertDnL_1( GGx6-4, GGy6-4);          // HA2_carry2 vert-1 rflx  // test

    //put_GliderReflxHorizUpR (GGx6-15-15, GGy6+15);   // HA2_carry2 horz rflx

    // GGG not to HA2-carry
    put_GliderGunRev( GGx5, GGy5 );
    EaterX[5] =  GGx5 + 5;
    EaterY[5] =  GGy5 + 14;

    put_GliderReflxVert( GGx4+45, GGy4 +4);
    put_GliderEaterDnRev(GGx4+60+5, GGy4 +2 +26, 0);




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




    //--------------------------------------------------------------
    // Timer
    // QTimer updateTimer;  // see: mainwindow.h!
    // onUpdateTime();      // see: mainwindow.h!


    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime()));
    // Start the timer
    updateTimer.start(1003-10*updspeed);// screen refresh in msec

 }



// GUI form
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    close();
}



void MainWindow::drawRectangles( int32_t ystart, int32_t ystop ) {
    // draw GoL screen dots
    for (int yrow=ystart; yrow <= ystop; yrow++) {
      for (int xcol=0; xcol <= GOLscrWidth; xcol++)  {
        // Draw all the "live" cells.
        if (board[yrow+frame][xcol+frame] ) {
            mtx.lock();
            rectangle = scene->addRect( (xcol)*blockSize,  (yrow)*blockSize ,
                                    blockSize, blockSize, outlinePen, blueBrush);
            mtx.unlock();
        }
      }
    }
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

      static int synctick1=0, synctick2=0, synctick3=0, sync1=1, sync2=1, sync3=1;


      synctick1=(GenerationCnt-8+30)%30;  // sync-in for quick runtime input changes
      synctick2=(GenerationCnt-8+30)%30;  // 30=GGG_period, 8=GE_offset
      synctick3=(GenerationCnt-8+30)%30;


      //--------------------------------
      if(stateGEaterglob[1]!=userInputGlobal[1] && sync1)  {  //sync GUI input A
          sync1=0;
      }
      if(synctick1==16  && !sync1) {   //sync GUI input A
             stateGEaterglob[1]=userInputGlobal[1];
             put_GliderEater( EaterXglob[1], EaterYglob[1], stateGEaterglob[1]);
             sync1=1;
      }


      //--------------------------------
      if(stateGEaterglob[2]!=userInputGlobal[2] && sync2)  {   //sync GUI input B
          sync2=0;
      }

      if(synctick2==16  && !sync2) {     //sync GUI input B
             stateGEaterglob[2]=userInputGlobal[2];
             put_GliderEaterRev( EaterXglob[2], EaterYglob[2], stateGEaterglob[2]);
             sync2=1;
      }

      //--------------------------------
      if(stateGEaterglob[3]!=userInputGlobal[3] && sync3)  {   //sync GUI input C
          sync3=0;
      }

      if(synctick3==16  && !sync3) {     //sync GUI input C
             stateGEaterglob[3]=userInputGlobal[3];
             put_GliderEater( EaterXglob[3], EaterYglob[3], stateGEaterglob[3]);
             sync3=1;
      }


      //--------------------------------
      ui->labelOut1->setText(QString::number(userInputGlobal[1]));
      ui->labelOut2->setText(QString::number(userInputGlobal[2]));
      ui->labelOut3->setText(QString::number(userInputGlobal[3]));


      // GoL: calculate next Generation
      if(updspeed>0) {

         // Clear the temp board for the next generation
         memset(tmpboard, 0, sizeof(tmpboard));

         const int par=50; // array parts, parallel threads
         int yn[par+1];

         yn[0]=1;
         for (uint32_t i=1; i<=par; i++) { yn[i]= (i*yrows)/par; }

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
      rectangle = scene->addRect( 0, 0, (GOLscrWidth+1)*blockSize, (GOLscrHeight+1)*blockSize, outlinePen, transpBrush);

      outlinePen.setWidth(1);




      //drawRectangles( 0, GOLscrHeight );

      // launch all threads
      std::thread dthr0 (&MainWindow::drawRectangles, this, 0,    GOLscrHeight/4 );
      std::thread dthr1 (&MainWindow::drawRectangles, this, 1+GOLscrHeight/4, 2*GOLscrHeight/4 );
      std::thread dthr2 (&MainWindow::drawRectangles, this, 1+2*GOLscrHeight/4, 3*GOLscrHeight/4 );
      std::thread dthr3 (&MainWindow::drawRectangles, this, 1+3*GOLscrHeight/4, GOLscrHeight );

      // join threads when calcs finished
      dthr0.join();
      dthr1.join();
      dthr2.join();
      dthr3.join();




      // generation monitor
      ui->labelGen->setText("gen="+QString::number(GenerationCnt));
      if(updspeed>0) GenerationCnt++;



      textposX[1] =(65+100)*blockSize ;      // A
      textposY[1] =(55)*blockSize-8;

      textposX[2] =(215+90)*blockSize ;      // B
      textposY[2] =(55)*blockSize-8;

      textposX[3] =(35+40+18)*blockSize ;       // Cin
      textposY[3] =(215+18)*blockSize-8;



      textposX[4] =(35+50)*blockSize+10 ;    // A&B top
      textposY[4] =(20)*blockSize-4;

      textposX[5] =(160+90)*blockSize ;      // AxorB
      textposY[5] =(220)*blockSize;

      textposX[6] =(160+10)*blockSize ;      // (AxorB)xorC (-> sum2)
      textposY[6] =(220+120)*blockSize;

      textposX[7] =(70+30)*blockSize ;       // (AxorB)andC
      textposY[7] =(160+25)*blockSize;

      textposX[8] =(70)*blockSize ;          // [ HA1 or HA2]
      textposY[8] =(120+15)*blockSize;

      textposX[9] =(5)*blockSize ;           // FA carry
      textposY[9] =(240)*blockSize;

      textposX[10] =(160+20)*blockSize ;     // FA sum2
      textposY[10] =(220+160)*blockSize;



      QGraphicsSimpleTextItem* text1 = scene->addSimpleText("A", QFont("Arial", 14) );
      text1->setBrush(Qt::red);
      text1->setPos(textposX[1], textposY[1]);

      QGraphicsSimpleTextItem* text2 = scene->addSimpleText("B", QFont("Arial", 14) );
      text2->setBrush(Qt::red);
      text2->setPos(textposX[2], textposY[2]);

      QGraphicsSimpleTextItem* text3 = scene->addSimpleText("Cin", QFont("Arial", 14) );
      text3->setBrush(Qt::red);
      text3->setPos(textposX[3], textposY[3]);


      QGraphicsSimpleTextItem* text4 = scene->addSimpleText(" HA_1 \nAandB \ncarry_1", QFont("Arial", 10) );
      text4->setBrush(Qt::red);
      text4->setPos(textposX[4], textposY[4]);

      QGraphicsSimpleTextItem* text5 = scene->addSimpleText(" HA_1 \nAxorB \nsum_1", QFont("Arial", 10) );
      text5->setBrush(Qt::red);
      text5->setPos(textposX[5], textposY[5]);


      QGraphicsSimpleTextItem* text6 = scene->addSimpleText("    HA_2 [xor]", QFont("Arial", 10) );
      text6->setBrush(Qt::red);
      text6->setPos(textposX[6], textposY[6]);

      QGraphicsSimpleTextItem* text7 = scene->addSimpleText(" HA_2 [and]", QFont("Arial", 10) );
      text7->setBrush(Qt::red);
      text7->setPos(textposX[7], textposY[7]);

      QGraphicsSimpleTextItem* text8 = scene->addSimpleText(" [HA_1 or HA_2]", QFont("Arial", 10) );
      text8->setBrush(Qt::red);
      text8->setPos(textposX[8], textposY[8]);

      QGraphicsSimpleTextItem* text9 = scene->addSimpleText(" FA\ncarry", QFont("Arial", 14) );
      text9->setBrush(Qt::red);
      text9->setPos(textposX[9], textposY[9]);

      QGraphicsSimpleTextItem* text10 = scene->addSimpleText(" FA \nsum", QFont("Arial", 14) );
      text10->setBrush(Qt::red);
      text10->setPos(textposX[10], textposY[10]);



      if(StepMode) {
          StepMode=false;
          userUSnew = 0;
      }

}




// Inoput Checkboxes

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

void MainWindow::on_checkBox3_clicked(bool checked)
{
    if (checked) userInputGlobal[3]=1;
    else userInputGlobal[3]=0;
}




// scene zoom

void MainWindow::on_SliderBlocksize_sliderMoved(int position)
{
    userBlsize = position;
}

void MainWindow::on_SliderBlocksize_valueChanged(int value)
{
     userBlsize=value;
}



// generation update processing

void MainWindow::on_SliderUpdateSpeed_sliderMoved(int position)
{
    userUSnew=position;
}

void MainWindow::on_SliderUpdateSpeed_valueChanged(int value)
{
    userUSnew=value;
    if(value!=0) updspeedSav = value;  //  <<<< neu!
}


void MainWindow::on_btnGenReset_clicked()
{
    ResetCircuit();
    GenerationCnt=1;
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


