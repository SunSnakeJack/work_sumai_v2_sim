#include <Arduino.h>

#include "alerts.h"
#include "audio_sensor.h"
#include "blynk_service.h"
#include "config.h"
#include "motion_sensor.h"

namespace {
bool audioReady = false;
bool motionReady = false;
float soundRms = 0.0f;
MotionReading motion = {};
uint32_t lastSensorReadMs = 0;
uint32_t lastStatusPrintMs = 0;

AlertState determineState(bool monitoringEnabled, bool soundAlert, bool motionAlert) {
  if (!monitoringEnabled) return AlertState::MONITORING_OFF;
  if (soundAlert && motionAlert) return AlertState::COMBINED_ALERT;
  if (soundAlert) return AlertState::SOUND_ALERT;
  if (motionAlert) return AlertState::MOTION_ALERT;
  return AlertState::NORMAL;
}
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MONITORING_SWITCH, INPUT_PULLUP);
  initializeAlerts();
  audioReady = beginAudioSensor();
  motionReady = initializeMotionSensor();
  initializeBlynkService();

  Serial.println();
  Serial.println("WORK_SUMAI V2 Wokwi system simulation");
  Serial.println("Audio source: potentiometer test adapter (NOT an INMP441/I2S simulation)");
  Serial.printf("Simulation audio adapter: %s\r\n", audioReady ? "READY" : "ERROR");
  Serial.printf("MPU6050: %s\r\n", motionReady ? "CONNECTED" : "ERROR");
  Serial.printf("Blynk compile-time mode: %s\r\n", ENABLE_BLYNK ? "ENABLED" : "DISABLED");
}

void loop() {
  const uint32_t nowMs = millis();
  const bool monitoringEnabled = digitalRead(PIN_MONITORING_SWITCH) == LOW;

  if (nowMs - lastSensorReadMs >= SENSOR_INTERVAL_MS) {
    soundRms = audioReady ? getSoundLevel() : 0.0f;
    motion = motionReady ? readMotionSensor() : MotionReading{};
    lastSensorReadMs = nowMs;
  }

  const bool soundAlert = audioReady && soundRms >= SIM_SOUND_RMS_THRESHOLD;
  const bool motionAlert = motionReady && isMotionAlert(motion);
  const AlertState state = determineState(monitoringEnabled, soundAlert, motionAlert);
  updateLocalAlerts(state, nowMs);
  serviceBlynk(state, soundRms, motion.accelMagnitude, motion.gyroMagnitude, monitoringEnabled, nowMs);

  if (nowMs - lastStatusPrintMs >= STATUS_INTERVAL_MS) {
    Serial.printf("SIM SoundRMS=%.0f | Accel=%.2fg | Gyro=%.1f dps | %s\r\n",
                  soundRms, motion.accelMagnitude, motion.gyroMagnitude, alertStateName(state));
    lastStatusPrintMs = nowMs;
  }
  delay(5);
}

