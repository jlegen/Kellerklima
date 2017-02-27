// Lüftungssteuerung V1.0
// (c)2016 jleg99@gmail.com
//
// adapt settings in Config.h
//
// switches a fan and/or damper, when a configurable level of humidity is measured by S1, and
// S2 reads a lower absolute humidity.
// If a dehydrator is connected and configured, it is enabled after a configurable fan runtime
// passed without reaching the set humidity level.
// 
// tested with:
//   - DHT22 sensors
//   - I2C-LCD 16x2
//   - parallel LCD 16x2
//   - ESP-07 with esp-link 3.0.14
// prepared for (untested):
//   - LCDs of other sizes
//   - BME280 sensors
//   - DHT11 sensors
//
// Software configured for Box 1 (Ulf)

#if defined(ESP8266)
 #include <pgmspace.h>
#else
 #include <avr/pgmspace.h>
#endif
//#include <digitalWriteFast.h> 
#include <Wire.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <Menu.h>
#include <EEPROM.h>
#include "Config.h"
#ifdef DHTPIN_I
 #include "DHT.h"
#endif
#ifdef SENSI2C1
 #include "cactus_io_BME280_I2C.h";
#endif

// I2C LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
#ifdef LCD_I2C
 #include <LiquidCrystal_I2C.h>
 LiquidCrystal_I2C lcd(LCD_ADDR, LCD_CHARS, LCD_LINES);
#else
 #include <LiquidCrystal.h>
 LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif

#include "Tools.h"
#include "Debug.h"

// esp-link connected
#ifdef HAVE_ESPLINK
 #include <ELClient.h>
 #include <ELClientRest.h>
 // Initialize a connection to esp-link using the normal hardware serial port both for
 // SLIP and for debug messages.
#ifdef DEBUG
 ELClient esp(&Serial, &Serial);
#else
 ELClient esp(&Serial); // suspected to crash el-client?
#endif
 // Initialize a REST client on the connection to esp-link
 ELClientRest rest(&esp);
 // Initialize the Web-Server client
 #ifdef HAVE_ESPWEB
  #include <ELClientWebServer.h>
  ELClientWebServer webServer(&esp);
 #endif
#endif


// ä \341
// ü \365
// ö \357
// ° \337

#ifdef DHTPIN_I
 DHT dht_i(DHTPIN_I, DHTTYPE_I);
 DHT dht_o(DHTPIN_O, DHTTYPE_O);
#endif
#ifdef SENSI2C1
 BME280_I2C bme_i(SENSI2C1); 
 BME280_I2C bme_o(SENSI2C2); 
#endif


// ----------------------------------------------


void timerIsr()
{
  Encoder.service();
}

// function prototypes
double DewPoint(double akt_temp, double humidity);
void trigger_backlight(void);
void update_screen(uint8_t screen);
void do_measure(void);
void show_activity(void);
void check_switch_rules(uint8_t dev, uint8_t dewp_i, uint8_t dewp_o);
void dev_stop(uint8_t is_stop, uint8_t mydev);
void sprint_report(void);
// end function prototypes

#ifdef HAVE_ESPLINK
 #include "Esplink.h"
 #ifdef HAVE_ESPWEB
   #include "HTML_Pages.h"
 #endif
#endif

