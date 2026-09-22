# Wokwi test plan

## Initial local mode

Build flags: `SIMULATION_MODE=1`, `ENABLE_BLYNK=0`.

- [ ] Boot: serial reports simulation adapter ready and MPU6050 connected.
- [ ] Normal: low potentiometer, near-1g acceleration, near-0 dps gyro; LED steady, buzzers silent, state NORMAL.
- [ ] Sound alert: raise potentiometer until simulated RMS is at least 2500; flashing LED and sound buzzer.
- [ ] Sound recovery: lower potentiometer below 2500; NORMAL and silent buzzers.
- [ ] Motion alert: set Wokwi MPU6050 gyro magnitude above 35 dps; flashing LED and motion buzzer.
- [ ] Motion recovery: return gyro below threshold and acceleration near 1g; NORMAL.
- [ ] Combined alert: high potentiometer and motion above threshold; alternating distinguishable buzzer types.
- [ ] Monitoring OFF: move switch to HIGH; LED off and both buzzers immediately silent.
- [ ] Re-enable: move switch to LOW; monitoring resumes from current inputs.
- [ ] Runtime: serial terminal contains zero repeating or first-use `LEDC is not initialized` errors.

## Optional cloud mode

Follow `blynk_simulation.md`. Verify Wokwi-GUEST, online state, V0-V4 telemetry, edge-triggered events, cooldown, loss of network without loss of local alerts, and recovery after reconnection.

Do not record a PASS for controls that were not actually exercised in Wokwi.

