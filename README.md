# WORK_SUMAI V2 Wokwi simulation

System-level Wokwi counterpart of `work_sumai_v2_real` for exercising the V2 alert state machine, MPU6050 logic, LED, buzzers, monitoring switch, and optional Blynk telemetry/events.

The potentiometer on GPIO0 is only a controllable test adapter that produces the same logical `SoundRMS` metric consumed by V2. It is **not** an INMP441 replacement, I2S emulation, microphone-quality model, or validation of the real audio driver. Real INMP441/I2S firmware remains in the separate read-only `work_sumai_v2_real` project.

The default build uses `SIMULATION_MODE=1` and `ENABLE_BLYNK=0`. Local simulation needs no credentials. See `docs/blynk_simulation.md` before enabling cloud tests.

## Local controls

- Potentiometer: simulated sound RMS, approximately 0-4095; alert threshold 2500.
- MPU6050: Wokwi motion control; motion thresholds are 0.25g acceleration deviation or 35 dps gyro magnitude.
- Slide switch: GPIO3. LOW enables monitoring; HIGH disables it.
- Status LED: GPIO6.
- Sound and motion buzzers: GPIO7 and GPIO10.

Build with PlatformIO, then start the existing Wokwi VS Code simulator using `diagram.json` and `wokwi.toml`.

This is a prototype simulation, not a medical device or safety certification. It must not replace adult supervision.

