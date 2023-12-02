#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
   #include <atomic>
   using namespace std;
   static atomic_int myval(1);
   static atomic_bool flag(true);
#else      
   #include <stdatomic.h>
   static atomic_int myval = 1;
   static atomic_bool flag = true;
#endif





int main() {
    printf("myval=%d  flag=%d \n", (int)myval, (bool)flag);
    myval = 42;
    flag = false;
    myval=myval/7; // myval /= 7; don't work for C++
    printf("myval=%d  flag=%d \n", (int)myval, (bool)flag);
}
