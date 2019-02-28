// Lüftungssteuerung V1.2
// (c)2016-19 jleg99@gmail.com
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
//   - SHT31 sensors
//   - I2C-LCD 16x2
//   - parallel LCD 16x2
//   - ESP-01 with esp-link 3.0.14 (using 32Mbit flash)
// prepared for (untested):
//   - LCDs of other sizes
//   - BME280 sensors
//

#include <avr/pgmspace.h>
#include <Wire.h>
#include <ClickEncoder.h>                   // https://github.com/0xPIT/encoder
#include <TimerOne.h>                       // https://github.com/PaulStoffregen/TimerOne
#include <Menu.h>                           // https://github.com/0xPIT/menu
#include <EEPROM.h>

#include "Config.h"

#if defined DHTPIN_I || defined DHTPIN_O
 #include "DHT.h"
#endif
#if defined BMEI2C1 || defined BMEI2C2
 #include "cactus_io_BME280_I2C.h";         // http://cactus.io/hookups/sensors/barometric/bme280/hookup-arduino-to-bme280-barometric-pressure-sensor
#endif
#if defined SHTI2C1 || defined SHTI2C2
 #include "cactus_io_SHT31.h";              // http://cactus.io/hookups/sensors/temperature-humidity/sht31/hookup-arduino-to-sensirion-SHT31-temp-humidity-sensor
#endif

#if defined BMEI2C1 || defined SHTI2C1
 #define I2C_SENSOR1 1
#endif
#if defined BMEI2C2 || defined SHTI2C2
 #define I2C_SENSOR2 1
#endif


// I2C LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
#ifdef LCD_I2C
 #include <LiquidCrystal_I2C.h>             // https://bitbucket.org/fmalpartida/new-liquidcrystal/overview (old: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library)
 LiquidCrystal_I2C lcd(LCD_ADDR, LCD_LED, POSITIVE); //, LCD_CHARS, LCD_LINES);
#else
 #include <LiquidCrystal.h>
 LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif

#include "Debug.h"
#include "Tools.h"

// esp-link connected
#ifdef HAVE_ESPLINK
 #include <ELClient.h>
 #include <ELClientRest.h>
 // Initialize a connection to esp-link using the normal hardware serial port both for
 // SLIP and for debug messages.
#ifdef DEBUG
 ELClient esp(&Serial, &Serial);
#else
 // ELClient esp(&Serial); // suspected to crash el-client?
 ELClient esp(&Serial, &Serial);
#endif
 // Initialize a REST client on the connection to esp-link
 ELClientRest rest(&esp);
#endif


// ä \341
// ü \365
// ö \357
// ° \337

#ifdef DHTPIN_I
 DHT dht_i(DHTPIN_I, DHTTYPE_I);
#endif
#ifdef DHTPIN_O
 DHT dht_o(DHTPIN_O, DHTTYPE_O);
#endif
#ifdef BMEI2C1
 BME280_I2C sens_i(BMEI2C1); 
#endif
#ifdef BMEI2C2
 BME280_I2C sens_o(BMEI2C2); 
#endif
#ifdef SHTI2C1
 cactus_io_SHT31 sens_i(SHTI2C1); 
#endif
#ifdef SHTI2C2
 cactus_io_SHT31 sens_o(SHTI2C2); 
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
#endif

