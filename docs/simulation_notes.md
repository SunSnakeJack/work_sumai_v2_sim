# Simulation scope and limitations

This is a V2 system-level simulation. Wokwi does not provide the usable ESP32-C3 I2S peripheral behavior required for this project's INMP441 path, so the projects are deliberately separated:

- `work_sumai_v2_real` contains the real `driver/i2s.h` receive path and RMS processing.
- `work_sumai_v2_sim` contains an ADC adapter that converts a potentiometer position into the same logical `SoundRMS` input used by the alert logic.

The potentiometer simulates the **output metric** of the microphone processing path. It does not simulate INMP441 electrical behavior, ESP32-C3 I2S timing or DMA, microphone frequency response, noise, sensitivity, sample quality, acoustic enclosure effects, or real threshold calibration.

This project also does not validate battery/TP4056 behavior, regulators, real buzzer drive current, wearable enclosure design, thermal behavior, mechanical protection, strap safety, or suitability for use on a child. Real V2 firmware and hardware verification remain separate.

## Simulation pin map

| Function | Simulation GPIO |
|---|---:|
| Potentiometer / simulated SoundRMS | 0 |
| Monitoring switch | 3 |
| MPU6050 SDA | 4 |
| MPU6050 SCL | 5 |
| Status LED | 6 |
| Sound buzzer | 7 |
| Motion buzzer | 10 |

This intentionally differs from the V2 REAL pin map. Never copy the simulation pin plan into the physical build without a separate hardware review.

The slide switch connects its center to GPIO3 and its sides to GND and 3.3V, while firmware also uses `INPUT_PULLUP`; neither switch position is floating.

