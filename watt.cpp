#include "watt.h"

float prevPowerAverage = 0;
volatile time_t interruptTime = 0;

//no need to be shared
void interrupt(); 



void gpioConfig() {

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(CONTROL_PIN, OUTPUT);
  pinMode(CONTROL_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(CONTROL_BUTTON), interrupt, FALLING);

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

void syncDeviceStatus() {

  if(Firebase.getBool(String("Devices/")+PLUG_ID+"/enabled") == true) {
    digitalWrite(CONTROL_PIN, HIGH);
    pinMode(WIFI_LED, OUTPUT);
    if(Firebase.getBool(String("Devices/")+PLUG_ID+"/stopAt/enabled") == true && Firebase.getInt(String("Devices/")+PLUG_ID+"/stopAt/hour") == hour() && Firebase.getInt(String("Devices/")+PLUG_ID+"/stopAt/min") == minute()) {
      digitalWrite(CONTROL_PIN, LOW);
      pinMode(WIFI_LED, INPUT);
      Firebase.setBool(String("Devices/")+PLUG_ID+"/enabled", false);
      Firebase.setBool(String("Devices/")+PLUG_ID+"/stopAt/enabled", false);
    }
  }

  if(Firebase.getBool(String("Devices/")+PLUG_ID+"/enabled") == false) {
    digitalWrite(CONTROL_PIN, LOW);
    pinMode(WIFI_LED, INPUT);
    if(Firebase.getBool(String("Devices/")+PLUG_ID+"/startAt/enabled") == true && Firebase.getInt(String("Devices/")+PLUG_ID+"/startAt/hour") == hour() && Firebase.getInt(String("Devices/")+PLUG_ID+"/startAt/min") == minute()) {
      digitalWrite(CONTROL_PIN, HIGH);
      pinMode(WIFI_LED, OUTPUT);
      Firebase.setBool(String("Devices/")+PLUG_ID+"/enabled", true);
      Firebase.setBool(String("Devices/")+PLUG_ID+"/startAt/enabled", false);
    }
  }

}

void interrupt() {

  //set the interrupt flag to handle in the main loop
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
  pinMode(WIFI_LED, digitalRead(CONTROL_PIN));
  Firebase.setBool(String("Devices/")+PLUG_ID+"/enabled", (bool)digitalRead(CONTROL_PIN));

  //reset interrupt status
  interruptTime = 0;

}

time_t getInterruptTime() {

  return interruptTime;

}

