
#define SYNCSLOT      0  // start sync signal of this Msg: bsync  (e.g. 0xff/255)
#define CKSSLOT       1  // chksum this Msg
#define BYTE0         2  // byte 0      // byte: 8-bit => 8 digital bits 0-7
#define BYTE1         3  // byte 1      // byte: 8-bit => 8 digital bits 8-15
#define LONG0         4  // int32_t 0      // 3x int32_t: 32-bit/4byte
#define LONG1         8  // int32_t 1
#define LONG2        12  // int32_t 2
#define DOUBL0       16  // double0     // 2x double 64bit/8byte
#define DOUBL1       24  // double1
#define FLOAT0       32  // float0      // 3x float 32bit/4byte
#define FLOAT1       36  // float1
#define FLOAT2       40  // float2
#define ANA0         44  // analog 0    // 9x ananog (ADC) 16-bit/2byte
#define ANA1         46  // analog 1    
#define ANA2         48  // analog 2
#define ANA3         50  // analog 3
#define ANA4         52  // analog 4
#define ANA5         54  // analog 5
#define ANA6         56  // analog 6
#define ANA7         58  // analog 7
#define ANA8         60  // analog 8
#define BYTE2        62  // byte 2      //  1 byte custom
#define TERM         63  // terminating //  1 byte custom 




//******************************************************************************
// bit and byte and pin operations
//******************************************************************************
// convert byte arrays to int

inline int16_t  ByteArrayToInt16(uint8_t  barray[], uint8_t  slot) {
  return ((barray[slot + 1] << 8) + (barray[slot]));
}

inline int32_t  ByteArrayToInt32(uint8_t  barray[], uint8_t  slot) {
  return ( (barray[slot+3]<<24) + (barray[slot+2]<<16) + (barray[slot+1]<<8) + barray[slot] );
}

//-----------------------------------------------------------------------------
// convert int to byte arrays

inline void Int16ToByteArray(int16_t vint16,  uint8_t barray[],  uint8_t slot) {
  memcpy(barray+slot*sizeof(char), &vint16, sizeof(int16_t));    // copy int16 to array
}

inline void Int32ToByteArray(int32_t vint32,  uint8_t barray[],  uint8_t slot) {
  memcpy(barray+slot*sizeof(char), &vint32, sizeof(int32_t));    // copy int32 to array
}


//-----------------------------------------------------------------------------
// convert float/double to byte arrays

void DoubleToByteArray(double fvar, uint8_t * barray,  uint8_t slot) {
   union {
     double  f;
     uint8_t b8[sizeof(double)];
   } u;
   u.f = fvar;
   memcpy(barray+slot*sizeof(char), u.b8, sizeof(double));  
}


double ByteArrayToDouble(uint8_t * barray, uint8_t  slot) {
   union {
     double  f;
     uint8_t b8[sizeof(double)];
   } u;
   memcpy( u.b8, barray+slot*sizeof(char), sizeof(double)); 
   return u.f;
}




void FloatToByteArray(float fvar, uint8_t * barray,  uint8_t slot) {
    union {
     double  f;
     uint8_t b4[sizeof(float)];
   } u;
   u.f = fvar;
   memcpy(barray+slot*sizeof(char), u.b4, sizeof(float));   
}



double ByteArrayToFloat(uint8_t * barray, uint8_t  slot) {
   union {
     double  f;
     uint8_t b4[sizeof(float)];
   } u;
   memcpy( u.b4, barray+slot*sizeof(char), sizeof(float)); 
   return u.f;
}
