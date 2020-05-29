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
  lcd.print("-----");   // aktuelle Temperatur
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
