

/*--------------------------------------- INCLUDES --------------------------------------------- */
// Include library for Temp/Humidity sensor
//#include <DHT_U.h>

// Include library for LORAWAN connection
#include <lmic.h>

// Include library for Cayenne Cloud which is used to prepare payloads in correct form
#include <CayenneLPP.h>

// Include hardware abstraction layer for arduino environment
#include <hal/hal.h>

// Include library for HX711 (Load sensor)
#include <HX711.h>

//Include library for I2C
#include <Wire.h>

//Include library for BH1750 (Lux sensor)
#include <ErriezBH1750.h>

//Include library for BMP280 sensor (temp/humidity/airpressure)
#include <BlueDot_BME280.h>


/*---------------------------------------- DEFINES ---------------------------------------------- */
// Temp/Humidity sensor
//#define DHTPIN 5           // Digital pin connected to the DHT sensor 
//#define DHTTYPE    DHT11   // Type DHT 11 is used as sensor

//Photoresistors
//#define photoresistor1 A0 //Analogport vom Photoresistor
//#define photoresistor2 A1

// HX711 circuit wiring
#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 4


/*------------------------------------ GLOBAL VARIABLES ----------------------------------------- */

/* //Photoresistor
 
float PRvalueLive1 =0; //livewerte am Eingang
float PRvalueLive2 = photoresistor2;
float PRvalueAdd1 = 0; //addierender Wert bis zum erreichen von PRmeasureCountGoal
float PRvalueAdd2 = 0;
float PR_AVG1 = 0; // Finaler Durchschnittswert
float PR_AVG2 = 0;
int PRavgXcount1 = 0; //anzahl der bereits addierten Messungen
int PRavgXcount2 = 0;
int PRmeasureCountGoal1 = 10; //Anzahl der Messwerte für Durchschnitt
int PRmeasureCountGoal2 =10;
*/

//Lux Sensor

uint16_t luxInnen;
uint16_t luxAussen;

//Waage
float rawData1WAAGE ; //livewert HX711 Waage

//Temp/Humidity/Pressure Sensor
int bme1Detected = 0;                                    //Checks if Sensor 1 is available
//int bme2Detected = 0;                                    //Checks if Sensor 2 is available



//TTN Settings##################################################

// Credentials which are necessary to connect to TTN (modify if you use other TTN account or device)
static const u1_t NWKSKEY[16] = { 0x76, 0x99, 0xE0, 0x63, 0x90, 0x0A, 0x93, 0x3F, 0x7D, 0x72, 0x8D, 0x7E, 0xE4, 0x5C, 0x5B, 0x07 };
static const u1_t APPSKEY[16] = { 0xA3, 0xD2, 0x85, 0xB8, 0x09, 0xBD, 0x80, 0xDE, 0xD4, 0x85, 0x47, 0x8E, 0xF2, 0xAA, 0x5B, 0xF7 };
static const u4_t DEVADDR = 0x260117B6 ;

// Schedule TX messages over Lora within defined seconds
const unsigned TX_INTERVAL = 150;

// Delay which is minimal needed between sensor readings
uint32_t delayMS;


//DHT11 Settings###############################################

// Instantiate object for Temp/Humidity sensor class
//DHT_Unified dht(DHTPIN, DHTTYPE);

// Instantiate object for DHT sensor which is used to read out infos from library
//sensor_t sensor;


//Cayenne Settings############################################

// Instantiate Cayenne format object with maximum of 51 bytes lenght
CayenneLPP lpp(51);

// Instantiate sendjob object from LMIC library
static osjob_t sendjob;


//HX711 Settings##############################################

// Instantiate HX711 object
HX711 scale;


//BH170 Settings##############################################
//Instantiate BH170 objects
BH1750 HelligkeitInnen(LOW);
BH1750 HelligkeitAussen(HIGH);
// ADDR line LOW/open:  I2C address 0x23 (0x46 including R/W bit) [default]
// ADDR line HIGH:      I2C address 0x5C (0xB8 including R/W bit)

//BMP280 Settings############################################
BlueDot_BME280 bme1;                                     //Object for Sensor 1
//BlueDot_BME280 bme2;                                   //Object for Sensor 2   


// Declare struct for Dragino Shield pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = {2, 6, 7},
};



/*-------------------------------------- PROTOTYPES ------------------------------------------- */




/*--------------------------------------- CALLBACKS -------------------------------------------- */
// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }



/*--------------------------------------- FUNCTIONS -------------------------------------------- */

