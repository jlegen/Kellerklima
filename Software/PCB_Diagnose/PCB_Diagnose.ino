
#define DHTPIN_I 10     // Pin Innensensor S1
#define DHTPIN_O 11     // Pin Aussensensor S2
//#define SENSI2C1 0x76 // use I2C sensors => BME280 breakouts
//#define SENSI2C2 0x77
//#define SHTI2C1 0x44 // use I2C sensors => SHT31 breakouts
//#define SHTI2C2 0x45

//#define TWI_FREQ 10000L;
#include <Wire.h>
#include <ClickEncoder.h>                   // https://github.com/0xPIT/encoder
#include <TimerOne.h>                       // https://github.com/PaulStoffregen/TimerOne

#ifdef DHTPIN_I
 //#include "cactus_io_DHT22.h";
 #include "DHT.h"                           
#endif
#ifdef SENSI2C1
 #include "cactus_io_BME280_I2C.h";
 #define I2C_SENSOR 1         
#endif
#ifdef SHTI2C1
 #include "cactus_io_SHT31.h";         
 #define I2C_SENSOR 1         
#endif

#define Relais_L 2     // Pin Lüfter/Klappe
#define Relais_E 3     // Pin Entfeuchter
#define ActLED 13      // Pin LED (Messung)

uint8_t my_relays[] = {Relais_L, Relais_E};

//#define RelaysLowActive // trigger relays with low signal


// LCD
//#define LCD_I2C  // use I2C LCD - parallel otherwise
// I2C LCD config
#define LCD_ADDR 0x27
#define LCD_CHARS 16
#define LCD_LINES  2
// parallel LCD config
#define LCD_RS     8 
#define LCD_EN     9
#define LCD_D4     7   
#define LCD_D5     6   
#define LCD_D6     5   
#define LCD_D7     4   
#define LCD_LED    17
// LCD R/W pin to ground
// LCD VSS pin to ground
// LCD VCC pin to 5V
// 10K resistor ends to +5V and ground

#ifdef LCD_I2C
 #define LCD_ON lcd.backlight(); 
 #define LCD_OFF lcd.noBacklight(); 
#else
 #define LCD_ON lcd.display(); digitalWrite(LCD_LED, true); 
 #define LCD_OFF lcd.noDisplay(); digitalWrite(LCD_LED, false); 
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


// Sensortyp
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE_I DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE_O DHT22   // DHT 22  (AM2302), AM2321


#ifdef DHTPIN_I
 DHT sens_i(DHTPIN_I, DHTTYPE_I);
 DHT sens_o(DHTPIN_O, DHTTYPE_O);
 //DHT22 sens_i(DHTPIN_I);
 //DHT22 sens_o(DHTPIN_O);
#endif
#ifdef SENSI2C1
 BME280_I2C sens_i(SENSI2C1); 
 BME280_I2C sens_o(SENSI2C2); 
#endif
#ifdef SHTI2C1
 cactus_io_SHT31 sens_i(SHTI2C1); 
 cactus_io_SHT31 sens_o(SHTI2C2); 
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
unsigned long prev_millis = 0;
unsigned long curr_millis = 0;
unsigned long milliMil = 0;

float hum_i = 0;
float hum_o = 0;
float temp_i = 0;
float temp_o = 0;

ClickEncoder Encoder(r_pha, r_phb, r_button, 2); // 2,3,4 = clicks per step (notches)

// Pieper
#define BUZZER 12        // pin for buzzer
//#define TONE_ON digitalWrite(BUZZER, true) 
//#define TONE_OFF digitalWrite(BUZZER, false) 
//passive buzzer - only when on PWM pin! (1, 9, 10, 12, 13)
#define TONE_ON tone(BUZZER, 1200)
#define TONE_OFF noTone(BUZZER)

// custom chars
enum Icons {
  IconLeft = 0,
  IconRight = 1,
  IconBack = 2,
  IconBlock = 3,
  IconHeart = 4,
};

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

void timerIsr()
{
  Encoder.service();
}

// Relais schalten
void set_relay(uint8_t relay, bool trig) {
#ifdef RelaysLowActive
  digitalWrite(my_relays[relay], !trig);  // set = true => relais = LOW
#else
  digitalWrite(my_relays[relay], trig);  // set = true => relais = LOW
#endif
}

void buzzer(uint16_t dura) {
#ifdef BUZZER
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
#endif
}

void lcdPrintDouble(double val, uint8_t precision = 1, uint8_t lpad=0) {
char tmp[6];
  dtostrf(val,lpad,precision,tmp);
  lcd.print(tmp);
}