void setup() {
  pinMode(ActLED, OUTPUT);
  pinMode(Relais_L, OUTPUT);
  pinMode(Relais_E, OUTPUT);
#ifdef BUZZER  
 pinMode(BUZZER, OUTPUT);
#endif

  // Alles aus
//  set_relay(0, false);              // Lüfter ausschalten
//  set_relay(1, false);              // Entf. ausschalten

  Timer1.initialize(4000);  // 1000000 = 1s
  Timer1.attachInterrupt(timerIsr);

  OUT_SERIAL(SERIAL_BAUD); // initialize only for if DEBUG or HAVE_ESPLINK

  // LCD
#ifdef LCD_I2C
  lcd.begin(LCD_CHARS, LCD_LINES);
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
  trigger_backlight();
  
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
  int16_t val = word(EEPROM.read(EEPROM_ADDR), EEPROM.read(EEPROM_ADDR+1));
  //if (word(EEPROM.read(EEPROM_ADDR), EEPROM.read(EEPROM_ADDR+1)) < 10 || word(EEPROM.read(EEPROM_ADDR), EEPROM.read(EEPROM_ADDR+1)) > 100 || (b != ClickEncoder::Open)) 
  if (val < 10 || val>100 || (b != ClickEncoder::Open))
  {
    write_to_eeprom();
    FPL(OutDefaults);
  } else {
    // load settings from EEPROM
    read_from_eeprom();
    FPL(OutLoaded);
#ifndef HAVE_ESPLINK
    cust_params[HAVE_WIFI] = 0;  // override eeprom in case of changed ESP-link config
#endif

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

  sprint_report();
 
  engine = new Menu::Engine(&Menu::NullItem);
  //menuExit(Menu::actionNone); // reset to initial state

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
#endif
#ifdef DHTPIN_O
  dht_o.begin();
#endif
#ifdef I2C_SENSOR1
 if (!sens_i.begin()) {
      DEBUG_PRINTLN(F("Failed to read from indoor sensor (S1)!"));
      FPL(OutErrSensor);
      lcd.print("S1!");
      buzzer(0);
 }
#endif
#ifdef I2C_SENSOR2
 if (!sens_o.begin()) {
      DEBUG_PRINTLN(F("Failed to read from outdoor sensor (S2)!"));
      FPL(OutErrSensor);
      lcd.print("S2!");
      buzzer(0);
 }
 // calibration
 //sens_i.setTempCal(-1);// Temp was reading high so subtract 1 degree
 //sens_o.setTempCal(-1);// Temp was reading high so subtract 1 degree
#endif

#ifdef HAVE_ESPLINK
 setup_wifi();
#endif

  OUT_SERLN(F("---------- END OF SETUP PHASE ----------"));
  delay(2000);
}

void loop() { 
int16_t *iValue;
//uint32_t last_days;

#ifdef HAVE_ESPLINK
// process ESP-link events
  if(cust_params[HAVE_WIFI]) {   
    esp.Process();
  }
#endif

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
  milliMil = curr_millis - prev_millis;

  if ( ((millis()/60000) % 60 == 0) && (((millis()/60000)/60 % 24) == 0)) { // reset daily runtime counters
    daily_run[FAN] = 0;
    daily_run[DEHYD] = 0;
  }
  
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

  if (lcd_on && ((curr_millis - lcd_millis) >= BACKLIGHT_OFF)) {   // turn off LCD backlight 
    LCD_OFF;
    if (systemState != State::Default) {
      systemState = State::Default;
      menuExit(Menu::actionNone); // reset to initial state
    }
  }
#ifdef SERIAL_OUT
 if (Serial.available()) {
  String cmd = Serial.readString(); // read the incoming command
  cmd.trim();
  int16_t val = get_value(cmd);
  uint8_t c = get_cmd(cmd);

  if (c == FAN || c == DEHYD) control_override = val; //switch commands; set/reset override (val= 0/1)

  switch (c) {
    case FAN:
     set_relay(FAN, val);
    break;
    case DEHYD:
     set_relay(DEHYD, val);
    break;
    case 2:
     cust_params[HUM_MAX] = val;
    break;
    case 3:
     cust_params[T_IN_MIN] = val;
    break;
    case 4:
     cust_params[T_OUT_MIN] = val;
    break;
    case 5:
     cust_params[MAX_PER_24H] = val;
    break;
    case 6:
     cust_params[INTERVAL] = val;
    break;
    case 7:
     write_to_eeprom();
    break;
    case 8:
     out_help();
    break;
  }

  if (c >= 0 && c < 9) {
    Serial.print("\n");
    Serial.print(get_help(c));
    if (val != -1000) {
      Serial.print(": ");
      Serial.print(val);
    }
    Serial.print("\n");
  } else {
    Serial.println(F("\nBefehl unbekannt"));
  }

 }
#endif

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
    hum_i  = dht_i.readHumidity();
    temp_i = dht_i.readTemperature();
#endif
#ifdef DHTPIN_O
    hum_o  = dht_o.readHumidity();
    temp_o = dht_o.readTemperature();
#endif
#ifdef I2C_SENSOR1 
  // SHT31, BME280
  // BME280 also needs:
  #ifdef BMEI2C1
   sens_i.readSensor();
  #endif
  hum_i = sens_i.getHumidity();
  temp_i = sens_i.getTemperature_C();
#endif
#ifdef I2C_SENSOR2
  #ifdef BMEI2C2
   sens_o.readSensor();
  #endif
  hum_o = sens_o.getHumidity();
  temp_o = sens_o.getTemperature_C();
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
      DEBUG_PRINTLN(F("Failed to read from outdoor sensor (S2)!"));
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

#if defined DEBUG || defined SERIAL_OUT
    sprint_report();
#endif

#ifdef HAVE_ESPLINK
    // Wifi - send values
    if(cust_params[HAVE_WIFI] && wifiConnected) { 
      uint8_t send_retries = 1;
      do {
       if (! wifi_send()) {
         OUT_SER(F("REST request failed: "));
         OUT_SERLN(send_retries);
         //lcd.setCursor(0, 1);
         //FPL(ERR_REST);
         //lcd.print(send_retries);
         delay(1000);
         esp.init();
         esp.Process();
         //esp.Sync();
         //rest.begin(api_host);
         setup_wifi();
        } else {
         OUT_SER(F("REST request sent successfully: "));
         OUT_SERLN(send_retries);
         break; 
        }
      } while (send_retries++ <= SEND_MAX_RETRY);
     if (send_retries > SEND_MAX_RETRY) {
        wifiConnected = false;
        OUT_SERLN(F("REST failure, skipped."));
     }
    } else {
      if (!cust_params[HAVE_WIFI]) {
        DEBUG_PRINTLN(F("Wifi not enabled!"));
      } else {
        DEBUG_PRINTLN(F("Wifi not working, retrying..."));
		    setup_wifi();
        //esp.init();
        //wifi_send();  // TEST...

      }
    }
#endif
  } // Ende Messinterval
}

