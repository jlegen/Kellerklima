//#define DEBUG true
#define HAVE_ESP true  // use ESP-01 remote console?
#define SERIAL_OUT true
//#define FAKE true

#ifdef SERIAL_OUT
  #define OUT_SER(x) Serial.print (x)
  #define OUT_SERLN(x) Serial.println (x)
  #define OUT_SERIAL(x) Serial.begin (x)
#else
  #define OUT_SER(x)
  #define OUT_SERLN(x)
  #define OUT_SERIAL(x)
#endif 

#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print (x)
  #define DEBUG_PRINTLN(x) Serial.println (x)
#else
  #define DEBUG_PRINT(x) 
  #define DEBUG_PRINTLN(x) 
#endif
 
 