void setup() {
  pinMode(ActLED, OUTPUT);
  pinMode(Relais_L, OUTPUT);
  pinMode(Relais_E, OUTPUT);
#ifdef BUZZER  
 pinMode(BUZZER, OUTPUT);
#endif

  Wire.begin(); // I2C-Scanner

  //Wire.setClock(400000L) 
  //Wire.setClock(100000L) // default
  //Wire.setClock(10000L) 

  //    long CPU_FREQ = 12000000L;
  //    TWBR = ((CPU_FREQ / 100000L) - 16) / 2; // 100KB/s default

  // Alles aus
  set_relay(0, false);              // Lüfter ausschalten
  set_relay(1, false);              // Entf. ausschalten

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

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
  lcd.setCursor(0, 0);
  lcd.print("LCD ");
  lcd.setCursor(4, 0);
  lcd.write(IconHeart);
  lcd.write(IconLeft);
  lcd.write(IconRight);
  lcd.write(IconBack);
  lcd.write(IconBlock);

  lcd.setCursor(0, 1);
  lcd.print("Umlaute \365 \341 \357 \337");
  buzzer(150);
  delay(3000);

 Serial.begin(57600);

  // initialize Sensors
#ifdef DHTPIN_I
  sens_i.begin();
  sens_o.begin();
#endif
#ifdef I2C_SENSOR
 if (!sens_i.begin()) {
  Serial.println("Error I2C for sens_i!");
 }
 if (!sens_o.begin()) {
  Serial.println("Error I2C for sens_o!");
 }
#endif

 //lcd.clear();
}

void loop() {
  byte i2c_error, i2c_address;
  int i2c_nDevices;
 
  curr_millis = millis();
  milliMil = (unsigned long) curr_millis - prev_millis;

  if (milliMil >= (unsigned long)3000 ) {
    prev_millis = curr_millis;
    digitalWrite(ActLED, true);
#ifdef DHTPIN_I
    // DHT22
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    hum_i = sens_i.readHumidity();
    hum_o = sens_o.readHumidity();
    // Read temperature as Celsius (the default)
    temp_i = sens_i.readTemperature();
    temp_o = sens_o.readTemperature();
    
    /*
    sens_i.readHumidity();
    sens_o.readHumidity();
    sens_i.readTemperature();
    sens_o.readTemperature();
    hum_i = sens_i.humidity;
    hum_o = sens_o.humidity;
    temp_i = sens_i.temperature_C;
    temp_o = sens_o.temperature_C;
    */
#endif
#ifdef SENSI2C1
    // BME280, SHT31
    // setTempCal(float) Allows you to define a temp calibration offset if it reads high
    hum_i = sens_i.getHumidity();
    hum_o = sens_o.getHumidity();
    temp_i = sens_i.getTemperature_C();
    temp_o = sens_o.getTemperature_C();
#endif
    digitalWrite(ActLED, false);

    Serial.print("Sensor 1 Temp: ");
    Serial.println(temp_i);
    Serial.print("Sensor 1 Hum: ");
    Serial.println(hum_i);
    Serial.print("Sensor 2 Temp: ");
    Serial.println(temp_o);
    Serial.print("Sensor 2 Hum: ");
    Serial.println(hum_o);

    /*lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(temp_i);
    lcd.print(" ");
    lcd.print(hum_i);
    lcd.setCursor(0, 1);
    lcd.print(temp_o);
    lcd.print(" ");
    lcd.print(hum_o);
*/
  }

  encMovement = Encoder.getValue();
  if (encMovement) {
    prev_millis = curr_millis;  // INTERVAL time for showing data
    encAbsolute += encMovement;
    lcd.setCursor(0, 1);
    lcd.print("Encoder: ");
    //lcd.setCursor(10,1);
    lcd.print(encAbsolute);
    lcd.print(" ");
  }
  
  // handle button
  switch (Encoder.getButton()) {
    case ClickEncoder::Clicked:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Beeping Buzzer!");
    buzzer(40);
    break;

    case ClickEncoder::Held:
    lcd.clear();
    set_relay(0, true);
    lcd.setCursor(0, 0);
    lcd.print("Relay 1 On!");
    delay(1000);
    set_relay(0, false);
    lcd.setCursor(0, 0);
    lcd.print("Relay 1 Off!");
    delay(1000);
    set_relay(1, true);
    lcd.setCursor(0, 1);
    lcd.print("Relay 2 On!");
    delay(1000);
    set_relay(1, false);
    lcd.setCursor(0, 1);
    lcd.print("Relay 2 Off!");
    break;

    case ClickEncoder::DoubleClicked:
    lcd.clear();
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.print("S1: ");
    lcd.print(hum_i,1);
    lcd.print(" ");
    lcdPrintDouble(temp_i, 1, 4);
    lcd.setCursor(0, 1);
    lcd.print("S2: ");
    lcd.print(hum_o,1);
    lcd.print(" ");
    lcdPrintDouble(temp_o, 1, 4);

    break;

  }

i2c_nDevices =0 ;

if (milliMil >= (unsigned long)3000 ) {
  for(i2c_address = 1; i2c_address < 127; i2c_address++ ) 
  {
    Wire.beginTransmission(i2c_address);
    i2c_error = Wire.endTransmission();

    if (i2c_error == 0)
    {
      //Serial.print("I2C device found at address 0x");
      lcd.clear();
      lcd.print("I2C device: ");
      if (i2c_address<16) 
        lcd.print("0");
      lcd.print(i2c_address,HEX);
      lcd.print(" ");

      i2c_nDevices++;
    }
    else if (i2c_error==4) 
    {
      lcd.print("error at 0x");
      if (i2c_address<16) 
        lcd.print("0");
      lcd.print(i2c_address,HEX);
    }    
  }
  if (i2c_nDevices == 0) {
    lcd.print("No I2C devices found");
  } else {
    lcd.print("No. I2C devices: ");
    lcd.print(i2c_nDevices);
  }
}


}
