#pragma once

struct MotionReading {
  float accelX;
  float accelY;
  float accelZ;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accelMagnitude;
  float gyroMagnitude;
  bool valid;
};

bool initializeMotionSensor();
MotionReading readMotionSensor();
bool isMotionAlert(const MotionReading& reading);

