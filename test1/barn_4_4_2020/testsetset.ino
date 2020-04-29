/*
 

 
 
 if ((timeNow >= doorsOpenTimeSEC) && (flag_doorsOpen == LOW))
    {
      if(TorAuf == LOW) 
      {
      TorAuf = HIGH; // Tor aufmachen
      flag_doorsOpen = HIGH;
      } 
    }
    
    if (flag_doorsOpen == HIGH && (luxAussen/2 < set_doorsCloseLux) && flag_closeMillis == LOW)
    {
    TorZu_millis = millis();
    flag_closeMillis = HIGH;    
    Serial.print("TorZumillis");  
    Serial.println(TorZu_millis);
    }
    
    if (luxAussen/2 > set_doorsCloseLux)
    {
    flag_closeMillis = LOW;
    }
     
    if(millis() > (doorsCloseDelaytimeMILLISEC + TorZu_millis))  
    {
    Serial.print("TODO");
    TorZu = HIGH;
    flag_doorsOpen = LOW;
    }
 
 
 
 ######################################################
 
 
 
 
 //Zeitschaltuhr Tore
  t = rtc.getTime();
  unsigned long HOUR = (unsigned long)t.hour * 3600;
  unsigned long MIN = (unsigned long)t.min * 60;
  unsigned long SEC = (unsigned long)t.sec; 
  unsigned long timeNow = HOUR + MIN + SEC; //aktuelle Tagesuhrzeit in Sekunden  
  //Serial.println(timeNow);
  
 // unsigned long ON = 50;
  //unsigned long OFF = 65;
  bool TorAuf;

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
  */
