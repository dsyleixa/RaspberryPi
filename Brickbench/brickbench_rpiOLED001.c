// HaWe Brickbench
// benchmark test for NXT/EV3 and similar Micro Controllers
// PL: GCC, Raspi, Raspbian  Linux
// Autor: (C) Helmut Wunder 2013,2014
// ported to Raspi  by "HaWe"
//
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach schriftlicher Genehmigung durch den Autor.
// protected under the friendly Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/
// version 1.09.OLED.007  16.08.2015
// i2c-1 clock = 400000

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>


#include <stdint.h>
#include <time.h>
#include <sys/time.h>


#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>
#include <getopt.h>


// Instantiate the display
ArduiPi_OLED display;



unsigned long runtime[8];

int a[500], b[500], c[500], t[500];

uint32_t timer()
{
   struct timeval now;
   uint32_t ticks;
   gettimeofday(&now, NULL);
   ticks=now.tv_sec*1000+now.tv_usec/1000;
   return(ticks);
}


//--------------------------------------------
// Mersenne Twister
//--------------------------------------------

unsigned long randM(void) {
   const int M = 7;
   const unsigned long A[2] = { 0, 0x8ebfd028 };

   static unsigned long y[25];
   static int index = 25+1;

   if (index >= 25) {
     int k;
     if (index > 25) {
        unsigned long r = 9, s = 3402;
        for (k=0 ; k<25 ; ++k) {
          r = 509845221 * r + 3;
          s *= s + 1;
          y[k] = s + (r >> 10);
        }
     }
     for (k=0 ; k<25-M ; ++k)
        y[k] = y[k+M] ^ (y[k] >> 1) ^ A[y[k] & 1];
     for (; k<25 ; ++k)
        y[k] = y[k+(M-25)] ^ (y[k] >> 1) ^ A[y[k] & 1];
     index = 0;
   }

   unsigned long e = y[index++];
   e ^= (e << 7) & 0x2b5b2500;
   e ^= (e << 15) & 0xdb8b0000;
   e ^= (e >> 16);
   return e;
}


//--------------------------------------------
// Matrix Algebra
//--------------------------------------------

// matrix * matrix multiplication (matrix product)
 
 void MatrixMatrixMult(int N, int M, int K, double *A, double *B, double *C) {
   int i, j, s;
   for (i = 0; i < N; ++i) {
      for (j = 0; j < K; ++j) {
         C[i*K+j] = 0;
         for (s = 0; s < M; ++s) {
            C[i*K+j] = C[i*K+j] + A[i*N+s] * B[s*M+j];
         }
      }
   }
}


// matrix determinant

double MatrixDet(int N, double A[]) {
   int i, j, i_count, j_count, count = 0;
   double Asub[N - 1][N - 1], det = 0;

   if (N == 1)
      return *A;
   if (N == 2)
      return ((*A) * (*(A+1+1*N)) - (*(A+1*N)) * (*(A+1)));

   for (count = 0; count < N; count++) {
      i_count = 0;
      for (i = 1; i < N; i++) {
         j_count = 0;
         for (j = 0; j < N; j++) {
            if (j == count)
               continue;
            Asub[i_count][j_count] = *(A+i+j*N);
            j_count++;
         }
         i_count++;
      }
      det += pow(-1, count) * A[0+count*N] * MatrixDet(N - 1, &Asub[0][0]);
   }
   return det;
}





//--------------------------------------------
// shell sort
//--------------------------------------------

void shellsort(int size, int* A)
{
  int i, j, increment;
  int temp;
  increment = size / 2;

  while (increment > 0) {
    for (i = increment; i < size; i++) {
      j = i;
      temp = A[i];
      while ((j >= increment) && (A[j-increment] > temp)) {
        A[j] = A[j - increment];
        j = j - increment;
      }
      A[j] = temp;
    }

    if (increment == 2)
       increment = 1;
    else
       increment = (unsigned int) (increment / 2.2);
  }
}

