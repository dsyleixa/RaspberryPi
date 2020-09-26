#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include "mcp23017_lib.h"
#include "mcp23017_lib.c"


int main(int argc, char *argv[])   {
  tMcp23017  expander; /* Verwaltungs-Structure */
  int data = 0x01;     /* Ausgabewert */
  int down = 0;        /* Richtungsangabe */

  expander = init_mcp23017(0x20,0,0,"/dev/i2c-1");

  while(1)
    {
    /* beide Ports identisch "bedienen" */
    write_mcp23017(expander,GPIOA,data);
    write_mcp23017(expander,GPIOB,data);

    if (data == 0x80) /* ganz links - umdrehen */
      down = 1;
    if (data == 0x01) /* ganz rechts - umdrehen */
      down = 0;

    if (down)
       data = data >> 1;
    else
       data = data << 1;
    usleep(100000); /* 100 ms Pause */
    }

  return 0;
}
