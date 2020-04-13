/* ===================================================================== *
 *                                                                       *
 * Menu Callback Function                                                *
 *                                                                       *
 * ===================================================================== *
 *
 * EXAMPLE CODE:

// *********************************************************************
void your_function_name(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // setup
    // is called only if it is started

    // starts a trigger event for the loop function every 100 milliseconds
    LCDML.FUNC_setLoopInterval(100);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop
    // is called when it is triggered
    // - with LCDML_DISP_triggerMenu( milliseconds )
    // - with every button status change

    // check if any button is pressed (enter, up, down, left, right)
    if(LCDML.BT_checkAny()) {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // loop end
    // you can here reset some global vars or delete it
  }
}


 * ===================================================================== *
 */

unsigned long barnmillis = 0; //Hilfsvariable für Tempsens delay
byte type_s = 0;
byte addr[8];

// *********************************************************************
void mFunc_temp_barn(uint8_t param)
// *********************************************************************
{
  const unsigned long delaytime = 1200;  //Zeit in Millisekunden Tempsens delay
            
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  { 
        
  LCDML.FUNC_disableScreensaver();

    
  LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  lcd.setCursor(2, 1);                
  lcd.print("Temperatur Stall");        
  lcd.setCursor(6, 2);                
  lcd.print("-");   // aktuelle Temperatur
  lcd.setCursor(12, 2);                
  lcd.write(223);       //223 °-Symbol    ANM.:HD44780U Hitachi char library 
  lcd.setCursor(13, 2);
  lcd.print("C");        
 
  encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
  Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
  Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
  
  barnmillis = 0;
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
  ClickEncoder::Button b = encoder->getButton();     // den Button-Status abfragen

  if (barnmillis == 0)
  {
    if ( !tempsens.search(addr))
    {
      tempsens.reset_search();
      return;
    }
    
    // the first ROM byte indicates which chip
    switch (addr[0])
    {
        case 0x10:
          type_s = 1;
          break;
        case 0x28:
          type_s = 0;
          break;
        case 0x22:
          type_s = 0;
          break;
        default:
          return;
      } 
    
      tempsens.reset();
      tempsens.select(addr);
      tempsens.write(0x44, 1);        // start conversion, with parasite power on at the end
  
      barnmillis = millis();
    }
    else
    {
      if(millis() > (delaytime + barnmillis))  
      {
        barnmillis = 0;
  
        byte present = tempsens.reset();
        tempsens.select(addr);    
        tempsens.write(0xBE);         // Read Scratchpad
      
        byte data[12];
        for (byte i = 0; i < 9; i++) // we need 9 bytes
        {           
          data[i] = tempsens.read();
        }       
        
        int16_t raw = (data[1] << 8) | data[0];
        
        if (type_s) 
        {
          raw = raw << 3; // 9 bit resolution default
          if (data[7] == 0x10)
          {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
          }
        } 
        else 
        {
          byte cfg = (data[4] & 0x60);
          // at lower res, the low bits are undefined, so let's zero them
          if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
          else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
          else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
          //// default is 12 bit resolution, 750 ms conversion time
        }
        
        float celsius = ((float)raw / 16.0)+float(set_barnOffset);
  
        lcd.setCursor(6, 2);                
        lcd.print(celsius);   // aktuelle Temperatur
      }
    }

    if (b != ClickEncoder::Open) 
     {
        switch (b) 
       {                                 // und entsprechend darauf reagieren
           case ClickEncoder::Clicked:   // Button wurde einmal angeklickt                  
            LCDML.FUNC_goBackToMenu(1);
            break;
       }   
     }
  }
}

  int SStempoffs = 1;
  bool flag_SStempoffs = LOW;
  bool flag_savetempoffs = LOW;

  int startPoint5 = 0;   //set starting point
  int endPoint5 = 0;    //set ending point
  String txtMsg5 = "Temperatur Offset";
  int startPoint6 = 0;   //set starting point
  int endPoint6 = 0;    //set ending point
  String txtMsg6 = "gespeichert !";
  int ee = 0, ff = 0;
  int gg = 0, hh = 0;

// *********************************************************************
void mFunc_tempOffset(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {    
  LCDML.FUNC_setLoopInterval(100);

  encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
  Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
  Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen

  SStempoffs = 1;
  flag_SStempoffs = LOW;
  flag_savetempoffs = LOW;

  startPoint5 = 2;   //set starting point
  endPoint5 = 18;    //set ending point
  startPoint6 = 5;   //set starting point
  endPoint6 = 17;    //set ending point

  set_barnOffset = EEPROM.get(21, set_barnOffset);   // den Wert aus EEPROM lesen

  lcd.setCursor(2, 0);                
  lcd.print("Temperatur Stall");        
  lcd.setCursor(6, 1);                
  lcd.print("Offset:");   
  lcd.setCursor(11, 2);                
  lcd.write(223);       //223 °-Symbol    ANM.:HD44780U Hitachi char library 
  lcd.setCursor(12, 2);
  lcd.print("C");  
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  { 
  ClickEncoder::Button b = encoder->getButton();     // den Button-Status abfragen
 
    if (flag_savetempoffs == LOW)
    {
      if ((set_barnOffset < 10) && !(set_barnOffset < 0))
      {
      lcd.setCursor(8, 2);
      lcd.print(set_barnOffset);
      lcd.setCursor(6, 2);
      lcd.print("  ");
      }
        else if (set_barnOffset > 9)
        {
        lcd.setCursor(7, 2);
        lcd.print(set_barnOffset);
        }
          else if ((set_barnOffset < 0) && !(set_barnOffset < -9))
          {
          lcd.setCursor(7, 2);
          lcd.print(set_barnOffset);
          lcd.setCursor(6, 2);
          lcd.print(" ");
          }
            else if (set_barnOffset < -9)
            {
            lcd.setCursor(6, 2);
            lcd.print(set_barnOffset);
            }
    }
    
  switch (SStempoffs) 
  {  
  case 1: //Offset °C
               
        lcd.setCursor(3, 2);
        lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
        lcd.setCursor(4, 2);  //126 Pfeil nach rechts
        lcd.write(126);  
  
        if(flag_SStempoffs == LOW) 
        {
        encValue = set_barnOffset;
        flag_SStempoffs = HIGH;
        }
            
        set_barnOffset = encValue;
    
        if((set_barnOffset)<-20) //Wertbegrenzung für Offset [°C] MIN
        {
        encValue=-20;
        }
            
        if((set_barnOffset)>20) //Wertbegrenzung für Offset [°C] MAX     
        {
        encValue=20;
        }       
            
        if (b != ClickEncoder::Open) 
        {
          switch (b) 
          {                                          
          case ClickEncoder::Clicked:            // Button wurde einmal angeklickt
          EEPROM.put(21, set_barnOffset);        // den Wert ins EEPROM schreiben 
          SStempoffs = 2;                            //zu Speicheranimation springen
          break; 
          }   
        } 
  break;  //case '1' Ende * * * * * *

     case 2: //Speicheranimation
    
            if(flag_savetempoffs == LOW) 
            {
            LCDML.DISP_clear();

            for (ff = txtMsg5.length() - 1; ff >= 0; ff--) //for each letter of the string starting from the last one.
            {
            startPoint5 = 0;
             for (ee = 0; ee < endPoint5; ee++) //for each position on the LCD display
             {
             lcd.setCursor(startPoint5, 0);
             lcd.print(txtMsg5[ff]);   

               if (startPoint5 != endPoint5 - 1) 
               {
               lcd.setCursor(startPoint5, 0);
               lcd.print(' ');
               }
      
            startPoint5++;
             }
            endPoint5--;   
            }


      for (gg = txtMsg6.length() - 1; gg >= 0; gg--)
      {
       startPoint6 = 0;

    
        for (hh = 0; hh < endPoint6; hh++) //for each position on the LCD display
        {
        lcd.setCursor(startPoint6, 2);
        lcd.print(txtMsg6[gg]);   

          if (startPoint6 != endPoint6 - 1) 
          {
          lcd.setCursor(startPoint6, 2);
          lcd.print(' ');
          }
        startPoint6++;
        }
      endPoint6--;   
      }

  
            flag_savetempoffs = HIGH;
            }
  
    if (b != ClickEncoder::Open) 
    {
      switch (b) 
      {                                 
        case ClickEncoder::Clicked:   // Button wurde einmal angeklickt           
          LCDML.FUNC_goBackToMenu(1);
        break;
      }   
    }

          
     break; //case '2' Ende * * * * *
          
 }//switch case Ende * * * * *
    
} // Loop Ende * * * * *
  
  encValue += encoder->getValue() * ValueStep; // Enkoderwert auslesen, mit ValueStep multiplizieren und zum Enkoderwert addieren
  if (encValue != lastValue)  // wenn der neue Wert vom letzten Wert abweicht, dann...
  {                      
    lastValue = encValue;     // neuen Wert merken
  }
}

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


// *********************************************************************
void mFunc_lux(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {    
  LCDML.FUNC_disableScreensaver();
    
  LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
  Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
  Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
 
  lcd.setCursor(0, 0);
  lcd.print("Helligkeit Aussen:");
  lcd.setCursor(8, 1);
  lcd.print("lux");

  lcd.setCursor(0, 2);
  lcd.print("Helligkeit Stall:");
  lcd.setCursor(8, 3);
  lcd.print("lux");
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  { 
    if((luxInnen/2)<10)  
    {
    lcd.setCursor(3, 1);
    lcd.print("    "); 
    } 
      else if ((luxInnen/2)<100)
      {
      lcd.setCursor(4, 1);
      lcd.print("   "); 
      }
        else if ((luxInnen/2)<1000)
        {
        lcd.setCursor(5, 1);
        lcd.print("  "); 
        }
          else if ((luxInnen/2)<10000)
          {
          lcd.setCursor(6, 1);
          lcd.print(" "); 
          }
          
  lcd.setCursor(2, 1);
  lcd.print(luxInnen / 2);

    if((luxAussen/2)<10)  
    {
    lcd.setCursor(3, 3);
    lcd.print("    "); 
    } 
      else if ((luxAussen/2)<100)
      {
      lcd.setCursor(4, 3);
      lcd.print("   "); 
      }
        else if ((luxAussen/2)<1000)
        {
        lcd.setCursor(5, 3);
        lcd.print("  "); 
        }
          else if ((luxAussen/2)<10000)
          {
          lcd.setCursor(6, 3);
          lcd.print(" "); 
          }
          
  lcd.setCursor(2, 3);
  lcd.print(luxAussen / 2);  

  
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


bool flag_lcdoffonce = 0; //flag für screensaver 
unsigned long offtimeMS = 0; //Zeit im screensaver bis zur Abschaltung
unsigned long screenmillis = 0; //Hilfsvariable für backlightOff
unsigned long offtimeS = 0;  //Zeit in Sekunden bis zum backlightOff
unsigned long ETmillis = 0;  //Hilfsvariable für ETscreenoff Anzeige
// *********************************************************************
void mFunc_screensaver(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
  flag_lcdoffonce = LOW;
  offtimeMS = 15000; //Zeit bis zum Display abschalten in Millisekunden
  screenmillis = millis();
  offtimeS = offtimeMS / 1000; // Verbleibende Zeit bis zum Abschalten in Sekunden
    
  // update LCD content
  lcd.setCursor(2, 0); 
  lcd.print("ENERGIESPARMODUS"); 
  lcd.setCursor(1, 1); 
  lcd.print("");
  lcd.setCursor(3, 2); 
  lcd.print("Abschaltung in:");
  lcd.setCursor(8, 3);
  lcd.print("Sekunden");
         
  LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())
  {  
    if(millis()> (ETmillis + 1000)&& flag_lcdoffonce == LOW) 
    {
    offtimeS--; 
    lcd.setCursor(4, 3);
    lcd.print(offtimeS);   //Verbleibende Zeit bis zur Displayabschaltung anzeigen
    ETmillis = millis();    
      if(offtimeS<10)  
      {
      lcd.setCursor(4, 3);
      lcd.print(" ");
      lcd.setCursor(5, 3);
      lcd.print(offtimeS);
      }    
        if(offtimeS < 2) // Sekunden auf Sekunde ändern bei weniger als 2 Sekunden Rest
        {
        lcd.setCursor(8, 3);
        lcd.print("Sekunde");
        lcd.setCursor(15, 3);
        lcd.print(" ");        
        }
    }
   
   
   if (flag_lcdoffonce==LOW)
   {
      if(millis() > offtimeMS + screenmillis)     
      { 
      screenmillis = millis();
      lcd.noBacklight();    //Display abschalten
      flag_lcdoffonce = HIGH;
      lcd.clear();  
      }
   }
    
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
    lcd.backlight();
    LCDML.FUNC_goBackToMenu();  // leave this function
    }
  }

  if(LCDML.FUNC_close())
  {
    LCDML.MENU_goRoot();
  }
}


// *********************************************************************
void mFunc_back(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
  // end function and go an layer back
  LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
  }
}


// *********************************************************************
void mFunc_goToRootMenu(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
  // go to root and display menu
  LCDML.MENU_goRoot();
  }
}


  byte CASEluxBegin = 1;

  bool flag_SAVEontime = LOW;  
  bool flag_starttimeH = LOW;
  bool flag_starttimeM = LOW; 
  bool flag_startdurationH = LOW;
  bool flag_startdurationM = LOW;

  int startPoint9 = 2;   //set starting point
  int endPoint9 = 18;    //set ending point
  String txtMsg9 = "Einschaltparameter";
  int startPoint10 = 5;   //set starting point
  int endPoint10 = 17;    //set ending point
  String txtMsg10 = "gespeichert !";
  int mm, nn;
  int oo, pp;


