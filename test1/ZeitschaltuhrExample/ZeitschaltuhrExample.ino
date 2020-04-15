void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:


//Zeitschaltuhr Tore
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



}
