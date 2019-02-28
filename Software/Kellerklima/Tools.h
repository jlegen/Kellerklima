// 
// Helpers and Tools
//

// ---------------------- Menu ---------------------------------

/*
    actionNone    = 0,
    actionLabel   = (1<<0), // render label when user scrolls through menu items
    actionDisplay = (1<<1), // display menu, after user selected a menu item
    actionTrigger = (1<<2), // trigger was pressed while menue was already active
    actionParent  = (1<<3), // before moving to parent, useful for e.g. "save y/n?" or autosave
    actionCustom  = (1<<7)  

 */
/*
void menuCallback(menuAction_t action) {  
  if (action == Menu::actionDisplay) {
    // initialy entering this menu item
  }

  if (action == Menu::actionTrigger) {
    // click on already active item
  }

  if (action == Menu::actionLabel) {
    // show thy label but don't do anything yet
  }

  if (action == Menu::actionParent) { 
    // navigating to self->parent
  }
}
 */

extern const Menu::Item_t miSettings1,miSettings2,miSetting3,miSL1,miSL2,miSL3,miSL4,miSL5,miSW1,miSW2,miSW3,miSW4,miSW5,miSW6,miSW7,miBack,miT1,miT2,miExit;
void set_relay(uint8_t relay, bool trig);
void trigger_backlight(void);

void getItemValuePointer(const Menu::Item_t *mi, int16_t **i) {
  if (mi == &miSL1)         *i = &cust_params[MAX_LRUN];
  if (mi == &miSL2)         *i = &cust_params[L_PAUSE];
  if (mi == &miSL3)         *i = &cust_params[HAVE_DEHYD];
  if (mi == &miSL4)         *i = &cust_params[HAVE_WIFI];
  if (mi == &miSL5)         *i = &cust_params[HAVE_BEEPER];
  if (mi == &miSW1)         *i = &cust_params[HUM_MAX];
  if (mi == &miSW2)         *i = &cust_params[T_IN_MIN];
  if (mi == &miSW3)         *i = &cust_params[T_OUT_MIN];
  if (mi == &miSW4)         *i = &cust_params[HYSTERESIS_ON];
  if (mi == &miSW5)         *i = &cust_params[HYSTERESIS_OFF];
  if (mi == &miSW6)         *i = &cust_params[INTERVAL];
  if (mi == &miSW7)         *i = &cust_params[MAX_PER_24H];
}

// format LCD output value, no. decimals, length of number, padded with " "
void lcdPrintDouble(double val, uint8_t precision = 1, uint8_t lpad=0) {
char tmp[6];
  dtostrf(val,lpad,precision,tmp);
  lcd.print(tmp);
}

// ----------------- Menu -----------------
void renderMenuItem(const Menu::Item_t *mi, uint8_t pos) {
  lcd.setCursor(0, pos);
  // cursor
  if (engine->currentItem == mi) {
    lcd.write((uint8_t)IconBlock);
  }
  else {
    lcd.write(20); // space
  }
  lcd.print(engine->getLabel(mi));
  // mark items that have children
  if (engine->getChild(mi) != &Menu::NullItem) {
    lcd.write(20);
    lcd.write((uint8_t)IconRight);
  }
  lcd.print("         ");
}

// leave menu mode
bool menuExit(const Menu::Action_t a) {
  Encoder.setAccelerationEnabled(false);  
  systemState = State::Default;
  DEBUG_PRINTLN(F("Exit menu!"));
  return true;
}

bool menuDummy(const Menu::Action_t a) {
#ifdef DEBUG
  if (a == Menu::actionTrigger) {
    DEBUG_PRINTLN(F("Dummy triggered!"));
  }
  if (a == Menu::actionDisplay) {
    DEBUG_PRINTLN(F("Dummy clicked!"));
  }
#endif
  return true;
}

