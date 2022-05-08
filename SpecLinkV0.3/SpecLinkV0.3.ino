


#include <ArduinoBLE.h>

#include <Arduino.h>
#include <U8g2lib.h>

#include <Arduino_LSM9DS1.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#include <avr/dtostrf.h>
#endif


U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_MIRROR, /* reset=*/ U8X8_PIN_NONE);  //Pick screen type, it is also mirrored here as well

//define Service and Charteristic UUID. The Service and Characteristic UUIDs cannot be the same, this will cause a crash in the android link app.
BLEService DeckSpecs("f2f9c7ab-a1fd-49c7-9dd7-0e46b6468fdb");

BLEIntCharacteristic hourChar("ac129c20-f67f-47d7-9e5a-751f7b2bf222", BLERead | BLEWrite);
BLEIntCharacteristic minChar("85803cef-11d6-495e-aa8b-d83c6b79559e", BLERead | BLEWrite);
BLEIntCharacteristic tempChar("24d665e4-1c58-4147-962e-43224a6fd101", BLERead | BLEWrite);

String Minute;
float min1;
char ConvertedFloat[20];
long UpdateInterval = 1000;
long PreviousMillis = 0;






void setup() {
  Serial.begin(9600); //debug serial interface
  IMU.begin();
  u8g2.begin();
  
  if (!BLE.begin()) { //BLE failure error handling
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);       }


  BLE.setLocalName("DeckSpecs"); //set up the service and name that will be broadcast
  BLE.setAdvertisedService(DeckSpecs);

  DeckSpecs.addCharacteristic(hourChar);
  DeckSpecs.addCharacteristic(minChar);
  DeckSpecs.addCharacteristic(tempChar);

  BLE.addService(DeckSpecs); 

  hourChar.writeValue(12); //set initial values for characteristics
  minChar.writeValue(12);
  tempChar.writeValue(72);

  
    // start advertising
  BLE.advertise();


}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  u8g2.clearBuffer();          // clear the internal memory

// if a central is connected to peripheral:
  if (central) {


   
    while (central.connected()) {   // while the central is still connected to peripheral:
      long CurrentMillis = millis();
      if (CurrentMillis - PreviousMillis > UpdateInterval){
          PreviousMillis = CurrentMillis;
          u8g2.setFont(u8g2_font_5x7_tf); // choose a suitable font
 
          itoa(hourChar.value() , ConvertedFloat , 10); //converts the sent integer to a string that u8g2 can display
          u8g2.drawStr(1,6,ConvertedFloat);
         
          itoa(minChar.value() , ConvertedFloat , 10); //converts the sent integer to a string that u8g2 can display
          u8g2.drawStr(15,6,ConvertedFloat);
          u8g2.drawStr(11,6,":"); // write something to the internal memory

          itoa(tempChar.value() , ConvertedFloat , 10); //converts the sent integer to a string that u8g2 can display
          u8g2.drawStr(1,13,ConvertedFloat);
          u8g2.setFont(u8g2_font_4x6_tr);
          u8g2.drawStr(8,12,"f"); // write something to the internal memory
          
          u8g2.sendBuffer();          // transfer internal memory to the display
          u8g2.clearBuffer();          // clear the internal memory
      }
    }



  }
}
