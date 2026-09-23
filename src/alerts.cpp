#include "alerts.h"

#include "config.h"

namespace {
int activeBuzzerPin = -1;
uint32_t activeFrequency = 0;

void stopBuzzers() {
  if (activeBuzzerPin < 0) return;
  ledcWriteTone(BUZZER_LEDC_CHANNEL, 0);
  ledcDetachPin(activeBuzzerPin);
  digitalWrite(activeBuzzerPin, LOW);
  activeBuzzerPin = -1;
  activeFrequency = 0;
}

void setBuzzer(int pin, uint32_t frequency) {
  if (activeBuzzerPin == pin && activeFrequency == frequency) return;
  stopBuzzers();
  ledcWriteTone(BUZZER_LEDC_CHANNEL, frequency);
  ledcAttachPin(pin, BUZZER_LEDC_CHANNEL);
  activeBuzzerPin = pin;
  activeFrequency = frequency;
}
}

void initializeAlerts() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_SOUND_BUZZER, OUTPUT);
  pinMode(PIN_MOTION_BUZZER, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LOW);
  digitalWrite(PIN_SOUND_BUZZER, LOW);
  digitalWrite(PIN_MOTION_BUZZER, LOW);

  ledcSetup(BUZZER_LEDC_CHANNEL, SOUND_BUZZER_FREQUENCY_HZ, BUZZER_LEDC_RESOLUTION_BITS);
  ledcAttachPin(PIN_SOUND_BUZZER, BUZZER_LEDC_CHANNEL);
  ledcWrite(BUZZER_LEDC_CHANNEL, 0);
  ledcDetachPin(PIN_SOUND_BUZZER);
}

void updateLocalAlerts(AlertState state, uint32_t nowMs) {
  if (state == AlertState::MONITORING_OFF) {
    digitalWrite(PIN_STATUS_LED, LOW);
    stopBuzzers();
    return;
  }
  if (state == AlertState::NORMAL) {
    digitalWrite(PIN_STATUS_LED, HIGH);
    stopBuzzers();
    return;
  }

  digitalWrite(PIN_STATUS_LED, ((nowMs / 200) % 2) ? HIGH : LOW);
  if (state == AlertState::SOUND_ALERT) {
    setBuzzer(PIN_SOUND_BUZZER, SOUND_BUZZER_FREQUENCY_HZ);
  } else if (state == AlertState::MOTION_ALERT) {
    setBuzzer(PIN_MOTION_BUZZER, MOTION_BUZZER_FREQUENCY_HZ);
  } else if ((nowMs / 250) % 2 == 0) {
    setBuzzer(PIN_SOUND_BUZZER, SOUND_BUZZER_FREQUENCY_HZ);
  } else {
    setBuzzer(PIN_MOTION_BUZZER, MOTION_BUZZER_FREQUENCY_HZ);
  }
}

const char* alertStateName(AlertState state) {
  switch (state) {
    case AlertState::MONITORING_OFF: return "MONITORING OFF";
    case AlertState::NORMAL: return "NORMAL";
    case AlertState::SOUND_ALERT: return "ALERT: SOUND DETECTED";
    case AlertState::MOTION_ALERT: return "ALERT: MOVEMENT DETECTED";
    case AlertState::COMBINED_ALERT: return "ALERT: SOUND + MOVEMENT";
  }
  return "UNKNOWN";
}
const char* alertStateBlynkValue(AlertState state) {
  switch (state) {
    case AlertState::MONITORING_OFF: return "MONITORING_OFF";
    case AlertState::NORMAL: return "NORMAL";
    case AlertState::SOUND_ALERT: return "SOUND_ALERT";
    case AlertState::MOTION_ALERT: return "MOTION_ALERT";
    case AlertState::COMBINED_ALERT: return "COMBINED_ALERT";
  }
  return "UNKNOWN";
}

