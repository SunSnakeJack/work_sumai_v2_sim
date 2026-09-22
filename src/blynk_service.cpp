#include "blynk_service.h"

#include "config.h"

#if ENABLE_BLYNK
#include "secrets.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

namespace {
AlertState previousState = AlertState::MONITORING_OFF;
uint32_t lastTelemetryMs = 0;
uint32_t lastWifiAttemptMs = 0;
uint32_t lastBlynkAttemptMs = 0;
uint32_t lastSoundEventMs = 0;
uint32_t lastMotionEventMs = 0;
uint32_t lastCombinedEventMs = 0;

bool cooldownExpired(uint32_t nowMs, uint32_t lastEventMs) {
  return lastEventMs == 0 || nowMs - lastEventMs >= BLYNK_EVENT_COOLDOWN_MS;
}

void sendTransitionEvent(AlertState state, uint32_t nowMs) {
  if (!Blynk.connected()) return;
  if (state == AlertState::SOUND_ALERT && previousState == AlertState::NORMAL && cooldownExpired(nowMs, lastSoundEventMs)) {
    Blynk.logEvent("sound_alert", "Simulated sound RMS crossed the test threshold");
    lastSoundEventMs = nowMs;
  } else if (state == AlertState::MOTION_ALERT && previousState == AlertState::NORMAL && cooldownExpired(nowMs, lastMotionEventMs)) {
    Blynk.logEvent("motion_alert", "Simulated movement crossed the prototype threshold");
    lastMotionEventMs = nowMs;
  } else if (state == AlertState::COMBINED_ALERT && previousState != AlertState::COMBINED_ALERT && cooldownExpired(nowMs, lastCombinedEventMs)) {
    Blynk.logEvent("combined_alert", "Simulated sound and movement alerts are active");
    lastCombinedEventMs = nowMs;
  }
}
}
#endif

void initializeBlynkService() {
#if ENABLE_BLYNK
  WiFi.mode(WIFI_STA);
  WiFi.begin(SIM_WIFI_SSID, SIM_WIFI_PASSWORD);
  Blynk.config(BLYNK_AUTH_TOKEN);
#endif
}

void serviceBlynk(AlertState state, float soundRms, float accelMagnitude,
                  float gyroMagnitude, bool monitoringEnabled, uint32_t nowMs) {
#if ENABLE_BLYNK
  if (WiFi.status() != WL_CONNECTED) {
    if (nowMs - lastWifiAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
      WiFi.begin(SIM_WIFI_SSID, SIM_WIFI_PASSWORD);
      lastWifiAttemptMs = nowMs;
    }
    previousState = state;
    return;
  }
  if (!Blynk.connected() && nowMs - lastBlynkAttemptMs >= BLYNK_RECONNECT_INTERVAL_MS) {
    Blynk.connect(50);
    lastBlynkAttemptMs = nowMs;
  }
  if (!Blynk.connected()) {
    previousState = state;
    return;
  }

  Blynk.run();
  sendTransitionEvent(state, nowMs);
  if (nowMs - lastTelemetryMs >= BLYNK_TELEMETRY_INTERVAL_MS) {
    Blynk.virtualWrite(V0, soundRms);
    Blynk.virtualWrite(V1, accelMagnitude);
    Blynk.virtualWrite(V2, gyroMagnitude);
    Blynk.virtualWrite(V3, static_cast<int>(state));
    Blynk.virtualWrite(V4, monitoringEnabled ? 1 : 0);
    lastTelemetryMs = nowMs;
  }
  previousState = state;
#else
  (void)state;
  (void)soundRms;
  (void)accelMagnitude;
  (void)gyroMagnitude;
  (void)monitoringEnabled;
  (void)nowMs;
#endif
}

