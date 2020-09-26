/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32|64 size display using SPI or I2C to communicate
4 or 5 pins are required to interface
						
*********************************************************************/

#include <stdio.h>
#include <stdint.h>

#include <termio.h>

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>

#include <getopt.h>


// Instantiate the OLED display
ArduiPi_OLED oledtft;


 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

const int16_t lcdresX = 128;
const int16_t lcdresY =  64;

const int16_t lcdlinesX = 21;
const int16_t lcdlinesY =  8;

int16_t fontwidth = lcdresX/lcdlinesX;
int16_t fontheight= lcdresY/lcdlinesY;



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

 
   
void the_matrix() {
   volatile int16_t   p, x, y, c, ch;
   long k;
   int16_t   matrixXacc[lcdlinesX];
   int16_t   matrixchar[lcdlinesX][lcdlinesY];
   int16_t   matrixccol[lcdlinesX][lcdlinesY];

   
   srand( millis() );   
   memset(matrixchar, 0, sizeof(matrixchar) );
   
   // init yacc
   for(p=0; p<lcdlinesX; p++) {
      if(p==0 || p==lcdlinesX-1) matrixXacc[p] = 1+ (random() % (4) ) ;  
      else 
      if (matrixXacc[p]==0 && matrixXacc[p-1]==0) matrixXacc[p] = 1+ (random() % (4) ) ;
      else      
      matrixXacc[p] = (random() % (5) ) ;  
      if( (p>1) && (matrixXacc[p]==matrixXacc[p-1]) ) matrixXacc[p]=1;
   }
   
   
   c=1;
   for(k=0;;k++) {      
     
      for (y=lcdlinesY-1; y>=1; y--) {        
        for(x=0; x<lcdlinesX; x++) {
           if( c ||  matrixXacc[x]>2 )
           { 
             matrixchar[x][y] = matrixchar[x][y-1] ;   
           }                                 
        }   
      }        
     
      for(x=0; x<lcdlinesX; x++) {  
         if( c ||  matrixXacc[x]>2 )    
         {        
           ch = (random() % (255));
           p = 1+ random() % 5;
           if(ch>175 && ch<224) ch-=127;
           if( (ch<31) || !(ch % p) || ( matrixXacc[x] == 0 ) ) ch=' ';
           if( (ch == '!') ) ch=';';
           else if( (ch == '#') ) ch=':';
           else if( (ch == '/') ) ch=',';
           else if( (ch == 92) ) ch='.';

           matrixchar[x][0] = ch;                        
         }    
         c=!c;   
      
      }  

       
      oledtft.setTextColor(WHITE, BLACK);
      for(x=0; x<lcdlinesX; x++) {
         {  
            for (y=0; y<lcdlinesY; y++)  {
              oledtft.setCursor(x*fontwidth, y*fontheight);
              oledtft.write(matrixchar[x][y]);              
            }                  
         }            
      }
      oledtft.display(); 

      
      if (kbhit())
      {
         int ch = getchar();
         if(ch==27) break;
         else ch = getchar() ;
      }  
      

      p = random() % 7;
       
      if(p==0) {
         //c=1;
         p = random() % lcdlinesX; 
         matrixXacc[p] = (random() % (5) );
         
      }
      if (!(k % 10)) printf("%ld\n", k);
       
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






/* ======================================================================
 main()
====================================================================== */
int main()
{
  int i;
	
  // Oled supported display in ArduiPi_SSD1306.h 
   
  setenv("WIRINGPI_GPIOMEM", "1", 1);       // no sudo for gpios required 

  // I2C change parameters to fit to your LCD
  if ( !oledtft.init(OLED_I2C_RESET, 6) )
    exit(EXIT_FAILURE);

  oledtft.begin();
	
  // init done
  
  oledtft.clearDisplay();   // clears the screen  buffer
  oledtft.display();   		// display it (clear display) 

  oledtft.clearDisplay();
  
  printf("Wake up Neo... \n\n");

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

  oledtft.display();
  sleep(2);
  
  oledtft.drawBitmap(128/2, 64/2, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  oledtft.display();
  sleep(3);
  oledtft.clearDisplay();
  oledtft.display();
  
  
  
  printf("toc, toc, Neo...! \n\n\n");
  the_matrix();

  
  // Free PI GPIO ports
  oledtft.clearDisplay();
  oledtft.display();
  
  oledtft.close();
  
  return 0;

}


