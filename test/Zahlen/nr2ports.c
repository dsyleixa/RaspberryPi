#include <stdio.h>
#include <stdint.h>
#include <string.h>


int nr2port(int nr) {
    int  port;
    
    port = (1 << (nr % 4) );
    return port;
}


int main () {
   int i, ch;
   
   for (i=0; i<10; i++) {
       
       printf("i=%3d  port=%3d\n", i, nr2port(i) );
   }
   
   ch=getchar();
       
    return(0);
}
