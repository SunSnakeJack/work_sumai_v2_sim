#include "audio_sensor.h"

#include <Arduino.h>

#include "config.h"

bool beginAudioSensor() {
  pinMode(PIN_SIM_SOUND_POT, INPUT);
  analogReadResolution(12);
  return true;
}

float getSoundLevel() {
  const int adcValue = analogRead(PIN_SIM_SOUND_POT);
  return static_cast<float>(adcValue) * (SIM_SOUND_RMS_MAX / 4095.0f);
}

