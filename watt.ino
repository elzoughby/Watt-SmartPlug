// Watt? Smart Plug

#include "watt.h"



void setup() {

  //GPIO Configuration
  gpioConfig();

  //Wifi configuration and connection
  wificonfig_start();

  //connect to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //initialize time
  time_init();

}

void loop() {

  //handling Interrupts
  if(getInterruptTime() != 0)  
    handleInterrupt();

  //mesurring realtime power consumption
  float realTime = readRealTime();
  Firebase.setFloat(String("Devices/")+DEVICE_ID+"/consumption", realTime);

  //just for debugging
  Serial.println(String("Real-time Power = ") + realTime);
  Serial.println(String("time = ") + hour() + ":" + minute());
  Serial.println();

  //sync device status with firebase
  syncDeviceStatus();
  
  delay(250);
}

