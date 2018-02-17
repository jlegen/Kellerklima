#define DEBUG true
//#define FAKE true

#ifdef DEBUG
  #define OUT_SER(x) Serial.print (x)
  #define OUT_SERLN(x) Serial.println (x)
  #define OUT_SERIAL(x) Serial.begin (x)

  #define DEBUG_PRINT(x) Serial.print (x)
  #define DEBUG_PRINTLN(x) Serial.println (x)
#else
 #ifndef HAVE_ESPLINK
  #define OUT_SER(x)
  #define OUT_SERLN(x)
  #define OUT_SERIAL(x)
 #endif
  #define DEBUG_PRINT(x) 
  #define DEBUG_PRINTLN(x) 
#endif

