//
// support for serial bridge esp-link
//

#ifdef HAVE_ESPLINK
 #ifndef DEBUG
  #define OUT_SERIAL(x) Serial.begin (x)
  #define OUT_SER(x) Serial.print (x)
  #define OUT_SERLN(x) Serial.println (x)
 #endif
#endif

void setup_wifi(void);

void wifiCb(void *response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);
    if(status == STATION_GOT_IP) {
      DEBUG_PRINTLN(F("Wifi connected! (Cb)"));
      cust_params[HAVE_WIFI] = 1;
    } else {
      DEBUG_PRINT(F("Wifi not ready (Cb): "));
      DEBUG_PRINTLN(status);
      cust_params[HAVE_WIFI] = 0; // let the arduino work without bothering with Wifi any longer
      sync_done = false; // allow clean retry (?)
    }
  }
}

void resetCb(void) {
  setup_wifi(); 
//#ifdef HAVE_ESPWEB
//  webServer.setup();
//#endif
}

void setup_wifi(void) {
int err;
if (cust_params[HAVE_WIFI]) {
  if (! sync_done) {
   DEBUG_PRINTLN(F("Wifi client starting..."));
   lcd.setCursor(0, 1);
   bool ok = false;
   do {
    //esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) {
      OUT_SER(FS(ERR_WIFI));
      OUT_SERLN(F(" Do a full reset or switch off & on."));
      FPL(ERR_WIFI);
      lcd.setCursor(0, 1);
      delay(1000);
      esp.init();
      esp.Process();
    }
   } while(!ok);
   OUT_SERLN(FS(OK_WIFI));
   FPL(OK_WIFI);
   delay(2000);
   sync_done = true;
  }
  uint8_t send_retries = 1;
  do {
   lcd.setCursor(0, 1);
   err = rest.begin(api_host);
   if (err != 0) {
    OUT_SER(FS(ERR_REST));
    OUT_SERLN(err);
    //DEBUG_PRINTLN(esp._status);
    FPL(ERR_REST);
   } else {
    OUT_SER(FS(OK_REST));
    OUT_SERLN(send_retries);
    FPL(OK_REST);
    break;
   }
   delay(500);
  } while (send_retries++ <= SEND_MAX_RETRY);
  if (err != 0) {
    cust_params[HAVE_WIFI] = 0; // give up on Wifi
  }
#ifdef HAVE_ESPWEB
  webServer.setup();
#endif
 }
}


//Wifi send using esp-link
bool wifi_send(uint8_t send_try) {
    esp.Process();

    char path_data[BUFLEN];
    char tmp[6];

    sprintf(path_data, "%s", "/update?api_key=");
    sprintf(path_data + strlen(path_data), "%s", api_key);
    sprintf(path_data + strlen(path_data), "%s", "&field1=");
    dtostrf(hum_o,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    sprintf(path_data + strlen(path_data), "%s", "&field2=");
    dtostrf(temp_o,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    sprintf(path_data + strlen(path_data), "%s", "&field3=");
    dtostrf(hum_i,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    sprintf(path_data + strlen(path_data), "%s", "&field4=");
    dtostrf(temp_i,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    sprintf(path_data + strlen(path_data), "%s", "&field5=");
    dtostrf(dew_o,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    sprintf(path_data + strlen(path_data), "%s", "&field6=");
    dtostrf(dew_i,0,1,tmp);
    sprintf(path_data + strlen(path_data), "%s", tmp);
    //rest.post(path_data,"");
    rest.get(path_data);
    char response[BUFLEN];
    memset(response, 0, sizeof(response));
    uint16_t code = rest.waitResponse(response, BUFLEN);
    DEBUG_PRINT(path_data);
    DEBUG_PRINT(" ");
    if(code == HTTP_STATUS_OK){
      DEBUG_PRINTLN(response);
      return true;
    } else {
      DEBUG_PRINTLN(code);
      return false;
    }
}

/*
char * get_date(void) {
 char tmp[20];

 #define SECS_PER_MIN  (60UL)
 #define SECS_PER_HOUR (3600UL)
 #define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
 #define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
 #define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
 #define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
 #define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  

 uint32_t secs = cmd.GetTime();
 int days = elapsedDays(secs);
 int hours = numberOfHours(secs);
 int minutes = numberOfMinutes(secs);
 int seconds = numberOfSeconds(secs);
 sprintf(tmp, "%d", days);
 sprintf(tmp + strlen(tmp), "%s", ":");
 sprintf(tmp + strlen(tmp), "%d", hours);
}

*/