// prüfe diverse Bedingungen für Lüfterstart
void check_switch_rules(uint8_t dev, uint8_t dewp_i, uint8_t dewp_o) {
bool do_stop = false;
//uint32_t runtime = 0;
//uint32_t daytime = 0;

  if (control_override) return; // do not control devices when in manual mode 
  
  // check pause for device
  if (fan_pause_millis > 0 && ((curr_millis - fan_pause_millis) >= (unsigned long) (cust_params[L_PAUSE] * 60000))) {
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

  // no dehumi configured, or no fan pause active => prevent DEHYD from starting
  if (dev == DEHYD && (!cust_params[HAVE_DEHYD] || fan_pause_millis == 0)) {  
    do_stop = true;
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
  if (is_dev_on[dev] && ((curr_millis - fan_run_millis) >= (unsigned long) (cust_params[MAX_LRUN] * 60000))  ) {   
//  if (is_dev_on[dev] && (runtime >= (unsigned long) (cust_params[MAX_LRUN] * 60000)) || (is_dev_on[dev] && ((total_run[dev]+(runtime/60000)) > cust_params[MAX_PER_24H]))) {   
     do_stop = true;
     fan_pause_millis = curr_millis;   // Timer starten
     fan_run_millis = 0;               // Laufzeit resetten
     buzzer(200);
  }

  if (is_dev_on[dev] && (daily_run[dev] < cust_params[MAX_PER_24H])) {
     daily_run[dev] += (millis() - dev_start) / 60000; // sum daily runtime for 24h-limit
  } else if (is_dev_on[dev] && (daily_run[dev] >= cust_params[MAX_PER_24H])) {
     do_stop = true;  // daily limit reached!
     lcd.setCursor(0, 1);
     FPL(MAXRUN); // max. Laufzeit
     lcd.print("err.");
     buzzer(200);
     delay(1000);
  }

  // is indoor dewpoint higher than outdoors, incl. hysreresis? (dp_in min. 2° more than dp_out, when hyst_on = 2)
  if (!do_stop && (dewp_o <= (dewp_i - cust_params[HYSTERESIS_ON]) || dev == DEHYD)) { 
    // check minimal temperatures for fan 
    if (dev == DEHYD || (temp_i >= cust_params[T_IN_MIN] && temp_o >= cust_params[T_OUT_MIN])) { 
         if (is_dev_on[dev] == false) {      // Lüfter/Dehum Laufzeit startet
           set_relay(dev, true);             // Lüfter/Dehum einschalten
           //fan_run_millis = curr_millis;     // Timer starten
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
//char curs[] = {"-","\","|","/"};
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
    OUT_SER(".");
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
  if (!lcd_on) {
    lcd_millis = curr_millis;  // light LCD
    LCD_ON;
    DEBUG_PRINTLN(F("LCD_ON triggered!"));
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
