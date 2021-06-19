#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

static _Atomic int myval = 1;
static _Atomic bool flag = true;

int main() {
    printf("myval=%d  flag=%d \n", myval, flag);
    myval = 42;
    flag = false;
    myval /= 7;
    printf("myval=%d  flag=%d \n", myval, flag);
}