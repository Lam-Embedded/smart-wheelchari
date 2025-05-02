#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <WiFiManager.h>

// MQTT broker (IP của Raspberry Pi)
const char* mqtt_server = "192.168.1.100";
const char* topic = "esp32/data";

WiFiClient espClient;
PubSubClient client(espClient);

// Cấu hình địa chỉ IP tĩnh (nếu cần)
IPAddress local_IP(192, 168, 1, 104);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// Chân điều khiển động cơ
const uint8_t RL_EN_L = 2;
const uint8_t R_PWM_L = 4;
const uint8_t L_PWM_L = 16;
const uint8_t RL_EN_R = 12;
const uint8_t R_PWM_R = 14;
const uint8_t L_PWM_R = 26;

// Hướng quay động cơ
#define DIR_ROTATE_STRAIGHT HIGH
#define DIR_ROTATE_BACK LOW

VL53L0X sensor1;

// Hàm khai báo
void wifiSetup();
void left_wheel(boolean dir_rotate, uint8_t L_speed);
void right_wheel(boolean dir_rotate, uint8_t R_speed);
void linear_motion(boolean dir_rotate, uint8_t speed);
void rotation_of_the_wheelchair(boolean dir_rotate, uint8_t speed);

// Callback xử lý dữ liệu MQTT nhận từ Raspberry Pi
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received on topic [");
    Serial.print(topic);
    Serial.print("]: ");

    String command = "";
    for (unsigned int i = 0; i < length; i++) {
        command += (char)payload[i];
    }
    Serial.println(command);

    command.trim();

    if (command == "1") {
        Serial.println("Đi thẳng");
        linear_motion(DIR_ROTATE_STRAIGHT, 100);
    } else if (command == "2") {
        Serial.println("Đi lùi");
        linear_motion(DIR_ROTATE_BACK, 100);
    } else if (command == "3") {
        Serial.println("Xoay trái");
        rotation_of_the_wheelchair(DIR_ROTATE_BACK, 100);
    } else if (command == "4") {
        Serial.println("Xoay phải");
        rotation_of_the_wheelchair(DIR_ROTATE_STRAIGHT, 100);
    } else if (command == "0") {
        Serial.println("Dừng lại");
        linear_motion(DIR_ROTATE_STRAIGHT, 0);
    } else {
        Serial.println("Lệnh không hợp lệ!");
    }
}

// Tự động kết nối lại MQTT nếu bị ngắt
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println("connected");
            client.subscribe(topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    wifiSetup();  // kết nối WiFi qua WiFiManager
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    sensor1.setTimeout(500);

    pinMode(RL_EN_L, OUTPUT);
    pinMode(R_PWM_L, OUTPUT);
    pinMode(L_PWM_L, OUTPUT);
    pinMode(RL_EN_R, OUTPUT);
    pinMode(R_PWM_R, OUTPUT);
    pinMode(L_PWM_R, OUTPUT);

    // Dừng động cơ lúc khởi động
    digitalWrite(R_PWM_L, HIGH);
    digitalWrite(L_PWM_L, HIGH);
    digitalWrite(R_PWM_R, HIGH);
    digitalWrite(L_PWM_R, HIGH);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

// -------------------- Điều khiển động cơ --------------------

void left_wheel(boolean dir_rotate, uint8_t L_speed) {
    digitalWrite(L_PWM_L, dir_rotate);
    digitalWrite(R_PWM_L, !dir_rotate);
    analogWrite(RL_EN_L, L_speed);
}

void right_wheel(boolean dir_rotate, uint8_t R_speed) {
    digitalWrite(L_PWM_R, dir_rotate);
    digitalWrite(R_PWM_R, !dir_rotate);
    analogWrite(RL_EN_R, R_speed);
}

void linear_motion(boolean dir_rotate, uint8_t speed) {
    left_wheel(dir_rotate, speed);
    right_wheel(dir_rotate, speed);
}

void rotation_of_the_wheelchair(boolean dir_rotate, uint8_t speed) {
    left_wheel(dir_rotate, speed);
    right_wheel(!dir_rotate, speed);
}

// ---------------------- WiFi Setup -----------------------

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

    WiFiManager wm;
    if (!wm.autoConnect("AutoConnectAP_MQTT")) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        Serial.println("WiFi connected!");
    }

    Serial.println(WiFi.localIP());
}