// "back" = up one level
bool menuBack(const Menu::Action_t a) {
  if (a == Menu::actionDisplay) {
    //engine->navigate(engine->getParent(engine->getParent()));
    engine->navigate(engine->getParent()); 
  }
  return true;
}

// Testmenu: switch devices on/off
bool menuOnOff(const Menu::Action_t a) {
uint8_t device = FAN;
  if (a == Menu::actionDisplay) {
    bool initial = (systemState != State::Edit);
    systemState = State::Edit;
    if (initial) {
      lcd.setCursor(0,1);
      FPL(MnuOnOff);
    }
    if (engine->currentItem == &miT2) device = DEHYD; 
    lcd.setCursor(10,1);
    if (encMovement > 0) {
      FPL(MnuOn); 
      set_relay(device, true);
      control_override = true;
    } else if (encMovement < 0) {
      FPL(MnuOff); 
      set_relay(device, false);
      control_override = false;
    }
    updateMenu = false;
  }
  return true;
}

bool menueditNumericalValue(const Menu::Action_t action) { 
int16_t factor = 1,tmp;
int16_t  *iValue = NULL;
char *buf = "";
bool is_bool = (engine->currentItem == &miSL3 || engine->currentItem == &miSL4 || engine->currentItem == &miSL5);
bool is_temp = (engine->currentItem == &miSW2 || engine->currentItem == &miSW3 || engine->currentItem == &miSW4 || engine->currentItem == &miSW5);

  if (action == Menu::actionDisplay) {
    bool initial = (systemState != State::Edit);
    systemState = State::Edit;

    if (initial) {
      Encoder.setAccelerationEnabled(true);
      lcd.setCursor(0,1);
      FPL(MnuEdit);
    }

    // get var containing value of current menu item
    getItemValuePointer(engine->currentItem, &iValue);

    // Hack - limit inputs somewhat
    //(engine->currentItem == &miSW2 || engine->currentItem == &miSW3 || engine->currentItem == &miSW4 || engine->currentItem == &miSW5);
    if (is_temp) {
      buf = "\337C";
      if (encAbsolute > 60) {
        encAbsolute = 60;
      } else if (encAbsolute < -30) {
        encAbsolute = -30;
      }
    }
    if (engine->currentItem == &miSW1) {
      buf = "%";
      if (encAbsolute > 99) {
        encAbsolute=99;
      } else if (encAbsolute < 0) {
        encAbsolute = 0;
      }
    }

    // Laufzeiten
    if (engine->currentItem == &miSL1 || engine->currentItem == &miSL2 || engine->currentItem == &miSW7) {
      buf="min";
      if (encAbsolute > 10080) { // 7 Tage
        encAbsolute = 10080;
      } else if (encAbsolute < 10) {
        encAbsolute = 10;
      }
    }

    // Messintervall
    if (engine->currentItem == &miSW6) {
      buf="sec";
      if (encAbsolute > 7200) { // 2h
        encAbsolute = 7200;
      } else if (encAbsolute < 5) {
        encAbsolute = 5;
      }
    }

    // Ein/aus = 0/1
    //if (engine->currentItem == &miSL3 || engine->currentItem == &miSL4 || engine->currentItem == &miSL5) is_bool = true;
    if (is_bool) {
      if (encMovement > 0) {
        encAbsolute = 1;
      } else if (encMovement < 0) {
        encAbsolute = 0;
      }
    }

    // first call
    if (initial) {
      tmp = *iValue;
      last_value = tmp;
      tmp *= factor;
      encAbsolute = (int16_t)tmp;
    } else {
      tmp = encAbsolute;
      tmp /= factor;
      *iValue = tmp;
    }      

    lcd.setCursor(9,1);
    
    // Hack: on/off bool
    if (is_bool) {
      if (tmp <= 0) {
        FPL(MnuNo); //"Nein";
      } else {
        FPL(MnuYes); //"  Ja";
      }
      buf = "";
    } else {
      lcdPrintDouble(tmp,0,3);
    }
    lcd.print(buf);
    
    updateMenu = false;

    //getItemValueLabel(Engine.currentItem, buf);
    //buf = engine->getLabel();
    //lcd.print(engine->getLabel());
  }

/*  
  if (action == Menu::actionParent || action == Menu::actionTrigger) {
    //clearLastMenuItemRenderState();
    updateMenu = true;
    engine->lastInvokedItem = &Menu::NullItem;
    menuExit(Menu::actionNone);
    Serial.println("actionParent?");

    if (systemState == State::Edit) { // leave edit mode, return to menu
      Serial.println("Exiting with or wihtout saving?");
      systemState = State::Settings;
      Encoder.setAccelerationEnabled(false);
      return false;
    }

    return true;
  }
*/
}


