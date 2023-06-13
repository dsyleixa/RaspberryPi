#ifndef GOL_DEFS_H
#define GOL_DEFS_H

char version[10]="v4.2";  // sprites for Duplicator (L/R)  and Reflector (LUp/Dn, RUp), new index


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
const int frame = 5;



// Make the board larger on either side to ensure that there's an invisible border of dead cells
/*
int yvisrows = (GOLscrHeight / blockSize);
int xviscols = (GOLscrWidth / blockSize);
*/

int yrows = GOLscrHeight + 2*frame;
int xcols = GOLscrWidth + 2*frame;

#define centeryrow (yrows/2)-1
#define centerxcol (xcols/2)-1

// two boards, one for the current generation and one for calculating the next one
char board[GOLscrHeight + 2*frame][GOLscrWidth + 2*frame];
char tmpboard[GOLscrHeight + 2*frame][GOLscrWidth + 2*frame];



//---------------------------------------------------------------------------
// GoL coordinates and patterns
//---------------------------------------------------------------------------

// coordinates
int EaterX[20], EaterY[20];
int textposX[20], textposY[20];


// variables for program control
int stateGEater[20] = {1} ; // current state: 0=block, 1=let Gliders pass
int userGEater[20] = {1} ;  // optional arbitrary user setting


