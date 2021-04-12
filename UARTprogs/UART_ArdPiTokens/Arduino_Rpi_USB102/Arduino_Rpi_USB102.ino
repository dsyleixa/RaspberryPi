// Arduino COM to Raspi
// (C) 2018 by dsyleixa


// i2c
#include <Wire.h>   // Incl I2C comm, but needed for not getting compile error



int32_t i0 = 0;

#define TOKLEN 30
#define MSGLEN 1024
#define iINVALID -29999

String  inputString="";
char    cval[TOKLEN];  // number as cstring
char    mbuf[MSGLEN];  // cstring msg buffer


//==================================================================
// setup
//==================================================================
void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

}


//==================================================================
// loop
//==================================================================

void loop() {
  static bool stringComplete = false;

  //-------------------------------------------------------------
  // receive
  
  int  n=0;
  char inChar;

     
  while (Serial.available() && n<MSGLEN-1 ) { 
       if(n==MSGLEN-2) inChar='\n';
       else  
       inChar = (char)Serial.read();
    
       if(inChar=='\n' || inChar>=' ') inputString += inChar;
       
       if (inChar == '\n')  {
         stringComplete = true;
       }
       n++;
  }

  
  if (stringComplete)  {
    inputString.toCharArray(mbuf, MSGLEN-1);

    // process mbuf !
    
    //
    
    inputString="";
    stringComplete = false;

    //delay?
    delay(10);
  }
  //-------------------------------------------------------------
  // send  
  

  //----------------------
  
  char formatstr[MSGLEN];
  strcpy(formatstr, "$");
  strcat(formatstr, "message from Arduino message from Arduino message from Arduino message from Arduino message from Arduino: %d;\n");
  sprintf(mbuf, formatstr, i0);                 
  
  for (int i=0; i<strlen(mbuf); i++ ) Serial.print(mbuf[i]);
  
  //strcpy(mbuf, "");  
  
  i0++;

  //delay?
  delay(10); 
}




// end of file
