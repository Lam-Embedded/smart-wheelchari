#include <Arduino.h>
#include "SPIFFS.h"
#include "Audio.h"
#include <WiFiManager.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>


#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

// MAC cua 2 esp8266
// uint8_t esp8266_heart_rate[] = {0xAC, 0x67, 0xB2, 0x1A, 0x2B, 0x3C};
// uint8_t esp8266_distance[] = {0xAC, 0x67, 0xB2, 0x4D, 0x5E, 0x6F};

IPAddress local_IP(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

Audio audio;

void wifiSetup();

// bool compareMac(uint8_t* esp8266_heart_rate, uint8_t* esp8266_distance) {
//     for (int i = 0; i < 6; i++) {
//       if (esp8266_heart_rate[i] != esp8266_distance[i]) return false;
//     }
//     return true;
// }

// void dataReceived(uint8_t* address, uint8_t* data, uint8_t len, signed int rssi, bool broadcast) {
//     Serial.printf("Received from " MACSTR ": %.*s\n", MAC2STR(address), len, data);
  
//     if (compareMac(address, esp8266_heart_rate)) {
//         Serial.println(">>> Dữ liệu đến từ ESP8266 số 1");
//         int receivedNumber = atoi((char*)data);
//         if (receivedNumber <= 50) {
//             audio.connecttospeech("Hãy chú ý, có chướng ngại vật phía trước, "vi");
//         }
//     } 
//     else (compareMac(address, esp8266_distance)) {
//         Serial.println(">>> Dữ liệu đến từ ESP8266 số 2");
//         if (receivedNumber >= 110) {
//             audio.connecttospeech("Chú ý, Nhịp tim đang không ổn định, "vi");
//         }
//     }
// }

void setup() {
    Serial.begin(115200);
    wifiSetup();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(200);
    audio.connecttospeech("Xin chào tôi là smart chair, trợ lý sức khỏe của bạn", "vi"); // Google TTS
    // WiFi.disconnect(false, true);

    // quickEspNow.onDataRcvd(dataReceived);
    // quickEspNow.begin(1, 0, false); // channel 1
}


void loop() {
  audio.loop();
}

void audio_info(const char *info) {
  Serial.print("audio_info: "); Serial.println(info);
}

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA fail to configure!!!");
    }

    WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP_audio");

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
