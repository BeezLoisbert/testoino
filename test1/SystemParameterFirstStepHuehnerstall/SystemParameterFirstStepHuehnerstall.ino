
#include <EEPROM.h>

uint8_t set_hh =17;
uint8_t set_mm =31;
uint8_t set_ss =31;

uint8_t set_DD =16;
uint8_t set_MM =07;
uint16_t set_YYYY =2021;

int set_screensaverTime =10; //Sekunden

int set_barnOffset = 0;

int set_pidSampletime = 100;
double set_PIDkp = 1.0;
double set_PIDki = 1.0;
double set_PIDkd = 1.0;

byte set_starttimeH = 5;
byte set_starttimeM = 5;
byte set_startdurationH = 1; 
byte set_startdurationM = 1;

byte set_suntimeH = 3;
byte set_suntimeM = 3;
byte set_enddurationH = 2;
byte set_enddurationM = 2;

int set_hystluxON = 100;
byte set_hysttimeONM = 1;
byte set_hysttimeONS = 1;

int set_hystluxOFF = 200;
byte set_hysttimeOFFM = 2;
byte set_hysttimeOFFS = 2;

byte set_doorsOpenH = 4;
byte set_doorsOpenM = 4;

int set_doorsCloseLux = 22;
byte set_doorsCloseM = 22;
byte set_doorsCloseS = 22;

int set_luxSOLL = 100;

  void setup() 
  {   
  }

  void loop() 
  {
  EEPROM.put(0, set_hh); //RTC Stunden
  EEPROM.put(2, set_mm); //RTC Minuten
  EEPROM.put(4, set_ss); //RTC Sekunden

  EEPROM.put(6, set_DD); //RTC Tage
  EEPROM.put(8, set_MM); //RTC Monate
  EEPROM.put(10, set_YYYY); //RTC Jahr

  EEPROM.put(20, set_screensaverTime); //Zeit bis zum Einschalten des Bildschirmschoners

  EEPROM.put(21, set_barnOffset); // Temperaturoffset

  EEPROM.put(28, set_pidSampletime); //PID-Regler Sampletime
  EEPROM.put(30, set_PIDkp);   // PID-Regler P-Faktor
  EEPROM.put(34, set_PIDki);   // PID-Regler I-Faktor
  EEPROM.put(38, set_PIDkd);   // PID-Regler D-Faktor

  EEPROM.put(42, set_starttimeH);   // "Sonnenaufgangszeit"[h]
  EEPROM.put(43, set_starttimeM);   // "Sonnenaufgangszeit"[min]
  EEPROM.put(44, set_startdurationH);   // "Sonnenaufgangsdauer"[h]
  EEPROM.put(45, set_startdurationM);   // "Sonnenaufgangsdauer"[min]

  EEPROM.put(46, set_suntimeH);  // "Sonnenzeit"[h]
  EEPROM.put(47, set_suntimeM);  // "Sonnenzeit"[min]
  EEPROM.put(48, set_enddurationH);  // "Sonnenabgangsdauer"[h]
  EEPROM.put(49, set_enddurationM);  // "Sonnenabgangsdauer"[min]

  EEPROM.put(50, set_hystluxON); //Hystereseparameter Einschalten [lx]
  EEPROM.put(52, set_hysttimeONM);  //Hystereseparameter Einschaltverzögerung [h]
  EEPROM.put(53, set_hysttimeONS);   //Hystereseparameter Einschaltverzögerung [min]

  EEPROM.put(54, set_hystluxOFF); //Hystereseparameter Ausschalten [lx]
  EEPROM.put(56, set_hysttimeOFFM);  //Hystereseparameter Ausschaltverzögerung [h]
  EEPROM.put(57, set_hysttimeOFFS);   //Hystereseparameter Ausschaltverzögerung [min]

  EEPROM.put(58, set_doorsOpenH);  // Uhrzeit Toröffnung [h]
  EEPROM.put(59, set_doorsOpenM);  // Uhrzeit Toröffnung [min]
 
  EEPROM.put(60, set_doorsCloseLux);  // Helligkeit Torschließung [lx]
  EEPROM.put(62, set_doorsCloseM);  // Uhrzeit Torschließung [h]
  EEPROM.put(63, set_doorsCloseS);  // Uhrzeit Torschließung [min]

  EEPROM.put(64, set_luxSOLL);  // Tageshelligkeit SOLL [lx]
   
  delay(5000);
}