// ****************
void timeOverflow() //Hilfsfunktion für Minutenüberlauf bei estEndtime-Berechnung
// ****************
{
  preEndM = set_starttimeM + set_startdurationM + set_suntimeM + set_enddurationM;

  if((preEndM > 59) && (preEndM < 120))
  {
  EndM = 1;
  estEndtimeM = preEndM - 60;
  }
    else if ((preEndM > 119) && (preEndM < 180))
    {
    EndM = 2;
    estEndtimeM = preEndM - 120; 
    }
      else if ((preEndM > 179) && (preEndM < 240))
      {
      EndM = 3;
      estEndtimeM = preEndM - 180; 
      }
        else
        {
        EndM = 0;
        estEndtimeM = preEndM;
        }
        
  estEndtimeH = set_starttimeH + set_startdurationH + set_suntimeH + set_enddurationH + EndM;
}    
// *********************************************************************
void mFunc_luxBegin(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEluxBegin = 1;
    flag_starttimeH = LOW;
    flag_starttimeM = LOW;
    flag_startdurationH = LOW;
    flag_startdurationM = LOW;
    flag_SAVEontime = LOW;
    startPoint9 = 2;   
    endPoint9 = 19;    
    startPoint10 = 5;   
    endPoint10 = 17;   
    
    set_starttimeH = EEPROM.get(42, set_starttimeH);  
    set_starttimeM = EEPROM.get(43, set_starttimeM);   
    set_startdurationH = EEPROM.get(44, set_startdurationH);   
    set_startdurationM = EEPROM.get(45, set_startdurationM);   
   
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    lcd.setCursor(13, 0);                
    lcd.print("hh:mm");        
    lcd.setCursor(0, 1);                
    lcd.print("Startzeit :");
    lcd.setCursor(15, 1);                
    lcd.print(":");    
    lcd.setCursor(0, 2);                
    lcd.print("Startdauer:");
    lcd.setCursor(15, 2);                
    lcd.print(":");
    lcd.setCursor(0, 3);                
    lcd.print("err.Endzeit:");
    lcd.setCursor(15, 3);                
    lcd.print(":"); 
    lcd.setCursor(14, 3);
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();

    timeOverflow();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEontime == LOW)
      {

        lcd.setCursor(15, 1);
        lcd.print(":");
        lcd.setCursor(15, 2);
        lcd.print(":");
        lcd.setCursor(18, 1);
        lcd.print(" ");
        lcd.setCursor(18, 2);
        lcd.print(" ");
        
        if ((set_starttimeH < 10) && (set_starttimeH >= 0))
        {
        lcd.setCursor(14, 1);
        lcd.print(set_starttimeH);
        lcd.setCursor(13, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 1);
          lcd.print(set_starttimeH);
          } 

        if ((set_starttimeM < 10) && (set_starttimeM >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_starttimeM);
        lcd.setCursor(16, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 1);
          lcd.print(set_starttimeM);
          }

        if ((set_startdurationH < 10) && (set_startdurationH >= 0))
        {
        lcd.setCursor(14, 2);
        lcd.print(set_startdurationH);
        lcd.setCursor(13, 2);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 2);
          lcd.print(set_startdurationH);
          } 

        if ((set_startdurationM < 10) && (set_startdurationM >= 0))
        {
        lcd.setCursor(17, 2);
        lcd.print(set_startdurationM);
        lcd.setCursor(16, 2);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 2);
          lcd.print(set_startdurationM);
          } 

        if ((estEndtimeH < 10) && (estEndtimeH >= 0))
        {
        lcd.setCursor(14, 3);
        lcd.print(estEndtimeH);
        lcd.setCursor(13, 3);
        lcd.print("0");
        }
          if (estEndtimeH > 23)
          {         
          lcd.setCursor(13, 3);
          lcd.print("ee");
          }
            else 
            {
            lcd.setCursor(13, 3);
            lcd.print(estEndtimeH);
            }
                    
        if ((estEndtimeM < 10) && (estEndtimeM >= 0))
        {
        lcd.setCursor(17, 3);
        lcd.print(estEndtimeM);
        lcd.setCursor(16, 3);
        lcd.print("0");
        }         
            else 
            {
            lcd.setCursor(16, 3);
            lcd.print(estEndtimeM);
            } 
      }
    
      switch (CASEluxBegin) 
      {
        case 1: //StarttimeH
                 
          lcd.setCursor(11, 1);
          lcd.print("*");       
    
          if(flag_starttimeH == LOW) 
          {
          encValue = set_starttimeH;
          flag_starttimeH = HIGH;
          }
              
          set_starttimeH = encValue;
      
          if((set_starttimeH) < 1) //Wertbegrenzung für Starttime [h] MIN
          {
          encValue=0;
          }    
          
          if((set_starttimeH) > 23) //Wertbegrenzung für Starttime [h] MAX
          {
          encValue=23;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(42, set_starttimeH);
                CASEluxBegin = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *
  
        case 2: //StarttimeM

          lcd.setCursor(11, 1);
          lcd.print(" ");         // * von Stundenverstellung Startzeit löschen
          
          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_starttimeM == LOW) 
          {
          encValue = set_starttimeM;
          flag_starttimeM = HIGH;
          }
              
          set_starttimeM = encValue;
      
          if((set_starttimeM) < 1) //Wertbegrenzung für Starttime [min] MIN
          {
          encValue=0;
          }    
          
          if((set_starttimeM) > 59) //Wertbegrenzung für Starttime [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(43, set_starttimeM);
                CASEluxBegin = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *

        case 3: //StartdurationH

          lcd.setCursor(19, 1);
          lcd.print(" ");         // * von Minutenverstellung Startzeit löschen
          
          lcd.setCursor(11, 2);
          lcd.print("*");       
    
          if(flag_startdurationH == LOW) 
          {
          encValue = set_startdurationH;
          flag_startdurationH = HIGH;
          }
              
          set_startdurationH = encValue;
      
          if((set_startdurationH) < 1) //Wertbegrenzung für Startduration [h] MIN
          {
          encValue=0;
          }    
          
          if((set_startdurationH) > 23) //Wertbegrenzung für Startduration [h] MAX
          {
          encValue=23;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(44, set_startdurationH);
                CASEluxBegin = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        case 4: //StartdurationM

          lcd.setCursor(11, 2);
          lcd.print(" ");         // * von Minutenverstellung Startzeit löschen
          
          lcd.setCursor(19, 2);
          lcd.print("*");       
    
          if(flag_startdurationM == LOW) 
          {
          encValue = set_startdurationM;
          flag_startdurationM = HIGH;
          }
              
          set_startdurationM = encValue;
      
          if((set_startdurationM) < 1) //Wertbegrenzung für Startduration [min] MIN
          {
          encValue=0;
          }    
          
          if((set_startdurationM) > 59) //Wertbegrenzung für Startduration [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(45, set_startdurationM);
                CASEluxBegin = 5; 
              break; 
            }   
          } 
        break;  //case '4' Ende * * * * * *
     
        
        case 5: //Speicheranimation
      
        if(flag_SAVEontime == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (nn = txtMsg9.length() - 1; nn >= 0; nn--) //for each letter of the string starting from the last one.
        {
        startPoint9 = 0;
             
          for (mm = 0; mm < endPoint9; mm++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint9, 0);
            lcd.print(txtMsg9[nn]);   
      
              if (startPoint9 != endPoint9 - 1) 
              {
              lcd.setCursor(startPoint9, 0);
              lcd.print(' ');
              }
                            
            startPoint9++;
          }
          
        endPoint9--;      
        }
  
  
         for (oo = txtMsg10.length() - 1; oo >= 0; oo--)
         {
         startPoint10 = 0;
     
          for (pp = 0; pp < endPoint10; pp++)
          {
            lcd.setCursor(startPoint10, 2);
            lcd.print(txtMsg10[oo]);   
      
              if (startPoint10 != endPoint10 - 1) 
              {
              lcd.setCursor(startPoint10, 2);
              lcd.print(' ');
              }
              
            startPoint10++;       
          }
          
         endPoint10--;    
         }    
         
        flag_SAVEontime = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '5' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }

}

  byte CASEluxEnd = 1;

  bool flag_SAVEofftime = LOW;  
  bool flag_suntimeH = LOW;
  bool flag_suntimeM = LOW; 
  bool flag_enddurationH = LOW;
  bool flag_enddurationM = LOW;
  
  int startPoint11 = 2;   //set starting point
  int endPoint11 = 18;    //set ending point
  String txtMsg11 = "Ausschaltparameter";
  int startPoint12 = 5;   //set starting point
  int endPoint12 = 17;    //set ending point
  String txtMsg12 = "gespeichert !";
  int qq, rr;
  int ss, tt;

