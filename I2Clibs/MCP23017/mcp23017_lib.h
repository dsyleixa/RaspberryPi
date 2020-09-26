#ifndef _MCP23017LIB_H_
  #define _MCP23017LIB_H_


/* Richtungsregister von Port A und Port B */
#define IODIRA 0x00
#define IODIRB 0x01

/* Datenregister Port A und Port B */
#define GPIOA 0x12
#define GPIOB 0x13

/* Register um Logik-Polaritaet umzustellen */
#define IPOLA 0x02
#define IPOLB 0x03

/* Interne Pull-Up-Widerstände einschalten */
#define GPPUA 0x0C
#define GPPUB 0x0D

/* Pins am Port */
#define P1 0x01
#define P2 0x02
#define P3 0x04
#define P4 0x08
#define P5 0x10
#define P6 0x20
#define P7 0x40
#define P8 0x80


/* Struktur fuer den Expander-Datentyp */
struct expander
{
  int address;      /* I2C-Bus-Adresse des Bausteines      */
  int directionA;   /* Datenrichtung Port A                */
  int directionB;   /* Datenrichtung Port B                */
  char* I2CBus;     /* I2C-Device ("/dev/i2c-1" für Bus 1) */
};
  
  
/* Expander-Datentyp */
typedef struct expander tMcp23017;


/* besser:
 * typedef struct 
{
  int address;      // I2C-Bus-Adresse des Bausteines       
  int directionA;   // Datenrichtung Port A                 
  int directionB;   // Datenrichtung Port B                 
  char* I2CBus;     // I2C-Device ("/dev/i2c-1" für Bus 1)  
} Mcp23017 ;

*/


/* Init des Expanders; gibt den Expander zurück
 * address: I2C-Busadresse des Bausteines (i2cdetect -y 1)
 * directionA/B: Richtungen der Ports
 * I2CBus: Pfad zum I2CBus ("/dev/i2c-1" für Bus 1)
 */
tMcp23017 init_mcp23017(int address, int directionA, int directionB, char* I2CBus);

/* Datenrichtung der Ports festlegen
 * IOdirmoderegister: muss "IODIRA" oder "IODIRB" sein!
 * value: Zuordnung der Bits (Input: 1, Output: 0)
 * Bei den Eingangspins wird der Pullup-Widerstand eingeschaltet und die Logik umgekehrt
 */
void setdir_mcp23017(tMcp23017 expander, int IOdirmoderegister, int value);

/* Oeffnet den Bus und gibt Filedescriptor zurueck
 * (write_mcp23017 und read_mcp23017 übernehmen das selbst)
 */
int open_mcp23017(tMcp23017 expander);

/* Schreibt in ein Register des Expanders
 * reg: Register in das geschrieben werden soll
 * value: Byte das geschrieben werden soll
 */
void write_mcp23017(tMcp23017 expander, int reg, int value);

/* Liest Register des Expanders
 * reg: Register, das ausgelesen wird;
 * gibt ausgelesenen Registerwert zurück
 */
int read_mcp23017(tMcp23017 expander, int reg);


#endif /* _MCP23017LIB_H_ */
