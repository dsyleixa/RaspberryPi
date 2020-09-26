/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32|64 size display using SPI or I2C to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution

02/18/2013 	Charles-Henri Hallard (http://hallard.me)
						Modified for compiling and use on Raspberry ArduiPi Board
						LCD size and connection are now passed as arguments on 
						the command line (no more #define on compilation needed)
						ArduiPi project documentation http://hallard.me/arduipi

						
*********************************************************************/

#include <stdio.h>
#include <stdint.h>

#include <termio.h>

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>



#include <getopt.h>

#define PRG_NAME        "oled_demo"
#define PRG_VERSION     "1.1"

// Instantiate the OLED display
ArduiPi_OLED oledtft;



 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

const int16_t lcdmaxX = 128;
const int16_t lcdmaxY =  64;

const int16_t lcdlineX = 21;
const int16_t lcdlineY =  8;

int16_t fontwidth = lcdmaxX/lcdlineX;
int16_t fontheight= lcdmaxY/lcdlineY;





// mimic conio.h kbhit
bool kbhit(void)
{
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);

    struct termios term;
    memcpy(&term, &original, sizeof(term));

    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int characters_buffered = 0;
    ioctl(STDIN_FILENO, FIONREAD, &characters_buffered);

    tcsetattr(STDIN_FILENO, TCSANOW, &original);

    bool pressed = (characters_buffered != 0);

    return pressed;
}



static unsigned char logo16_glcd_bmp[] =
{ 0b00000000,0b11000000,
  0b00000001,0b11000000,
  0b00000001,0b11000000,
  0b00000011,0b11100000,
  0b11110011,0b11100000,
  0b11111110,0b11111000,
  0b01111110,0b11111111,
  0b00110011,0b10011111,
  0b00011111,0b11111100,
  0b00001101,0b01110000,
  0b00011011,0b10100000,
  0b00111111,0b11100000,
  0b00111111,0b11110000,
  0b01111100,0b11110000,
  0b01110000,0b01110000,
  0b00000000,0b00110000 };

   int16_t   matrixXacc[lcdlineX];
   int16_t   matrixchar[lcdlineX][lcdlineY];
   int16_t   matrixccol[lcdlineX][lcdlineY];
   

   
void the_matrix() {
   volatile int16_t   p, x, y, k, c, ch;
   
   srand( millis() );   
   memset(matrixchar, 0, sizeof(matrixchar) );
   
   // init yacc
   for(p=0; p<lcdlineX; p++) {
      if(p==0 || p==lcdlineX-1) matrixXacc[p] = 1+ (random() % (4) ) ;  
      else 
      if (matrixXacc[p]==0 && matrixXacc[p-1]==0) matrixXacc[p] = 1+ (random() % (4) ) ;
      else      
      matrixXacc[p] = (random() % (5) ) ;  
      if( (p>1) && (matrixXacc[p]==matrixXacc[p-1]) ) matrixXacc[p]=1;
   }
   
   
   c=1;
   for(;;) {      
     
      for (y=lcdlineY-1; y>=1; y--) {        
        for(x=0; x<lcdlineX; x++) {
           if( c ||  matrixXacc[x]>2 )
           { 
             matrixchar[x][y] = matrixchar[x][y-1] ;   
           }                                 
        }   
      }        
     
      for(x=0; x<lcdlineX; x++) {  
         if( c ||  matrixXacc[x]>2 )    
         {        
           ch = (random() % (127));
           if( !(ch % 14) ) ch=':';
           if( !(ch % 13) ) ch='.';
           if( (ch<31) || !(ch % 8) || ( matrixXacc[x] == 0 ) ) ch=' ';
           if( !(ch % 5) && ( matrixXacc[x] == 1 ) ) ch=' '; 
           matrixchar[x][0] = ch;                        
         }    
         c=!c;            
      }   
       
      oledtft.setTextColor(WHITE, BLACK);
      for(x=0; x<lcdlineX; x++) {
         {  
            for (y=0; y<lcdlineY; y++)  {
              oledtft.setCursor(x*fontwidth, y*fontheight);
              oledtft.write(matrixchar[x][y]);              
            }                  
         }            
      }
      oledtft.display(); 
      
      if (kbhit())
      {
         int c = getchar();
         if(c==27) break;
      }  
     
   }

}




void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random() % oledtft.width();
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
    
    //printf("x: %d", icons[f][XPOS]);
    //printf("y: %d", icons[f][YPOS]);
    //printf("dy: %d\n", icons[f][DELTAY]);
  }

  for (int i=0; i<100; ++i) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oledtft.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    oledtft.display();
    //usleep(100000/sleep_divisor);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oledtft.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > oledtft.height()) {
	icons[f][XPOS] = random() % oledtft.width();
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
   }
}