// ----------------------------------------------------------------------------
// Name, Label, Next, Previous, Parent, Child, Callback
#ifdef GERMAN
MenuItem(miExit, "Setup beenden...", Menu::NullItem, Menu::NullItem, Menu::NullItem, miSettings1, menuExit);

MenuItem(miSettings1, "Setup Ger\341te",miSettings2,    Menu::NullItem, miExit, miSL1, menuDummy);
  MenuItem(miSL1,    "max. Laufzeit",   miSL2,          Menu::NullItem, miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL2,    "Dauer der Pause", miSL3,          miSL1,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL3,    "Entfeuchter J/N", miSL4,          miSL2,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL4,    "Wifi        J/N", miSL5,          miSL3,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL5,    "Beeper      J/N", miSLBack,       miSL4,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSLBack, "Zur\365ck \02",   Menu::NullItem, miSL5,          miSettings1, Menu::NullItem, menuBack);

MenuItem(miSettings2, "Setup Daten",   miSettings3,    miSettings1,    miExit,      miSW1,          menuDummy);
  MenuItem(miSW1,    "Schwellwert rH%",miSW2,          Menu::NullItem, miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW2,    "Mindesttemp. S1",miSW3,          miSW1,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW3,    "Mindesttemp. S2",miSW4,          miSW2,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW4,    "Hysterese Ein",  miSW5,          miSW3,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW5,    "Hysterese Aus",  miSW6,          miSW4,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW6,    "Messintervall",  miSW7,          miSW5,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW7,    "Laufzeit/Tag",   miSWBack,       miSW6,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSWBack, "Zur\365ck \02",  Menu::NullItem, miSW7,          miSettings2, Menu::NullItem, menuBack);

//MenuItem(miSettings3, "Schalten",      miBack,         miSettings2,    miExit,      miT1,          menuDummy);
MenuItem(miSettings3, "Schalten",      Menu::NullItem, miSettings2,    miExit,      miT1,          menuDummy);
  MenuItem(miT1,      "L\365fter",     miT2,           Menu::NullItem, miSettings3, Menu::NullItem, menuOnOff);
  MenuItem(miT2,      "Entfeuchter",   miTBack,        miT1,           miSettings3, Menu::NullItem, menuOnOff);
  MenuItem(miTBack,   "Zur\365ck \02", Menu::NullItem, miT2,           miSettings3, Menu::NullItem, menuBack);

//MenuItem(miBack, "Zur\365ck \02",      Menu::NullItem, miSettings2,    miExit,      Menu::NullItem, menuBack);
#endif

#ifdef ENGLISH
MenuItem(miExit, "Exit Setup...", Menu::NullItem, Menu::NullItem, Menu::NullItem, miSettings1, menuExit);

MenuItem(miSettings1, "Setup Devices",miSettings2,    Menu::NullItem, miExit, miSL1, menuDummy);
  MenuItem(miSL1,    "max. Runtime",    miSL2,          Menu::NullItem, miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL2,    "Length of pause", miSL3,          miSL1,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL3,    "Dehydrator Y/N",  miSL4,          miSL2,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL4,    "Wifi       Y/N",  miSL5,          miSL3,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSL5,    "Beeper     Y/N",  miSLBack,       miSL4,          miSettings1, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSLBack, "Back \02",        Menu::NullItem, miSL5,          miSettings1, Menu::NullItem, menuBack);

