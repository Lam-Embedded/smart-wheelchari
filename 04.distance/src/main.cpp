#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);  // Đồng bộ baudrate với Raspberry Pi
  Wire.begin();
  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println("Error: Không tìm thấy cảm biến VL53L0X!");
    while (1);
  }
}

void loop() {
  int distance = sensor.readRangeSingleMillimeters();

  if (sensor.timeoutOccurred()) {
    Serial.println("Error: Cảm biến timeout!");
  } else {
    Serial.println(distance);
  }

  delay(500);  // Giảm tải CPU và tránh gửi quá nhanh
}