void setup() {
  pinMode(ActLED, OUTPUT);
  pinMode(Relais_L, OUTPUT);
  pinMode(Relais_E, OUTPUT);
#ifdef BUZZER  
 pinMode(BUZZER, OUTPUT);
#endif

  // Alles aus
  set_relay(0, false);              // Lüfter ausschalten
  set_relay(1, false);              // Entf. ausschalten

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  OUT_SERIAL(SERIAL_BAUD); // initialize only for if DEBUG or HAVE_ESPLINK

#ifdef HAVE_ESPLINK 
 delay(2000); // wait for esp-link monitor
#endif

  // LCD
#ifdef LCD_I2C
  lcd.begin();
#else
  pinMode(LCD_LED, OUTPUT);
  lcd.begin(LCD_CHARS, LCD_LINES);
#endif
  lcd.createChar(IconHeart, heart);
  lcd.createChar(IconLeft,  left);
  lcd.createChar(IconRight, right);
  lcd.createChar(IconBack,  back);
  lcd.createChar(IconBlock, block);

  lcd.clear();
  //lcd.backlight();
  LCD_ON;
  
  //         0123456789012345
  FPL(HEADER1);
  lcd.print(VERSION);
  lcd.setCursor(0, 1);
  FPL(HEADER2);
  delay(1500);
  lcd.setCursor(0, 1);
  FPL(HEADER3);
  delay(1500);
  lcd.setCursor(0, 1);
  FPL(HEADER4);
  buzzer(150);
  delay(2000);

  // read params from eeprom
  // if no valid settings found in EEPROM, or button is pressed => set to default values
  ClickEncoder::Button b = Encoder.getButton();          // pressed during Startup?
  lcd.setCursor(0, 1);
  lcd.write(IconRight);
  if (EEPROM.read(EEPROM_ADDR) < 10 || EEPROM.read(EEPROM_ADDR) > 100 || (b != ClickEncoder::Open)) 
  {
    write_to_eeprom();
    FPL(OutDefaults);
  } else {
    // load settings from EEPROM
    read_from_eeprom();
    FPL(OutLoaded);
#ifndef HAVE_ESPLINK
    cust_params[HAVE_WIFI] = 0;
#endif
/*
    if (cust_params[HAVE_WIFI] >1) cust_params[HAVE_WIFI] = 0;
    if (cust_params[HAVE_BEEPER] >1) cust_params[HAVE_BEEPER] = 1;
    if (cust_params[INTERVAL] <10) cust_params[INTERVAL] = 60;
*/
#ifdef FAKE
    cust_params[HAVE_DEHYD] = 0;
    cust_params[MAX_LRUN] = 6;
    cust_params[L_PAUSE] = 5;
#endif
  }
  delay(2000);

  OUT_SERLN("");
  OUT_SER(F("Prima Kellerklima "));
  OUT_SERLN(VERSION);

  OUT_SER(F("Entfeuchter angeschlossen: "));
  OUT_SERLN(cust_params[HAVE_DEHYD]);
  OUT_SER(F("Wifi Cloud-Upload aktiv: "));
  OUT_SERLN(cust_params[HAVE_WIFI]);
  OUT_SER(F("Beeper enabled: "));
  OUT_SERLN(cust_params[HAVE_BEEPER]);
  OUT_SER(F("Messintervall: "));
  OUT_SERLN(cust_params[INTERVAL]);

  OUT_SER(F("Max. Feuchte/Startwert: "));
  OUT_SER(cust_params[HUM_MAX]);
  OUT_SERLN("%");
  OUT_SER(F("Min. Innentemperatur: "));
  OUT_SER(cust_params[T_IN_MIN]);
  OUT_SERLN("*C");
  OUT_SER(F("Min. Aussentemperatur: "));
  OUT_SER(cust_params[T_OUT_MIN]);
  OUT_SERLN("*C");
  OUT_SER(F("Hysterese ein: "));
  OUT_SER(cust_params[HYSTERESIS_ON]);
  OUT_SERLN("*C");
  OUT_SER(F("Hysterese aus: "));
  OUT_SER(cust_params[HYSTERESIS_OFF]);
  OUT_SERLN("*C");
  OUT_SER(F("Max. Luefterlaufzeit: "));
  OUT_SER(cust_params[MAX_LRUN]);
  OUT_SERLN(F("Min."));
  //Serial.print(F("Max. Entfeuchterlaufzeit: "));
  //Serial.print(d_maxrun);
  //Serial.println("Min.");
  OUT_SER(F("Luefterzwangspause: "));
  OUT_SER(cust_params[L_PAUSE]);
  OUT_SERLN(F("Min."));
  OUT_SER(F("Free RAM is "));
  OUT_SERLN((int)freeRam);
//  OUT_SER(" / ");
//  OUT_SERLN((int)freeRam2);
  OUT_SERLN(F("----------------------------------------"));

  engine = new Menu::Engine(&Menu::NullItem);
  menuExit(Menu::actionNone); // reset to initial state

  // LCD
  //lcd.home();
  lcd.clear();
  FPL(OutTrigger);
  lcd.print(cust_params[HUM_MAX]);
  lcd.print("%");
  lcd.setCursor(0, 1);
  //         0123456789012345
  FPL(dTau);
  lcd.print(cust_params[HYSTERESIS_ON]);
  lcd.print("\337C / ");
  lcd.print(cust_params[HYSTERESIS_OFF]);
  lcd.print("\337C ");
  delay(2000);

  // initialize Sensors
#ifdef DHTPIN_I
  dht_i.begin();
  dht_o.begin();
#endif
#ifdef SENSI2C1
 bme_i.begin();
 bme_o.begin();
#endif

#ifdef HAVE_ESPLINK
 //esp.resetCb = resetCb;
 setup_wifi();
 #ifdef HAVE_ESPWEB
  setup_Init();
 #endif
#endif
}

