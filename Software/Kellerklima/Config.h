//
// most configurable stuff here...
//
#define VERSION  1.3

//#define DHTPIN_I 10     // Pin Innensensor S1
//#define DHTPIN_O 11     // Pin Aussensensor S2
//#define BMEI2C1 0x76 // use I2C sensors => BME280 breakouts
//#define BMEI2C2 0x77
#define SHTI2C1 0x44  // use I2C sensors => SHT31 breakouts
#define SHTI2C2 0x45
#define Relais_L 2     // Pin Lüfter/Klappe
#define Relais_E 3     // Pin Entfeuchter
#define ActLED 13      // Pin LED (Messung)

#define AINTV 500      // Intervall für Aktivitätsanzeige

//#define RelaysLowActive 1 // trigger relays with low signal (PCB1.2: do not define)

// use esp-link?
//#define HAVE_ESPLINK true
#ifdef HAVE_ESPLINK
 // Change to your own Thingspeak API key
 char *api_key = "X436YTGNCGV1SFG8";
 char *api_host = "184.106.153.149";
 #define BUFLEN 266
 #define SEND_MAX_RETRY 4	// retry of REST requests 
 //bool sync_done = false;    // status of communication with ESP-link 
 bool wifiConnected = false;  // status of communication with ESP-link
#endif

// LCD
#define LCD_CHARS 16
#define LCD_LINES  2
//#define LCD_I2C  // use I2C LCD - parallel otherwise
// I2C LCD config
#define LCD_ADDR 0x27
// parallel LCD config
#define LCD_RS     8 
#define LCD_EN     9
#define LCD_D4     7   
#define LCD_D5     6   
#define LCD_D6     5   
#define LCD_D7     4   
#define LCD_LED    17    // pin to switch backlight
// LCD R/W pin to ground
// LCD VSS pin to ground
// LCD VCC pin to 5V
// 10K resistor ends to +5V and ground

#define BACKLIGHT_OFF 180000 // switch off backlight (3min)
#ifdef LCD_I2C
 #define LCD_ON lcd.backlight(); lcd_on = true;
 #define LCD_OFF lcd.noBacklight(); lcd_on = false;
#else
 #define LCD_ON lcd.display(); digitalWrite(LCD_LED, true); lcd_on = true;
 #define LCD_OFF lcd.noDisplay(); digitalWrite(LCD_LED, false); lcd_on = false;
#endif

// ClickEncoder 
// https://github.com/0xPIT/
//
#define r_button 16                //rotary encoder pushbutton, PB1
#define r_pha 15                   //rotary encoder phase A, PB2
#define r_phb 14                   //rotary encoder phase B, PB3

int16_t encMovement;
int16_t encAbsolute;
int16_t encLastAbsolute = -1;
//bool lastEncoderAccelerationState = false;

ClickEncoder Encoder(r_pha, r_phb, r_button, 2); // 2,3,4 = clicks per step (notches)

// Pieper
#define BUZZER 12 // pin for buzzer
//#define TONE_ON digitalWrite(BUZZER, true) 
//#define TONE_OFF digitalWrite(BUZZER, false) 
//passive buzzer - only when on PWM pin! (1, 9, 10, 12, 13) (PCB 1.2)
#define TONE_ON tone(BUZZER, 1200)
#define TONE_OFF noTone(BUZZER)

#define SERIAL_BAUD 57600
#define EEPROM_ADDR 10  // start address for avr eeprom 

// equivalent to F() macro, also works with lcd.print()
#define FS(x) (__FlashStringHelper*)(x)   // return string from flash
#define FPL(x); lcd.print(FS(x));         // print flash string on LCD
#define FT(x,y);  const char x[] PROGMEM = {y};  // shorter flash string defs

// Sensortyp
//#define DHTTYPE_I DHT11   // DHT 11
//#define DHTTYPE_O DHT11   // DHT 21 (AM2301)
#define DHTTYPE_I DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE_O DHT22   // DHT 22  (AM2302), AM2321

// define language
#define GERMAN
//#define ENGLISH

#define FAN 0    // device index
#define DEHYD 1

#define TEMP_HYSTERESIS 1  // universal hysteresis to be used for temperature measures (min. inner/outer temp)

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

#ifdef GERMAN
const PROGMEM char cmd_0[] = {"r1"};
const PROGMEM char cmd_1[] = {"r2"};
const PROGMEM char cmd_2[] = {"hum"};
const PROGMEM char cmd_3[] = {"tempi"};
const PROGMEM char cmd_4[] = {"tempo"};
const PROGMEM char cmd_5[] = {"max"};
const PROGMEM char cmd_6[] = {"int"};
const PROGMEM char cmd_7[] = {"hysthum"};
const PROGMEM char cmd_8[] = {"dtauon"};
const PROGMEM char cmd_9[] = {"dtauoff"};
const PROGMEM char cmd_10[] = {"save"};
const PROGMEM char cmd_11[] = {"hilfe"};

