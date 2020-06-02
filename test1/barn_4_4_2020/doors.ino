  
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
void mFunc_doors(uint8_t param) //UHRZEIT FÜR ÖFFNUNG DER TORE
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
void mFunc_Hystdoors(uint8_t param) //HYSTERESE FÜR TORSCHLIESSUNG (UNTERSCHREITUNG VON X-LUX + WARTEZEIT)
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
