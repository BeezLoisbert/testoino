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