MenuItem(miSettings2, "Setup Params",   miSettings3,    miSettings1,    miExit,      miSW1,          menuDummy);
  MenuItem(miSW1,    "Trigger rH%",     miSW2,          Menu::NullItem, miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW2,    "min. temp. S1",   miSW3,          miSW1,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW3,    "min. temp. S2",   miSW4,          miSW2,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW4,    "Hysteresis On",   miSW5,          miSW3,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW5,    "Hysteresis Off",  miSW6,          miSW4,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW6,    "Meas. interval",  miSW7,          miSW5,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSW7,    "Runtime/Day",     miSWBack,       miSW6,          miSettings2, Menu::NullItem, menueditNumericalValue);
  MenuItem(miSWBack, "Back \02",        Menu::NullItem, miSW7,          miSettings2, Menu::NullItem, menuBack);

MenuItem(miSettings3, "Switch on/off",  miBack,           miSettings2,       miExit,      miT1,          menuDummy);
  MenuItem(miT1,      "Fan",            miT2,             Menu::NullItem,    miSettings3, Menu::NullItem, menuOnOff);
  MenuItem(miT2,      "Dehydrator",     miTBack,          miT1,              miSettings3, Menu::NullItem, menuOnOff);
  MenuItem(miTBack,   "Back \02",       Menu::NullItem,   miT2,              miSettings3, Menu::NullItem, menuBack);

MenuItem(miBack, "Back \02",   Menu::NullItem,    miSettings2,       miExit,      Menu::NullItem, menuBack);
#endif
// ----------------------------------------------------------------------------

void buzzer(uint16_t dura) {
 trigger_backlight(); // alerting display
#ifdef BUZZER
 if (cust_params[HAVE_BEEPER]) {
  if (dura == 0) { // 0 = Alert
    while ((millis() - curr_millis) <= 3000) {
     TONE_ON;
     delay(400);
     TONE_OFF;
     delay(400);
    }
  } else {
    TONE_ON;
    delay(dura);
    TONE_OFF;
    delay(dura);
    TONE_ON;
    delay(dura);
    TONE_OFF;
  }
 }
#endif
}

/*
void buzz_alarm(uint16_t dura) {
 trigger_backlight(); // alerting display
#ifdef BUZZER
 if (cust_params[HAVE_BEEPER]) {
  while ((millis() - curr_millis) <= dura) {
    digitalWrite(BUZZER, true);
    delay(400);
    digitalWrite(BUZZER, false);
    delay(400);
  }
 }
#endif
}
*/

// Relais schalten
void set_relay(uint8_t relay, bool trig) {
#ifdef RelaysLowActive
  digitalWrite(my_relays[relay], !trig);  // set = true => relais = LOW
#else
  digitalWrite(my_relays[relay], trig);  // set = true => relais = HIGH
#endif
 if (trig && !is_dev_on[relay]) {
  dev_start = millis();                     // store start time
  fan_run_millis = dev_start;
 } else if (is_dev_on[relay] && !trig) {  
  total_run[relay] += (millis() - dev_start) / 60000; 
  //daily_run[relay] += (millis() - dev_start) / 60000; // sum daily runtime for 24h-limit
 }
 is_dev_on[relay] = trig;
}

void write_to_eeprom(void) {
 int len = ARRAY_SIZE(cust_params); //sizeof(cust_params) / sizeof(int16_t);
 for (int i=0; i<len; i++) {
   EEPROM.update(EEPROM_ADDR + (i*2),     highByte(cust_params[i]));
   EEPROM.update(EEPROM_ADDR + ((i*2)+1), lowByte(cust_params[i]));
 }
}

