// Watt? Smart Plug


#include <ESP8266WiFi.h>		//https://github.com/esp8266/Arduino
#include <FirebaseArduino.h>	//https://github.com/firebase/firebase-arduino
#include "watt_wificonfig.h"
#include "watt_time.h"

#define FIREBASE_HOST "watt-project-eg.firebaseio.com"
#define FIREBASE_AUTH "BIiq1X5t2MYbzj9mQxat1BuABRNIX8VT7YGIz7Mb"
#define DEVICE_ID "-KfWxUaA7tnwZr4ZIbrq"

#define CONTROL_PIN 16  //4 for real board
#define CONTROL_BUTTON 5
#define WIFI_LED 2


float prevPowerAverage = 0;
float realTime = 0;
time_t interruptTime = 0;


void setup() {

  //configurations
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(CONTROL_PIN, OUTPUT);
  pinMode(CONTROL_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(CONTROL_BUTTON), interrupt, FALLING);
  pinMode(WIFI_LED, OUTPUT);

  //Wifi configuration and connection
  Serial.println("connecting");
  wificonfig_start();

  //connect to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //initialize time
  time_init();

}


void loop() {

  //handling Interrupts
  if(interruptTime != 0)  
    handleInterrupt();

  //mesurring realtime power consumption
  realTime = readRealTime();
  Firebase.setFloat(String("Devices/")+DEVICE_ID+"/consumption", realTime);

  //just for debugging
  Serial.println(String("Real-time Power = ") + realTime);
  Serial.println(String(hour()) + ":" + minute());
  Serial.println(" ");

  //sync device status with firebase
  if(Firebase.getBool(String("Devices/")+DEVICE_ID+"/enabled") == true) {
    digitalWrite(CONTROL_PIN, HIGH);
    if(Firebase.getBool(String("Devices/")+DEVICE_ID+"/stopAt/enabled") == true && Firebase.getInt(String("Devices/")+DEVICE_ID+"/stopAt/hour") == hour() && Firebase.getInt(String("Devices/")+DEVICE_ID+"/stopAt/min") == minute()) {
      digitalWrite(CONTROL_PIN, LOW);
      Firebase.setBool(String("Devices/")+DEVICE_ID+"/enabled", false);
      Firebase.setBool(String("Devices/")+DEVICE_ID+"/stopAt/enabled", false);
    }
  }

  if(Firebase.getBool(String("Devices/")+DEVICE_ID+"/enabled") == false) {
    digitalWrite(CONTROL_PIN, LOW);
    if(Firebase.getBool(String("Devices/")+DEVICE_ID+"/startAt/enabled") == true && Firebase.getInt(String("Devices/")+DEVICE_ID+"/startAt/hour") == hour() && Firebase.getInt(String("Devices/")+DEVICE_ID+"/startAt/min") == minute()) {
      digitalWrite(CONTROL_PIN, HIGH);
      Firebase.setBool(String("Devices/")+DEVICE_ID+"/enabled", true);
      Firebase.setBool(String("Devices/")+DEVICE_ID+"/startAt/enabled", false);
    }
  }
  
  delay(250);
}


float readRealTime() {

  float powerSum= 0;
  float powerAverage= 0;

  if(digitalRead(CONTROL_PIN) == 0)
    return 0;

  for(int j=0; j < 10; j++) {

    unsigned int sensorValue = 0;
    unsigned int sensorArray[50];
    unsigned int sensorMax =0;
    unsigned int sensorSum =0;
    unsigned int difference=0;
    unsigned int sensorAverage =0;
    double voltage=0;
    double current=0;
    double power=0;

    for(int i=0; i<50; i++) {
      sensorValue = analogRead(A0);
      sensorArray[i] = sensorValue;
      sensorSum += sensorValue;
      if (sensorArray[i]>sensorMax) sensorMax = sensorArray[i];
      delay(5);
    }
    sensorAverage = sensorSum/50;
    difference = sensorMax-sensorAverage;

    voltage = (double)difference * 3.3 / 1023;
    current = voltage / 0.1;
    current = current * 0.707;
    power = current * 220;

    powerSum += power;
  }

  powerAverage = powerSum/10;

  // return onChange
  if(powerAverage > (prevPowerAverage + 5) || powerAverage < (prevPowerAverage - 5)) {
      prevPowerAverage = powerAverage;
      return powerAverage;
  }
  else
    return prevPowerAverage;

}


void interrupt() {

  interruptTime = now();
  
}
  

void handleInterrupt() {

  //reset if the button pushed for more than 5 seconds
  while(digitalRead(CONTROL_BUTTON) == 0) {
    if(now() - interruptTime >= 3) {
      wificonfig_reset();
      ESP.reset();
    }
  }

  //toggle device if the button pushed for less than 5 seconds
  digitalWrite(CONTROL_PIN, digitalRead(CONTROL_PIN)^1);
  Firebase.setBool(String("Devices/")+DEVICE_ID+"/enabled", (bool)digitalRead(CONTROL_PIN));

  //reset interrupt status
  interruptTime = 0;

}

