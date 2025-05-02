#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void wifiSetup();

void setup() {
    Serial.begin(115200);
    // Setup wifi
    wifiSetup();

}

void loop() {
      
}

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA fail to configure!!!");
    }

    WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP");

    if(!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } 
    else {  
        Serial.println("connected...yeey :)");
    }

    Serial.println("Setup done!!!!");
    Serial.println(WiFi.localIP());
    Serial.printf("RSSI: %d", WiFi.RSSI());
}