const PROGMEM char hlp_0[] = {"Luefter r1=1: an | r1=0: aus"};
const PROGMEM char hlp_1[] = {"Entfeuchter r2=1: an | r2=0: aus"};
const PROGMEM char hlp_2[] = {"Feuchte-Schwellwert (%)"};
const PROGMEM char hlp_3[] = {"Mindest-Temperatur 'Innen'"};
const PROGMEM char hlp_4[] = {"Mindest-Temperatur 'Aussen'"};
const PROGMEM char hlp_5[] = {"maximale Geraetelaufzeit pro 24h (Min.)"};
const PROGMEM char hlp_6[] = {"Messintervall (Sek.)"};
const PROGMEM char hlp_7[] = {"Hysterese Feuchteschwellwert (%)"};
const PROGMEM char hlp_8[] = {"Taupunktdifferenz ein (K)"};
const PROGMEM char hlp_9[] = {"Taupunktdifferenz aus (K)"};
const PROGMEM char hlp_10[] = {"Speichern aller Parameter"};
const PROGMEM char hlp_11[] = {""};
#endif

const char *const CMDS[] PROGMEM = {cmd_0,cmd_1,cmd_2,cmd_3,cmd_4,cmd_5,cmd_6,cmd_7,cmd_8,cmd_9,cmd_10,cmd_11};
const char *const HELP[] PROGMEM = {hlp_0,hlp_1,hlp_2,hlp_3,hlp_4,hlp_5,hlp_6,hlp_7,hlp_8,hlp_9,hlp_10,hlp_11};

#ifdef GERMAN
const char *GNAME[] = {"L\365fter", "Entfeuchter"};
//               01234567890123456
FT(HEADER1,     "Kellerklima V");
FT(HEADER2,     "Automatisch");
FT(HEADER3,     "L\365ften und ");
FT(HEADER4,     "Entfeuchten! ");
FT(OutDefaults, " Setze Defaults ");
FT(OutLoaded,   " Daten geladen  ");
FT(OutTrigger,  "Schwellwert: ");
FT(OutErrSensor,"Fehler Sensor ");
FT(OutPause,    "L\365fterpause ");
FT(PauseFrom,   "Pause seit: ");
FT(OutStart,    "Start ");
FT(OutStop,     "Ende");
FT(MSGOK,       "Feuchte Ok!");
FT(MSGCOLD,     "zu kalt: ");
FT(RUNTIME,     "Laufzeit: ");
FT(CURRDATA,    "Aktuelle Daten:");
FT(MEASRUN,     "Messung l\341uft...");
FT(HYSTHUM,      "Hyster. Feuchte: ");
FT(DTAUDIFF1,   "dTau ein: ");
FT(DTAUDIFF0,   "dTau aus: ");
FT(DEVON,       "Ger\341t an: ");
FT(MAXRUN,      "max. Laufz. ");
FT(PauseLen,    "Dauer Pause ");
FT(MinTempSens, "Min.Temp ");
FT(MnuYes,      "   Ja");
FT(MnuNo,       " Nein");
FT(DewS1,       "Tau S1: ");
FT(DewS2,       "Tau S2: ");
FT(TempS1,      "Innentemp: ");
FT(TempS2,      "Aussentemp: ");
FT(StartFrom,   "Start ab: ");
FT(dTau,        "dTau: ");
FT(Blank4,      "      ");
FT(MnuEdit,     "Eingabe:        ");
FT(MnuOnOff,    "Schalten:       ");
FT(MnuOn,       " An");
FT(MnuOff,      "Aus");
FT(TOTAL,       "Gesamt-");
FT(DAILY,       "Tages-");
FT(WIFI_ON,     "Wifi an:");
FT(ERR_WIFI,    "No Wifi sync!   ");
FT(OK_WIFI,     "Wifi now synced!");
FT(ERR_REST,    "REST Error: ");
FT(OK_REST,     "REST Setup Ok!   ");
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
FT(HYSTHUM,     "Hyster. hum.: ");
FT(DTAUDIFF1,   "dDewp. on: ");
FT(DTAUDIFF0,   "dDewp. off: ");
FT(DEVON,       "Device on: ");
FT(MAXRUN,      "max. Runt. ");
FT(PauseLen,    "Len. Pause ");
FT(MinTempSens, "Min.Temp ");
FT(MnuYes,      " Yes");
FT(MnuNo,       "  No");
FT(DewS1,       "Dew S1 ");
FT(DewS2,       "Dew S2 ");
FT(TempS1,      "Indoor T: ");
FT(TempS2,      "Outdoor T: ");
FT(StartFrom,   "Starting: ");
FT(dTau,        "dDew: ");
FT(Blank4,      "      ");
FT(MnuEdit,     "Input:          ");
FT(MnuOnOff,    "Toggle:         ");
FT(MnuOn,       " On");
FT(MnuOff,      "Off");
FT(TOTAL,       "Total ");
FT(DAILY,       "Daily ");
FT(WIFI_ON,     "Wifi on:");
FT(ERR_WIFI,    "No Wifi sync!   ");
FT(OK_WIFI,     "Wifi now synced!");
FT(ERR_REST,    "REST Set. Err:  ");
FT(OK_REST,     "REST Setup Ok!  ");
#endif

