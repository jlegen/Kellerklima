#ifdef HAVE_ESPWEB

//#include <ELClientWebServer.h>

// called at button pressing
void setupSetFieldCb(const char * field)
{
  String fld = field;
    Serial.print("Field: ");
    Serial.println(fld);
  if( fld == F("api_key") )
  {
    String key = webServer.getArgString();
    Serial.print("API_KEY: ");
    Serial.println(key);
  }
}

void setupLoadCb(const char * url)
{
  webServer.setArgString(F("api_key"), api_key);
  //ledRefreshCb( url );
}
void setupRefreshCb(const char * url)
{
  webServer.setArgString(F("text"), F("Test-Überschrift"));
  //ledRefreshCb( url );
}

void setupButtonPressCb(char * btnId)
{
  String id = btnId;
    Serial.print("Button: ");
    Serial.println(id);
  
}

/*  void resetCb(void) {
  Serial.println("EL-Client (re-)starting!");
  bool ok = false;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println("EL-Client sync failed!");
  } while(!ok);
  Serial.println("EL-Client synced!");
  
  webServer.setup();
}
*/

void setup_Init()
{
  // set mode to output and turn LED off
//  pinMode(LED_PIN, OUTPUT);
//  digitalWrite(LED_PIN, false);
  
  URLHandler *setupHandler = webServer.createURLHandler(F("/KlimaSetup.html.json"));
  setupHandler->buttonCb.attach(setupButtonPressCb);
  setupHandler->setFieldCb.attach(setupSetFieldCb);
  setupHandler->loadCb.attach(setupLoadCb);
  setupHandler->refreshCb.attach(setupRefreshCb);
}

#endif