// *********************************************************************
void mFunc_luxEnd(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEluxEnd = 1;
    flag_suntimeH = LOW;
    flag_suntimeM = LOW;
    flag_enddurationH = LOW;
    flag_enddurationM = LOW;
    flag_SAVEofftime = LOW;
    startPoint11 = 2;   
    endPoint11 = 19;    
    startPoint12 = 5;   
    endPoint12 = 17;   
    
    set_suntimeH = EEPROM.get(46, set_suntimeH);  
    set_suntimeM = EEPROM.get(47, set_suntimeM);   
    set_enddurationH = EEPROM.get(48, set_enddurationH);   
    set_enddurationM = EEPROM.get(49, set_enddurationM);   
   
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    lcd.setCursor(13, 0);                
    lcd.print("hh:mm");        
    lcd.setCursor(0, 1);                
    lcd.print("Sonnenzeit:");
    lcd.setCursor(15, 1);                
    lcd.print(":");    
    lcd.setCursor(0, 2);                
    lcd.print("Enddauer  :");
    lcd.setCursor(15, 2);                
    lcd.print(":");
    lcd.setCursor(0, 3);                
    lcd.print("err.Endzeit:");
    lcd.setCursor(15, 3);                
    lcd.print(":"); 
    lcd.setCursor(14, 3);              
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();

    timeOverflow();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEofftime == LOW)
      {
        lcd.setCursor(15, 1);
        lcd.print(":");
        lcd.setCursor(15, 2);
        lcd.print(":");
        lcd.setCursor(18, 1);
        lcd.print(" ");
        lcd.setCursor(18, 2);
        lcd.print(" ");
        lcd.setCursor(18, 3);
        lcd.print(" ");
        
        if ((set_suntimeH < 10) && (set_suntimeH >= 0))
        {
        lcd.setCursor(14, 1);
        lcd.print(set_suntimeH);
        lcd.setCursor(13, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 1);
          lcd.print(set_suntimeH);
          } 

        if ((set_suntimeM < 10) && (set_suntimeM >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_suntimeM);
        lcd.setCursor(16, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 1);
          lcd.print(set_suntimeM);
          }

        if ((set_enddurationH < 10) && (set_enddurationH >= 0))
        {
        lcd.setCursor(14, 2);
        lcd.print(set_enddurationH);
        lcd.setCursor(13, 2);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 2);
          lcd.print(set_enddurationH);
          } 

        if ((set_enddurationM < 10) && (set_enddurationM >= 0))
        {
        lcd.setCursor(17, 2);
        lcd.print(set_enddurationM);
        lcd.setCursor(16, 2);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 2);
          lcd.print(set_enddurationM);
          }
        
        if ((estEndtimeH < 10) && (estEndtimeH >= 0))
        {
        lcd.setCursor(14, 3);
        lcd.print(estEndtimeH);
        lcd.setCursor(13, 3);
        lcd.print("0");
        }
          if (estEndtimeH > 23)
          {         
          lcd.setCursor(13, 3);
          lcd.print("ee");
          }
            else 
            {
            lcd.setCursor(13, 3);
            lcd.print(estEndtimeH);
            }
             
        if ((estEndtimeM < 10) && (estEndtimeM >= 0))
        {     
        lcd.setCursor(17, 3);
        lcd.print(estEndtimeM);
        lcd.setCursor(16, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 3);
          lcd.print(estEndtimeM);
          }                       
      }
    
      switch (CASEluxEnd) 
      {
        case 1: //SuntimeH
                 
          lcd.setCursor(11, 1);
          lcd.print("*");       
    
          if(flag_suntimeH == LOW) 
          {
          encValue = set_suntimeH;
          flag_suntimeH = HIGH;
          }
              
          set_suntimeH = encValue;
      
          if((set_suntimeH) < 1) //Wertbegrenzung für Suntime [h] MIN
          {
          encValue=0;
          }    
          
          if((set_suntimeH) > 23) //Wertbegrenzung für Suntime [h] MAX
          {
          encValue=23;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(46, set_suntimeH);
                CASEluxEnd = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *
  
        case 2: //SuntimeM

          lcd.setCursor(11, 1);
          lcd.print(" ");         // * von Stundenverstellung Sonnenzeit löschen
          
          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_suntimeM == LOW) 
          {
          encValue = set_suntimeM;
          flag_suntimeM = HIGH;
          }
              
          set_suntimeM = encValue;
      
          if((set_suntimeM) < 1) //Wertbegrenzung für Suntime [m] MIN
          {
          encValue=0;
          }    
          
          if((set_suntimeM) > 59) //Wertbegrenzung für Suntime [m] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(47, set_suntimeM);
                CASEluxEnd = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *

        case 3: //EnddurationH

          lcd.setCursor(19, 1);
          lcd.print(" ");         // * von Minutenverstellung Endzeit löschen
          
          lcd.setCursor(11, 2);
          lcd.print("*");       
    
          if(flag_enddurationH == LOW) 
          {
          encValue = set_enddurationH;
          flag_enddurationH = HIGH;
          }
              
          set_enddurationH = encValue;
      
          if((set_enddurationH) < 1) //Wertbegrenzung für Endduration [h] MIN
          {
          encValue=0;
          }    
          
          if((set_enddurationH) > 23) //Wertbegrenzung für Endduration [h] MAX
          {
          encValue=23;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(48, set_enddurationH);
                CASEluxEnd = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        case 4: //EnddurationM

          lcd.setCursor(11, 2);
          lcd.print(" ");         // * von Minutenverstellung Startzeit löschen
          
          lcd.setCursor(19, 2);
          lcd.print("*");       
    
          if(flag_enddurationM == LOW) 
          {
          encValue = set_enddurationM;
          flag_enddurationM = HIGH;
          }
              
          set_enddurationM = encValue;
      
          if((set_enddurationM) < 1) //Wertbegrenzung für Endduration [min] MIN
          {
          encValue=0;
          }    
          
          if((set_enddurationM) > 59) //Wertbegrenzung für Endduration [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(49, set_enddurationM);
                CASEluxEnd = 5; 
              break; 
            }   
          } 
        break;  //case '4' Ende * * * * * *
     
        
        case 5: //Speicheranimation
      
        if(flag_SAVEofftime == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (rr = txtMsg11.length() - 1; rr >= 0; rr--) //for each letter of the string starting from the last one.
        {
        startPoint11 = 0;
             
          for (qq = 0; qq < endPoint11; qq++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint11, 0);
            lcd.print(txtMsg11[rr]);   
      
              if (startPoint11 != endPoint11 - 1) 
              {
              lcd.setCursor(startPoint11, 0);
              lcd.print(' ');
              }
                            
            startPoint11++;
          }
          
        endPoint11--;      
        }
  
  
         for (ss = txtMsg12.length() - 1; ss >= 0; ss--)
         {
         startPoint12 = 0;
     
          for (tt = 0; tt < endPoint12; tt++)
          {
            lcd.setCursor(startPoint12, 2);
            lcd.print(txtMsg12[ss]);   
      
              if (startPoint12 != endPoint12 - 1) 
              {
              lcd.setCursor(startPoint12, 2);
              lcd.print(' ');
              }
              
            startPoint12++;       
          }
          
         endPoint12--;    
         }    
         
        flag_SAVEofftime = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '5' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }

}


  byte CASEhystON = 1;

  bool flag_SAVEhystON = LOW;  
  bool flag_hystluxON = LOW;
  bool flag_hysttimeONM = LOW; 
  bool flag_hysttimeONS = LOW;
  
  int startPoint13 = 2;   //set starting point
  int endPoint13 = 18;    //set ending point
  String txtMsg13 = "Hystereseparameter";
  int startPoint14 = 5;   //set starting point
  int endPoint14 = 17;    //set ending point
  String txtMsg14 = "gespeichert !";
  int uu, vv;
  int ww, xx;