//---------------------------------------------------------------------------
void put_Clock(int startx, int starty) {    //
  int x,y;

  char sprite[4][4] = {  //
    {0,0,1,0},
    {1,1,0,0},
    {0,0,1,1},
    {0,1,0,0},
  } ;

  for(x=0; x<4; x++) {
    for(y=0; y<4; y++) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_DoubleBlock(int startx, int starty) {    //
  int x,y;

  char sprite[2][2] = {  //
     {1,1},
     {1,1}
  } ;

  for(x=0; x<2; x++) {
    for(y=0; y<2; y++) {
      board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}



//---------------------------------------------------------------------------
// GliderGuns plus eaters
//---------------------------------------------------------------------------

char sprite_GGliderGun [9][36] =  {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,1,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
} ;


void put_GliderGun(int startx, int starty) {  // Gosper Glider Gun, period=30

  int x,y;
  /*
  char sprite[9][37] =
  { //
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  } ;
  */

  for(x=0; x<36; x++) {   // NXT screen (0,0) is bottom left, not top left  !
    for(y=0; y<9; y++) {
      board[starty+frame+y][startx+frame+x] = sprite_GGliderGun[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderGunRev(int startx, int starty) {  // Gosper Glider Gun, period=30

  int x,y;
  //
  /*
  char sprite[9][37] =  {
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
  */

  for(x=0; x<36; x++) {
    for(y=0; y<9; y++) {
      board[starty+frame+y][startx+frame+x] = sprite_GGliderGun [y][35-x] ; // <<<<<<<<<<<
    }
  }
}


//---------------------------------------------------------------------------
// Glider Duplicator

char sprite_GliderDuplicator [47][50] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,1,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};


//---------------------------------------------------------------------------
void put_GliderDuplic_LD_RUD(int startx, int starty) {  // Gosper Glider Gun, period=30

  int x,y, xdim=50, ydim=47; 
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderDuplicator[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderDuplic_RD_LUD(int startx, int starty) {  // Gosper Glider Gun, period=30

  int x,y, xdim=50, ydim=47;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderDuplicator[y][xdim-x-1] ;
    }
  }
}




// Glider Reflector sync 1

char sprite_GliderReflector_1[22][11] = {  //
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

  
// Glider Reflector sync 30

char sprite_GliderReflector[22][11] = {  // NO->SO
{0,0,1,1,0,0,0,0,0,1,1},
{0,0,0,1,0,0,0,0,0,1,1},
{0,0,0,1,0,1,0,0,0,0,0},
{0,0,0,0,1,1,0,0,0,0,0},  // Pfeil

{0,0,0,0,0,0,0,0,1,1,0},  // mini Pfeil
{0,0,0,0,0,0,0,1,0,1,0},
{0,0,0,0,0,0,0,0,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0},

{0,1,0,0,0,0,0,0,0,0,0},  // Stempel
{1,1,1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,1,1,0},
{0,0,0,0,0,0,0,1,1,0,1},

{1,1,1,0,0,0,1,0,0,0,1},  // 3er
{0,0,0,0,0,0,1,0,0,1,0},
{1,0,1,0,0,0,0,0,0,0,0},  // 2er Säule
{1,0,1,0,0,0,0,0,0,0,0},

{0,0,0,0,0,0,1,0,0,1,0},

{1,1,1,0,0,0,1,0,0,0,1},  // 3er vorn
{0,0,0,0,0,0,0,1,1,0,1},  // 2 vorne leer
{0,0,0,0,0,0,0,1,1,1,0},
{1,1,1,0,0,0,0,0,0,0,0},  // Stempel up
{0,1,0,0,0,0,0,0,0,0,0}
} ;



//---------------------------------------------------------------------------
void put_GliderReflxVert_1(int startx, int starty) {
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream
  // GG offset:
  //  dX[0] =  GGx + 5  (-i*10);
  //  dY[0] =  GGy + 1  (+i*10);

  int x,y, xdim=11, ydim=22;


  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector_1[y][x] ;
    }
  }
}




//---------------------------------------------------------------------------
void put_GliderReflxVert(int startx, int starty) { // Dn-NO-SO
  // Variante sync Gen 1 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderReflxVertUpR(int startx, int starty) { // Dn-SO-NO
  // Variante sync Gen 1 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector[ydim-1-y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderReflxVertUpL(int startx, int starty) { // Dn-SO-NO
  // Variante sync Gen 1 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector[ydim-1-y][xdim-1-x] ;
    }
  }
}



//---------------------------------------------------------------------------
void put_GliderReflxVertDnR_1(int startx, int starty) { // Dn-SO-NO
  // Variante sync Gen 30 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector_1[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderReflxVertDnL_1(int startx, int starty) { // Dn-SO-NO
  // Variante sync Gen 30 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+y][startx+frame+x]=sprite_GliderReflector_1[y][xdim-1-x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderReflxHorizDnR_1(int startx, int starty) { // Dn-SO-SW
  // Variante sync Gen 30 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+x][startx+frame+y]=sprite_GliderReflector_1[ydim-1-y][x] ; // sync+1
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderReflxHorizDnR(int startx, int starty) { // Dn-SO-SW
  // Variante sync Gen 1 init start
  // www.conwaylife.com/wiki/P15_bouncer
  // downleft stream to downright stream

  int x,y, xdim=11, ydim=22;
  for(x=0; x<xdim; x++) {
    for(y=0; y<ydim; y++) {
       board[starty+frame+x][startx+frame+y]=sprite_GliderReflector[ydim-1-y][x] ; // sync+1
    }
  }
}






// Glider Eater
//---------------------------------------------------------------------------
void put_GliderEater(int startx, int starty, char V) {
  int x,y;

  char sprite[4][4] = {  //
  {1 ,1 ,0 ,0 },
  {1 ,0 ,0 ,0 },
  {V ,1 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  } ;

  for(x=0; x<4; x++) {
    for(y=0; y<4; y++) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderEaterRev(int startx, int starty, char V) {
  int x,y;

  char sprite[4][4] = {  //
  {0 ,0 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  {1 ,1 ,1 ,V },
  {1 ,0 ,0 ,0 },
  } ;

  for(x=0; x<4; x++) {
    for(y=0; y<4; y++) {
       board[starty+frame+y][startx+frame+x]=sprite[y][x] ;
    }
  }
}


//---------------------------------------------------------------------------
void put_GliderEaterDn(int startx, int starty, char V) {
  int x,y;

  char sprite[4][4] = {  //
  {1 ,1 ,0 ,0 },
  {1 ,0 ,0 ,0 },
  {V ,1 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  } ;

  for(x=0; x<4; x++) {
    for(y=0; y<4; y++) {
       board[starty+frame+y][startx+frame+x]=sprite[3-y][x] ;
    }
  }
}

//---------------------------------------------------------------------------
void put_GliderEaterDnRev(int startx, int starty, char V) {
  int x,y;

  char sprite[4][4] = {  //
  {1 ,1 ,0 ,0 },
  {1 ,0 ,0 ,0 },
  {V ,1 ,1 ,1 },
  {0 ,0 ,0 ,1 },
  } ;

  for(x=0; x<4; x++) {
    for(y=0; y<4; y++) {
       board[starty+frame+y][startx+frame+x]=sprite[3-y][3-x] ;
    }
  }
}





//*********************************************************
#endif // GOL_DEFS_H

