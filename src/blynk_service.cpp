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
uint32_t blynkAttemptStartedMs = 0;
uint32_t lastSoundEventMs = 0;
uint32_t lastMotionEventMs = 0;
uint32_t lastCombinedEventMs = 0;
bool wifiWasConnected = false;
bool blynkWasConnected = false;
bool blynkAttemptInProgress = false;
#if ENABLE_BLYNK_NETWORK_DIAGNOSTICS
bool networkDiagnosticsHaveRun = false;
#endif
bool pendingSoundEvent = false;
bool pendingMotionEvent = false;
bool pendingCombinedEvent = false;
bool alertIncidentActive = false;
bool combinedOccurredInIncident = false;
bool soundQueuedInIncident = false;
bool motionQueuedInIncident = false;
#if ENABLE_BLYNK_NETWORK_DIAGNOSTICS
uint32_t lastNetworkDiagnosticsMs = 0;

constexpr uint32_t NETWORK_DIAGNOSTICS_INTERVAL_MS = 30000;
constexpr int32_t NETWORK_DIAGNOSTICS_TCP_TIMEOUT_MS = 500;

void runNetworkDiagnostics(uint32_t nowMs) {
  if (networkDiagnosticsHaveRun &&
      nowMs - lastNetworkDiagnosticsMs < NETWORK_DIAGNOSTICS_INTERVAL_MS) {
    return;
  }

  networkDiagnosticsHaveRun = true;
  lastNetworkDiagnosticsMs = nowMs;
  Serial.printf("[BlynkDiag] DNS lookup: %s\r\n", BLYNK_SERVER_HOST);

  IPAddress resolvedIp;
  if (!WiFi.hostByName(BLYNK_SERVER_HOST, resolvedIp)) {
    Serial.println("[BlynkDiag] DNS FAILED");
    return;
  }

  Serial.print("[BlynkDiag] DNS OK: ");
  Serial.println(resolvedIp);
  Serial.printf("[BlynkDiag] TCP test: %s:%u\r\n", BLYNK_SERVER_HOST,
                BLYNK_SERVER_PORT);

  WiFiClient diagnosticClient;
  const bool tcpConnected = diagnosticClient.connect(
      resolvedIp, BLYNK_SERVER_PORT, NETWORK_DIAGNOSTICS_TCP_TIMEOUT_MS);
  Serial.println(tcpConnected ? "[BlynkDiag] TCP OK"
                              : "[BlynkDiag] TCP FAILED");
  diagnosticClient.stop();
}
#endif

bool cooldownExpired(uint32_t nowMs, uint32_t lastEventMs) {
  return lastEventMs == 0 || nowMs - lastEventMs >= BLYNK_EVENT_COOLDOWN_MS;
}

void handleAlertTransition(AlertState state, uint32_t nowMs) {
  if (state == previousState) return;

  if (state == AlertState::NORMAL || state == AlertState::MONITORING_OFF) {
    alertIncidentActive = false;
    combinedOccurredInIncident = false;
    soundQueuedInIncident = false;
    motionQueuedInIncident = false;
    return;
  }

  if (!alertIncidentActive) {
    alertIncidentActive = true;
    combinedOccurredInIncident = false;
    soundQueuedInIncident = false;
    motionQueuedInIncident = false;
  }

  const bool offline = WiFi.status() != WL_CONNECTED || !Blynk.connected();

  if (state == AlertState::SOUND_ALERT) {
    if (combinedOccurredInIncident) {
      Serial.println(offline
                         ? "[Blynk] sound_alert not queued (combined pending)"
                         : "[Blynk] sound_alert suppressed (combined incident)");
    } else if (offline) {
      if (!pendingSoundEvent) {
        pendingSoundEvent = true;
        soundQueuedInIncident = true;
        Serial.println("[Blynk] sound_alert queued (offline)");
      }
    } else if (cooldownExpired(nowMs, lastSoundEventMs)) {
      Blynk.logEvent("sound_alert", "Simulated sound RMS crossed the test threshold");
      lastSoundEventMs = nowMs;
      Serial.println("[Blynk] sound_alert sent");
    }
  } else if (state == AlertState::MOTION_ALERT) {
    if (combinedOccurredInIncident) {
      Serial.println(offline
                         ? "[Blynk] movement_alert not queued (combined pending)"
                         : "[Blynk] movement_alert suppressed (combined incident)");
    } else if (offline) {
      if (!pendingMotionEvent) {
        pendingMotionEvent = true;
        motionQueuedInIncident = true;
        Serial.println("[Blynk] movement_alert queued (offline)");
      }
    } else if (cooldownExpired(nowMs, lastMotionEventMs)) {
      Blynk.logEvent("movement_alert", "Simulated movement crossed the prototype threshold");
      lastMotionEventMs = nowMs;
      Serial.println("[Blynk] movement_alert sent");
    }
  } else if (state == AlertState::COMBINED_ALERT) {
    combinedOccurredInIncident = true;
    if (offline) {
      if (soundQueuedInIncident && pendingSoundEvent) {
        pendingSoundEvent = false;
        soundQueuedInIncident = false;
        Serial.println("[Blynk] sound_alert superseded by combined_alert");
      }
      if (motionQueuedInIncident && pendingMotionEvent) {
        pendingMotionEvent = false;
        motionQueuedInIncident = false;
        Serial.println("[Blynk] movement_alert superseded by combined_alert");
      }
      if (!pendingCombinedEvent) {
        pendingCombinedEvent = true;
        Serial.println("[Blynk] combined_alert queued (offline)");
      }
    } else if (cooldownExpired(nowMs, lastCombinedEventMs)) {
      Blynk.logEvent("combined_alert", "Simulated sound and movement alerts are active");
      lastCombinedEventMs = nowMs;
      Serial.println("[Blynk] combined_alert sent");
    }
  }
}
void sendPendingEvents(uint32_t nowMs) {
  if (!Blynk.connected()) return;

  if (pendingSoundEvent && cooldownExpired(nowMs, lastSoundEventMs)) {
    Blynk.logEvent("sound_alert", "Simulated sound RMS crossed the test threshold");
    pendingSoundEvent = false;
    lastSoundEventMs = nowMs;
    Serial.println("[Blynk] pending sound_alert sent");
  }
  if (pendingMotionEvent && cooldownExpired(nowMs, lastMotionEventMs)) {
    Blynk.logEvent("movement_alert", "Simulated movement crossed the prototype threshold");
    pendingMotionEvent = false;
    lastMotionEventMs = nowMs;
    Serial.println("[Blynk] pending movement_alert sent");
  }
  if (pendingCombinedEvent && cooldownExpired(nowMs, lastCombinedEventMs)) {
    Blynk.logEvent("combined_alert", "Simulated sound and movement alerts are active");
    pendingCombinedEvent = false;
    lastCombinedEventMs = nowMs;
    Serial.println("[Blynk] pending combined_alert sent");
  }
}
}
#endif