// *********************************************************************
void mFunc_HystON(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEhystON = 1;
    flag_hystluxON = LOW;
    flag_hysttimeONM = LOW;
    flag_hysttimeONS = LOW;
    flag_SAVEhystON = LOW;
    startPoint13 = 2;   
    endPoint13 = 19;    
    startPoint14 = 5;   
    endPoint14 = 17;   

    set_hystluxON = EEPROM.get(50, set_hystluxON); 
    set_hysttimeONM = EEPROM.get(52, set_hysttimeONM);  
    set_hysttimeONS = EEPROM.get(53, set_hysttimeONS);   
           
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    lcd.setCursor(15, 0);                
    lcd.print("lux");        
    lcd.setCursor(0, 1);                
    lcd.print("Limit:");
    lcd.setCursor(13, 2);                
    lcd.print("mm:ss");            
    lcd.setCursor(0, 3);                
    lcd.print("Wartezeit:");
    lcd.setCursor(15, 3);                
    lcd.print(":");              
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEhystON == LOW)
      {
        lcd.setCursor(15, 2);
        lcd.print(":");
        lcd.setCursor(15, 3);
        lcd.print(":");
        lcd.setCursor(18, 3);
        lcd.print(" ");      
        
        if ((set_hystluxON < 10) && (set_hystluxON >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_hystluxON);
        lcd.setCursor(14, 1);
        lcd.print("   ");
        }
          else if ((set_hystluxON > 9) && (set_hystluxON < 100))
          {
          lcd.setCursor(16, 1);
          lcd.print(set_hystluxON);
          lcd.setCursor(15, 1);
          lcd.print(" ");
          }
            else if ((set_hystluxON > 99) && (set_hystluxON < 1000))
            {
            lcd.setCursor(15, 1);
            lcd.print(set_hystluxON);
            lcd.setCursor(14, 1);
            lcd.print(" ");
            }
              else
              {
              lcd.setCursor(14, 1);
              lcd.print(set_hystluxON);
              }
         

        if ((set_hysttimeONM < 10) && (set_hysttimeONM >= 0))
        {
        lcd.setCursor(14, 3);
        lcd.print(set_hysttimeONM);
        lcd.setCursor(13, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 3);
          lcd.print(set_hysttimeONM);
          }

        if ((set_hysttimeONS < 10) && (set_hysttimeONS >= 0))
        {
        lcd.setCursor(17, 3);
        lcd.print(set_hysttimeONS);
        lcd.setCursor(16, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 3);
          lcd.print(set_hysttimeONS);
          }            
      }
    
      switch (CASEhystON) 
      {
        case 1: //HystluxON
                 
          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_hystluxON == LOW) 
          {
          encValue = set_hystluxON;
          flag_hystluxON = HIGH;
          }
              
          set_hystluxON = encValue;
      
          if((set_hystluxON) < 1) //Wertbegrenzung für HystLuxON [lx] MIN
          {
          encValue=0;
          }    
          
          if((set_hystluxON) > 9990) //Wertbegrenzung für HystLuxON [lx] MAX
          {
          encValue=9990;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(50, set_hystluxON);
                CASEhystON = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *
  
        case 2: //HystVerzögerungONM

          lcd.setCursor(19, 1);
          lcd.print(" ");         // * von Luxverstellung Hysterese EIN löschen
          
          lcd.setCursor(11, 3);
          lcd.print("*");       
    
          if(flag_hysttimeONM == LOW) 
          {
          encValue = set_hysttimeONM;
          flag_hysttimeONM = HIGH;
          }
              
          set_hysttimeONM = encValue;
      
          if((set_hysttimeONM) < 1) //Wertbegrenzung für HysttimeON [min] MIN
          {
          encValue=0;
          }    
          
          if((set_hysttimeONM) > 59) //Wertbegrenzung für HysttimeON [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(52, set_hysttimeONM);
                CASEhystON = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *

        case 3: //HystVerzögerungONS

          lcd.setCursor(11, 3);
          lcd.print(" ");         // * von Luxverstellung Hysterese EIN löschen
          
          lcd.setCursor(19, 3);
          lcd.print("*");       
    
          if(flag_hysttimeONS == LOW) 
          {
          encValue = set_hysttimeONS;
          flag_hysttimeONS = HIGH;
          }
              
          set_hysttimeONS = encValue;
      
          if((set_hysttimeONS) < 1) //Wertbegrenzung für HysttimeON [s] MIN
          {
          encValue=0;
          }    
          
          if((set_hysttimeONS) > 59) //Wertbegrenzung für HysttimeON [s] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(53, set_hysttimeONS);
                CASEhystON = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        
        case 4: //Speicheranimation
      
        if(flag_SAVEhystON == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (vv = txtMsg13.length() - 1; vv >= 0; vv--) //for each letter of the string starting from the last one.
        {
        startPoint13 = 0;
             
          for (uu = 0; uu < endPoint13; uu++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint13, 0);
            lcd.print(txtMsg13[vv]);   
      
              if (startPoint13 != endPoint13 - 1) 
              {
              lcd.setCursor(startPoint13, 0);
              lcd.print(' ');
              }
                            
            startPoint13++;
          }
          
        endPoint13--;      
        }
  
  
         for (ww = txtMsg14.length() - 1; ww >= 0; ww--)
         {
         startPoint14 = 0;
     
          for (xx = 0; xx < endPoint14; xx++)
          {
            lcd.setCursor(startPoint14, 2);
            lcd.print(txtMsg14[ww]);   
      
              if (startPoint14 != endPoint14 - 1) 
              {
              lcd.setCursor(startPoint14, 2);
              lcd.print(' ');
              }
              
            startPoint14++;       
          }
          
         endPoint14--;    
         }    
         
        flag_SAVEhystON = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '5' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }

}


  byte CASEhystOFF = 1;

  bool flag_SAVEhystOFF = LOW;  
  bool flag_hystluxOFF = LOW;
  bool flag_hysttimeOFFM = LOW; 
  bool flag_hysttimeOFFS = LOW;
  
  int startPoint15 = 2;   //set starting point
  int endPoint15 = 18;    //set ending point
  String txtMsg15 = "Hystereseparameter";
  int startPoint16 = 5;   //set starting point
  int endPoint16 = 17;    //set ending point
  String txtMsg16 = "gespeichert !";
  int yy, zz;
  int aaa, bbb;

