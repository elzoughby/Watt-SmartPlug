#include "watt_wificonfig.h"


WiFiManager wifiManager;


void wificonfig_start() {
    wifiManager.autoConnect(AP_SSID, AP_PASSWORD);
}


void wificonfig_reset() {
    wifiManager.resetSettings();
}