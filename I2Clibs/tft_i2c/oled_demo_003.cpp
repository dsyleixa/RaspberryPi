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

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>


#include <getopt.h>

#define PRG_NAME        "oled_demo"
#define PRG_VERSION     "1.1"

// Instantiate the OLED display
ArduiPi_OLED oledtft;


// Config Option
struct s_opts
{
	int oled;
	int verbose;
} ;

int sleep_divisor = 1 ;
	

// default options values
s_opts opts = {
	OLED_ADAFRUIT_SPI_128x32,	// Default oled
  false							// Not verbose
};


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
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
  usleep(250000/sleep_divisor);
  
  oledtft.clearDisplay();
  for (int16_t i=0; i<oledtft.width(); i+=4) {
    oledtft.drawLine(0, oledtft.height()-1, i, 0, WHITE);
    oledtft.display();
  }
  for (int16_t i=oledtft.height()-1; i>=0; i-=4) {
    oledtft.drawLine(0, oledtft.height()-1, oledtft.width()-1, i, WHITE);
    oledtft.display();
  }
  usleep(250000/sleep_divisor);
  
  oledtft.clearDisplay();
  for (int16_t i=oledtft.width()-1; i>=0; i-=4) {
    oledtft.drawLine(oledtft.width()-1, oledtft.height()-1, i, 0, WHITE);
    oledtft.display();
  }
  for (int16_t i=oledtft.height()-1; i>=0; i-=4) {
    oledtft.drawLine(oledtft.width()-1, oledtft.height()-1, 0, i, WHITE);
    oledtft.display();
  }
  usleep(250000/sleep_divisor);

  oledtft.clearDisplay();
  for (int16_t i=0; i<oledtft.height(); i+=4) {
    oledtft.drawLine(oledtft.width()-1, 0, 0, i, WHITE);
    oledtft.display();
  }
  for (int16_t i=0; i<oledtft.width(); i+=4) {
    oledtft.drawLine(oledtft.width()-1, 0, i, oledtft.height()-1, WHITE); 
    oledtft.display();
  }
  usleep(250000/sleep_divisor);
}

void testscrolltext(void) {
  oledtft.setTextSize(2);
  oledtft.setTextColor(WHITE);
  oledtft.setCursor(10,0);
  oledtft.clearDisplay();
  
  if (opts.oled == OLED_SH1106_I2C_128x64)
    oledtft.print("No scroll\non SH1106");
  else
    oledtft.print("scroll");
  oledtft.display();
 
  oledtft.startscrollright(0x00, 0x0F);
  sleep(2);
  oledtft.stopscroll();
  sleep(1);
  oledtft.startscrollleft(0x00, 0x0F);
  sleep(2);
  oledtft.stopscroll();
  sleep(1);    
  oledtft.startscrolldiagright(0x00, 0x07);
  sleep(2);
  oledtft.startscrolldiagleft(0x00, 0x07);
  sleep(2);
  oledtft.stopscroll();
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
  else opts.oled = OLED_SH1106_I2C_128x64; 
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
  sleep(5);
  
  oledtft.drawBitmap(128/2, 64/2, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  oledtft.display();
  sleep(3);
  oledtft.clearDisplay();

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
 
  oledtft.setTextSize(2);
  oledtft.setTextColor(BLACK, WHITE); // inverted
  oledtft.printf("0x%8X", 0xDEADBEEF);
  
  oledtft.setTextSize(3);
  oledtft.setTextColor(WHITE);
  oledtft.printf("Size=3");
  
  oledtft.display();
  sleep(5); getchar();


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

  // Free PI GPIO ports
  oledtft.clearDisplay();
  oledtft.display();
  
  oledtft.close();

}


