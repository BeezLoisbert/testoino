
uint8_t set_hh = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(0, set_hh);
uint8_t set_mm = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(1, set_mm);
uint8_t set_ss = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(2, set_ss);

uint8_t set_DD = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(3, set_DD);
uint8_t set_MM = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(4, set_MM);
uint16_t set_YYYY = 0; //Initialwert bei Erstinbetriebnahme EEPROM.write(5, set_YYYY);

bool flag_hh = LOW;
bool flag_mm = LOW;
bool flag_ss = LOW;

bool flag_DD = LOW;
bool flag_MM = LOW;
bool flag_YYYY = LOW;

bool flag_save = LOW;

int dattime = 4;

//Speicheranimation * * * * *

int startPoint1 = 0;
int endPoint1 = 0;
int startPoint2 = 0;
int endPoint2 = 0;
int i = 0, j = 0;
int k = 0, l = 0;

// text to display
String txtMsg1 ;
String txtMsg2 ;
//Speicheranimation Ende * * * * *

// *********************************************************************
void mFunc_timeSetup(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
  LCDML.FUNC_setLoopInterval(100); //Triggerintervall für Loop (milliseconds)

  set_hh = EEPROM.get(0, set_hh);
  set_mm = EEPROM.get(2, set_mm);
  set_ss = EEPROM.get(4, set_ss);

  set_DD = EEPROM.get(6, set_DD);
  set_MM = EEPROM.get(8, set_MM);
  set_YYYY = EEPROM.get(10, set_YYYY);

  dattime = 4; 
  
  flag_hh = LOW;
  flag_mm = LOW;
  flag_ss = LOW;

  flag_DD = LOW;
  flag_MM = LOW;
  flag_YYYY = LOW;

  flag_save = LOW;
  startPoint1 = 2;   //set starting point
  endPoint1 = 18;    //set ending point
  txtMsg1 = "Datum und Uhrzeit";
  startPoint2 = 5;   //set starting point
  endPoint2 = 17;    //set ending point
  txtMsg2 = "gespeichert !";
  
    
  // setup function
  encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
  Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
  Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen

  lcd.setCursor(0, 0);
  lcd.print(F("Datum:")); 

  lcd.setCursor(0, 3);
  lcd.print(F("Uhrzeit:"));  
  
  } //Setup Ende * * * * *


