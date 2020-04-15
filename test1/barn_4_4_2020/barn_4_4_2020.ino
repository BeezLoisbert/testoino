
// *********************************************************************
// includes
// *********************************************************************
#include <Wire.h> // I2C
#include <LiquidCrystal_I2C.h>  //Display
#include <LCDMenuLib2.h>  //Menü
#include <DS1302.h>   //RealTimeClock  
#include <EEPROM.h>    //Eeprom
#include <Encoder.h>  //Clickencoder Menüverwaltung
#include <TimerOne.h> //Interrupt
#include <ClickEncoder.h> //Clickencoder Werteverstellung
#include <OneWire.h> //1-Draht Sensoren
#include <DallasTemperature.h>  //Temperatursensor DS18B20
#include <ErriezBH1750.h> //Luxsensor BH1750
#include <PID_v1.h> //PID Regler
#include <Adafruit_MCP4725.h> // DAC MCP4725

// *********************************************************************
// LCDML display settings
// *********************************************************************
// settings for LCD
#define _LCDML_DISP_cols  20
#define _LCDML_DISP_rows  4

#define _LCDML_DISP_cfg_cursor     0x7E   // cursor Symbol
#define _LCDML_DISP_cfg_scrollbar  1      // enable a scrollbar

#define LCD_ENTRYRIGHT 

LiquidCrystal_I2C lcd(0x27, _LCDML_DISP_cols, _LCDML_DISP_rows);

