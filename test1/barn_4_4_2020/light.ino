

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