void read_from_eeprom(void) {
 int len = ARRAY_SIZE(cust_params); //sizeof(cust_params) / sizeof(int16_t);
 for (int i=0; i<len; i++) {
   byte hi = EEPROM.read(EEPROM_ADDR + (i*2));
   byte lo = EEPROM.read(EEPROM_ADDR + ((i*2)+1));
   cust_params[i] = word(hi, lo);
 }
}

size_t freeRam ()
{
  return RAMEND - size_t (__malloc_heap_start);
} // end of freeRam

/*
int freeRam2 (void) {
    extern int __heap_start, *__brkval;
    int v=(int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return v;
}
*/

// serial output of measurements - either with DEBUG on, or ESP-LINK enabled!
void sprint_report(void) {
#if defined DEBUG || defined SERIAL_OUT 
  uint32_t rtime =0;
  OUT_SERLN(F("\n----------------"));
  OUT_SER(F("Innen S1:  "));
  OUT_SER(F("Feuchte: "));
  OUT_SER(hum_i);
  OUT_SER(F("%\t Temp: "));
  OUT_SER(temp_i);
  OUT_SER(F("°C\t Taupunkt: "));
  OUT_SERLN(dew_i);

  OUT_SER(F("Aussen S2: "));
  OUT_SER(F("Feuchte: "));
  OUT_SER(hum_o);
  OUT_SER(F("%\t Temp: "));
  OUT_SER(temp_o);
  OUT_SER(F("°C\t Taupunkt: "));
  OUT_SERLN(dew_o);

  OUT_SER(F("Entfeuchter angeschl.: "));
  OUT_SER(cust_params[HAVE_DEHYD]);
  OUT_SER(F(" | Wifi an: "));
  OUT_SERLN(cust_params[HAVE_WIFI]);
  OUT_SER(F("Messintervall: "));
  OUT_SER(cust_params[INTERVAL]);
  OUT_SER(F("s | Hysterese ein: "));
  OUT_SER(cust_params[HYSTERESIS_ON]);
  OUT_SER(F("°K | aus: "));
  OUT_SER(cust_params[HYSTERESIS_OFF]);
  OUT_SERLN("°K");
  OUT_SER(F("max. Luefterlaufzeit: "));
  OUT_SER(cust_params[MAX_LRUN]);
  OUT_SER(F("min. | Luefterpause: "));
  OUT_SER(cust_params[L_PAUSE]);
  OUT_SERLN(F("min."));

  OUT_SER(F("Schwellwert: "));
  OUT_SER(cust_params[HUM_MAX]);
  OUT_SER(F("% | MinTemp. Innen: "));
  OUT_SER(cust_params[T_IN_MIN]);
  OUT_SER(F("°C | MinTemp. Aussen: "));
  OUT_SER(cust_params[T_OUT_MIN]);
  OUT_SERLN("°C");

  const PROGMEM char *DEV[] = {"Luefter         ", "Entfeuchter     "};
  for (int i=0; i<2; i++) {    
      Serial.print(DEV[i]);
      rtime=0;
      if (is_dev_on[i]) {
        Serial.print(F("Geraet ist AN! "));
        Serial.print(F("Laufzeit: "));
        rtime = ((millis() - fan_run_millis)/60000);
        Serial.print(rtime);
        Serial.print(F(" min. | "));
      }      
      Serial.print(F("Gesamt: "));
      Serial.print(total_run[i] + rtime);
      Serial.println(F(" min."));
  }

  DEBUG_PRINT(F("Switch on [Dewpoint_S2 <= (Dewpoint_S1 - Hysteresis_on)]: "));
  DEBUG_PRINT(dew_o);
  DEBUG_PRINT(F(" <= "));
  DEBUG_PRINT(dew_i - cust_params[HYSTERESIS_ON]);
  DEBUG_PRINTLN("?");
  DEBUG_PRINT(F("Switch off [Dewpoint_S2 > (Dewpoint_S1 - Hysteresis_off)]: "));
  DEBUG_PRINT(dew_o);
  DEBUG_PRINT(F(" > "));
  DEBUG_PRINT(dew_i - cust_params[HYSTERESIS_OFF]);
  DEBUG_PRINTLN("?");

  OUT_SER(F("Gesamtbetriebszeit: "));
  rtime = (millis()/60000)/60/24;
  OUT_SER(rtime);
  OUT_SER(F("d "));
  rtime = (millis()/60000/60) - (rtime * 24);
  OUT_SER(rtime);
  OUT_SER(F("h "));
  rtime = (millis()/60000 % 60) ;
  OUT_SER(rtime);
  OUT_SER(F("m | max. Laufzeit/Tag: "));
  OUT_SER(cust_params[MAX_PER_24H]);
  OUT_SER(F("m | RAM: "));
  OUT_SERLN((int)freeRam);

  DEBUG_PRINT(F("curr_millis: "));
  DEBUG_PRINT(curr_millis);
  DEBUG_PRINT(" (");
  DEBUG_PRINT(curr_millis/1000/60);
  DEBUG_PRINTLN("m)");
  DEBUG_PRINT(F("LCD_ON: "));
  DEBUG_PRINTLN(lcd_on);
  DEBUG_PRINT(F("LCD_TIME: "));
  DEBUG_PRINTLN(curr_millis - lcd_millis);
    
  OUT_SERLN(F("\t+-+-+-+ 'hilfe' eingeben fuer Befehlsuebersicht +-+-+-+"));
#endif
}

