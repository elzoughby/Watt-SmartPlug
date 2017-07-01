#include "watt_wificonfig.h"

void wificonfig_start() {
    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    wifiManager.autoConnect(AP_SSID, AP_PASSWORD);
}