const uint8_t scroll_bar[5][8] = 
  {
  {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
  {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1
  {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
  {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
  {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  };

// *********************************************************************
// ClickEncoder settings
// *********************************************************************

//Encoder für Wertverstellung
#define enc_PinA 7          // Dreh-Enkoder "A" an Pin 7
#define enc_PinB 2          // Dreh-Enkoder "B" an Pin 2
#define enc_PinC 3          // Dreh-Enkoder "Click" an Pin 3
#define enc_Step 4          // Dreh-Enkoder (Werte pro Dreh-Schritt / je nach Dreh-Enkoder anpassen)

#define DAC_RESOLUTION    (8) // Set this value to 9, 8, 7, 6 or 5  DAC-Auflösung
// *********************************************************************
// Temperatursensor settings
// *********************************************************************
#define DS18B20MODEL 0x28
OneWire  tempsens(4);  // on pin 4 (a 4.7K resistor is necessary) Temperatursensor

// *********************************************************************
// Prototypes
// *********************************************************************
void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();

// *********************************************************************
// Global variables
// *********************************************************************

int set_screensaverTime = EEPROM.get(20, set_screensaverTime);   // den Wert aus EEPROM lesen

// *********************************************************************
//Clickencoder für Werteverstellung
// *********************************************************************
double encValue = 0;           // Wert, der mit dem Dreh-Enkoder geaendert wird
double ValueStep = 1;          // Schrittweite fuer den Enkoder-Wert (wird durch Buttonklick auf einen der folgenden Werte geaendert)
const int steps[4] = {1, 10, 100, 1000}; // die Werte fuer die Schrittweite
double lastValue = 0;          // Variable zum speichern des letzten Wertes, damit die LCD-Anzeige nur bei einer Aenderung aktualisiert wird
bool ValueChanged = false;  // Variable zum merken, ob eine Aenderung vorgenommen wurde (um den Wert nach Ablauf der writeTime ins EEPROM zu schreiben)
uint32_t encTime = 0;       // Variable zum speichern der Millisekunden (wird bei jeder Aenderung auf millis() gesetzt)
const int writeTime = 5000; // Zeit in Millisekunden, nach der ein geaenderter Wert ins EEPROM geschrieben wird
//const byte eeAddress = 0;   // Adresse im EEPROM fuer den Enkoder-Wert (fuer einen Integer Datentyp werden 2 Bytes geschrieben)

// *********************************************************************
// // Lux Sensor & Lichtsteuerung
// *********************************************************************
uint16_t luxInnen = 0;
uint16_t luxAussen = 0;

// *********************************************************************
// Temperatur Sensor
// *********************************************************************
int set_barnOffset = EEPROM.get(21, set_barnOffset);   

// *********************************************************************
// PID Regler
// *********************************************************************
const int PIN_OUTPUT = 5; //PWM LED Pin
double Setpoint = 0, Input = 0, Output = 0;
double Kp=1, Ki=0.2, Kd=0.05; //Reglerfaktoren
double dblLuxInnen = 0;
double set_PIDkp = EEPROM.get(30, set_PIDkp);   
double set_PIDki = EEPROM.get(34, set_PIDki);   
double set_PIDkd = EEPROM.get(38, set_PIDkd);   
int set_pidSampletime = EEPROM.get(28, set_pidSampletime); 
double Faktor = 100; // Hilfsvariable für Schrittgöße Encoder

// *********************************************************************
// Lichtsteuerung
// *********************************************************************
byte set_starttimeH = EEPROM.get(42, set_starttimeH);   
byte set_starttimeM = EEPROM.get(43, set_starttimeM);   
byte set_startdurationH = EEPROM.get(44, set_startdurationH);   
byte set_startdurationM = EEPROM.get(45, set_startdurationM);   

byte set_suntimeH = EEPROM.get(46, set_suntimeH);  
byte set_suntimeM = EEPROM.get(47, set_suntimeM);  
byte set_enddurationH = EEPROM.get(48, set_enddurationH);  
byte set_enddurationM = EEPROM.get(49, set_enddurationM);  

byte estEndtimeH = 0;
byte estEndtimeM = 0;
byte EndM = 0; ////Hilfvariable Stunden/Minutenüberlaufsberechnung von estEndtime
int preEndM = 0; //Hilfvariable Stunden/Minutenüberlaufsberechnung von estEndtime

int set_hystluxON = EEPROM.get(50, set_hystluxON); 
byte set_hysttimeONM = EEPROM.get(52, set_hysttimeONM);  
byte set_hysttimeONS = EEPROM.get(53, set_hysttimeONS);

int set_hystluxOFF = EEPROM.get(54, set_hystluxOFF); 
byte set_hysttimeOFFM = EEPROM.get(56, set_hysttimeOFFM);  
byte set_hysttimeOFFS = EEPROM.get(57, set_hysttimeOFFS);

// *********************************************************************
// Torsteuerung
// *********************************************************************
byte set_doorsOpenH = EEPROM.get(58, set_doorsOpenH);  
byte set_doorsOpenM = EEPROM.get(59, set_doorsOpenM); 

int set_doorsCloseLux = EEPROM.get(60, set_doorsCloseLux);  
byte set_doorsCloseM = EEPROM.get(62, set_doorsCloseM);  
byte set_doorsCloseS = EEPROM.get(63, set_doorsCloseS);    
   
// *********************************************************************
// Objects
// *********************************************************************
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);
DS1302 rtc(8, 12, 10);  // (RST,DAT,CLK) Creation of the Real Time Clock Object
Time t; //Uhrzeit aus RTC auslesen
ClickEncoder *encoder;      // Dreh-Enkoder mit Klickbutton
BH1750 HelligkeitInnen(LOW); // ADDR line LOW/open:  I2C address 0x23 (0x46 including R/W bit) [default]
BH1750 HelligkeitAussen(HIGH); // ADDR line HIGH:      I2C address 0x5C (0xB8 including R/W bit)
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); //PID Regler
Adafruit_MCP4725 dac; //DAC
// *********************************************************************
//DAC
// *********************************************************************
const PROGMEM uint16_t DACLookup_FullSine_8Bit[256] =
{
  2048, 2098, 2148, 2198, 2248, 2298, 2348, 2398,
  2447, 2496, 2545, 2594, 2642, 2690, 2737, 2784,
  2831, 2877, 2923, 2968, 3013, 3057, 3100, 3143,
  3185, 3226, 3267, 3307, 3346, 3385, 3423, 3459,
  3495, 3530, 3565, 3598, 3630, 3662, 3692, 3722,
  3750, 3777, 3804, 3829, 3853, 3876, 3898, 3919,
  3939, 3958, 3975, 3992, 4007, 4021, 4034, 4045,
  4056, 4065, 4073, 4080, 4085, 4089, 4093, 4094,
  4095, 4094, 4093, 4089, 4085, 4080, 4073, 4065,
  4056, 4045, 4034, 4021, 4007, 3992, 3975, 3958,
  3939, 3919, 3898, 3876, 3853, 3829, 3804, 3777,
  3750, 3722, 3692, 3662, 3630, 3598, 3565, 3530,
  3495, 3459, 3423, 3385, 3346, 3307, 3267, 3226,
  3185, 3143, 3100, 3057, 3013, 2968, 2923, 2877,
  2831, 2784, 2737, 2690, 2642, 2594, 2545, 2496,
  2447, 2398, 2348, 2298, 2248, 2198, 2148, 2098,
  2048, 1997, 1947, 1897, 1847, 1797, 1747, 1697,
  1648, 1599, 1550, 1501, 1453, 1405, 1358, 1311,
  1264, 1218, 1172, 1127, 1082, 1038,  995,  952,
   910,  869,  828,  788,  749,  710,  672,  636,
   600,  565,  530,  497,  465,  433,  403,  373,
   345,  318,  291,  266,  242,  219,  197,  176,
   156,  137,  120,  103,   88,   74,   61,   50,
    39,   30,   22,   15,   10,    6,    2,    1,
     0,    1,    2,    6,   10,   15,   22,   30,
    39,   50,   61,   74,   88,  103,  120,  137,
   156,  176,  197,  219,  242,  266,  291,  318,
   345,  373,  403,  433,  465,  497,  530,  565,
   600,  636,  672,  710,  749,  788,  828,  869,
   910,  952,  995, 1038, 1082, 1127, 1172, 1218,
  1264, 1311, 1358, 1405, 1453, 1501, 1550, 1599,
  1648, 1697, 1747, 1797, 1847, 1897, 1947, 1997
};

// *********************************************************************
//Clickencoder für Werteverstellung
// *********************************************************************
void timerIsr() 
  {                               // die Timer-Interrupt-Funktion
  encoder->service();                           // fuer den Dreh-Enkoder
  }


byte getDigits(long val)                        // Funktion zum ermitteln, wie viele Dezimalstellen eine Zahl hat
  {
  byte digits = 0;                              // Anzahl der Dezimalstellen auf Null setzen
  while (abs(val) >= pow(10, digits)) digits++; // in der Schleife die Anzahl der Dezimalstellen ermitteln
  return digits + (val <= 0);                   // vor der Rueckgabe noch eine Stelle addieren, wenn die Zahl kleiner/gleich Null ist
  }


void checkStep()                                // Funktion zum aendern der Schrittweite
  {
  byte index = getDigits(ValueStep);            // Anzahl der Dezimalstellen des bisherigen Wertes holen (dient als Index fuer das Array)
  if (index > 3) index = 0;                     // wenn groesser als 3, dann wieder den niedrigsten Wert zuweisen
  ValueStep = steps[index];                     // ValueStep den Wert aus dem Array zuweisen
  }

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
        // NULL = no menu function
LCDML_add         (0  , LCDML_0          , 1  , "Sensorwerte"        , NULL);                    
LCDML_add         (1  , LCDML_0_1        , 1  , "Temperatur Stall"   , mFunc_temp_barn);         // "temperature" tab
LCDML_add         (2  , LCDML_0_1        , 2  , "Licht Aussen/Innen" , mFunc_lux);               // "light" tab
LCDML_add         (3  , LCDML_0_1        , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (4  , LCDML_0          , 2  , "Datum/Uhrzeit"      , mFunc_timer_info);        // "RTCtimeSetup" tab
LCDML_add         (5  , LCDML_0          , 4  , "Einstellungen"      , NULL);                    
LCDML_add         (6  , LCDML_0_4        , 1  , "Datum/Uhrzeit"      , mFunc_timeSetup);         // "RTCtimeSetup" tab
LCDML_add         (7  , LCDML_0_4        , 2  , "Lichtsteuerung"     , NULL);                    
LCDML_add         (8  , LCDML_0_4        , 3  , "Torsteuerung"       , NULL);                    
LCDML_add         (9  , LCDML_0_4_3      , 1  , "Tor Auf Parameter"  , mFunc_doors);             // "doors" tab
LCDML_add         (10 , LCDML_0_4_3      , 2  , "Tor Zu Hysterese"   , mFunc_Hystdoors);         // "doors" tab
LCDML_add         (11 , LCDML_0_4_3      , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (12 , LCDML_0_4        , 4  , "Temperatur Offset"  , mFunc_tempOffset);        // "temperature" tab
LCDML_add         (13 , LCDML_0_4        , 5  , "Energiesparmodus"   , mFunc_screensaverSetup);  // "menuFunction" tab
LCDML_add         (14 , LCDML_0_4        , 6  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (15 , LCDML_0_4_2      , 1  , "Zeiten"             , NULL);                    
LCDML_add         (16 , LCDML_0_4_2_1    , 1  , "Startzeit"          , mFunc_luxBegin);          // "light" tab
LCDML_add         (17 , LCDML_0_4_2_1    , 2  , "Tages- + Endzeit"   , mFunc_luxEnd);            // "light" tab
LCDML_add         (18 , LCDML_0_4_2_1    , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (19 , LCDML_0_4_2      , 2  , "Hysteresen"         , NULL);                    
LCDML_add         (20 , LCDML_0_4_2_2    , 1  , "Hysterese Ein"      , mFunc_HystON);            // "light" tab
LCDML_add         (21 , LCDML_0_4_2_2    , 2  , "Hysterese Aus"      , mFunc_HystOFF);           // "light" tab
LCDML_add         (22 , LCDML_0_4_2_2    , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (23 , LCDML_0_4_2      , 3  , "Regler"             , mFunc_luxPID);            // "light" tab
LCDML_add         (24 , LCDML_0_4_2      , 4  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (25 , LCDML_0          , 5  , "Info"               , mFunc_info);              // "menuFunction" tab
LCDML_add         (26 , LCDML_0          , 6  , "Energiesparmodus"   , mFunc_screensaver);       // "menuFunction" tab

// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    26

// create menu
LCDML_createMenu(_LCDML_DISP_cnt);

// SETUP  #######################################################################################################################################################
void setup()
{
  Serial.begin(9600);          
  Wire.begin(); // Initialize I2C bus
  lcd.init(); //LCD initialisieren
  lcd.backlight(); //LCD Licht EIN

  // Lux Sensor * * * * *
  HelligkeitInnen.begin(ModeContinuous, ResolutionHigh); // Initialize sensor in continues mode, high 0.5 lx resolution
  HelligkeitAussen.begin(ModeContinuous, ResolutionHigh); // Initialize sensor in continues mode, high 0.5 lx resolution
  HelligkeitInnen.startConversion(); // Start conversion
  HelligkeitAussen.startConversion();// Start conversion

  
  // LCD Begin
  // set special chars for scrollbar
  lcd.createChar(0, (uint8_t*)scroll_bar[0]);
  lcd.createChar(1, (uint8_t*)scroll_bar[1]);
  lcd.createChar(2, (uint8_t*)scroll_bar[2]);
  lcd.createChar(3, (uint8_t*)scroll_bar[3]);
  lcd.createChar(4, (uint8_t*)scroll_bar[4]);

  // LCDMenuLib Setup
  LCDML_setup(_LCDML_DISP_cnt);

  

  // Enable Menu Rollover
  LCDML.MENU_enRollover();

  // Enable Screensaver (screensaver menu function, time to activate in ms)
  //LCDML.SCREEN_enable(mFunc_screensaver, set_screensaverTime*1000); // //Zeit bis zum Einschalten des Bildschirmschoners 
  //LCDML.SCREEN_disable();
 


  // You can jump to a menu function from anywhere with
  //LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name



  // Set the realtimeclock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);


  //PID * * * * *
  //initialize the variables we're linked to
  Input = double(luxInnen/2);
  Setpoint = 80;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  pinMode(PIN_OUTPUT, OUTPUT);

  //DAC * * * * * 
  dac.begin(0x62); // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
}


// LOOP  #######################################################################################################################################################
void loop()
{
  LCDML.loop();
  // Enable Screensaver (screensaver menu function, time to activate in ms)
  LCDML.SCREEN_enable(mFunc_screensaver, set_screensaverTime*1000); // //Zeit bis zum Einschalten des Bildschirmschoners 
  //LCDML.SCREEN_disable();

  //PID
  dblLuxInnen = double(luxInnen);

  Input = dblLuxInnen/2; 
  
  myPID.SetTunings(set_PIDkp, set_PIDki, set_PIDkd);

  myPID.SetSampleTime(set_pidSampletime); 
  
  myPID.Compute();
 
  analogWrite(PIN_OUTPUT, Output);
  //PID ENDE


  //DAC - Digital Analog Converter 0-10VDC ANM.: *25 Skalierungsfaktor für Ausgangsspannung, false: Wert nicht in EEPROM speichern
  dac.setVoltage((uint16_t(Output)*25), false);
  

  //Zeitschaltuhr Tore
  t = rtc.getTime();
  unsigned long HOUR = (unsigned long)t.hour * 3600;
  unsigned long MIN = (unsigned long)t.min * 60;
  unsigned long SEC = (unsigned long)t.sec; 
  unsigned long timeNow = HOUR + MIN + SEC; //aktuelle Tagesuhrzeit in Sekunden  
  //Serial.println(timeNow);
  
  unsigned long ON = 50;
  unsigned long OFF = 65;
  bool STATE;

  if (ON <= OFF) //an-aus am selben tag
  {
    if (timeNow >= ON && timeNow < OFF)
    {
      if(STATE == LOW) 
        {
        STATE = HIGH; // SSR Relais EIN
        } 
    }
    else
    {
      if(STATE == HIGH) 
      {
      STATE = LOW; // SSR Relais AUS
      } 
    }
  }
  else  //schaltzeit über mitternacht hinaus
  {
    if (timeNow >= ON || timeNow < OFF)
    {
      if(STATE == LOW) 
      {
      STATE = HIGH; // SSR Relais EIN
      }  
    }
    else
    {
      if(STATE == HIGH)
      {
      STATE = LOW; // SSR Relais AUS
      }  
    }
  }
 
  //* * * * *

  //Lux Sensor * * * * *
  if (HelligkeitInnen.isConversionCompleted()) // Wait for completion (blocking busy-wait delay)
  {        
  luxInnen = HelligkeitInnen.read(); // Read light
  }

  if (HelligkeitAussen.isConversionCompleted()) // Wait for completion (blocking busy-wait delay)
  {       
  luxAussen = HelligkeitAussen.read(); // Read light
  }
   
}