void loop() { 
int16_t *iValue;

  curr_millis = millis();
/* not necessary
  if (prev_millis > curr_millis) {     // fix millis() wrap-around after ~50 days
      prev_millis = curr_millis;
      lcd_millis = curr_millis;
      fan_run_millis = curr_millis;
      fan_pause_millis = 0;
      DEBUG_PRINTLN(F("Timer: millis wrapped around!"));
  }
*/
  milliMil = (unsigned long) curr_millis - prev_millis;

  // handle encoder
  encMovement = Encoder.getValue();
  if (encMovement) {
    prev_millis = curr_millis;  // INTERVAL time for showing data
    encAbsolute += encMovement;
    switch (systemState) {
      case State::Settings:
        engine->navigate((encMovement > 0) ? engine->getNext() : engine->getPrev());
        updateMenu = true;
        break;

      // scroll through data to show
      case State::Default:
       if (encMovement > 0 && show_screen < MAX_SHOW_SCREENS) show_screen++;
       if (encMovement < 0 && show_screen >  0) show_screen--;
       update_screen(show_screen);
      break;
    }
  }

  // handle button
  switch (Encoder.getButton()) {
    case ClickEncoder::Clicked:
      trigger_backlight();
      switch (systemState) {
        // navigate menu structure
        case State::Settings:
          engine->invoke();
          updateMenu = true;
          break;

        // exit show data mode
        case State::Default:
          if (show_screen > 0) {
            show_screen = 0;
            update_screen(show_screen);
          }
          break;
        
        // save value
        case State::Edit:
          DEBUG_PRINTLN(F("Save changes!"));
          buzzer(80);
          write_to_eeprom();
          Encoder.setAccelerationEnabled(false);
          engine->navigate(&miSettings1);
          systemState = State::Settings;
          previousSystemState = systemState;
          updateMenu = true;
#ifdef HAVE_ESPLINK
          setup_wifi();
#else
          cust_params[HAVE_WIFI] = 0; // don't allow Wifi if not #defined
#endif
          break;

      }
      break;

    case ClickEncoder::DoubleClicked:
      trigger_backlight();
      // menu: goto one level up
      if (systemState == State::Settings) {
        DEBUG_PRINTLN(F("Doubleclick in Settings"));
        engine->navigate(engine->getParent());
        updateMenu = true;
      }
      // escaped from edit mode
      if (systemState == State::Edit) {
        DEBUG_PRINTLN(F("Doubleclick in Edit"));
        Encoder.setAccelerationEnabled(false);
        // Reset parameter to previous value
        getItemValuePointer(engine->currentItem, &iValue);
        *iValue = last_value;
        systemState = State::Settings;
        previousSystemState = systemState;
        engine->navigate(engine->currentItem);
        updateMenu = true;
      }
      break;

    case ClickEncoder::Held:
      trigger_backlight();
      // enter menu
      if (systemState != State::Settings) {
        Encoder.setAccelerationEnabled(false);
        engine->navigate(&miSettings1);
        systemState = State::Settings;
        previousSystemState = systemState;
        updateMenu = true;
      }
      break;
  }

  // ----------------------------------------------------
  // update value modifications while in edit mode
  //
  if (systemState == State::Edit) {
    if (engine->currentItem != &Menu::NullItem) {
      engine->executeCallbackAction(Menu::actionDisplay);      
    }
  }

  if (updateMenu) {
    updateMenu = false;
    if (!encMovement) { // clear menu on child/parent navigation
      lcd.clear();
    }

    // render the menu
    engine->render(renderMenuItem, LCD_LINES);
  }

  // "application"
  if (systemState == State::Default) {
    if (systemState != previousSystemState) {
      previousSystemState = systemState;
      encLastAbsolute = -999;
      show_screen = 0;
      update_screen(show_screen);
      //if (LCD_LINES > 2) {
      //  lcd.setCursor(0, LCD_LINES - 1);
      //  lcd.print("Dr\337cken");
      //}
    }

    // ------------------------------------------------------
    // "Mainscreen"
    //
    // Messintervall prüfen, Sensoren lesen, Werte anzeigen, Schaltbedingungen prüfen
    do_measure();

    // show activity animations 
    show_activity();            // heart beat + show active fan and/or dehum
    // ------------------------------------------------------
    
    // encoder was moved
    if (encAbsolute != encLastAbsolute) {
      encLastAbsolute = encAbsolute; 
      trigger_backlight();
    }

  }

  if (lcd_on && (curr_millis - lcd_millis) >= BACKLIGHT_OFF) {   // turn off LCD backlight 
    //lcd_on = false;
    LCD_OFF;
    if (systemState != State::Default) {
      systemState = State::Default;
      menuExit(Menu::actionNone); // reset to initial state
    }
  }
}

