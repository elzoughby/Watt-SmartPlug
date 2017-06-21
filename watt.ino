// Watt?
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "watt_time.h"

#define FIREBASE_HOST "watt-project-eg.firebaseio.com"
#define FIREBASE_AUTH "BIiq1X5t2MYbzj9mQxat1BuABRNIX8VT7YGIz7Mb"

#define WIFI_SSID "Eng"
#define WIFI_PASSWORD "A205080/*"



float prevPowerAverage = 0;
float realTime = 0;
short currDay = 0;
short currMonth = 0;
unsigned int currYear = 0;


void setup() {
  //configurations
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT);

  // connect to wifi.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  //just for debugging
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //connect to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //initialize time
  time_init();

}


void loop() {

  realTime = readRealTime();
  Firebase.setFloat("Devices/-KfWxUaA7tnwZr4ZIbrq/consumption", realTime);

  //just for debugging
  Serial.print("Real-time Power = ");
  Serial.println(realTime);
  Serial.println(hour());
  Serial.println(minute());
  Serial.println(" ");


  if(Firebase.getBool("Devices/-KfWxUaA7tnwZr4ZIbrq/enabled") == true) {
    digitalWrite(LED_BUILTIN, HIGH);
    if(Firebase.getBool("Devices/-KfWxUaA7tnwZr4ZIbrq/stopAt/enabled") == true && Firebase.getInt("Devices/-KfWxUaA7tnwZr4ZIbrq/stopAt/hour") == hour() && Firebase.getInt("Devices/-KfWxUaA7tnwZr4ZIbrq/stopAt/min") == minute()) {
      digitalWrite(LED_BUILTIN, LOW);
      Firebase.setBool("Devices/-KfWxUaA7tnwZr4ZIbrq/enabled", false);
      Firebase.setBool("Devices/-KfWxUaA7tnwZr4ZIbrq/stopAt/enabled", false);
    }
  }
    
  if(Firebase.getBool("Devices/-KfWxUaA7tnwZr4ZIbrq/enabled") == false) {
    digitalWrite(LED_BUILTIN, LOW);
    if(Firebase.getBool("Devices/-KfWxUaA7tnwZr4ZIbrq/startAt/enabled") == true && Firebase.getInt("Devices/-KfWxUaA7tnwZr4ZIbrq/startAt/hour") == hour() && Firebase.getInt("Devices/-KfWxUaA7tnwZr4ZIbrq/startAt/min") == minute()) {
      digitalWrite(LED_BUILTIN, HIGH);
      Firebase.setBool("Devices/-KfWxUaA7tnwZr4ZIbrq/enabled", true);
      Firebase.setBool("Devices/-KfWxUaA7tnwZr4ZIbrq/startAt/enabled", false);
    }
  }
  
  delay(500);
}


float readRealTime() {

  float powerSum= 0;
  float powerAverage= 0;

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
  if( powerAverage > (prevPowerAverage + 5) || powerAverage < (prevPowerAverage - 5)) {
      prevPowerAverage = powerAverage;
      return powerAverage;
  }
  else
    return prevPowerAverage;

}