//--------------------------------------------
// gnu quick sort
// (0ptional)
//--------------------------------------------

int compare_int (const int *a, const int *b)
{
  int  temp = *a - *b;

  if (temp > 0)          return  1;
  else if (temp < 0)     return -1;
  else                   return  0;
}

// gnu qsort:
// void qsort (void *a , size_a count, size_a size, compare_function)
// gnu qsort call for a[500] array of int:
// qsort (a , 500, sizeof(a), compare_int)



//--------------------------------------------
// benchmark test procedures
//--------------------------------------------


int test_Int_Add() {
   int i=1, j=11, k=112, l=1111, m=11111, n=-1, o=-11, p=-111, q=-1112, r=-11111;
   int x;
   volatile long s=0;
   for(x=0;x<10000;++x) {
     s+=i; s+=j; s+=k; s+=l; s+=m; s+=n; s+=o; s+=p; s+=q; s+=r;
   }
   return s;
}



long test_Int_Mult() {
  int x,y;
  volatile long s;

  for(y=0;y<2000;++y) {
    s=1;
    for(x=1;x<=13;++x) { s*=x;}
    for(x=13;x>0;--x) { s/=x;}

  }
  return s;
}


#define PI  M_PI


double test_float_math() {

  volatile double s=PI;
  int y;

  for(y=0;y<1000;++y) {
     s*=sqrt(s);
     s=sin(s);
     s=exp(s);
     s*=s;
  }
  return s;
}


long test_rand_MT(){
  volatile unsigned long s;
  int y;

  for(y=0;y<5000;++y) {
     s=randM()%10001;
  }
  return s;
}







double test_matrix_math() {
  int x;

  double A[2][2], B[2][2], C[2][2];
  double S[3][3], T[3][3];
  unsigned long s;

  for(x=0;x<250;++x) {

    A[0][0]=1;   A[0][1]=3;
    A[1][0]=2;   A[1][1]=4;

    B[0][0]=10;  B[0][1]=30;
    B[1][0]=20;  B[1][1]=40;

    MatrixMatrixMult(2, 2, 2, A[0], B[0], C[0]); // <<<<<<<<<<<<<<<<<<<

    A[0][0]=1;   A[0][1]=3;
    A[1][0]=2;   A[1][1]=4;
    MatrixDet(2, A[0]);                          // <<<<<<<<<<<<<<<<<<<

    S[0][0]=1;   S[0][1]=4;  S[0][2]=7;
    S[1][0]=2;   S[1][1]=5;  S[1][2]=8;
    S[2][0]=3;   S[2][1]=6;  S[2][2]=9;

    MatrixDet(3, S[0]);                          // <<<<<<<<<<<<<<<<<<<

  }

  s=(S[0][0]*S[1][1]*S[2][2]);
  return s;
}





// for array copy using void *memcpy(void *dest, const void *src, size_t n);

long test_Sort(){
  unsigned long s;
  int y;
  int t[500];

  for(y=0;y<30;++y) {
    memcpy(t, a, sizeof(a));
    shellsort(500, t);
   
    memcpy(t, a, sizeof(b));
    shellsort(500, t);
   
    memcpy(t, a, sizeof(c));
    shellsort(500, t);
  }

  return y;
}




long test_TextOut(){


  int  y=77;
  char buf[120];
  display.setTextSize(1);
  display.setTextColor(WHITE);

  for(y=0;y<20;++y) {
    display.clearDisplay();
    
    sprintf (buf, "%3d %4d  int_Add",    0, 1000); display.setCursor(6, 0);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  int_Mult",   1, 1010); display.setCursor(6, 8);  display.printf(buf); display.display(); 
    sprintf (buf, "%3d %4d  float_op",   2, 1020); display.setCursor(6,16);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  randomize",  3, 1030); display.setCursor(6,24);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  matrx_algb", 4, 1040); display.setCursor(6,32);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  arr_sort",   5, 1050); display.setCursor(6,40);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  displ_txt",  6, 1060); display.setCursor(6,48);  display.printf(buf); display.display();
    sprintf (buf, "%3d %4d  testing...", 7, 1070); display.setCursor(6,56);  display.printf(buf); display.display();
    

  }

  return y;
}