void do_measure(void) {
// Wait INTERVAL between measurements.
  if (milliMil >= ((unsigned long) cust_params[INTERVAL]*1000)) {
    prev_millis = curr_millis;
    show_screen = 0; // default; disable showing data

    // D13 activity LED
    digitalWrite(ActLED, true);

#ifdef DHTPIN_I
    // DHT22
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    hum_i = dht_i.readHumidity();
    hum_o = dht_o.readHumidity();
    // Read temperature as Celsius (the default)
    temp_i = dht_i.readTemperature();
    temp_o = dht_o.readTemperature();
#endif
#ifdef SENSI2C1
    // BME280
    hum_i = bme_i.getHumidity();
    hum_o = bme_o.getHumidity();
    temp_i = bme_i.getTemperature_C();
    temp_o = bme_o.getTemperature_C();
#endif

#ifdef FAKE
    hum_i = 62.0;
    hum_o = 55.0;
    temp_i = 15.0;
    temp_o = 12.0;
#endif

    lcd.clear();
    lcd.home();

    // Check if any reads failed and exit early (to try again).
    if (isnan(hum_i) || isnan(temp_i) ) {
      DEBUG_PRINTLN(F("Failed to read from indoor sensor (S1)!"));
      FPL(OutErrSensor);
      lcd.print("S1!");
      buzzer(0);
      return;  // skip this measure cycle
    }
    if (isnan(hum_o) || isnan(temp_o) ) {
      DEBUG_PRINTLN(F("Failed to read from outdoor sensor (2)!"));
      FPL(OutErrSensor);
      lcd.print("S2!");
      buzzer(0);
      return;  // skip this measure cycle
    }
    
    // Dewpoints
    dew_i = DewPoint(temp_i, hum_i);
    dew_o = DewPoint(temp_o, hum_o);

    lcd.print("S1:");
    lcdPrintDouble(temp_i,1,4);
    lcd.print("\337C ");
    lcd.print(hum_i,1);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("S2:");
    lcdPrintDouble(temp_o, 1, 4);
    lcd.print("\337C ");
    lcd.print(hum_o,1);
    lcd.print("%");

    // LED Indicator
    digitalWrite(ActLED, false);

    // Lüftersteuerung
    check_switch_rules(FAN, dew_i, dew_o); 
    // Entfeuchtersteuerung
    check_switch_rules(DEHYD, dew_i, dew_o);

#ifdef DEBUG
    sprint_report();
#endif

#ifdef HAVE_ESPLINK
 #ifndef DEBUG 
   // use serial output to esp-link to display measures also without debug enabled
   sprint_report();
 #endif
    // Wifi - send values
    if(cust_params[HAVE_WIFI]) { 
      uint8_t send_retries = 1;
      do {
       if (! wifi_send(send_retries)) {
         OUT_SER(F("REST request failed: "));
         OUT_SERLN(send_retries);
         lcd.setCursor(0, 1);
         FPL(ERR_REST);
         lcd.print(send_retries);
         delay(1000);
         esp.init();
         esp.Process();
         setup_wifi();
        } else {
         OUT_SER(F("REST request sent successfully: "));
         OUT_SERLN(send_retries);
         break; 
        }
      } while (send_retries++ <= SEND_MAX_RETRY);
     if (send_retries > SEND_MAX_RETRY) {
        //cust_params[HAVE_WIFI] = 0;
        // try re-sync ?
        sync_done = false;
     }
    } else {
      DEBUG_PRINTLN(F("Wifi not connected!"));
    }
#endif
  } // Ende Messinterval
}