///////////////////////////////////////////////////////////////////////////////////////////////////
// Setup - will be processed once at the beginning
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {


  // Initialize serial interface with 9600 Baud-Rate
  Serial.begin(115200);
 // Initialize I2C bus
    Wire.begin();

//LUX Sensor $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 
 // Initialize sensor in continues mode, high 0.5 lx resolution
    HelligkeitInnen.begin(ModeContinuous, ResolutionHigh);
    
 // Initialize sensor in continues mode, high 0.5 lx resolution
    HelligkeitAussen.begin(ModeContinuous, ResolutionHigh);

    // Start conversion
    HelligkeitInnen.startConversion();
    HelligkeitAussen.startConversion();
//LUX Sensor ENDE $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




//Temp/Humidity/Pressure++++++++++++++++++++++++++++++++++++++++++++
  
  bme1.parameter.I2CAddress = 0x76;                    //I2C Address for Sensor 1 (bme1)
  //bme2.parameter.I2CAddress = 0x76;                    //I2C Address for Sensor 2 (bme2)
  
  bme1.parameter.communication = 0;                    //I2C communication for Sensor 1 (bme1)
  //bme2.parameter.communication = 0;                    //I2C communication for Sensor 2 (bme2)

  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)
  
   bme1.parameter.sensorMode = 0b11;                    //Setup Sensor mode for Sensor 1
   //bme2.parameter.sensorMode = 0b11;                    //Setup Sensor mode for Sensor 2 

  //The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  //In short, a high factor value means less noise, but measurements are also less responsive
  //You can play with these values and check the results!
  //In doubt just leave on default

  //0b000:      factor 0 (filter off)
  //0b001:      factor 2
  //0b010:      factor 4
  //0b011:      factor 8
  //0b100:      factor 16 (default value)

    bme1.parameter.IIRfilter = 0b100;                   //IIR Filter for Sensor 1
    //bme2.parameter.IIRfilter = 0b100;                   //IIR Filter for Sensor 2

  //Next you'll define the oversampling factor for the measurements
  //Again, higher values mean less noise, but slower responses
  //If you don't want to measure , set the oversampling to zero

  //0b000:      factor 0 (Disable humidity/pressure/temperatrue measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

    bme1.parameter.humidOversampling = 0b101;            //Humidity Oversampling for Sensor 1
    //bme2.parameter.humidOversampling = 0b101;            //Humidity Oversampling for Sensor 2

    bme1.parameter.tempOversampling = 0b101;              //Temperature Oversampling for Sensor 1
    //bme2.parameter.tempOversampling = 0b101;              //Temperature Oversampling for Sensor 2

    bme1.parameter.pressOversampling = 0b101;             //Pressure Oversampling for Sensor 1
    //bme2.parameter.pressOversampling = 0b101;             //Pressure Oversampling for Sensor 2 

    bme1.parameter.pressureSeaLevel = 1004.25;            //default value of 1013.25 hPa (Sensor 1)
    //bme2.parameter.pressureSeaLevel = 1013.25;            //default value of 1013.25 hPa (Sensor 2)

    bme1.parameter.tempOutsideCelsius = 15;               //default value of 15°C(average outside)
    //bme2.parameter.tempOutsideCelsius = 15;               //default value of 15°C
  
    bme1.parameter.tempOutsideFahrenheit = 59;            //default value of 59°F(average outside)
    //bme2.parameter.tempOutsideFahrenheit = 59;            //default value of 59°F


    if (bme1.init() != 0x60)
  {    
    Serial.println(F("Ops! First BME280 Sensor not found!"));
    bme1Detected = 0;
  }

  else
  {
    Serial.println(F("First BME280 Sensor detected!"));
    bme1Detected = 1;
  }

  /*if (bme2.init() != 0x60)
  {    
    Serial.println(F("Ops! Second BME280 Sensor not found!"));
    bme2Detected = 0;
  }

  else
  {
    Serial.println(F("Second BME280 Sensor detected!"));
    bme2Detected = 1;
  }*/

  if (bme1Detected == 0)//&(bme2Detected == 0))
  {
    Serial.println();
    Serial.println();
    Serial.println(F("Troubleshooting Guide"));
    Serial.println(F("*************************************************************"));
    Serial.println(F("1. Let's check the basics: Are the VCC and GND pins connected correctly? If the BME280 is getting really hot, then the wires are crossed."));
    Serial.println();
    Serial.println(F("2. Did you connect the SDI pin from your BME280 to the SDA line from the Arduino?"));
    Serial.println();
    Serial.println(F("3. And did you connect the SCK pin from the BME280 to the SCL line from your Arduino?"));
    Serial.println();
    Serial.println(F("4. One of your sensors should be using the alternative I2C Address(0x76). Did you remember to connect the SDO pin to GND?"));
    Serial.println();
    Serial.println(F("5. The other sensor should be using the default I2C Address (0x77. Did you remember to leave the SDO pin unconnected?"));

    Serial.println();
    
    while(1);
  }
    
  Serial.println();
  Serial.println();
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





  
//WAAGE__________________________________________________________________
Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
            // by the SCALE parameter (not set yet)

  scale.set_scale(-2301458/100000.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");

//WAAGE ENDE__________________________________________________________________
  
/*

  Serial.println(F("Starting..."));
  

  // Start Temp/Humidity sensor
  dht.begin(); //DHT11 Sensor starten

  // Get all data of temperature sensor and send it over the serial interface
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));


  // Get all data of humidity sensor and send it over the serial interface
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  

*/

  // Set delay between sensor DHT11 readings based on sensor details.
  //delayMS = sensor.min_delay / 1000;

  // Initialize LMIC lib
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters.
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);


  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF12, 14);

  // Start job manually once; afterwards it will be started every defined seconds (see variable TX_INTERVAL )
  do_send(&sendjob);
}

 