// *********************************************************************
void mFunc_HystOFF(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEhystOFF = 1;
    flag_hystluxOFF = LOW;
    flag_hysttimeOFFM = LOW;
    flag_hysttimeOFFS = LOW;
    flag_SAVEhystOFF = LOW;
    startPoint15 = 2;   
    endPoint15 = 19;    
    startPoint16 = 5;   
    endPoint16 = 17;   

    set_hystluxOFF = EEPROM.get(54, set_hystluxOFF); 
    set_hysttimeOFFM = EEPROM.get(56, set_hysttimeOFFM);  
    set_hysttimeOFFS = EEPROM.get(57, set_hysttimeOFFS);   
           
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    lcd.setCursor(15, 0);                
    lcd.print("lux");        
    lcd.setCursor(0, 1);                
    lcd.print("Limit:");
    lcd.setCursor(13, 2);                
    lcd.print("mm:ss");            
    lcd.setCursor(0, 3);                
    lcd.print("Wartezeit:");
    lcd.setCursor(15, 3);                
    lcd.print(":");              
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEhystOFF == LOW)
      {
        lcd.setCursor(15, 2);
        lcd.print(":");
        lcd.setCursor(15, 3);
        lcd.print(":");
        lcd.setCursor(18, 3);
        lcd.print(" ");      
        
        if ((set_hystluxOFF < 10) && (set_hystluxOFF >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_hystluxOFF);
        lcd.setCursor(14, 1);
        lcd.print("   ");
        }
          else if ((set_hystluxOFF > 9) && (set_hystluxOFF < 100))
          {
          lcd.setCursor(16, 1);
          lcd.print(set_hystluxOFF);
          lcd.setCursor(15, 1);
          lcd.print(" ");
          }
            else if ((set_hystluxOFF > 99) && (set_hystluxOFF < 1000))
            {
            lcd.setCursor(15, 1);
            lcd.print(set_hystluxOFF);
            lcd.setCursor(14, 1);
            lcd.print(" ");
            }
              else
              {
              lcd.setCursor(14, 1);
              lcd.print(set_hystluxOFF);
              }
         

        if ((set_hysttimeOFFM < 10) && (set_hysttimeOFFM >= 0))
        {
        lcd.setCursor(14, 3);
        lcd.print(set_hysttimeOFFM);
        lcd.setCursor(13, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 3);
          lcd.print(set_hysttimeOFFM);
          }

        if ((set_hysttimeOFFS < 10) && (set_hysttimeOFFS >= 0))
        {
        lcd.setCursor(17, 3);
        lcd.print(set_hysttimeOFFS);
        lcd.setCursor(16, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 3);
          lcd.print(set_hysttimeOFFS);
          }            
      }
    
      switch (CASEhystOFF) 
      {
        case 1: //HystluxOFF
                 
          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_hystluxOFF == LOW) 
          {
          encValue = set_hystluxOFF;
          flag_hystluxOFF = HIGH;
          }
              
          set_hystluxOFF = encValue;
      
          if((set_hystluxOFF) < 1) //Wertbegrenzung für HystLuxOFF [lx] MIN
          {
          encValue=0;
          }    
          
          if((set_hystluxOFF) > 9990) //Wertbegrenzung für HystLuxOFF [lx] MAX
          {
          encValue=9990;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(54, set_hystluxOFF);
                CASEhystOFF = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *
  
        case 2: //HystVerzögerungOFFM

          lcd.setCursor(19, 1);
          lcd.print(" ");         // * von Luxverstellung Hysterese AUS löschen
          
          lcd.setCursor(11, 3);
          lcd.print("*");       
    
          if(flag_hysttimeOFFM == LOW) 
          {
          encValue = set_hysttimeOFFM;
          flag_hysttimeOFFM = HIGH;
          }
              
          set_hysttimeOFFM = encValue;
      
          if((set_hysttimeOFFM) < 1) //Wertbegrenzung für HysttimeOFF [min] MIN
          {
          encValue=0;
          }    
          
          if((set_hysttimeOFFM) > 59) //Wertbegrenzung für HysttimeOFF [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(56, set_hysttimeOFFM);
                CASEhystOFF = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *

        case 3: //HystVerzögerungOFFS

          lcd.setCursor(11, 3);
          lcd.print(" ");         // * von Luxverstellung Hysterese AUS löschen
          
          lcd.setCursor(19, 3);
          lcd.print("*");       
    
          if(flag_hysttimeOFFS == LOW) 
          {
          encValue = set_hysttimeOFFS;
          flag_hysttimeOFFS = HIGH;
          }
              
          set_hysttimeOFFS = encValue;
      
          if((set_hysttimeOFFS) < 1) //Wertbegrenzung für HysttimeOFF [s] MIN
          {
          encValue=0;
          }    
          
          if((set_hysttimeOFFS) > 59) //Wertbegrenzung für HysttimeOFF [s] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(57, set_hysttimeOFFS);
                CASEhystOFF = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        
        case 4: //Speicheranimation
      
        if(flag_SAVEhystOFF == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (zz = txtMsg15.length() - 1; zz >= 0; zz--) //for each letter of the string starting from the last one.
        {
        startPoint15 = 0;
             
          for (yy = 0; yy < endPoint15; yy++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint15, 0);
            lcd.print(txtMsg15[zz]);   
      
              if (startPoint15 != endPoint15 - 1) 
              {
              lcd.setCursor(startPoint15, 0);
              lcd.print(' ');
              }
                            
            startPoint15++;
          }
          
        endPoint15--;      
        }
  
  
         for (aaa = txtMsg16.length() - 1; aaa >= 0; aaa--)
         {
         startPoint16 = 0;
     
          for (bbb = 0; bbb < endPoint16; bbb++)
          {
            lcd.setCursor(startPoint16, 2);
            lcd.print(txtMsg16[aaa]);   
      
              if (startPoint16 != endPoint16 - 1) 
              {
              lcd.setCursor(startPoint16, 2);
              lcd.print(' ');
              }
              
            startPoint16++;       
          }
          
         endPoint16--;    
         }    
         
        flag_SAVEhystOFF = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '5' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }

}

  int CASEpid = 1;
  bool flag_pidfaktor = LOW;
  bool flag_SAVEpidfaktor = LOW;
  bool flag_sampletime = LOW;
  bool flag_Kp = LOW;
  bool flag_Ki = LOW;
  bool flag_Kd = LOW;

  int startPoint7 = 2;   //set starting point
  int endPoint7 = 18;    //set ending point
  String txtMsg7 = "Reglerparameter";
  int startPoint8 = 5;   //set starting point
  int endPoint8 = 17;    //set ending point
  String txtMsg8 = "gespeichert !";
  int ii, jj;
  int kk, ll;
  double ValueStepPID = 0.01; //Faktor für Wertänderung pro Encoderstep
  
