//
// most configurable stuff here...
//
#define VERSION  1.2

#define DHTPIN_I 10     // Pin Innensensor S1
#define DHTPIN_O 11     // Pin Aussensensor S2
//#define SENSI2C1 0x76 // use I2C sensors => BME280 breakouts
//#define SENSI2C2 0x77
#define Relais_L 2     // Pin Lüfter/Klappe
#define Relais_E 3     // Pin Entfeuchter
#define ActLED 13      // Pin LED (Messung)
#define AINTV 500      // Intervall für Aktivitätsanzeige
#define BACKLIGHT_OFF 180000 // switch off backlight (3min)

#define RelaysLowActive // trigger relays with low signal

// esp-link present?
#define HAVE_ESPLINK 1
#ifdef HAVE_ESPLINK
//#define HAVE_ESPWEB 1
 // Change to your own Thingspeak API key
 char *api_key = "X436YTGNCGV1SFG8";
 char *api_host = "184.106.153.149";
 #define BUFLEN 266
 #define SEND_MAX_RETRY 5
 bool sync_done = false;
#endif

// LCD
//#define LCD_I2C  // use I2C LCD - parallel otherwise
// I2C LCD config
#define LCD_ADDR 0x27
#define LCD_CHARS 16
#define LCD_LINES  2
// parallel LCD config
#define LCD_RS     4 
#define LCD_EN     5
#define LCD_D4     6   
#define LCD_D5     7   
#define LCD_D6     8   
#define LCD_D7     9   
#define LCD_LED    17
// LCD R/W pin to ground
// LCD VSS pin to ground
// LCD VCC pin to 5V
// 10K resistor ends to +5V and ground

#ifdef LCD_I2C
 #define LCD_ON lcd.backlight(); lcd_on = true;
 #define LCD_OFF lcd.noBacklight(); lcd_on = false;
#else
 #define LCD_ON lcd.display(); digitalWrite(LCD_LED, true); lcd_on = true;
 #define LCD_OFF lcd.noDisplay(); digitalWrite(LCD_LED, false); lcd_on = false;
#endif

#define FAN 0    // device index
#define DEHYD 1

#define SERIAL_BAUD 57600
#define EEPROM_ADDR 10  // start address for avr eeprom 

// equivalent to F() macro, also works with lcd.print()
#define FS(x) (__FlashStringHelper*)(x)   // return string from flash
#define FPL(x); lcd.print(FS(x));         // print flash string on LCD
#define FT(x,y);  const char x[] PROGMEM = {y};  // shorter flash string defs

// Sensortyp
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE_I DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE_O DHT22   // DHT 22  (AM2302), AM2321

// ClickEncoder 
// https://github.com/0xPIT/
//
#define r_button 16                //rotary encoder pushbutton, PB1
#define r_pha 15                   //rotary encoder phase A, PB2
#define r_phb 14                   //rotary encoder phase B, PB3


// define language
#define GERMAN
//#define ENGLISH


