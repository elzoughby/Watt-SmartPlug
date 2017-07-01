#include <ESP8266WiFi.h>		//https://github.com/esp8266/Arduino
#include <DNSServer.h>          //built-in arduino library
#include <ESP8266WebServer.h>   //esp8266 library
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager

#define AP_SSID "Watt?"
#define AP_PASSWORD "12345678"

void wificonfig_start();

void wificonfig_reset();