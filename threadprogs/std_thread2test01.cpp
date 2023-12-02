 
#include <stdio.h>       
#include <thread>         // std::thread


 
void foo(int f, int m, int *toreturn)   // of course once this func will be much bigger
{
  int res;
  res= f*m;
  *toreturn=res;
}



int main() 
{
  int res1, res2; 

  for (int n=0; n<10; n++) {
       std::thread first = std::thread(&foo, n, 2, &res1);      
       std::thread secnd = std::thread(&foo, n, 3, &res2);   
              
       first.join();               
       secnd.join();
       printf("loop=%d, result1=%d, result2=%d\n", n, res1, res2);
  }


  return 0;
}
