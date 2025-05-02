#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sensor1.setTimeout(500);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

#if defined LONG_RANGE
  sensor1.setSignalRateLimit(0.1);
  sensor1.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor1.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

#endif
#if defined HIGH_SPEED
  sensor1.setMeasurementTimingBudget(20000);
  
#elif defined HIGH_ACCURACY
  sensor1.setMeasurementTimingBudget(200000);
#endif
}

void loop()
{
  Serial.print(sensor1.readRangeSingleMillimeters());
  if (sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
