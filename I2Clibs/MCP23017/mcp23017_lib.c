#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "mcp23017_lib.h"


/* defined in <linux/i2c-dev.h>
#define I2C_SLAVE 0x703 */



/* Init des Expanders; gibt den Expander zurück
 * address: I2C-Busadresse des Bausteines (i2cdetect -y 1)
 * directionA/B: Richtungen der Ports
 * I2CBus: Pfad zum I2CBus ("/dev/i2c-1" für Bus 1)
 */
 
tMcp23017 init_mcp23017(int address, int directionA, int directionB, char* I2CBus) {
  int fd;                 /* Filehandle */
  tMcp23017 expander;   /* Rueckgabedaten */

  /* Structure mit Daten fuellen */
  expander.address = address;
  expander.directionA = directionA;
  expander.directionB = directionB;
  expander.I2CBus = I2CBus;
  // Port-IO-dirmode (Eingabe/Ausgabe) setzen  
  fd = open_mcp23017(expander);
  setdir_mcp23017(expander, IODIRA, expander.directionA);
  setdir_mcp23017(expander, IODIRB, expander.directionB);
  close(fd);
  return expander;
}



/* Datenrichtung der Ports festlegen
 * IOdirmoderegister: muss "IODIRA" oder "IODIRB" sein!
 * value: Zuordnung der Bits (Input: 1, Output: 0)
 * Bei Eingangspins wird Pullup Widerstand eingschalten und Logik gedreht
 */
 
void setdir_mcp23017(tMcp23017 expander, int IOdirmoderegister, int value)  {
  if(IOdirmoderegister == IODIRA)
    {
    /* Datenrichtung schreiben */
    write_mcp23017(expander, IODIRA, value);
    /* Pull-Up-Widerstaende einschalten Port A */
    write_mcp23017(expander, GPPUA, value);
    /* Logik umkehren */
    write_mcp23017(expander, IPOLA, value);
    }
  else if(IOdirmoderegister == IODIRB)
    {
    /* Datenrichtung schreiben */
    write_mcp23017(expander, IODIRB, value);
    /* Pull-Up-Widerstaende einschalten Port B */
    write_mcp23017(expander, GPPUB, value);
    /*Logik umkehren */
    write_mcp23017(expander, IPOLB, value);
    }
  else
    {
    printf("IO-dirmode register faulty!\n");
    exit(1);
    }
}
  
  

/* Oeffnet den Bus und gibt Filedescriptor zurueck
 * (write_mcp23017 und read_mcp23017 übernehmen das selbst)
 */
 
int open_mcp23017(tMcp23017 expander)   {
  int fd;
  if ((fd = open(expander.I2CBus, O_RDWR)) < 0)
    {
    printf("Failed to open the i2c bus\n");
    exit(1);
    }

  /* Spezifizieren der Adresse des slave device */
  if (ioctl(fd, I2C_SLAVE, expander.address) < 0)
    {
    printf("Failed to acquire bus access and/or talk to slave\n");
    exit(1);
    }
  return fd;
}



/* Schreibt in ein Register des Expanders
 * reg: Register in das geschrieben werden soll
 * value: Byte das geschrieben werden soll
 */
 
void write_mcp23017(tMcp23017 expander, int reg, int value)   {
  int fd;
  fd = open_mcp23017(expander);
  if(i2c_smbus_write_byte_data(fd,reg,value) < 0)
    {
    printf("Failed to write to the i2c bus\n");
    exit(1);
    }
  close(fd);
}



/* Liest Register des Expanders
 * reg: Register, das ausgelesen wird;
 * gibt ausgelesenen Registerwert zurück
 */
 
int read_mcp23017(tMcp23017 expander, int reg)    {
   int value,fd;
   fd = open_mcp23017(expander);
   if((value = i2c_smbus_read_byte_data(fd, reg)) < 0)
     {
     printf("Failed to read from the i2c bus\n");
     close(fd);
     exit(1);
     return 0;
     }
   else
     {
     close(fd);
     return value;
     }
}