void testdrawchar(void) {
  oledtft.setTextSize(1);
  oledtft.setTextColor(WHITE);
  oledtft.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    oledtft.write(i);
    if ((i > 0) && (i % 21 == 0))
      oledtft.print("\n");
  }    
  oledtft.display();
}



void testdrawcircle(void) {
  for (int16_t i=0; i<oledtft.height(); i+=2) {
    oledtft.drawCircle(oledtft.width()/2, oledtft.height()/2, i, WHITE);
    oledtft.display();
  }
}



void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<oledtft.height()/2; i+=3) {
    // alternate colors
    oledtft.fillRect(i, i, oledtft.width()-i*2, oledtft.height()-i*2, color%2);
    oledtft.display();
    color++;
  }
}


void testdrawtriangle(void) {
  for (int16_t i=0; i<min(oledtft.width(),oledtft.height())/2; i+=5) {
    oledtft.drawTriangle(oledtft.width()/2, oledtft.height()/2-i,
                     oledtft.width()/2-i, oledtft.height()/2+i,
                     oledtft.width()/2+i, oledtft.height()/2+i, WHITE);
    oledtft.display();
  }
}


void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(oledtft.width(),oledtft.height())/2; i>0; i-=5) {
    oledtft.fillTriangle(oledtft.width()/2, oledtft.height()/2-i,
                     oledtft.width()/2-i, oledtft.height()/2+i,
                     oledtft.width()/2+i, oledtft.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oledtft.display();
  }
}


void testdrawroundrect(void) {
  for (int16_t i=0; i<oledtft.height()/2-2; i+=2) {
    oledtft.drawRoundRect(i, i, oledtft.width()-2*i, oledtft.height()-2*i, oledtft.height()/4, WHITE);
    oledtft.display();
  }
}


void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<oledtft.height()/2-2; i+=2) {
    oledtft.fillRoundRect(i, i, oledtft.width()-2*i, oledtft.height()-2*i, oledtft.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oledtft.display();
  }
}

   
void testdrawrect(void) {
  for (int16_t i=0; i<oledtft.height()/2; i+=2) {
    oledtft.drawRect(i, i, oledtft.width()-2*i, oledtft.height()-2*i, WHITE);
    oledtft.display();
  }
}


void testdrawline() {  
  for (int16_t i=0; i<oledtft.width(); i+=4) {
    oledtft.drawLine(0, 0, i, oledtft.height()-1, WHITE);
    oledtft.display();
  }
  for (int16_t i=0; i<oledtft.height(); i+=4) {
    oledtft.drawLine(0, 0, oledtft.width()-1, i, WHITE);
    oledtft.display();
  }
  usleep(250000);
  
  oledtft.clearDisplay();
  for (int16_t i=0; i<oledtft.width(); i+=4) {
    oledtft.drawLine(0, oledtft.height()-1, i, 0, WHITE);
    oledtft.display();
  }
  for (int16_t i=oledtft.height()-1; i>=0; i-=4) {
    oledtft.drawLine(0, oledtft.height()-1, oledtft.width()-1, i, WHITE);
    oledtft.display();
  }
  usleep(250000);
  
  oledtft.clearDisplay();
  for (int16_t i=oledtft.width()-1; i>=0; i-=4) {
    oledtft.drawLine(oledtft.width()-1, oledtft.height()-1, i, 0, WHITE);
    oledtft.display();
  }
  for (int16_t i=oledtft.height()-1; i>=0; i-=4) {
    oledtft.drawLine(oledtft.width()-1, oledtft.height()-1, 0, i, WHITE);
    oledtft.display();
  }
  usleep(250000);

  oledtft.clearDisplay();
  for (int16_t i=0; i<oledtft.height(); i+=4) {
    oledtft.drawLine(oledtft.width()-1, 0, 0, i, WHITE);
    oledtft.display();
  }
  for (int16_t i=0; i<oledtft.width(); i+=4) {
    oledtft.drawLine(oledtft.width()-1, 0, i, oledtft.height()-1, WHITE); 
    oledtft.display();
  }
  usleep(250000);
}