void initializeBlynkService() {
#if ENABLE_BLYNK
  WiFi.mode(WIFI_STA);
  Serial.println("[Blynk] WiFi connection attempt");
  WiFi.begin(SIM_WIFI_SSID, SIM_WIFI_PASSWORD);
  Serial.printf("[Blynk] server: %s\r\n", BLYNK_SERVER_HOST);
  Serial.printf("[Blynk] port: %u\r\n", BLYNK_SERVER_PORT);
  Blynk.config(BLYNK_AUTH_TOKEN, BLYNK_SERVER_HOST, BLYNK_SERVER_PORT);
#endif
}

void serviceBlynk(AlertState state, float soundRms, float accelMagnitude,
                  float gyroMagnitude, bool monitoringEnabled, uint32_t nowMs) {
#if ENABLE_BLYNK
  handleAlertTransition(state, nowMs);
  if (WiFi.status() != WL_CONNECTED) {
    wifiWasConnected = false;
    blynkWasConnected = false;
    blynkAttemptInProgress = false;
#if ENABLE_BLYNK_NETWORK_DIAGNOSTICS
    networkDiagnosticsHaveRun = false;
#endif
    if (nowMs - lastWifiAttemptMs >= WIFI_RECONNECT_INTERVAL_MS) {
      Serial.println("[Blynk] WiFi reconnect attempt");
      WiFi.begin(SIM_WIFI_SSID, SIM_WIFI_PASSWORD);
      lastWifiAttemptMs = nowMs;
    }
    previousState = state;
    return;
  }
  if (!wifiWasConnected) {
    Serial.println("[Blynk] WiFi connected");
    wifiWasConnected = true;
  }
#if ENABLE_BLYNK_NETWORK_DIAGNOSTICS
  if (!Blynk.connected()) {
    runNetworkDiagnostics(nowMs);
  }
#endif
  if (!Blynk.connected() && !blynkAttemptInProgress &&
      nowMs - lastBlynkAttemptMs >= BLYNK_RECONNECT_INTERVAL_MS) {
    Serial.println("[Blynk] reconnect attempt");
    (void)Blynk.connect(0);
    blynkAttemptInProgress = true;
    blynkAttemptStartedMs = nowMs;
    lastBlynkAttemptMs = nowMs;
  }
  if (blynkAttemptInProgress && !Blynk.connected()) {
    Blynk.run();
    if (Blynk.connected()) {
      Serial.println("[Blynk] connect result: SUCCESS");
      blynkAttemptInProgress = false;
    } else if (nowMs - blynkAttemptStartedMs >=
               BLYNK_CONNECT_SESSION_TIMEOUT_MS) {
      Serial.println("[Blynk] connect result: FAILED");
      Blynk.disconnect();
      blynkAttemptInProgress = false;
    }
  }
  if (!Blynk.connected()) {
    blynkWasConnected = false;
    previousState = state;
    return;
  }
  if (!blynkWasConnected) {
    Serial.println("[Blynk] connected");
    blynkWasConnected = true;
  }

  Blynk.run();
  sendPendingEvents(nowMs);
  if (nowMs - lastTelemetryMs >= BLYNK_TELEMETRY_INTERVAL_MS) {
    Blynk.virtualWrite(V0, soundRms);
    Blynk.virtualWrite(V1, accelMagnitude);
    Blynk.virtualWrite(V2, gyroMagnitude);
    Blynk.virtualWrite(V3, alertStateBlynkValue(state));
    Blynk.virtualWrite(V4, monitoringEnabled ? 1 : 0);
    lastTelemetryMs = nowMs;
    Serial.println("[Blynk] telemetry sent");
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