// prüfe diverse Bedingungen für Lüfterstart
void check_switch_rules(uint8_t dev, uint8_t dewp_i, uint8_t dewp_o) {
bool do_stop = false;

  //if (control_override) return; // do not control devices when in manual mode [deactivated - test-menu should never timeout]
  
  // no dehumi configured, or no fan pause active => prevent DEHYD from starting
  if (dev == DEHYD && (!cust_params[HAVE_DEHYD] || fan_pause_millis == 0)) {  
    do_stop = true;
  }

  // check pause for device
  if (fan_pause_millis > 0 && ((unsigned long)(curr_millis - fan_pause_millis) >= (unsigned long) (cust_params[L_PAUSE] * 60000))) {
     // pause has been active, and max pause time has been reached
     fan_pause_millis = 0;  // resetting pause counter
     lcd.clear();
     FPL(OutPause); // Lüfterpause Ende
     FPL(MnuOff);
     lcd.setCursor(0, 1);
     if (is_dev_on[DEHYD]) {
      lcd.print(GNAME[dev]); // Dehum is active, switch off now
      lcd.print(" ");
      FPL(MnuOff);
      FPL(Blank4);
     }
     buzzer(200);
     delay(1000);
  } else if (fan_pause_millis > 0 && dev == FAN) {   // Pause geht weiter?
     // pause still active, update lcd if fan is used
     lcd.setCursor(0, 1);
     FPL(OutPause); // Lüfterpause XXm
     lcd.print((curr_millis - fan_pause_millis)/60000);
     lcd.print("m  ");
     //delay(1000);
     return;
  }

  // do not start any device if indoor humidity is too low; stop device if running
  // if (hum_i < cust_params[HUM_MAX] - cust_params[HYSTERESIS_OFF]) { // hysteresis needed here as well?
  if (hum_i < cust_params[HUM_MAX]) {              
    do_stop = true;
    if (is_dev_on[dev]) {
      lcd.clear();
      FPL(MSGOK);
      lcd.setCursor(0,1);
      lcd.print("S1: ");
      lcd.print(hum_i,1);
      lcd.print("% < ");
      lcd.print(cust_params[HUM_MAX]);
      lcd.print("%");
      buzzer(200);
      delay(1000);
    }
  }

  // switch off device when configured max runtime is reached = fan pause start ==> dehum start
  if (is_dev_on[dev] && ((unsigned long)(curr_millis - fan_run_millis) >= (unsigned long) (cust_params[MAX_LRUN] * 60000))) {   
     fan_pause_millis = curr_millis;   // Timer starten
     fan_run_millis = 0;               // Laufzeit resetten
     buzzer(200);
     do_stop = true;
  }

  // is indoor dewpoint higher than outdoors, incl. hysreresis? (dp_in min. 2° more than dp_out, when hyst_on = 2)
  if (!do_stop && (dewp_o <= (dewp_i - cust_params[HYSTERESIS_ON]) || dev == DEHYD)) { 
    // check minimal temperatures for fan 
    if (dev == DEHYD || (temp_i >= cust_params[T_IN_MIN] && temp_o >= cust_params[T_OUT_MIN])) { 
         if (is_dev_on[dev] == false) {      // Lüfter/Dehum Laufzeit startet
           set_relay(dev, true);             // Lüfter/Dehum einschalten
           fan_run_millis = curr_millis;     // Timer starten
           lcd.setCursor(0, 1);
           lcd.print(GNAME[dev]);
           FPL(MnuOn);
           FPL(Blank4);
         }
      } else if (dev != DEHYD && (temp_i < (cust_params[T_IN_MIN]-TEMP_HYSTERESIS) || temp_o < (cust_params[T_OUT_MIN]-TEMP_HYSTERESIS))) {  // Temperaturen zu niedrig
         if (is_dev_on[dev]) {
          do_stop = true;
          lcd.setCursor(0,1);
          if (temp_i < cust_params[T_IN_MIN]) {
           lcd.print("S1 ");
           FPL(MSGCOLD);
           lcd.print(temp_i,1);
          } else {
           lcd.print("S2 ");
           FPL(MSGCOLD);
           lcd.print(temp_o,1);
          }
         //delay(1000);
         }
      }
  } else if (dewp_o > (dewp_i - cust_params[HYSTERESIS_OFF])) { // switch off when dp_out only max. 1° less than dp_in (when hyst_off = 1)
         do_stop = true;
  }

  dev_stop(do_stop, dev);
}