if(LCDML.FUNC_loop())           // ****** LOOP *********
{
ClickEncoder::Button b = encoder->getButton();     // den Button-Status abfragen

  if(flag_save == LOW)
  { 
    lcd.setCursor(10, 3); //Stunde
    if (set_hh < 10)
    {
    lcd.print("0");
    }
    lcd.print(set_hh);
     
    lcd.setCursor(12, 3); //Doppelpunkt    
    lcd.print(":");
    
    lcd.setCursor(13, 3); //Minute
    if (set_mm < 10)
    {
    lcd.print("0");
    }
    lcd.print(set_mm);

    lcd.setCursor(15, 3); //Doppelpunkt
    lcd.print(":");
    
    lcd.setCursor(16, 3); //Sekunde
    if (set_ss < 10)
    {
    lcd.print("0");
    }
    lcd.print(set_ss);
    
    lcd.setCursor(8, 0); //Tag
    if (set_DD < 10)
    {
    lcd.print("0");
    }
    lcd.print(set_DD);

    lcd.setCursor(10, 0); //Trennstrich
    lcd.print("/");

    lcd.setCursor(11, 0); //Monat
    if (set_MM < 10)
    {
    lcd.print("0");
    }
    lcd.print(set_MM);

    lcd.setCursor(13, 0); //Trennstrich
    lcd.print("/");

    lcd.setCursor(14, 0); //Jahr
    lcd.print(set_YYYY);
  }
  
   
    
  switch (dattime) 
  {
    case 1: //Stunden
      lcd.setCursor(14, 1);
      lcd.print("    ");       //mm Pfeil nach links löschen
      // lcd.setCursor(12, 1); 
      //lcd.print(" ");       //mm Pfeil nach rechts löschen
        
      lcd.setCursor(10, 2);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(11, 2); //126 Pfeil nach rechts
      lcd.write(126);  
  
      if(flag_hh == LOW) 
      {
      encValue = set_hh;
      flag_hh = HIGH;
      }
      set_hh = encValue;
    
      if(set_hh<0) //Wertbegrenzung für Stundenanzeige MIN
      {
      encValue=23;
      }    
      
      if(set_hh>23) //Wertbegrenzung für Stundenanzeige MAX
      {
      encValue=0;
      }    
      
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                                    
          case ClickEncoder::Clicked:      // Button wurde einmal angeklickt
          EEPROM.put(0, set_hh);           // den Wert ins EEPROM schreiben 
          dattime = 2;                     //zu Minuten springen
          break; 
        }   
       } 
    break;  //case '1' Ende * * * * * *

   
    case 2: //Minuten
    
      lcd.setCursor(10, 2);
      lcd.print(" ");       //hh Pfeil nach links löschen
      lcd.setCursor(11, 2); 
      lcd.print(" ");       //hh Pfeil nach rechts löschen
          
      lcd.setCursor(13, 2);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(14, 2); //126 Pfeil nach rechts
      lcd.write(126);  
  
      if(flag_mm == LOW) 
      {
      encValue = set_mm;
      flag_mm = HIGH;
      }
            
      set_mm = encValue;
    
      if(set_mm<0) //Wertbegrenzung für Minutenanzeige MIN
      {
      encValue=59;
      }    
      
      if(set_mm>59) //Wertbegrenzung für Minutenanzeige MAX
      {
      encValue=0;
      }    
    
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                                
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt
          EEPROM.put(2, set_mm);       // den Wert ins EEPROM schreiben 
          dattime = 3;                  //zu Sekunden springen
          break;
        }      
      }

    break; //case '2' Ende * * * * *

   
    case 3: //Sekunden
    
      lcd.setCursor(13, 2);
      lcd.print(" ");       //mm Pfeil nach links löschen
      lcd.setCursor(14, 2); //mm Pfeil nach rechts löschen
      lcd.print(" ");
      lcd.setCursor(18, 3); //letzten beiden Ziffern bei Zahlenüberlauf nach unten löschen (Bsp. 255)
      lcd.print("  ");
      
      lcd.setCursor(16, 2);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(17, 2); //126 Pfeil nach rechts
      lcd.write(126);
             
      if(flag_ss == LOW) 
      {
      encValue = set_ss;
      flag_ss = HIGH;
      }
              
      set_ss = encValue;
      
      if(set_ss<0) //Wertbegrenzung für Sekundenanzeige MIN
      {
      encValue=59;
      }    
      
      if(set_ss>59) //Wertbegrenzung für Sekundenanzeige MAX
      {
      encValue=0;
      }     
      
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                                
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt
          EEPROM.put(4, set_ss);       // den Wert ins EEPROM schreiben 
          dattime = 7;                  //zu Tage springen
          break;
         }        
       }

    break; //case '3' Ende * * * * *   

  
    case 4: //Tage
    
      lcd.setCursor(16, 2);
      lcd.print(" ");       //mm Pfeil nach links löschen
      lcd.setCursor(17, 2); 
      lcd.print(" ");       //mm Pfeil nach rechts löschen
               
      lcd.setCursor(8, 1);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(9, 1); //126 Pfeil nach rechts
      lcd.write(126);
           
      if(flag_DD == LOW) 
      {
      encValue = set_DD;
      flag_DD = HIGH;
      }
            
      set_DD = encValue;
    
      if(set_DD<0) //Wertbegrenzung für Tagesanzeige MIN      
      {
      encValue=31;
      }    
      
      if(set_DD>31) //Wertbegrenzung für Tagesanzeige MAX
      {
      encValue=0;
      }    
             
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                               
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt
            EEPROM.put(6, set_DD);       // den Wert ins EEPROM schreiben 
            dattime = 5;                  //zu Monat springen
          break;
        }              
      }
    break; //case '4' Ende * * * * *   


    case 5: //Monat
    
      lcd.setCursor(8, 1);
      lcd.print(" ");       //mm Pfeil nach links löschen
      lcd.setCursor(9, 1); 
      lcd.print(" ");       //mm Pfeil nach rechts löschen
               
      lcd.setCursor(11, 1);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(12, 1); //126 Pfeil nach rechts
      lcd.write(126);
            
      if(flag_MM == LOW) 
      {
      encValue = set_MM;
      flag_MM = HIGH;
      }
            
      set_MM = encValue;
    
      if(set_MM<0) //Wertbegrenzung für Monatsanzeige MIN
      {
      encValue=12;
      }    
      
      if(set_MM>12) //Wertbegrenzung für Monatsanzeige MAX
      {
      encValue=0;
      }           
      
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                               
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt
            EEPROM.put(8, set_MM);       // den Wert ins EEPROM schreiben 
            dattime = 6;                  //zu Jahre springen
          break;
         }             
       }
    break; //case '5' Ende * * * * *   
  

    case 6: //Jahr
    
      lcd.setCursor(11, 1);
      lcd.print(" ");       //mm Pfeil nach links löschen
      lcd.setCursor(12, 1); 
      lcd.print(" ");       //mm Pfeil nach rechts löschen
                
      lcd.setCursor(14, 1);
      lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
      lcd.setCursor(17, 1); //126 Pfeil nach rechts
      lcd.write(126);
      lcd.setCursor(15, 1);
      lcd.print("--");
          
      if(flag_YYYY == LOW)
      {
      encValue = set_YYYY;
      flag_YYYY = HIGH;
      }
            
      set_YYYY = encValue;
    
      if(set_YYYY<2020) //Wertbegrenzung für Jahresanzeige MIN
      {
      encValue=2020;
      }    
      
      if(set_YYYY>3000) //Wertbegrenzung für Jahresanzeige MAX
      {
      encValue=3000;
      }    
        
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                                
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt
            EEPROM.put(10, set_YYYY);    // den Wert ins EEPROM schreiben 
            dattime = 1;                  //zum Speicherscreen springen
          break;
        }   
           
      }
    break; //case '6' Ende * * * * *    


    case 7: //Speicheranimation
    
      if(flag_save == LOW) 
      {
      LCDML.DISP_clear();

          for (i = txtMsg1.length() - 1; i >= 0; i--) //for each letter of the string starting from the last one.
          {
          startPoint1 = 0;
      
            for (j = 0; j < endPoint1; j++) //for each position on the LCD display
            {
            lcd.setCursor(startPoint1, 0);
            lcd.print(txtMsg1[i]);   
  
              if (startPoint1 != endPoint1 - 1) 
              {
              lcd.setCursor(startPoint1, 0);
              lcd.print(' ');
              }
            startPoint1++;
            }
          endPoint1--;  
          }
  
        for (k = txtMsg2.length() - 1; k >= 0; k--)
        {
        startPoint2 = 0;
      
          for (l = 0; l < endPoint2; l++) //for each position on the LCD display
          {
          lcd.setCursor(startPoint2, 2);
          lcd.print(txtMsg2[k]);   
  
            if (startPoint2 != endPoint2 - 1) 
            {
            lcd.setCursor(startPoint2, 2);
            lcd.print(' ');
            }
          startPoint2++;
          }
        endPoint2--;  
        }
   
      flag_save = HIGH;
      }
  
      if (b != ClickEncoder::Open) 
      {
        switch (b) 
        {                                 
          case ClickEncoder::Clicked:   // Button wurde einmal angeklickt                         
            rtc.setTime(set_hh, set_mm, set_ss);     // Set the time  (24hr format)
            rtc.setDate(set_DD, set_MM, set_YYYY);   // Set the date 
            LCDML.FUNC_goBackToMenu(1);
           break;
         }   
       }
    break; //case '7' Ende * * * * *      
  } //switch case Ende * * * * *

} // Loop Ende * * * * *

  encValue += encoder->getValue() * ValueStep; // Enkoderwert auslesen, mit ValueStep multiplizieren und zum Enkoderwert addieren
  
  if (encValue != lastValue) 
  {                       
  lastValue = encValue; // neuen Wert merken  
  }
  
  LCDML.FUNC_disableScreensaver();

} //mFunc_timeSetup Ende * * * * * 


// *********************************************************************
void mFunc_timer_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {

    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
    
    lcd.setCursor(0, 0);
    lcd.print(F("Datum:")); 

    lcd.setCursor(0, 3);
    lcd.print(F("Uhrzeit:"));  
  }


  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {       
  lcd.setCursor(8, 0);                // set cursor pos
  lcd.print(rtc.getDateStr());        // print actual date from RTC
  lcd.setCursor(10, 3);                // set cursor pos
  lcd.print(rtc.getTimeStr());        // print actual time from RTC
   
  ClickEncoder::Button b = encoder->getButton();     // den Button-Status abfragen
    if (b != ClickEncoder::Open) 
    {
      switch (b) 
      {  
        case ClickEncoder::Clicked:   // Button wurde einmal angeklickt                                     
          LCDML.FUNC_goBackToMenu(1);
        break;
      }   
    }  
  }
}