// *********************************************************************
void mFunc_luxPID(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEpid = 1;
    flag_pidfaktor = LOW;
    flag_SAVEpidfaktor = LOW;
    flag_sampletime = LOW;
    flag_Kp = LOW;
    flag_Ki = LOW;
    flag_Kd = LOW;
    startPoint7 = 2;   
    endPoint7 = 18;    
    startPoint8 = 5;   
    endPoint8 = 17;   
    ValueStepPID = 0.01;
    
    set_PIDkp = EEPROM.get(30, set_PIDkp);   
    set_PIDki = EEPROM.get(34, set_PIDki);   
    set_PIDkd = EEPROM.get(38, set_PIDkd);
    set_pidSampletime = EEPROM.get(28, set_pidSampletime);
   
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    lcd.setCursor(0, 0);                
    lcd.print("Taktrate:");        
    lcd.setCursor(0, 1);                
    lcd.print("prop. Kp:");   
    lcd.setCursor(0, 2);                
    lcd.print("int.  Ki:");       
    lcd.setCursor(0, 3);
    lcd.print("diff. Kd:");    
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEpidfaktor == LOW)
      {
        if ((set_pidSampletime < 100)&&(set_pidSampletime > 9))
        {
        lcd.setCursor(14, 0);
        lcd.print(set_pidSampletime);
        lcd.setCursor(13, 0);
        lcd.print(" ");
        }
          else if (set_pidSampletime > 999)
          {
          lcd.setCursor(12, 0);
          lcd.print(set_pidSampletime);
          } 
            else if ((set_pidSampletime < 999)&&(set_pidSampletime > 99))
            {
            lcd.setCursor(13, 0);
            lcd.print(set_pidSampletime);
            lcd.setCursor(12, 0);
            lcd.print(" ");
            }                    
              else if (set_pidSampletime < 10)
              {
              lcd.setCursor(15, 0);
              lcd.print(set_pidSampletime);
              lcd.setCursor(14, 0);
              lcd.print(" ");
              lcd.setCursor(16, 0);
              lcd.print(" ");
              } 
                else 
                {
                lcd.setCursor(13, 0);
                lcd.print(set_pidSampletime);    
                }
        
        if (set_PIDkp < 0.01)
        {
        lcd.setCursor(12, 1);
        lcd.print(set_PIDkp);  
        lcd.setCursor(16, 1);
        lcd.print("  ");  
        }       
          else if ((set_PIDkp > 0.01) && (set_PIDkp < 9.99))
          {
          lcd.setCursor(12, 1);
          lcd.print(set_PIDkp);
          lcd.setCursor(11, 1);
          lcd.print(" ");
          }            
            else if ((set_PIDkp > 9.99) && (set_PIDkp < 99.99))
            {
            lcd.setCursor(11, 1);
            lcd.print(set_PIDkp);
            lcd.setCursor(10, 1);
            lcd.print(" ");
            }
              else if ((set_PIDkp > 99.99) && (set_PIDkp < 999.99))
              {
              lcd.setCursor(10, 1);
              lcd.print(set_PIDkp);
              lcd.setCursor(9, 1);
              lcd.print(" ");
              }
                else if ((set_PIDkp > 999.99) && (set_PIDkp < 9999.99))
                {
                lcd.setCursor(9, 1);
                lcd.print(set_PIDkp);
                }

        if (set_PIDki < 0.01)
        {
        lcd.setCursor(12, 2);
        lcd.print(set_PIDki);  
        lcd.setCursor(16, 2);
        lcd.print("  ");  
        }       
          else if ((set_PIDki > 0.01) && (set_PIDki < 9.99))
          {
          lcd.setCursor(12, 2);
          lcd.print(set_PIDki);
          lcd.setCursor(11, 2);
          lcd.print(" ");
          }            
            else if ((set_PIDki > 9.99) && (set_PIDki < 99.99))
            {
            lcd.setCursor(11, 2);
            lcd.print(set_PIDki);
            lcd.setCursor(10, 2);
            lcd.print(" ");
            }
              else if ((set_PIDki > 99.99) && (set_PIDki < 999.99))
              {
              lcd.setCursor(10, 2);
              lcd.print(set_PIDki);
              lcd.setCursor(9, 2);
              lcd.print(" ");
              }
                else if ((set_PIDki > 999.99) && (set_PIDki < 9999.99))
                {
                lcd.setCursor(9, 2);
                lcd.print(set_PIDki);
                }

        if (set_PIDkd < 0.01)
        {
        lcd.setCursor(12, 3);
        lcd.print(set_PIDkd);  
        lcd.setCursor(16, 3);
        lcd.print("  ");  
        }       
          else if ((set_PIDkd > 0.01) && (set_PIDkd < 9.99))
          {
          lcd.setCursor(12, 3);
          lcd.print(set_PIDkd);
          lcd.setCursor(11, 3);
          lcd.print(" ");
          }            
            else if ((set_PIDkd > 9.99) && (set_PIDkd < 99.99))
            {
            lcd.setCursor(11, 3);
            lcd.print(set_PIDkd);
            lcd.setCursor(10, 3);
            lcd.print(" ");
            }
              else if ((set_PIDkd > 99.99) && (set_PIDkd < 999.99))
              {
              lcd.setCursor(10, 3);
              lcd.print(set_PIDkd);
              lcd.setCursor(9, 3);
              lcd.print(" ");
              }
                else if ((set_PIDkd > 999.99) && (set_PIDkd < 9999.99))
                {
                lcd.setCursor(9, 3);
                lcd.print(set_PIDkd);
                }                  
      }
    
      switch (CASEpid) 
      {
        case 1: //Sampletime
                 
          lcd.setCursor(18, 0);
          lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
          lcd.setCursor(19, 0);  //126 Pfeil nach rechts
          lcd.write(126);  
    
          if(flag_sampletime == LOW) 
          {
          encValue = set_pidSampletime;
          flag_sampletime = HIGH;
          }
              
          set_pidSampletime = encValue;
      
          if((set_pidSampletime) < 2) //Wertbegrenzung für Sampletime [ms] MIN
          {
          encValue=1;
          }    
          
          if((set_pidSampletime) > 5000) //Wertbegrenzung für Sampletime [ms] MAX
          {
          encValue=5000;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(28, set_pidSampletime);
                CASEpid = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *
  
        case 2: //PID-Faktor Kp

          lcd.setCursor(18, 0);
          lcd.print(" ");         // Samplerate Pfeil nach links löschen
          lcd.setCursor(19, 0);   // Samplerate Pfeil nach rechts löschen
          lcd.print(" ");  
                
          lcd.setCursor(18, 1);
          lcd.write(127);        //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
          lcd.setCursor(19, 1);  //126 Pfeil nach rechts
          lcd.write(126);  
    
          if(flag_Kp == LOW) 
          {
          encValue = set_PIDkp;
          flag_Kp = HIGH;
          }
              
          set_PIDkp = encValue;
      
          if((set_PIDkp) < 0.01) //Wertbegrenzung für Reglerfaktor Kp  MIN
          {
          encValue=0;
          }    
          
          if((set_PIDkp)>5000) //Wertbegrenzung für Reglerfaktor Kp  MAX
          {
          encValue=5000;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                 EEPROM.put(30, set_PIDkp);
                CASEpid = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *

        case 3: //PID-Faktor Ki

          lcd.setCursor(18, 1);
          lcd.print(" ");         // Reglerfaktor Kp Pfeil nach links löschen
          lcd.setCursor(19, 1);   // Reglerfaktor Kp nach rechts löschen
          lcd.print(" ");  
                
          lcd.setCursor(18, 2);
          lcd.write(127);        //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
          lcd.setCursor(19, 2);  //126 Pfeil nach rechts
          lcd.write(126);  
    
          if(flag_Ki == LOW) 
          {
          encValue = set_PIDki;
          flag_Ki = HIGH;
          }
              
          set_PIDki = encValue;
      
          if((set_PIDki) < 0.01) //Wertbegrenzung für Reglerfaktor Ki  MIN
          {
          encValue=0;
          }    
          
          if((set_PIDki)>5000) //Wertbegrenzung für Reglerfaktor Ki  MAX
          {
          encValue=5000;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                 EEPROM.put(34, set_PIDki);
                CASEpid = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        case 4: //PID-Faktor Kd

          lcd.setCursor(18, 2);
          lcd.print(" ");         // Reglerfaktor Ki Pfeil nach links löschen
          lcd.setCursor(19, 2);   // Reglerfaktor Ki nach rechts löschen
          lcd.print(" ");  
                
          lcd.setCursor(18, 3);
          lcd.write(127);        //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
          lcd.setCursor(19, 3);  //126 Pfeil nach rechts
          lcd.write(126);  
    
          if(flag_Kd == LOW) 
          {
          encValue = set_PIDkd;
          flag_Kd = HIGH;
          }
              
          set_PIDkd = encValue;
      
          if((set_PIDkd) < 0.01) //Wertbegrenzung für Reglerfaktor Kd  MIN
          {
          encValue=0;
          }    
          
          if((set_PIDkd)>5000) //Wertbegrenzung für Reglerfaktor Kd  MAX
          {
          encValue=5000;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                 EEPROM.put(38, set_PIDkd);
                CASEpid = 5; 
              break; 
            }   
          } 
        break;  //case '4' Ende * * * * * *
     
        
        case 5: //Speicheranimation
      
        if(flag_SAVEpidfaktor == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (jj = txtMsg7.length() - 1; jj >= 0; jj--) //for each letter of the string starting from the last one.
        {
        startPoint7 = 0;
             
          for (ii = 0; ii < endPoint7; ii++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint7, 0);
            lcd.print(txtMsg7[jj]);   
      
              if (startPoint7 != endPoint7 - 1) 
              {
              lcd.setCursor(startPoint7, 0);
              lcd.print(' ');
              }
                            
            startPoint7++;
          }
          
        endPoint7--;      
        }
  
  
         for (kk = txtMsg8.length() - 1; kk >= 0; kk--)
         {
         startPoint8 = 0;
     
          for (ll = 0; ll < endPoint8; ll++)
          {
            lcd.setCursor(startPoint8, 2);
            lcd.print(txtMsg8[kk]);   
      
              if (startPoint8 != endPoint8 - 1) 
              {
              lcd.setCursor(startPoint8, 2);
              lcd.print(' ');
              }
              
            startPoint8++;       
          }
          
         endPoint8--;    
         }    
         
        flag_SAVEpidfaktor = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '5' Ende * * * * * 
      } //switch case Ende * * * * *

      if(CASEpid==1)
        {
        Faktor = 1;       
        }
          else
          {
          Faktor = ValueStepPID;
          }
          
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * Faktor; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }
  
    if(LCDML.FUNC_close())      // ****** STABLE END *********
    {    
    }
}


  byte CASEdoors = 1;

  bool flag_SAVEdoors = LOW;  
  bool flag_doorsOpenH = LOW;
  bool flag_doorsOpenM = LOW;
  
  
  int startPoint17 = 2;   //set starting point
  int endPoint17 = 16;    //set ending point
  String txtMsg17 = "Torparameter";
  int startPoint18 = 5;   //set starting point
  int endPoint18 = 17;    //set ending point
  String txtMsg18 = "gespeichert !";
  int ccc, ddd;
  int eee, fff;

