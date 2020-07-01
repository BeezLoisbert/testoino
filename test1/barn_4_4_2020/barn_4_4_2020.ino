
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

// *********************************************************************
// Temperatursensor settings
// *********************************************************************
#define DS18B20MODEL 0x28
OneWire  tempsens(4);  // on pin 4 (a 4.7K resistor is necessary) Temperatursensor

// *********************************************************************
// Torsteuerung settings
// *********************************************************************
#define chickenONOFF_IN1 39
#define chicken_IN2 41
#define chicken_IN3 43

#define turkeyONOFF_IN4 45
#define turkey_IN5 47
#define turkey_IN6 49

// *********************************************************************
// Lichtsteuerung settings
// *********************************************************************
#define lightON_L_IN7 51
#define lightON_N_IN8 53

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

int set_luxSOLL = EEPROM.get(64, set_luxSOLL); 

unsigned long HOUR = 0; //Hilfsvariable für Berechnung der aktuellen Tagesuhrzeit in Sekunden
unsigned long MIN = 0;  //Hilfsvariable für Berechnung der aktuellen Tagesuhrzeit in Sekunden
unsigned long SEC = 0;  //Hilfsvariable für Berechnung der aktuellen Tagesuhrzeit in Sekunden
unsigned long timeNow = 0; //aktuelle Tagesuhrzeit in Sekunden  

byte CaseLight = 1;
int PIDSunTime = 0; //Wert von 0-1000, entsprechend der Sonnenlichtrampe (0=Dimmstufe niedrig bzw. aus, 1000 = Sollwert LUX Tag)
double fSunPID = 0; // PIDSunTime / 1000,      Gleitkommawert zwischen 0,000 und 1,000    wird als PID Sollwert verwendet
double fPIDsollwert = 0; // Sollwert für PID Regler (Sollwert Tageshelligkeit[lx] * fPotiSunPID)


bool flag_luxPIDon = LOW;
bool flag_PID_ON = LOW;
bool flag_sunrise = LOW;
bool flag_sunset = LOW;
bool flag_lux_PIDoff = LOW;
unsigned long luxPIDon_millis= 0; 
unsigned long luxPIDoff_millis= 0;

double mapDoubleClamped(double x, double in_min, double in_max, double out_min, double out_max);
double rampDaytime(uint32_t now_sec, uint32_t sunriseStart_sec, uint32_t sunriseEnd_sec, uint32_t sunsetStart_sec, uint32_t sunsetEnd_sec);

uint32_t gNow_sec = 0;
   

// *********************************************************************
// Torsteuerung
// *********************************************************************
byte set_doorsOpenH = EEPROM.get(58, set_doorsOpenH);  
byte set_doorsOpenM = EEPROM.get(59, set_doorsOpenM); 

int set_doorsCloseLux = EEPROM.get(60, set_doorsCloseLux);  
byte set_doorsCloseM = EEPROM.get(62, set_doorsCloseM);  
byte set_doorsCloseS = EEPROM.get(63, set_doorsCloseS);

unsigned long doorsOpenTimeSEC= 0;
unsigned long doorsCloseDelaytimeMILLISEC= 0;

byte CaseDoorsOpenClose = 1;
bool TorAuf = HIGH;
bool TorZu = HIGH;
bool flag_closeMillis = LOW;
bool flag_doorsOpen = LOW;
bool flag_doorsClose = LOW;
bool flag_relaisTOF = LOW;
unsigned long DoorsRelaisHoldTimeMillis = 15000;//300000; //Abfallverzögerung für Auf/Zu-Relais in Millisekunden
unsigned long TorAuf_millis = 0;
unsigned long TorZuDelay_millis= 0;
unsigned long TorZu_millis= 0;
unsigned long relais_TONTOF = 1000; //Ein-/Ausschaltverzögerung für Relais 1 und 4 (Ein/Ausschalter für Torantriebe) in Millisekunden
unsigned long relaisTOF_millis= 0;
   
