//
// support for serial bridge esp-link
//

void setup_wifi(void);

void wifiCb(void *response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);
    if(status == STATION_GOT_IP) {
      DEBUG_PRINTLN(F("Wifi connected! (Cb)"));
      wifiConnected = true;
    } else {
      DEBUG_PRINT(F("Wifi not ready (Cb): "));
      DEBUG_PRINTLN(status);
      wifiConnected = false; 
    }
  }
}

void resetCb(void) {
  wifiConnected = false;
  DEBUG_PRINTLN(F("resetCB called!"));
  //setup_wifi(); 
//#ifdef HAVE_ESPWEB
//  webServer.setup();
//#endif
}

void setup_wifi(void) {
int err;
uint8_t send_retries = 1;
 if (cust_params[HAVE_WIFI]) {
  if (! wifiConnected) {
   DEBUG_PRINTLN(F("Setup Wifi client..."));
   lcd.setCursor(0, 1);
   bool ok = false;
   //esp.resetCb = resetCb;
   //esp.wifiCb.attach(wifiCb);
   do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) {
      OUT_SER(FS(ERR_WIFI));
      OUT_SERLN(F(" Do a full reset or switch off & on."));
      FPL(ERR_WIFI);
      lcd.setCursor(0, 1);
      delay(500);
      esp.init();
      esp.Process();
    }
    send_retries++ ;
    //esp.Process();
   } while(!ok && send_retries < SEND_MAX_RETRY);
   if (ok) {
    OUT_SERLN(FS(OK_WIFI));
    FPL(OK_WIFI);
    esp.GetWifiStatus();
    //ELClientPacket *packet;
    //if ((packet=esp.WaitReturn(2000)) != NULL) {
    //  OUT_SER("Packet: ");
    //  OUT_SERLN(packet->value);
    //}
    delay(1000);
    wifiConnected = true;
    //rest = NULL;
   } else {
    wifiConnected = false;
    cust_params[HAVE_WIFI] = 0;
   }
  }
  send_retries = 1;
  do {
   //lcd.setCursor(0, 1);
   err = rest.begin(api_host);
   if (err != 0) {
    OUT_SER(FS(ERR_REST));
    OUT_SERLN(err);
    //FPL(ERR_REST);
   } else {
    OUT_SER(FS(OK_REST));
    OUT_SERLN(send_retries);
    //FPL(OK_REST);
    wifiConnected = true;
    break;
   }
   delay(500);
   //esp.Process();
  } while (wifiConnected && (send_retries++ <= SEND_MAX_RETRY));
  if (err != 0) {
    wifiConnected = false; // give up on Wifi
  }
#ifdef HAVE_ESPWEB
  webServer.setup();
#endif
 }
}


//Wifi send using esp-link
bool wifi_send(void) {
    //esp.Process();

    char path_data[BUFLEN];
    char tmp[6];
    char response[BUFLEN];

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
    DEBUG_PRINTLN(path_data);
    //rest.post(path_data,"");
    rest.get(path_data);
    memset(response, 0, sizeof(response));
    uint16_t code = rest.waitResponse(response, BUFLEN-1);
   // DEBUG_PRINT(" ");
    if(code == HTTP_STATUS_OK){
      DEBUG_PRINT(F("REST success: "));
      DEBUG_PRINTLN(response);
      return true;
    } else {
      DEBUG_PRINT(F("REST HTTP error: "));
      DEBUG_PRINT(code);
      DEBUG_PRINT(" - ");
      DEBUG_PRINTLN(response);
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