///////////////////////////////////////////////////////////////////////////////////////////////////
// Loop will be processed after setup for infinite times
///////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {



//Lux Sensor

// Wait for completion (blocking busy-wait delay)
    if (HelligkeitInnen.isConversionCompleted()) {
        // Read light
        luxInnen = HelligkeitInnen.read();

        // Print light
        Serial.print(F("Licht Innen: "));
        Serial.print(luxInnen / 2);
        Serial.print(F("."));
        Serial.print(luxInnen % 10);
        Serial.println(F(" LUX"));
    }

// Wait for completion (blocking busy-wait delay)
    if (HelligkeitAussen.isConversionCompleted()) {
        // Read light
        luxAussen = HelligkeitAussen.read();

        // Print light
        Serial.print(F("Licht Außen: "));
        Serial.print(luxAussen / 2);
        Serial.print(F("."));
        Serial.print(luxAussen % 10);
        Serial.println(F(" LUX"));
    }


//Temp/Humidity/Pressure++++++++++++++++++++++++++++++++++++++++++++

Serial.print(F("Duration in Seconds:  "));
  Serial.println(float(millis())/1000);

  if (bme1Detected)
  {
    Serial.print(F("Temperature Sensor 1 [°C]:\t\t")); 
    Serial.println(bme1.readTempC());
    //Serial.print(F("Temperature Sensor 1 [°F]:\t\t"));            
    //Serial.println(bme1.readTempF());
    Serial.print(F("Humidity Sensor 1 [%]:\t\t\t")); 
    Serial.println(bme1.readHumidity());
    Serial.print(F("Pressure Sensor 1 [hPa]:\t\t")); 
    Serial.println(bme1.readPressure());
    Serial.print(F("Altitude Sensor 1 [m]:\t\t\t")); 
    Serial.println(bme1.readAltitudeMeter());
    //Serial.print(F("Altitude Sensor 1 [ft]:\t\t\t")); 
    //Serial.println(bme1.readAltitudeFeet());
    Serial.println(F("****************************************"));    
  }

  else
  {
    Serial.print(F("Temperature Sensor 1 [°C]:\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Temperature Sensor 1 [°F]:\t\t")); 
    //Serial.println(F("Null"));
    Serial.print(F("Humidity Sensor 1 [%]:\t\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Pressure Sensor 1 [hPa]:\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Altitude Sensor 1 [m]:\t\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Altitude Sensor 1 [ft]:\t\t\t")); 
    //Serial.println(F("Null"));
    Serial.println(F("****************************************"));   
  }

 /* if (bme2Detected)
  {
    Serial.print(F("Temperature Sensor 2 [°C]:\t\t")); 
    Serial.println(bme2.readTempC());
    //Serial.print(F("Temperature Sensor 2 [°F]:\t\t")); 
    //Serial.println(bme2.readTempF());
    Serial.print(F("Humidity Sensor 2 [%]:\t\t\t")); 
    Serial.println(bme2.readHumidity());
    Serial.print(F("Pressure Sensor 2 [hPa]:\t\t")); 
    Serial.println(bme2.readPressure());
    Serial.print(F("Altitude Sensor 2 [m]:\t\t\t")); 
    Serial.println(bme2.readAltitudeMeter());
    //Serial.print(F("Altitude Sensor 2 [ft]:\t\t\t")); 
    //Serial.println(bme2.readAltitudeFeet());    
  }

  else
  {
    Serial.print(F("Temperature Sensor 2 [°C]:\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Temperature Sensor 2 [°F]:\t\t")); 
    //Serial.println(F("Null"));
    Serial.print(F("Humidity Sensor 2 [%]:\t\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Pressure Sensor 2 [hPa]:\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Altitude Sensor 2 [m]:\t\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Altitude Sensor 2 [ft]:\t\t\t")); 
    //Serial.println(F("Null"));
  }
   */
   Serial.println();
   Serial.println();

   delay(1000);
   
 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//WAAGE_____________________________________
Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);
 
  rawData1WAAGE = scale.get_units();
  Serial.println(rawData1WAAGE);

