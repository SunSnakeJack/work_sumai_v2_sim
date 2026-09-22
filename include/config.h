#pragma once

#include <Arduino.h>

#ifndef SIMULATION_MODE
#define SIMULATION_MODE 1
#endif

#ifndef ENABLE_BLYNK
#define ENABLE_BLYNK 0
#endif

static_assert(SIMULATION_MODE == 1, "This project is the Wokwi simulation adapter; real I2S remains in work_sumai_v2_real");

constexpr uint8_t MPU6050_ADDRESS = 0x68;
constexpr int PIN_SIM_SOUND_POT = 0;
constexpr int PIN_MONITORING_SWITCH = 3;
constexpr int PIN_MPU_SDA = 4;
constexpr int PIN_MPU_SCL = 5;
constexpr int PIN_STATUS_LED = 6;
constexpr int PIN_SOUND_BUZZER = 7;
constexpr int PIN_MOTION_BUZZER = 10;

constexpr float SIM_SOUND_RMS_MAX = 4095.0f;
constexpr float SIM_SOUND_RMS_THRESHOLD = 2500.0f;
constexpr float ACCEL_DEVIATION_THRESHOLD_G = 0.25f;
constexpr float GYRO_THRESHOLD_DPS = 35.0f;

constexpr uint8_t BUZZER_LEDC_CHANNEL = 0;
constexpr uint8_t BUZZER_LEDC_RESOLUTION_BITS = 10;
constexpr uint32_t SOUND_BUZZER_FREQUENCY_HZ = 1500;
constexpr uint32_t MOTION_BUZZER_FREQUENCY_HZ = 2200;

constexpr uint32_t SENSOR_INTERVAL_MS = 100;
constexpr uint32_t STATUS_INTERVAL_MS = 500;
constexpr uint32_t BLYNK_TELEMETRY_INTERVAL_MS = 1000;
constexpr uint32_t BLYNK_RECONNECT_INTERVAL_MS = 10000;
constexpr uint32_t WIFI_RECONNECT_INTERVAL_MS = 15000;
constexpr uint32_t BLYNK_EVENT_COOLDOWN_MS = 30000;

constexpr char SIM_WIFI_SSID[] = "Wokwi-GUEST";
constexpr char SIM_WIFI_PASSWORD[] = "";

