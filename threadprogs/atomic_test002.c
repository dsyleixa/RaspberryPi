#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
#include <atomic>
using namespace std;
atomic_int myval(1);
atomic_bool flag(true);
#else      
#include <stdatomic.h>
static _Atomic  int myval = 1;
static _Atomic  bool flag = true;

#endif





int main() {
    printf("myval=%d  flag=%d \n", (int)myval, (bool)flag);
    myval = 42;
    flag = false;
    myval |= 7;
    printf("myval=%d  flag=%d \n", (int)myval, (bool)flag);
}
