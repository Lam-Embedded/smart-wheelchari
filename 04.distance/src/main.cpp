#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <VL53L0X.h>
#include <QuickEspNow.h>
#include <PubSubClient.h>

const char* mqtt_server = "192.168.1.100";  // thay bằng IP thật của RPi
WiFiClient espClient;
PubSubClient client(espClient);

const char* client_id = "esp2";
const char* topic = "esp2/data";

IPAddress local_IP(192, 168, 1, 103);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

static uint8_t receiver[] = { 0xec, 0xe3, 0x34, 0xc0, 0x74, 0x6c }; 
#define DEST_ADDR receiver

void wifiSetup();
void reconnect();

VL53L0X sensor;

void setup() {
    Serial.begin(115200);
    wifiSetup();
    client.setServer(mqtt_server, 1883);
    Serial.println("Initializing...");

    Wire.begin();
    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("Error: Không tìm thấy cảm biến VL53L0X!");
        while (1);
    }

    quickEspNow.begin(1, 0, false);
}

void loop() {
    if (!client.connected()) {
        reconnect();
        delay(100);
    }
    client.loop();
    

    int distance = sensor.readRangeSingleMillimeters();

    if (sensor.timeoutOccurred()) {
        Serial.println("Error: Cảm biến timeout!");
    } else {
        Serial.println(distance);
    }

    // ESPNOW
    quickEspNow.send(DEST_ADDR, (const uint8_t*)&distance, sizeof(distance));
    Serial.printf(">> Đã gửi BPM = %d\n", distance);


    // MQTT
    String payload = String(distance);
    client.publish(topic, payload.c_str());
    Serial.println("ESP2 sent: " + payload);
    delay(500);
}

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.begin(); 

    WiFiManager wm;
    if (!wm.autoConnect("AutoConnectAP_distance")) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        Serial.println("WiFi connected!");
    }

    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect(client_id)) {
          Serial.println("Connected to MQTT");
        } else {
          delay(2000);
        }
    }
}