/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments: 
====================================================================== */
int main(int argc, char **argv)
{
  int i;
	
  // Oled supported display in ArduiPi_SSD1306.h
  
  setenv("WIRINGPI_GPIOMEM", "1", 1);      // no sudo for gpios required
     

  // I2C change parameters to fit to your LCD
  if ( !oledtft.init(OLED_I2C_RESET, 6) )
    exit(EXIT_FAILURE);

  oledtft.begin();
	
  // init done
  
  oledtft.clearDisplay();   // clears the screen  buffer
  oledtft.display();   		// display it (clear display) 

  oledtft.clearDisplay();

  // test
 
  oledtft.drawBitmap(128/2, 64/2, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  oledtft.setTextSize(1);
  oledtft.setTextColor(WHITE);
  oledtft.setTextSize(1); 
  oledtft.setCursor(0, 0);  oledtft.print(" 0 Hello, world!\n");
  oledtft.setCursor(0, 8);  oledtft.print(" 8 i2c OLED");  
  oledtft.setCursor(0,16);  oledtft.print("16 128x64 addr 0x3c");
  oledtft.setCursor(0,24);  oledtft.print("24 8x21 char size=1");
  oledtft.setCursor(0,32);  oledtft.print("32 ");
  oledtft.setCursor(0,40);  oledtft.print("40 ");
  oledtft.setCursor(0,48);  oledtft.print("48 456789112345678921");
  oledtft.setCursor(0,56);  oledtft.print("56 ");
  //oledtft.setTextColor(BLACK, WHITE); // 'inverted' text
  oledtft.display();
  sleep(2);
  
  oledtft.drawBitmap(128/2, 64/2, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  oledtft.display();
  sleep(3);
  oledtft.clearDisplay();
  oledtft.display();
  
  
  
  
  the_matrix();


  /*
  // draw many lines
  testdrawline();
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

  // draw rectangles
  testdrawrect();
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

  // draw a white circle, 10 pixel radius
  oledtft.fillCircle(oledtft.width()/2, oledtft.height()/2, 10, WHITE);
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

  testdrawroundrect();
  sleep(2);
  oledtft.clearDisplay();

  testfillroundrect();
  sleep(2);
  oledtft.clearDisplay();

  testdrawtriangle();
  sleep(2);
  oledtft.clearDisplay();
   
  testfilltriangle();
  sleep(2);
  oledtft.clearDisplay();
  
  

  // draw the first ~12 characters in the font
  testdrawchar();
  oledtft.display();
  sleep(2);
  oledtft.clearDisplay();

	// text display tests
  oledtft.setTextSize(1);
  oledtft.setTextColor(WHITE);
  oledtft.setCursor(0,0);
  oledtft.print("Hello, world!\n");
  oledtft.setTextColor(BLACK, WHITE); // 'inverted' text
  oledtft.printf("%f\n", 3.141592);
  oledtft.setTextSize(2);
  oledtft.setTextColor(WHITE);
  oledtft.printf("0x%8X\n", 0xDEADBEEF);
  oledtft.display();
  sleep(2); 

  */

   
  
  /*
	// horizontal bargraph tests
  oledtft.clearDisplay();
  oledtft.setTextSize(1);
  oledtft.setTextColor(WHITE);
	for ( i =0 ; i<=100 ; i++)
	{
		oledtft.clearDisplay();
		oledtft.setCursor(0,0);
		oledtft.print("Gauge Graph!\n");
		oledtft.printf("  %03d %%", i);
		oledtft.drawHorizontalBargraph(0,16, (int16_t) oledtft.width(),16,1, i);
		oledtft.display();
		usleep(25000/sleep_divisor);
	}
	
	// vertical bargraph tests
  oledtft.clearDisplay();
  oledtft.setTextSize(1);
  oledtft.setTextColor(WHITE);
	for ( i =0 ; i<=100 ; i++)
	{
		oledtft.clearDisplay();
		oledtft.setCursor(0,0);
		oledtft.print("Gauge !\n");
		oledtft.printf("%03d %%", i);
		oledtft.drawVerticalBargraph(114,0,8,(int16_t) oledtft.height(),1, i);
		oledtft.display();
		usleep(25000/sleep_divisor);
	}
	
		
  // draw scrolling text
  testscrolltext();
  sleep(2);
  oledtft.clearDisplay();


  // miniature bitmap display
  oledtft.clearDisplay();
  oledtft.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  oledtft.display();

  // invert the display
  oledtft.invertDisplay(true);
  sleep(1); 
  oledtft.invertDisplay(false);
  sleep(1); 
 
  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
  */
  
  
  // Free PI GPIO ports
  oledtft.clearDisplay();
  oledtft.display();
  
  oledtft.close();

}


