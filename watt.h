#include <ESP8266WiFi.h>		//https://github.com/esp8266/Arduino
#include <FirebaseArduino.h>	//https://github.com/firebase/firebase-arduino
#include "watt_wificonfig.h"
#include "watt_time.h"

#define FIREBASE_HOST "watt-project-eg.firebaseio.com"
#define FIREBASE_AUTH "BIiq1X5t2MYbzj9mQxat1BuABRNIX8VT7YGIz7Mb"
#define PLUG_ID "-KfWxUaA7tnwZr4ZIbrq"

#define CONTROL_PIN 4
#define CONTROL_BUTTON 5
#define WIFI_LED 2



void gpioConfig();

float readRealTime();

void syncDeviceStatus();

void handleInterrupt();

time_t getInterruptTime();