#ifdef GERMAN
const char *GNAME[] = {"L\365fter", "Entfeuchter"};
//           01234567890123456
FT(HEADER1,     "Kellerklima V");
FT(HEADER2,     "Automatisch");
FT(HEADER3,     "L\365ften und ");
FT(HEADER4,     "Entfeuchten! ");
FT(OutDefaults, " Setze Defaults ");
FT(OutLoaded,   " Daten geladen  ");
FT(OutTrigger,  "Schwellwert: ");
FT(OutErrSensor,"Error Sensor ");
//FT(OutErrS2,    "Error Sensor S2!");
FT(OutPause,    "L\365fterpause ");
FT(PauseFrom,   "Pause seit: ");
FT(OutStart,    "Start ");
FT(OutStop,     "Ende");
FT(MSGOK,       "Feuchte Ok!");
FT(MSGCOLD,     "zu kalt: ");
FT(RUNTIME,     "Laufzeit: ");
FT(CURRDATA,    "Aktuelle Daten:");
FT(MEASRUN,     "Messung l\341uft...");
FT(HYSTON,      "Hyster. ein: ");
FT(HYSTOFF,     "Hyster. aus: ");
//FT(DEVFAN,      "L\365fter ");
//FT(DEVDEHYD,    "Entfeuch. ");
FT(DEVON,       "Ger\341t an: ");
FT(MAXRUN,      "max. Laufz. ");
FT(PauseLen,    "Dauer Pause ");
FT(MinTempSens, "Min.Temp ");
FT(MnuYes,      "   Ja");
FT(MnuNo,       " Nein");
FT(DewS1,       "Tau S1: ");
FT(DewS2,       "Tau S2: ");
FT(StartFrom,   "Start ab: ");
FT(dTau,        "dTau: ");
FT(Blank4,      "      ");
FT(MnuEdit,     "Eingabe:        ");
FT(MnuOnOff,    "Schalten:       ");
FT(MnuOn,       " An");
FT(MnuOff,      "Aus");
FT(TOTAL,       "Gesamt-");
FT(WIFI_ON,     "Wifi an:");
FT(ERR_WIFI,    "No Wifi sync!   ");
FT(OK_WIFI,     "Wifi now synced!");
FT(ERR_REST,    "REST Error: ");
FT(OK_REST,     "REST Setup Ok! ");
#endif

#ifdef ENGLISH
const char *GNAME[] = {"Fan", "Dehydrator"};
                 //                    0123456789012345
FT(HEADER1,     "Climatrol V");
FT(HEADER2,     "Automatic  ");
FT(HEADER3,     "Ventilation + ");
FT(HEADER4,     "Dehydration! ");
FT(OutDefaults, " Set Defaults  ");
FT(OutLoaded,   " Load Data     ");
FT(OutTrigger,  "Triggervalue ");
FT(OutErrSensor,"Error Sensor ");
//FT(OutErrS2,    "Error Sensor S2");
FT(OutPause,    "Fan paused    ");
FT(PauseFrom,   "Paused for: ");
FT(OutStart,    "Start ");
FT(OutStop,     "End");
FT(MSGOK,       "Humidity Ok!");
FT(MSGCOLD,     "too cold: ");
FT(RUNTIME,     "Runtime: ");
FT(CURRDATA,    "Current Data:   ");
FT(MEASRUN,     "Cycle running...");
FT(HYSTON,      "Hyster. on: ");
FT(HYSTOFF,     "Hyster. off: ");
//FT(DEVFAN,      "Fan ");
//FT(DEVDEHYD,    "Dehydr.  ");
FT(DEVON,       "Device on: ");
FT(MAXRUN,      "max. Runt. ");
FT(PauseLen,    "Len. Pause ");
FT(MinTempSens, "Min.Temp ");
FT(MnuYes,      " Yes");
FT(MnuNo,       "  No");
FT(DewS1,       "Dew S1 ");
FT(DewS2,       "Dew S2 ");
FT(StartFrom,   "Starting: ");
FT(dTau,        "dDew: ");
FT(Blank4,      "      ");
FT(MnuEdit,     "Input:          ");
FT(MnuOnOff,    "Toggle:         ");
FT(MnuOn,       " On");
FT(MnuOff,      "Off");
FT(TOTAL,       "Total ");
FT(WIFI_ON,     "Wifi on:");
FT(ERR_WIFI,    "No Wifi sync!   ");
FT(OK_WIFI,     "Wifi now synced!");
FT(ERR_REST,    "REST Set. Err:  ");
FT(OK_REST,     "REST Setup Ok!  ");
#endif

//char buf[20]; // generic char buffer

#define MAX_SHOW_SCREENS 15 // number of LCD screens to update if rotating

int16_t encMovement;
int16_t encAbsolute;
int16_t encLastAbsolute = -1;
//bool lastEncoderAccelerationState = false;

ClickEncoder Encoder(r_pha, r_phb, r_button, 2); // 2,3,4 = clicks per step (notches)

// Pieper
#define BUZZER 12

uint8_t my_relays[] = {Relais_L, Relais_E};

