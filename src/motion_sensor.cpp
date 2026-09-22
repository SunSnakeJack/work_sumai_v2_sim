#include "motion_sensor.h"

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#include "config.h"

namespace {
bool writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}
}

bool initializeMotionSensor() {
  Wire.begin(PIN_MPU_SDA, PIN_MPU_SCL);
  Wire.beginTransmission(MPU6050_ADDRESS);
  if (Wire.endTransmission() != 0) return false;
  return writeRegister(0x6B, 0x00);
}

MotionReading readMotionSensor() {
  MotionReading reading = {};
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return reading;
  Wire.requestFrom(MPU6050_ADDRESS, static_cast<uint8_t>(14));
  if (Wire.available() < 14) return reading;

  const int16_t rawAX = (Wire.read() << 8) | Wire.read();
  const int16_t rawAY = (Wire.read() << 8) | Wire.read();
  const int16_t rawAZ = (Wire.read() << 8) | Wire.read();
  Wire.read();
  Wire.read();
  const int16_t rawGX = (Wire.read() << 8) | Wire.read();
  const int16_t rawGY = (Wire.read() << 8) | Wire.read();
  const int16_t rawGZ = (Wire.read() << 8) | Wire.read();

  reading.accelX = rawAX / 16384.0f;
  reading.accelY = rawAY / 16384.0f;
  reading.accelZ = rawAZ / 16384.0f;
  reading.gyroX = rawGX / 131.0f;
  reading.gyroY = rawGY / 131.0f;
  reading.gyroZ = rawGZ / 131.0f;
  reading.accelMagnitude = sqrtf(reading.accelX * reading.accelX + reading.accelY * reading.accelY + reading.accelZ * reading.accelZ);
  reading.gyroMagnitude = sqrtf(reading.gyroX * reading.gyroX + reading.gyroY * reading.gyroY + reading.gyroZ * reading.gyroZ);
  reading.valid = true;
  return reading;
}

bool isMotionAlert(const MotionReading& reading) {
  return reading.valid &&
         (fabsf(reading.accelMagnitude - 1.0f) > ACCEL_DEVIATION_THRESHOLD_G ||
          reading.gyroMagnitude > GYRO_THRESHOLD_DPS);
}