// *********************************************************************
void mFunc_doors(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEdoors = 1;
    flag_SAVEdoors = LOW;  
    flag_doorsOpenH = LOW;
    flag_doorsOpenM = LOW;
    startPoint17 = 2;   
    endPoint17 = 16;    
    startPoint18 = 5;   
    endPoint18 = 17;   
    
    set_doorsOpenH = EEPROM.get(58, set_doorsOpenH);  
    set_doorsOpenM = EEPROM.get(59, set_doorsOpenM); 
           
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
         
    lcd.setCursor(13, 0);                
    lcd.print("hh:mm");   
    lcd.setCursor(0, 1);                
    lcd.print("Tor Auf:");                                   
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();
      
    ClickEncoder::Button b = encoder->getButton();
       
      if (flag_SAVEdoors == LOW)
      {
        lcd.setCursor(15, 1);
        lcd.print(":");
        lcd.setCursor(18, 1);                
        lcd.print(" ");                        

        if ((set_doorsOpenH < 10) && (set_doorsOpenH >= 0))
        {
        lcd.setCursor(14, 1);
        lcd.print(set_doorsOpenH);
        lcd.setCursor(13, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 1);
          lcd.print(set_doorsOpenH);
          }

        if ((set_doorsOpenM < 10) && (set_doorsOpenM >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_doorsOpenM);
        lcd.setCursor(16, 1);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 1);
          lcd.print(set_doorsOpenM);
          }
      }
    
      switch (CASEdoors) 
      {  
        case 1: //DoorsOpenH
          
          lcd.setCursor(11, 1);
          lcd.print("*");       
    
          if(flag_doorsOpenH == LOW) 
          {
          encValue = set_doorsOpenH;
          flag_doorsOpenH = HIGH;
          }
              
          set_doorsOpenH = encValue;
      
          if((set_doorsOpenH) < 1) //Wertbegrenzung für DoorsOpenH [h] MIN
          {
          encValue=0;
          }    
          
          if((set_doorsOpenH) > 23) //Wertbegrenzung für DoorsOpenH [h] MAX
          {
          encValue=23;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(58, set_doorsOpenH);
                CASEdoors = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *

        case 2: //DoorsOpenM

          lcd.setCursor(11, 1);
          lcd.print(" ");         // * von Luxverstellung Hysterese AUS löschen
          
          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_doorsOpenM == LOW) 
          {
          encValue = set_doorsOpenM;
          flag_doorsOpenM = HIGH;
          }
              
          set_doorsOpenM = encValue;
      
          if((set_doorsOpenM) < 1) //Wertbegrenzung für DoorsOpenM [min] MAX
          {
          encValue=0;
          }    
          
          if((set_doorsOpenM) > 59) //Wertbegrenzung für DoorsOpenM [min] MIN
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(59, set_doorsOpenM);
                CASEdoors = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *
               
       
        case 3: //Speicheranimation
      
        if(flag_SAVEdoors == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (ddd = txtMsg17.length() - 1; ddd >= 0; ddd--) //for each letter of the string starting from the last one.
        {
        startPoint17 = 0;
             
          for (ccc = 0; ccc < endPoint17; ccc++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint17, 0);
            lcd.print(txtMsg17[ddd]);   
      
              if (startPoint17 != endPoint17 - 1) 
              {
              lcd.setCursor(startPoint17, 0);
              lcd.print(' ');
              }
                            
            startPoint17++;
          }
          
        endPoint17--;      
        }
  
  
         for (eee = txtMsg18.length() - 1; eee >= 0; eee--)
         {
         startPoint18 = 0;
     
          for (fff = 0; fff < endPoint18; fff++)
          {
            lcd.setCursor(startPoint18, 2);
            lcd.print(txtMsg18[eee]);   
      
              if (startPoint18 != endPoint18 - 1) 
              {
              lcd.setCursor(startPoint18, 2);
              lcd.print(' ');
              }
              
            startPoint18++;       
          }
          
         endPoint18--;    
         }    
         
        flag_SAVEdoors = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '3' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }
}

 
  
  byte CASEdoorsClose = 1;

  bool flag_SAVEdoorsClose = LOW;  
  bool flag_doorsCloselux = LOW;
  bool flag_doorsCloseM = LOW; 
  bool flag_doorsCloseS = LOW;
  
  int startPoint19 = 2;   //set starting point
  int endPoint19 = 18;    //set ending point
  String txtMsg19 = "Torparameter";
  int startPoint20 = 5;   //set starting point
  int endPoint20 = 17;    //set ending point
  String txtMsg20 = "gespeichert !";
  int ggg, hhh;
  int iii, jjj;

// *********************************************************************
void mFunc_Hystdoors(uint8_t param)
// *********************************************************************
{
    if(LCDML.FUNC_setup())          // ****** SETUP *********
    {
    CASEdoorsClose = 1;
    flag_SAVEdoorsClose = LOW;  
    flag_doorsCloselux = LOW;
    flag_doorsCloseM = LOW; 
    flag_doorsCloseS = LOW;
    startPoint19 = 2;   
    endPoint19 = 16;    
    startPoint20 = 5;   
    endPoint20 = 17;   

    set_doorsCloseLux = EEPROM.get(60, set_doorsCloseLux);
    set_doorsCloseM = EEPROM.get(62, set_doorsCloseM);  
    set_doorsCloseS = EEPROM.get(63, set_doorsCloseS);    
           
    LCDML.FUNC_setLoopInterval(100);
    
    encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
    Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
    Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen
     
    
    lcd.setCursor(0, 0);                
    lcd.print("Aussensensor   lux");
    lcd.setCursor(0, 1);                
    lcd.print("Tor Zu:");
    lcd.setCursor(13, 2);                
    lcd.print("mm:ss");   
    lcd.setCursor(0, 3);                
    lcd.print("Wartezeit:");
    lcd.setCursor(15, 3);                
    lcd.print(":");        
    }

    if(LCDML.FUNC_loop())           // ****** LOOP *********
    {  
    LCDML.FUNC_disableScreensaver();
      
    ClickEncoder::Button b = encoder->getButton();
      
      if (flag_SAVEdoorsClose == LOW)
      { 
        lcd.setCursor(18, 1);
        lcd.print(" ");  
        lcd.setCursor(15, 3);
        lcd.print(":");
        lcd.setCursor(18, 3);
        lcd.print(" ");  

        if ((set_doorsCloseLux < 10) && (set_doorsCloseLux >= 0))
        {
        lcd.setCursor(17, 1);
        lcd.print(set_doorsCloseLux);
        lcd.setCursor(14, 1);
        lcd.print("   ");
        }
          else if ((set_doorsCloseLux > 9) && (set_doorsCloseLux < 100))
          {
          lcd.setCursor(16, 1);
          lcd.print(set_doorsCloseLux);
          lcd.setCursor(15, 1);
          lcd.print(" ");
          }
            else if ((set_doorsCloseLux > 99) && (set_doorsCloseLux < 1000))
            {
            lcd.setCursor(15, 1);
            lcd.print(set_doorsCloseLux);
            lcd.setCursor(14, 1);
            lcd.print(" ");
            }
              else
              {
              lcd.setCursor(14, 1);
              lcd.print(set_doorsCloseLux);
              }          
         
        if ((set_doorsCloseM < 10) && (set_doorsCloseM >= 0))
        {
        lcd.setCursor(14, 3);
        lcd.print(set_doorsCloseM);
        lcd.setCursor(13, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(13, 3);
          lcd.print(set_doorsCloseM);
          }

        if ((set_doorsCloseS < 10) && (set_doorsCloseS >= 0))
        {
        lcd.setCursor(17, 3);
        lcd.print(set_doorsCloseS);
        lcd.setCursor(16, 3);
        lcd.print("0");
        }
          else 
          {
          lcd.setCursor(16, 3);
          lcd.print(set_doorsCloseS);
          }            
      }
    
      switch (CASEdoorsClose) 
      {
        case 1: //DoorsCloseLux

          lcd.setCursor(19, 1);
          lcd.print("*");       
    
          if(flag_doorsCloselux == LOW) 
          {
          encValue = set_doorsCloseLux;
          flag_doorsCloselux = HIGH;
          }
              
          set_doorsCloseLux = encValue;
      
          if((set_doorsCloseLux) < 1) //Wertbegrenzung für DoorsCloseLux [lx] MIN
          {
          encValue=0;
          }    
          
          if((set_doorsCloseLux) > 9990) //Wertbegrenzung für DoorsCloseLux [lx] MAX
          {
          encValue=9990;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(60, set_doorsCloseLux);
                CASEdoorsClose = 2; 
              break; 
            }   
          } 
        break;  //case '1' Ende * * * * * *

  
        case 2: //HystdoorsCloseM

          lcd.setCursor(19, 1);
          lcd.print(" ");         // * von Luxverstellung DoorsCloseM löschen
          
          lcd.setCursor(11, 3);
          lcd.print("*");       
    
          if(flag_doorsCloseM == LOW) 
          {
          encValue = set_doorsCloseM;
          flag_doorsCloseM = HIGH;
          }
              
          set_doorsCloseM = encValue;
      
          if((set_doorsCloseM) < 1) //Wertbegrenzung für HystDoorsClose [min] MIN
          {
          encValue=0;
          }    
          
          if((set_doorsCloseM) > 59) //Wertbegrenzung für HystDoorsClose [min] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(62, set_doorsCloseM);
                CASEdoorsClose = 3; 
              break; 
            }   
          } 
        break;  //case '2' Ende * * * * * *


        case 3: //HystdoorsCloseS

          lcd.setCursor(11, 3);
          lcd.print(" ");         // * von Hysterese DoorsCloseM löschen
          
          lcd.setCursor(19, 3);
          lcd.print("*");       
    
          if(flag_doorsCloseS == LOW) 
          {
          encValue = set_doorsCloseS;
          flag_doorsCloseS = HIGH;
          }
              
          set_doorsCloseS = encValue;
      
          if((set_doorsCloseS) < 1) //Wertbegrenzung für HystDoorsClose [s] MIN
          {
          encValue=0;
          }    
          
          if((set_doorsCloseS) > 59) //Wertbegrenzung für HystDoorsClose [s] MAX
          {
          encValue=59;
          }
                      
          if (b != ClickEncoder::Open) 
          {
            switch (b) 
            {                                         
              case ClickEncoder::Clicked:            
                EEPROM.put(63, set_doorsCloseS);
                CASEdoorsClose = 4; 
              break; 
            }   
          } 
        break;  //case '3' Ende * * * * * *

        
        case 4: //Speicheranimation
      
        if(flag_SAVEdoorsClose == LOW) 
        {
    
        LCDML.DISP_clear();
  
        for (hhh = txtMsg19.length() - 1; hhh >= 0; hhh--) //for each letter of the string starting from the last one.
        {
        startPoint19 = 0;
             
          for (ggg = 0; ggg < endPoint19; ggg++) //for each position on the LCD display
          {
            lcd.setCursor(startPoint19, 0);
            lcd.print(txtMsg19[hhh]);   
      
              if (startPoint19 != endPoint19 - 1) 
              {
              lcd.setCursor(startPoint19, 0);
              lcd.print(' ');
              }
                            
            startPoint19++;
          }
          
        endPoint19--;      
        }
  
  
         for (iii = txtMsg20.length() - 1; iii >= 0; iii--)
         {
         startPoint20 = 0;
     
          for (jjj = 0; jjj < endPoint20; jjj++)
          {
            lcd.setCursor(startPoint20, 2);
            lcd.print(txtMsg20[iii]);   
      
              if (startPoint20 != endPoint20 - 1) 
              {
              lcd.setCursor(startPoint20, 2);
              lcd.print(' ');
              }
              
            startPoint20++;       
          }
          
         endPoint20--;    
         }    
         
        flag_SAVEdoorsClose = HIGH;
        }   
          
        if (b != ClickEncoder::Open) 
             {
             switch (b) 
               {                                
                 case ClickEncoder::Clicked:              
                  LCDML.FUNC_goBackToMenu(1);
                 break;
               }   
             }
        break;  //case '4' Ende * * * * * 
      } //switch case Ende * * * * *
      
    } // LOOP Ende * * * * * 
    
    encValue += encoder->getValue() * ValueStep; 
    if (encValue != lastValue) 
    {                       
    lastValue = encValue; 
    }

}


  int SStime = 1;
  bool flag_SStime = LOW;
  bool flag_saveSS = LOW;

  int startPoint3 = 2;   //set starting point
  int endPoint3 = 16;    //set ending point
  String txtMsg3 = "Abschaltzeit";
  int startPoint4 = 5;   //set starting point
  int endPoint4 = 17;    //set ending point
  String txtMsg4 = "gespeichert !";
  int aa = 0, bb = 0;
  int cc = 0, dd = 0;