// Gerät stoppen
void dev_stop(uint8_t is_stop, uint8_t mydev) {
  if (is_dev_on[mydev]) {
    lcd.setCursor(0, 1);
    if (is_stop) {
      set_relay(mydev, false);
      fan_run_millis = curr_millis;               // Timer reset
      //trigger_backlight();
      lcd.print(GNAME[mydev]);
      lcd.print(" ");
      FPL(MnuOff);
      FPL(Blank4);
    } else if (milliMil % ((unsigned long) (cust_params[INTERVAL] * 2000)) == 0) {  // show runtime every 2nd Interval
      FPL(RUNTIME);
      lcd.print((curr_millis - fan_run_millis)/60000);
      lcd.print("m");
    }
    //delay(1000);
  }
}  

// print data on LCD when rotating encoder
void update_screen(uint8_t screen) {
 lcd.clear();
 FPL(CURRDATA);
 lcd.setCursor(0,1);
 switch (screen) {
  case 0:
    //lcd.print("...warte auf");
    //lcd.setCursor(0,1);
    FPL(MEASRUN);
  case 1:
    FPL(DewS1);
    lcd.print(dew_i,1);
    lcd.print("\337C");
  break;
  case 2:
    FPL(DewS2);
    lcd.print(dew_o,1);
    lcd.print("\337C");
  break;
  case 3:
    FPL(StartFrom);
    lcd.print(cust_params[HUM_MAX]);
    lcd.print("%");
  break;
  case 4:
    FPL(HYSTON);
    lcd.print(cust_params[HYSTERESIS_ON]);
    lcd.print("\337C");
  break;
  case 5:
    FPL(HYSTOFF);
    lcd.print(cust_params[HYSTERESIS_OFF]);
    lcd.print("\337C");
  break;
  case 6:
    //FPL(DEVON);
    if (is_dev_on[FAN]) {
      //FPL(DEVFAN);
      lcd.print(GNAME[FAN]);
      FPL(MnuOn);
      lcd.print(" ");
      lcd.print((curr_millis - fan_run_millis)/60000);
      lcd.print("m");
    } else if (is_dev_on[DEHYD]) {
      //FPL(DEVDEHYD);
      lcd.print(FS(GNAME[DEHYD]));
      //FPL(MnuOn);
      lcd.print(" ");
      lcd.print((curr_millis - fan_pause_millis)/60000);
      lcd.print("m");
    } else {
      FPL(DEVON);
      FPL(MnuNo);
    }
  break;
  case 7:
    FPL(PauseFrom);
    if (fan_pause_millis > 0) {
      lcd.print((curr_millis - fan_pause_millis)/60000);
      lcd.print("m");
    } else {
      lcd.print("-");
    }
  break;
  case 8:
    FPL(MAXRUN);
    lcd.print(cust_params[MAX_LRUN]);
    lcd.print("m");
  break;
  case 9:
    FPL(PauseLen);
    lcd.print(cust_params[L_PAUSE]);
    lcd.print("m");
  break;
  case 10:
    FPL(MinTempSens);
    lcd.print("S1 ");
    lcd.print(cust_params[T_IN_MIN]);
    lcd.print("\337C");
  break;
  case 11:
    FPL(MinTempSens);
    lcd.print("S2 ");
    lcd.print(cust_params[T_OUT_MIN]);
    lcd.print("\337C");
  break;  
  case 12:
    lcd.print(GNAME[DEHYD]);
    if (cust_params[HAVE_DEHYD]) {
      FPL(MnuYes);  
    } else {
      FPL(MnuNo);
    }
  break;  
  case 13:
    FPL(WIFI_ON);
    if (cust_params[HAVE_WIFI]) {
      FPL(MnuYes);  
    } else {
      FPL(MnuNo);
    }
  break;  
  case 14:
    lcd.clear();
    FPL(TOTAL);
    FPL(RUNTIME);
    lcd.setCursor(0,1);
    //FPL(DEVFAN);
    lcd.print(GNAME[FAN]);
    lcd.print(" ");
    lcd.print(total_run[FAN]);
    lcd.print("m");
  break;  
  case 15:
    lcd.clear();
    FPL(TOTAL);
    FPL(RUNTIME);
    lcd.setCursor(0,1);
    //FPL(DEVDEHYD);
    lcd.print(GNAME[DEHYD]);
    lcd.print(" ");
    lcd.print(total_run[DEHYD]);
    lcd.print("m");
  break;  
 }
}

