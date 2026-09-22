#pragma once

#include <Arduino.h>

#include "alerts.h"

void initializeBlynkService();
void serviceBlynk(AlertState state, float soundRms, float accelMagnitude,
                  float gyroMagnitude, bool monitoringEnabled, uint32_t nowMs);