// *********************************************************************
void mFunc_screensaverSetup(uint8_t param) //Zeit bis zum Einschalten des Bildschirmschoners
// *********************************************************************
{
 
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {    
  LCDML.FUNC_setLoopInterval(100);    
  SStime = 1;
  flag_SStime = LOW;
  flag_saveSS = LOW;

  startPoint3 = 2;   //set starting point
  endPoint3 = 16;    //set ending point
  startPoint4 = 5;   //set starting point
  endPoint4 = 17;    //set ending point
  
  set_screensaverTime = EEPROM.get(20, set_screensaverTime); //Zeit bis zum Einschalten des Bildschirmschoners
 
  encoder = new ClickEncoder(enc_PinA, enc_PinB, enc_PinC, enc_Step); // den Dreh-Enkoder initialisieren
  Timer1.initialize(1000);                      // den Interrupt-Timer fuer den Dreh-Enkoder initialisieren
  Timer1.attachInterrupt(timerIsr);             // und die Interrupt-Funktion festlegen

  lcd.setCursor(2, 0);
  lcd.print(F("Energiesparmodus")); 

  lcd.setCursor(3, 1);
  lcd.print(F("Verzoegerung"));  

  lcd.setCursor(2, 2);
  lcd.print(F("Display Infotext:"));  

  lcd.setCursor(10, 3);
  lcd.print(F("Sekunden"));  
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {    
  ClickEncoder::Button b = encoder->getButton();     // den Button-Status abfragen
    if(flag_saveSS == LOW) 
    {  
      //Sekunden   
      if (set_screensaverTime < 10)
      {
      lcd.setCursor(8, 3);
      lcd.print(set_screensaverTime);
      lcd.setCursor(6, 3);
      lcd.print("  ");
      }
        else if ((set_screensaverTime > 9) && !(set_screensaverTime >99))
        {
        lcd.setCursor(7, 3);
        lcd.print(set_screensaverTime);
        }
          else
          {
          lcd.setCursor(6, 3);
          lcd.print(set_screensaverTime);
          lcd.setCursor(4, 3);
          lcd.print("  ");
          lcd.setCursor(9, 3);
          lcd.print(" ");
          }
    }

    switch (SStime) 
    { 
      case 1: //Sekunden
                 
        lcd.setCursor(3, 3);
        lcd.write(127);       //127 Pfeil nach links    ANM.:HD44780U Hitachi char library 
        lcd.setCursor(4, 3);  //126 Pfeil nach rechts
        lcd.write(126);  
    
        if(flag_SStime == LOW) 
        {
        encValue = set_screensaverTime;
        flag_SStime = HIGH;
        }
                
        set_screensaverTime = encValue;
        
        if((set_screensaverTime)<0) //Wertbegrenzung für Ausschaltzeit [s] MIN
        {
        encValue=300;
        }    
        
        if((set_screensaverTime)>300) //Wertbegrenzung für Ausschaltzeit [s] MAX 
        {
        encValue=0;
        }           
            
        if (b != ClickEncoder::Open) 
        {
          switch (b) 
          {                                         
            case ClickEncoder::Clicked:            // Button wurde einmal angeklickt
            EEPROM.put(20, set_screensaverTime);   // den Wert ins EEPROM schreiben 
            SStime = 2;                            //zu Speicheranimation springen
            break; 
          }   
         } 
      break;  //case '1' Ende * * * * * *
  
        case 2: //Speicheranimation
      
          if(flag_saveSS == LOW) 
          {
          LCDML.DISP_clear();
          
            for (bb = txtMsg3.length() - 1; bb >= 0; bb--) //for each letter of the string starting from the last one.
            {
            startPoint3 = 0; 
              
              for (aa = 0; aa < endPoint3; aa++)  //for each position on the LCD display
              {
              lcd.setCursor(startPoint3, 0);
              lcd.print(txtMsg3[bb]);   
                
                if (startPoint3 != endPoint3 - 1) 
                {
                lcd.setCursor(startPoint3, 0);
                lcd.print(' ');
                }
              startPoint3++;
              }
            endPoint3--;   
            }
  
           for (cc = txtMsg4.length() - 1; cc >= 0; cc--)
           {
           startPoint4 = 0;   
            
            for (dd = 0; dd < endPoint4; dd++) //for each position on the LCD display
            {
            lcd.setCursor(startPoint4, 2);
            lcd.print(txtMsg4[cc]);   
    
              if (startPoint4 != endPoint4 - 1) 
              {
              lcd.setCursor(startPoint4, 2);
              lcd.print(' ');
              }
            startPoint4++;
            }
          endPoint4--; 
          }
    
          flag_saveSS = HIGH;
          }
    
            if (b != ClickEncoder::Open) 
            {
              switch (b) 
              {                                 // und entsprechend darauf reagieren
                case ClickEncoder::Clicked:   // Button wurde einmal angeklickt           
                  LCDML.FUNC_goBackToMenu(1);
                break;
              }   
             }       
        break; //case '2' Ende * * * * *
            
    }//switch case Ende * * * * *
    
  } // Loop Ende * * * * *
  
  encValue += encoder->getValue() * ValueStep;       // Enkoderwert auslesen, mit ValueStep multiplizieren und zum Enkoderwert addieren
  
  if (encValue != lastValue) // wenn der neue Wert vom letzten Wert abweicht, dann...
  {                       
  lastValue = encValue; // neuen Wert merken
  }
}

 // *********************************************************************
void mFunc_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {    
  LCDML.FUNC_setLoopInterval(100);

  lcd.setCursor(2, 0);
  lcd.print("Easy Chicken 1.0");
  lcd.setCursor(4, 1); 
  lcd.print("copyright by"); 
  lcd.setCursor(0, 2); 
  lcd.print("Reinhard Poechacker");
  lcd.setCursor(0, 3); 
  lcd.print("Tel.: 0664 100 66 39");    
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {    
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
    LCDML.FUNC_goBackToMenu(1);  // leave this function
    }
  }
}