long test_graphics(){
    int y=0;
     
    for(y=0;y<100;++y) {
        
        display.clearDisplay();
   
        display.drawCircle(50, 40, 10, WHITE);  // circles
        display.display();

        display.fillCircle(30, 24, 10, WHITE);
        display.display();

        display.drawLine(10, 10, 60, 60, WHITE);   // just 2 intersecting lines
        display.display();
        display.drawLine(50, 20, 97, 63, WHITE);
        display.display();

        display.drawRect(20, 20, 40, 40, WHITE);     // rectangles
        display.display();
      
        display.fillRect(65, 25, 20, 30, WHITE);
        display.display();

        display.drawCircle(70, 30, 20, WHITE);  // no ellipse
        display.display();
    }
    return y;
}




inline void displayValues() {

  char buf[120];
    
    sprintf (buf, "%3d %7ld  int_Add",    0, runtime[0]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  int_Mult",   1, runtime[1]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  float_op",   2, runtime[2]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  randomize",  3, runtime[3]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  matrx_algb", 4, runtime[4]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  arr_sort",   5, runtime[5]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  displ_txt",  6, runtime[6]); printf(buf); printf("\n");
    sprintf (buf, "%3d %7ld  graphics",   7, runtime[7]); printf(buf); printf("\n");
}



int main(){

  unsigned long time0, x, y;
  float s;
  char  buf[120];
  
  char str[3];
  
  int i;
	
  // Oled supported display in ArduiPi_SSD1306.h

  // I2C change parameters to fit to your LCD
  if ( !display.init(OLED_I2C_RESET, 6) )
		exit(EXIT_FAILURE);

  display.begin();

  printf("hw brickbench"); printf("\n");
  printf("initializing..."); printf("\n");
 

  for(y=0;y<500;++y) {
    a[y]=randM()%30000; b[y]=randM()%30000; c[y]=randM()%30000;
  }
  
  time0= timer();
  s=test_Int_Add();
  runtime[0]=timer()-time0;
  sprintf (buf, "%3d %7ld  int_Add",    0, runtime[0]);  printf(buf); printf("\n");

  time0=timer();
  s=test_Int_Mult();
  runtime[1]=timer()-time0;
  sprintf (buf, "%3d %7ld  int_Mult",   0, runtime[1]);  printf(buf); printf("\n");

  time0=timer();
  s=test_float_math();
  runtime[2]=timer()-time0;
  sprintf (buf, "%3d %7ld  float_op",   0, runtime[2]);  printf(buf); printf("\n");

  time0=timer();
  s=test_rand_MT();
  runtime[3]=timer()-time0;
  sprintf (buf, "%3d %7ld  randomize",  0, runtime[3]);  printf(buf); printf("\n");

  time0=timer();
  s=test_matrix_math();
  runtime[4]=timer()-time0;
  sprintf (buf, "%3d %7ld  matrx_algb", 0, runtime[4]);  printf(buf); printf("\n");


  time0=timer();
  s=test_Sort();
  runtime[5]=timer()-time0;
  sprintf (buf, "%3d %7ld  arr_sort",   0, runtime[5]);  printf(buf); printf("\n");
  
  time0=timer();
  s=test_TextOut();
  runtime[6]=timer()-time0;

  time0=timer();
  s=test_graphics();
  runtime[7]=timer()-time0;

  
  y=0;
  for(x=0;x<8;++x) {y+= runtime[x];}
  printf("\n"); 
  printf("\n");
  

   
  displayValues();

  sprintf (buf, "gesamt ms: %ld ", y);           printf(buf); printf("\n");
  sprintf (buf, "benchmark: %ld ", 50000000/y ); printf(buf); printf("\n");



  fgets(str, 2, stdin);                   // look at the pic, end with [RETURN]
  
  exit(0);
}

