#include "watt_smartconfig.h"

void smartconfig_init() {
    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    wifiManager.autoConnect(AP_SSID, AP_PASSWORD);
}