//WAAGE ENDE________________________________



/*
///////////////////////////////////////////////////////////////////////////////////////////////////
//AVG Photoresistors
///////////////////////////////////////////////////////////////////////////////////////////////////

  //PR Sensor 1

PRvalueLive1= analogRead(photoresistor1);


if (PRvalueLive1<0 or PRvalueLive1>1500)
    {PRvalueAdd1=PRvalueAdd1;
     PRavgXcount1 = PRavgXcount1;
    }
    else
    {PRvalueAdd1 = PRvalueAdd1 + PRvalueLive1;
     PRavgXcount1 = PRavgXcount1 +1;
    }
if (PRavgXcount1 == PRmeasureCountGoal1)
    {PR_AVG1 = PRvalueAdd1/PRmeasureCountGoal1;
    PRvalueAdd1 = 0;
    PRavgXcount1 = 0;
    }

    //PR Sensor 2
    
PRvalueLive2= analogRead(photoresistor2);


if (PRvalueLive2<0 or PRvalueLive2>1500)
    {PRvalueAdd2=PRvalueAdd2;
     PRavgXcount2 = PRavgXcount2;
    }
    else
    {PRvalueAdd2 = PRvalueAdd2 + PRvalueLive2;
     PRavgXcount2 = PRavgXcount2 +1;
    }
if (PRavgXcount2 == PRmeasureCountGoal2)
    {PR_AVG2 = PRvalueAdd2/PRmeasureCountGoal2;
    PRvalueAdd2 = 0;
    PRavgXcount2 = 0;
    }
Serial.println("---------------------");
Serial.println("PR Sensor 1 :  ");
Serial.print("Live Wert :  "); Serial.println(PRvalueLive1);
Serial.print("Addierter Wert :  "); Serial.println(PRvalueAdd1);
Serial.print("Anzahl der Messwerte :  "); Serial.println(PRavgXcount1);
Serial.print("Anzahl der zu addierenden Werte :  "); Serial.println(PRmeasureCountGoal1);
Serial.print("Durchschnittswert :  "); Serial.println(PR_AVG1);

Serial.println("---------------------");
Serial.println("PR Sensor 2 :  ");
Serial.print("Live Wert :  "); Serial.println(PRvalueLive2);
Serial.print("Addierter Wert :  "); Serial.println(PRvalueAdd2);
Serial.print("Anzahl der Messwerte :  "); Serial.println(PRavgXcount2);
Serial.print("Anzahl der zu addierenden Werte :  "); Serial.println(PRmeasureCountGoal2);
Serial.print("Durchschnittswert :  "); Serial.println(PR_AVG2);

*/
///////////////////////////////////////////////////////////////////////////////////////////////////

  // Let LMIC run in loop
  os_runloop_once();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// onEvent is Handler which is called every time a event occurs in the LMIC stack
///////////////////////////////////////////////////////////////////////////////////////////////////
void onEvent (ev_t ev) {

  // If the event was trigger by completing a sendjob
  if (ev == EV_TXCOMPLETE) {

    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

    // Schedule next transmission
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// do_send is called manually or automatically by the LMIC OS every time a timer elapses TX_INTERVAL seconds
///////////////////////////////////////////////////////////////////////////////////////////////////
void do_send(osjob_t* j) {
 
  // Instantiate float variables for temp and humidity value
 // float temperature;
 // float humidity;
 
  /*

  // Instantiate sensor event object
  sensors_event_t event;


  // Minimum delay between measurements.
  delay(delayMS);


  
  // Get temperature data
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    temperature = event.temperature;
  }


  // Get humidity data
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    humidity = event.relative_humidity;
    
     
     Serial.print("Sensor Wert Durchschnitt PR 1 =  "); //serial Monitor Textausgabe
     Serial.println(PR_AVG1); //serial Monitor Wertausgabe der Variable

     Serial.print("Sensor Wert Durchschnitt PR 2 =  ");
     Serial.println(PR_AVG2);
     
  }
  
 */
  

  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    // Prepare upstream data transmission at the next possible time.
    lpp.reset();
    lpp.addTemperature(1, bme1.readTempC());
    lpp.addRelativeHumidity(2, bme1.readHumidity());
    lpp.addBarometricPressure(3, bme1.readPressure()); 
    lpp.addLuminosity(4, luxInnen);
    lpp.addLuminosity(5, luxAussen);
    lpp.addTemperature(6,rawData1WAAGE);

    LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
    Serial.println(F("Sending uplink packet..."));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}