// *********************************************************************
// Objects
// *********************************************************************
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);
DS1302 rtc(8, 12, 10);  // (RST,DAT,CLK) Creation of the Real Time Clock Object
Time t; //Uhrzeit aus RTC auslesen
ClickEncoder *encoder;      // Dreh-Enkoder mit Klickbutton
BH1750 HelligkeitInnen(HIGH); // ADDR line LOW/open:  I2C address 0x23 (0x46 including R/W bit) [default]
BH1750 HelligkeitAussen(LOW); // ADDR line HIGH:      I2C address 0x5C (0xB8 including R/W bit)
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); //PID Regler

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
LCDML_add         (15 , LCDML_0_4_2      , 1  , "Helligkeit"         , mFunc_luxSOLL);           // 
LCDML_add         (16 , LCDML_0_4_2      , 2  , "Zeiten"             , NULL);                    
LCDML_add         (17 , LCDML_0_4_2_2    , 1  , "Startzeit"          , mFunc_luxBegin);          // "light" tab
LCDML_add         (18 , LCDML_0_4_2_2    , 2  , "Tages- + Endzeit"   , mFunc_luxEnd);            // "light" tab
LCDML_add         (19 , LCDML_0_4_2_2    , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (20 , LCDML_0_4_2      , 3  , "Hysteresen"         , NULL);                    
LCDML_add         (21 , LCDML_0_4_2_3    , 1  , "Hysterese Ein"      , mFunc_HystON);            // "light" tab
LCDML_add         (22 , LCDML_0_4_2_3    , 2  , "Hysterese Aus"      , mFunc_HystOFF);           // "light" tab
LCDML_add         (23 , LCDML_0_4_2_3    , 3  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (24 , LCDML_0_4_2      , 4  , "Regler"             , mFunc_luxPID);            // "light" tab
LCDML_add         (25 , LCDML_0_4_2      , 5  , "Zurueck"            , mFunc_back);              // "menuFunction" tab
LCDML_add         (26 , LCDML_0          , 5  , "Info"               , mFunc_info);              // "menuFunction" tab
LCDML_add         (27 , LCDML_0          , 6  , "Energiesparmodus"   , mFunc_screensaver);       // "menuFunction" tab

// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    27

// create menu
LCDML_createMenu(_LCDML_DISP_cnt);

// SETUP  #######################################################################################################################################################
void setup()
{
  Serial.begin(9600);          
  Wire.begin(); // Initialize I2C bus
  lcd.init(); //LCD initialisieren
  lcd.backlight(); //LCD Licht EIN
  Serial.println("Input Output fPIDsollwert");

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
  myPID.SetMode(AUTOMATIC); //turn the PID on

  //Torsteuerung * * * * *
  pinMode(chickenONOFF_IN1, OUTPUT);
  pinMode(chicken_IN2, OUTPUT);
  pinMode(chicken_IN3, OUTPUT);
  digitalWrite(chickenONOFF_IN1, HIGH);
  digitalWrite(chicken_IN2, HIGH);
  digitalWrite(chicken_IN3, HIGH);
  
  pinMode(turkeyONOFF_IN4, OUTPUT);
  pinMode(turkey_IN5, OUTPUT);
  pinMode(turkey_IN6, OUTPUT);
  digitalWrite(turkeyONOFF_IN4, HIGH);
  digitalWrite(turkey_IN5, HIGH);
  digitalWrite(turkey_IN6, HIGH);

  //Lichtsteuerung * * * * *
  pinMode(lightON_L_IN7, OUTPUT);
  pinMode(lightON_N_IN8, OUTPUT);
  digitalWrite(lightON_L_IN7, HIGH);
  digitalWrite(lightON_N_IN8, HIGH);



    
    
}


// LOOP  #######################################################################################################################################################
void loop()
{
  LCDML.loop();
  // Enable Screensaver (screensaver menu function, time to activate in ms)
  LCDML.SCREEN_enable(mFunc_screensaver, set_screensaverTime*1000); // //Zeit bis zum Einschalten des Bildschirmschoners 
  //LCDML.SCREEN_disable();


  //Lux Sensor * * * * *
  if (HelligkeitInnen.isConversionCompleted()) // Wait for completion (blocking busy-wait delay)
  {        
  luxInnen = HelligkeitInnen.read(); // Read light
  }

  if (HelligkeitAussen.isConversionCompleted()) // Wait for completion (blocking busy-wait delay)
  {       
  luxAussen = HelligkeitAussen.read(); // Read light
  }   

  
  
  //Lichtsteuerung 
  
  unsigned long starttimeHOUR = (unsigned long) set_starttimeH * 3600;
  unsigned long starttimeMIN = (unsigned long) set_starttimeM * 60;
  unsigned long starttimeSEC = starttimeHOUR + starttimeMIN; //Eingestellte "Sonnenaufgangsuhrzeit" in Sekunden

  unsigned long startdurationHOUR = (unsigned long) set_startdurationH * 3600;
  unsigned long startdurationMIN = (unsigned long) set_startdurationM * 60;
  unsigned long startdurationSEC = startdurationHOUR + startdurationMIN; //Eingestellte Dauer des "Sonnenaufgangs" (Dauer 0-100% Leuchtkraft)

  unsigned long startdurationEND = starttimeSEC + startdurationSEC; //Uhrzeit Ende Sonnenaufgang, Beginn Sonnentag

  unsigned long suntimeHOUR = (unsigned long) set_suntimeH * 3600;  
  unsigned long suntimeMIN = (unsigned long) set_suntimeM * 60;   
  unsigned long suntimeSEC = suntimeHOUR + suntimeMIN; //Eingestellte Dauer des "Sonnentages" 
    
  unsigned long enddurationHOUR = (unsigned long) set_enddurationH * 3600;
  unsigned long enddurationMIN = (unsigned long) set_enddurationM * 60;
  unsigned long enddurationSEC = enddurationHOUR + enddurationMIN; //Eingestellte Dauer des "Sonnenuntergangs" (Dauer 100-0% Leuchtkraft)  
    
  unsigned long enddurationSTART = startdurationEND + suntimeSEC; //Uhrzeit Ende Sonnentag, Beginn Sonnenuntergang
  unsigned long endtime = enddurationSTART + enddurationSEC; //Uhreit Ende Sonnenuntergang

  unsigned long hysttimeOnMIN = (unsigned long) set_hysttimeONM * 60;
  unsigned long hysttimeOnSEC = hysttimeOnMIN + (unsigned long) set_hysttimeONS; //Eingestellte Hysteresenwartezeit für "Licht EIN"

  unsigned long hysttimeOffMIN = (unsigned long) set_hysttimeOFFM * 60;
  unsigned long hysttimeOffSEC = hysttimeOffMIN + (unsigned long) set_hysttimeOFFS; //Eingestellte Hysteresenwartezeit für "Licht AUS"

  gNow_sec = (uint32_t)timeNow;

  double ramp = rampDaytime(gNow_sec, starttimeSEC, startdurationEND, enddurationSTART, endtime); // floating Value 0.00-1.00

  if ((gNow_sec < starttimeSEC) || (gNow_sec > endtime)) // NACHT
  {
    fPIDsollwert = 0;
    Serial.print("NACHT");
  }
  else if (gNow_sec < startdurationEND) // SONNENAUFGANG
  {
    fPIDsollwert = ramp * (double)set_luxSOLL;
    flag_sunrise = HIGH;
    Serial.print("SUNRISE");
  }
  else if (gNow_sec < enddurationSTART) // TAG
  {Serial.print("TAG");
      if ((luxInnen/2 < (set_luxSOLL - set_hystluxON)) && flag_luxPIDon == LOW) // Hysterese Lichtregelung EIN
      {
      luxPIDon_millis = millis();
      flag_luxPIDon = HIGH;    
      }
      
      if (luxInnen/2 > (set_luxSOLL - set_hystluxON) && flag_PID_ON == LOW) // Hysterese Lichtregelung EIN
      {
      flag_luxPIDon = LOW;
      }

      if((millis() > (hysttimeOnSEC * 1000 + luxPIDon_millis)) && flag_luxPIDon == HIGH && flag_PID_ON == LOW)  // Licht EIN (mit PID Regler)
      { 
      fPIDsollwert = (double)set_luxSOLL;
      flag_PID_ON = HIGH; 
      }

      if (luxInnen/2 > (set_luxSOLL + set_hystluxOFF) && flag_PID_ON == HIGH && flag_lux_PIDoff == LOW) // Hysterese Lichtregelung AUS
      {
      luxPIDoff_millis = millis();
      flag_lux_PIDoff = HIGH;      
      }

      if (luxInnen/2 < (set_luxSOLL + set_hystluxON) && flag_PID_ON == HIGH) // Hysterese Lichtregelung AUS
      {
      flag_lux_PIDoff = LOW;
      }

      if((millis() > (hysttimeOffSEC * 1000 + luxPIDoff_millis)) && flag_lux_PIDoff == HIGH && flag_PID_ON == HIGH)  // Licht AUS
      { 
      flag_PID_ON = HIGH; 
      }
  }
  else
  {
    fPIDsollwert = ramp * (double) set_luxSOLL; //SONNENUNTERGANG
    flag_sunset = HIGH;
    Serial.print("SUNSET");
  }
    Serial.print(Input);
    Serial.print(","); 
    Serial.print(Output);
    Serial.print(",");
    Serial.println(fPIDsollwert);
   
      
  if(flag_sunrise == HIGH || flag_PID_ON == HIGH || flag_sunset == HIGH)
  {
  digitalWrite(lightON_L_IN7, LOW);
  digitalWrite(lightON_N_IN8, LOW);
  Input = (double) luxInnen/2; //Messeingang für Regler
  Setpoint = fPIDsollwert; //Sollwert für Regler 
  myPID.SetTunings(set_PIDkp, set_PIDki, set_PIDkd); //Reglerfaktoren
  //myPID.SetTunings(set_PIDkp, set_PIDki, 0.005); //Reglerfaktoren
  myPID.SetSampleTime(set_pidSampletime); //Regler Taktrate 
  myPID.Compute(); // PID Regler ausführen
  int PotiInput = map((int)Output, 0, 255, 15, 177);
  Wire.beginTransmission(0x2C);
  Wire.write(byte(0x00)); //sends instruction byte  
  Wire.write(PotiInput);
  Wire.endTransmission();
  }
    else
    {
    digitalWrite(lightON_L_IN7, HIGH);
    digitalWrite(lightON_N_IN8, HIGH);  
    }


  //Torsteuerung
  t = rtc.getTime();
  HOUR = (unsigned long)t.hour * 3600;
  MIN = (unsigned long)t.min * 60;
  SEC = (unsigned long)t.sec; 
  timeNow = HOUR + MIN + SEC; //aktuelle Tagesuhrzeit in Sekunden  
  

  doorsOpenTimeSEC = ((unsigned long)set_doorsOpenH * 3600) + ((unsigned long)set_doorsOpenM * 60); //eingestellte Öffnungsuhrzeit in Sekunden
  doorsCloseDelaytimeMILLISEC = (((unsigned long)set_doorsCloseM * 60) + ((unsigned long)set_doorsCloseS))*1000; // eingestellte (Millisekunden) Wartezeit für Torschließung bei Unterschreitung x-Lux
 
    switch (CaseDoorsOpenClose)  // WICHTIG!!!! : Ansteuerung vom Relaismodul funktioniert nur invertiert (LOW = 1, HIGH = 0)
    { 
      case 1: //Tor Öffnen
  
      flag_doorsClose = LOW;
        
      if ((timeNow >= doorsOpenTimeSEC) && flag_doorsOpen == LOW) // Tor öffnen
      {
      TorAuf = LOW; 
      TorAuf_millis = millis();
      flag_doorsOpen = HIGH;
      }

      if((millis() > (DoorsRelaisHoldTimeMillis + TorAuf_millis)) && flag_doorsOpen == HIGH) //Abfallverzögerung Relais  
      {
      TorAuf = HIGH; 
      relaisTOF_millis = millis();            
      CaseDoorsOpenClose = 2;
      }
      break;  //case '1' Ende * * * * * *

        case 2: //Tor Schließen
        
        flag_doorsOpen = LOW;
        
        if ((luxAussen/2 < set_doorsCloseLux) && flag_closeMillis == LOW) // Hysterese Tor schließen
        {
        TorZuDelay_millis = millis();
        flag_closeMillis = HIGH;    
        }
        
        if (luxAussen/2 > set_doorsCloseLux) // Hysterese Tor schließen
        {
        flag_closeMillis = LOW;
        }
         
        if((millis() > (doorsCloseDelaytimeMILLISEC + TorZuDelay_millis)) && flag_doorsClose == LOW && flag_closeMillis == HIGH)  // Tor schließen
        {
        TorZu = LOW; 
        TorZu_millis = millis();
        flag_doorsClose = HIGH;
        }
  
        if(millis() > (DoorsRelaisHoldTimeMillis + TorZu_millis) && flag_doorsClose == HIGH) //Abfallverzögerung Relais    
        {
        TorZu = HIGH;
          if(flag_relaisTOF == LOW)
          {
          relaisTOF_millis = millis();
          flag_relaisTOF = HIGH;         
          }
            if((timeNow > 1) && (timeNow < 60)) //Casesprung kommt erst bei aktueller Tagesuhrzeit zw. 00:00:00 und 00:01:00
            {
            flag_relaisTOF = LOW;
            CaseDoorsOpenClose = 1;                
            }         
        }      
        break;  //case '2' Ende * * * * * *        
    }


    //Relais Ansteuerung zur Verhinderung von Kontaktüberschneidungen. Das "+" schaltende Relais wird immer als erstes ausgeschalten, bzw. als letztes eingeschalten.
    
    if(CaseDoorsOpenClose == 1 && TorAuf == HIGH) //Relais Tor AUF - Ausschalten 
    {
    digitalWrite(chickenONOFF_IN1, HIGH);
    digitalWrite(turkeyONOFF_IN4, HIGH);         
      if(millis() > (relaisTOF_millis + relais_TONTOF))
      {
      digitalWrite(chicken_IN2, HIGH);
      digitalWrite(chicken_IN3, HIGH);      
      digitalWrite(turkey_IN5, HIGH);
      digitalWrite(turkey_IN6, HIGH);
      }   
    }   
      else if(CaseDoorsOpenClose == 1 && TorAuf == LOW) //Relais Tor AUF - EINschalten 
      {      
      digitalWrite(chicken_IN2, HIGH);
      digitalWrite(chicken_IN3, HIGH);        
      digitalWrite(turkey_IN5, HIGH);
      digitalWrite(turkey_IN6, HIGH);
        if(millis() > (TorAuf_millis + relais_TONTOF))
        {
        digitalWrite(chickenONOFF_IN1, LOW);
        digitalWrite(turkeyONOFF_IN4, LOW); 
        }
      }

    if(CaseDoorsOpenClose == 2 && TorZu == HIGH) //Relais Tor ZU - Ausschalten 
    {
    digitalWrite(chickenONOFF_IN1, HIGH);
    digitalWrite(turkeyONOFF_IN4, HIGH);         
      if(millis() > (relaisTOF_millis + relais_TONTOF))
      {
      digitalWrite(chicken_IN2, HIGH);
      digitalWrite(chicken_IN3, HIGH);      
      digitalWrite(turkey_IN5, HIGH);
      digitalWrite(turkey_IN6, HIGH);
      }   
    }  
      else if(CaseDoorsOpenClose == 2 && TorZu == LOW) //Relais Tor ZU - Einschalten 
      {
      digitalWrite(chicken_IN2, LOW);
      digitalWrite(chicken_IN3, LOW);  
      digitalWrite(turkey_IN5, LOW);
      digitalWrite(turkey_IN6, LOW);
       if(millis() > (TorZu_millis + relais_TONTOF))
       {
        digitalWrite(chickenONOFF_IN1, LOW);
        digitalWrite(turkeyONOFF_IN4, LOW); 
        }
      }
    
}
