/*
 *
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <wiringPi.h>

long order = 0;


long long fib( long n ) {
  long long llbuf;

  if ( n==0 )    { llbuf = 0;  printf("."); }
  else   
  if ( n==1 )    { llbuf = 1;  }
  else {
     llbuf = fib(n-1) + fib(n-2);     
     if(n > order) {
         order = n;
         printf("\n order: %ld  %lld   \n" , order, llbuf); 
     }     
  }
  return (llbuf); 
}

int main(int argc, char **argv)
{
	long n;
    long long llres;
    
    
    printf("enter Fibonacci order: ");
    scanf("%ld", &n);    
        
    llres = fib(n);
    
    printf("\n\nFibonacci (%ld) is = %lld \n", n, llres);

    getchar();
    
	return 0;
}

