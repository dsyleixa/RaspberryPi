#include <stdio.h>
#include <stdint.h>
#include <string.h>



void double2bytebuf(double fvar, uint8_t * bytearr) {
    
   union {
      double  f;
      uint8_t b8[8];
   }  u;
   
   u.f = fvar;
   memcpy(bytearr, u.b8, 8);
   
}

double  bytebuf2double(uint8_t * bytearr) {    
   double f;
   int i;
   union {
      double  f;
      uint8_t b8[8];
   }  u;
   
   memcpy(u.b8, bytearr, 8); 
   return u.f;   
}


int main () {

    uint8_t buf6[6], buf8[8];
    double  f1=0, f2=0;
    int ch, i;
    
    memset(buf6, 0, 6);
    memset(buf8, 0, 8);
    
        
    f1=1234567890.1234567890; // double: 15-stellige Genauigkeit 
    //f1=1000000000000000 ;
       
    printf("f1=%20.5f \n 64bit (8byte)  double: 15-stellige Genauigkeit\n", f1);
    printf("\n");
    
    double2bytebuf(f1, buf8);
    
    printf("b8\n");
    for(i=0; i<8; i++) {printf("%6x", buf8[i]);}    
    printf("\n");
    
    memcpy(buf6, buf8 +2, 6);  // copy to 6 bytes array, cut away 2 MSB
    
    printf("b6\n");
    for(i=0; i<6; i++) {printf("%6x", buf6[i]);}
    printf("\n");
    
    //...
    
    memset(buf8, 0, 8);        // clear dest buffer
    
    memcpy(buf8 +2, buf6, 6);  // rebuild 8-bytes buffer, skip 2 MSB 
    
    printf("b8 neu\n");
    for(i=0; i<8; i++) {printf("%6x", buf8[i]);}
    printf("\n");
    
    printf("f2=%20.5f f2 check before conversion\n", f2);
    
    f2=bytebuf2double(buf8);     // recreate double from byte buffer    
    printf("f2=%20.5f \n 48bit (6 byte)  float: 10-stellige Genauigkeit\n", f2);
 
    
    ch=getchar();
    
    
    
    
    return(0);
}
