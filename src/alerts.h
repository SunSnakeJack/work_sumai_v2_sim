#pragma once

#include <Arduino.h>

enum class AlertState : uint8_t {
  MONITORING_OFF,
  NORMAL,
  SOUND_ALERT,
  MOTION_ALERT,
  COMBINED_ALERT
};

void initializeAlerts();
void updateLocalAlerts(AlertState state, uint32_t nowMs);
const char* alertStateName(AlertState state);
const char* alertStateBlynkValue(AlertState state);

