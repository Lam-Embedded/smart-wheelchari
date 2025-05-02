#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <VL53L0X.h>

// Định nghĩa chân điều khiển động cơ
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

// Các hàm điều khiển động cơ
void left_wheel(boolean dir_rotate, uint8_t L_speed);
void right_wheel(boolean dir_rotate, uint8_t R_speed);
void linear_motion(boolean dir_rotate, uint8_t speed);
void rotation_of_the_wheelchair(boolean dir_rotate, uint8_t speed);

void setup() {
    Serial.begin(115200);
    Wire.begin();
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
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        Serial.print("Nhận lệnh từ Pi: ");
        Serial.println(command);

        if (command == "forward") {
            Serial.println("Đi thẳng");
            linear_motion(DIR_ROTATE_STRAIGHT, 100);
        } else if (command == "backward") {
            Serial.println("Đi lùi");
            linear_motion(DIR_ROTATE_BACK, 100);
        } else if (command == "left") {
            Serial.println("Xoay trái");
            rotation_of_the_wheelchair(DIR_ROTATE_BACK, 100);
        } else if (command == "right") {
            Serial.println("Xoay phải");
            rotation_of_the_wheelchair(DIR_ROTATE_STRAIGHT, 100);
        } else if (command == "stop") {
            Serial.println("Dừng lại");
            linear_motion(DIR_ROTATE_STRAIGHT, 0);
        } else {
            Serial.println("Lệnh không hợp lệ!");
        }
    }
}

// ------------------------ Điều khiển động cơ ------------------------

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