void show_activity(void) {
  if (curr_millis % AINTV == 0 && show_screen == 0 && systemState == State::Default) {
    act_symb = (act_symb == 0) ? 1 : 0;
    if (is_dev_on[FAN]) {
      lcd.setCursor(LCD_CHARS - 1, 0);
      lcd.write(IconFan[act_symb]);
    }
    if (is_dev_on[DEHYD]) {
      lcd.setCursor(LCD_CHARS - 1, 1);
      lcd.write(IconDeh[act_symb]);
    }
    if (!(is_dev_on[FAN] || is_dev_on[DEHYD])) {
      lcd.setCursor(LCD_CHARS - 1, 0);
      lcd.write(IconDef[act_symb]);
    }
#ifndef LCD_I2C
    delay(10);  // fix timing of parallel LCD 
#endif
  }
  // let LED flash for heart beat
  if (curr_millis % (AINTV * 4) == 0) {
    digitalWrite(ActLED, true);
    delay(80);
    digitalWrite(ActLED, false);
  }
}


// ***************** Taupunktberechnung ****************
// *      in Abhängigkeit von der relativen Feuchte   
// *              und der aktuellen Temperatur         
// *****************************************************
double DewPoint(double akt_temp, double humidity) 
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * akt_temp) / (b + akt_temp) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}


// backlight on, timer reset
void trigger_backlight(void) {
  if (lcd_on == false) {
    lcd_millis = curr_millis;  // light LCD
    //lcd_on = true;
    //lcd.backlight();
    LCD_ON;
  }
}

/*
float getAbsHumidity(float relhum, float temp) {
  double x = 13.233 * relhum;
  double y = 7.5 * temp;
  double z = 237 + temp;

  y = y / z;
  float m = pow(10,y);
  m = m / z;
  return m * x;
}
*/