// serial command input (testing, remote control)
uint8_t get_cmd(String arg) {
 String cmd = arg.substring(0, arg.indexOf("="));
 char buf[6];
 for (int i=0; i<(ARRAY_SIZE(CMDS)); i++) {
  strcpy_P(buf, (char *)pgm_read_word(&(CMDS[i])));
  if (cmd == buf) {
    return i;
    break;
  }
 }
 return(-1);
}

int16_t get_value(String arg) {
  String str = arg.substring(arg.indexOf("=")+1, arg.length());
  return ((str == arg) ? -1000 : str.toInt());
/*  if (str == NULL) {
    return(-1000);  // hack: empty string
  } else {
   return(str.toInt());
  }*/
}

String get_help(int i) {
 char buf[50];
 strcpy_P(buf, (char *)pgm_read_word(&(HELP[i])));
 return(buf);
}

void out_help(void) {
 char buf[6];
 Serial.println(F("\nBefehle:"));
 for (int i=0; i<(ARRAY_SIZE(CMDS)-1); i++) { 
   strcpy_P(buf, (char *)pgm_read_word(&(CMDS[i])));
   Serial.print(buf);
   Serial.print(": \t");
   Serial.println(get_help(i));
 }
}


/*
void lcd_message(char* m1, char* m2) {
  lcd.clear();
  lcd.print(m1);
  lcd.setCursor(0,1);
  lcd.print(m2);
}

const char PROGMEM format[] = "Running %08u ms";
char buffer[64];
void setup() 
{ 
   Serial.begin(9600);
}
void loop() 
{
  sprintf_P(buffer,format,millis());
  Serial.println(buffer);
  delay(997);
}

uint8_t countDigits(uint32_t n) {
  uint8_t d = 1;
  switch (n) {
    case  100000000 ... 999999999:  d++;
    case   10000000 ... 99999999:   d++;
    case    1000000 ... 9999999:    d++;
    case     100000 ... 999999:     d++;
    case      10000 ... 99999:      d++;
    case       1000 ... 9999:       d++;
    case        100 ... 999:        d++;
    case         10 ... 99:         d++;
  }
  return d;
}

void alignRightPrefix(uint16_t v) {
  if (v < 1e2) lcd.print(' '); 
  if (v < 1e1) lcd.print(' ');
}

float roundFloat(float number, int decimals) {
  float rounded_float = round(number * pow(10, decimals)) / pow(10, decimals);
  return rounded_float; 
}
*/