// Messvars
unsigned long prev_millis = 0;
unsigned long curr_millis = 0;
unsigned long fan_run_millis = 0;
unsigned long fan_pause_millis = 0;
unsigned long lcd_millis = 0;
unsigned long milliMil = 0;

unsigned long total_run[] = {0, 0};
unsigned long dev_start = 0;

// Messwerte
float hum_i = 0;
float hum_o = 0;
float temp_i = 0;
float temp_o = 0;
float dew_i = 0;
float dew_o = 0;

static char is_dev_on[] = { false, false } ; // Lüfter/Entfeuchter aktiv
uint8_t act_symb = 0; // show activity symbol
uint8_t lcd_on = true;    // is backlight on?
//uint8_t control_override = false; // override controller by manual switch

//
// Kellerparameter
//
#define HUM_MAX        0 //starte Lüftung ab x%
#define T_IN_MIN       1 //kein Start unter x° innen
#define T_OUT_MIN      2 //kein Start unter x° aussen
#define HYSTERESIS_ON  3 //Einschalthysterese (Taupunktdiff.)
#define HYSTERESIS_OFF 4 //Ausschalthysterese (Taupunktdiff.)
#define HAVE_DEHYD     5 //Entfeuchter angeschlossen?
#define MAX_LRUN       6 //max. Lüfterlaufzeit in Minuten
#define L_PAUSE        7 //Lüfterpause in Minuten (Stop nach max. Laufzeiten ohne pos. Ergebnis)
#define HAVE_WIFI      8 //Wifi enabled?
#define HAVE_BEEPER    9 //Buzzer enabled?
#define INTERVAL      10 // Messintervall (seconds), default=60s

// NB: HUM_MAX-Hysteresis = hyst_off

// initial defaults - values of defined params above
int16_t cust_params[11] = {60, 12, 2,  3, 1, 0, 120, 120, 0, 1, 60};
int16_t last_value = 0; // menu item helper

#define TEMP_HYSTERESIS 1  // universal hysteresis to be used for temperature measures (min. inner/outer temp)

// Encoder drehen = Anzeige durchschalten
uint8_t show_screen = 0;

// custom chars
enum Icons {
  IconLeft = 0,
  IconRight = 1,
  IconBack = 2,
  IconBlock = 3,
  IconHeart = 4,
};

const byte IconFan[] = {0xA5, 0xDB};
const byte IconDeh[] = {0xDB, 0xFF};
const byte IconDef[] = {IconHeart, 0x14};

byte heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};

byte left[8] = {
  0b00010,
  0b00110,
  0b01110,
  0b11110,
  0b01110,
  0b00110,
  0b00010,
  0b00000
};

byte right[8] = {
  0b01000,
  0b01100,
  0b01110,
  0b01111,
  0b01110,
  0b01100,
  0b01000,
  0b00000
};

byte back[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01101,
  0b00101,
  0b00001,
  0b00000
};

byte block[8] = {
  0b00000,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b00000
};

//byte fan[8]   = { 0x03, 0x19, 0x16, 0x04, 0x05, 0x0B, 0x08, 0x0C};
//byte fan2[8] = { B00110, B01000, B01011, B00100, B11010, B00010, B01100, B00000 }; 

Menu::Engine *engine;

namespace State {  
  typedef enum SystemMode_e {
    None      = 0,
    Default   = (1<<0),
    Settings  = (1<<1),
    Edit      = (1<<2)
  } SystemMode;
};

uint8_t systemState = State::Default;
uint8_t previousSystemState = State::None;

bool updateMenu = false;

/*

I2C speed

long speed[] = { 50, 100, 200, 250, 400, 500, 800 };
TWBR = (F_CPU/(speed*1000) - 16)/2;

TWBR   prescaler   Frequency

 12       1       400   kHz  (the maximum supported frequency)
 32       1       200   kHz
 72       1       100   kHz  (the default)
152       1        50   kHz
 78       4        25   kHz
158       4        12.5 kHz


 */