//char buf[20]; // generic char buffer

#define MAX_SHOW_SCREENS 18 // number of LCD screens to update if rotating

uint8_t my_relays[] = {Relais_L, Relais_E};

// Messvars
unsigned long twentyfour_millis = 0;    // 24h-Periodenzähler f. Zurücksetzen der Tageslaufzeiten
unsigned long prev_millis = 0;
unsigned long dev_run_millis = 0;       // aktuelle Laufzeit Gerät
unsigned long fan_pause_millis = 0;     // Timer Pause für Lüfter nach max. Laufzeit
unsigned long lcd_millis = 0;
unsigned long milliMil = 0;
#define DECIMALS 2
#define TWENTYFOUR 86400000

unsigned long total_run[] = {0, 0};
unsigned long daily_run[] = {0, 0};
unsigned long dev_start = 0;

// Messwerte
float hum_i = 0;
float hum_o = 0;
float temp_i = 0;
float temp_o = 0;
float dew_i = 0;
float dew_o = 0;
//float dew_i2 = 0;
//float dew_o2 = 0;

static char is_dev_on[] = { false, false } ; // Lüfter/Entfeuchter aktiv
uint8_t act_symb = 0; // show activity symbol
uint8_t lcd_on = false;    // is backlight on?
uint8_t control_override = false; // override controller by manual switch

//
// Kellerparameter
//
#define HUM_MAX           0 //starte Lüftung ab x%
#define T_IN_MIN          1 //kein Start unter x° innen
#define T_OUT_MIN         2 //kein Start unter x° aussen
#define HYSTERESIS_HUM    3 //Hysterese Feuchte % (Abschaltbedingung)
#define TAUPUNKTDIFF_ON   4 //obere Taupunktdifferenz K
#define HAVE_DEHYD        5 //Entfeuchter angeschlossen?
#define MAX_LRUN          6 //max. Lüfterlaufzeit in Minuten
#define L_PAUSE           7 //Lüfterpause in Minuten (Stop nach max. Laufzeiten ohne pos. Ergebnis)
#define HAVE_WIFI         8 //Wifi enabled?
#define HAVE_BEEPER       9 //Buzzer enabled?
#define INTERVAL         10 //Messintervall (seconds), default=60s
#define MAX_PER_24H      11 //max. Lüfterlaufzeit pro 24h (min.)
#define TAUPUNKTDIFF_OFF 12 //untere Taupunktdifferenz K
// for now: use TAUPUNKTDIFF/2 as hysteresis

/* source1: https://www.schwille.de/wp-content/uploads/100-720-Taupunkt-L%C3%BCftungssteuerung.pdf
   Die Steuerung arbeitet nur nach der eingestellten Taupunktdifferenz (absolute Feuchtigkeit). Die Werkseinstellung ist bei 5°C Taupunkt. 
   Dies ist ein Erfahrungswert über lange Jahre hin, um die Entfeuchtung zu ge-währleisten und die Ereignisse, in denen die äußeren Bedingungen 
   dafür geeignet sind. Sie können aber im Prog 12 die Differenz auch verändern von 2°C bis 9 °C Tp. Wenn Sie die Differenz verkleinern, also 
   gegen 2 gehen lassen, wird sehr oft gelüftet, jedoch kaum noch entfeuchtet. Dies kann natürlich für die Belüftung von Werkstätten oder Büros 
   sehr nützlich sein. Umgekehrt, erhöhen Sie die Taupunktdifferenz gegen 9 °C wird mehr entfeuchtet, jedoch werden die Ereignisse, dass die 
   Bedingungen dazu passen, eher seltener  
   
   Die  Kellerluft  wird  durch  die  Querlüftung  relativ  schnell  mit  trockener  Außenluft  ausgetauscht, ohne dass sich die Wände abkühlen. 
   Danach stoppt die Steuerung (bei Taupunkt < 1°C) und wartet erneut auf die eingestellten Lüftungsbedingungen. 
 */

/* source2: https://www.keller-doktor.de/wcms/ftp//k/keller-doktor.de/uploads/kellerlueftung_10-72_beschreibung.pdf
   Der Lüfter jedoch ist taupunktgesteuert und arbeitet nur, wenn der Taupunkt am Außensensor (rechteAnzeige) um 2 – 9 °C niedriger ist, 
   als der Taupunkt am Innensensor. Der Lüfter stoppt wenn dieTaupunkt-Differenz am Außensensor kleiner als 1 °C beträgt (Hysterese = 4 °C). 
*/


// NB: HUM_MAX-Hysteresis = hyst_off


// initial defaults
int16_t cust_params[13] = {60, 12, 4,  2, 3, 0, 120, 120, 0, 1, 60, 360, 1};
int16_t last_value = 0;

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
