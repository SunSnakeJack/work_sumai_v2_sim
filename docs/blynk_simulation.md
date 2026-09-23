# Manual Blynk simulation test

The current Blynk simulation build uses `ENABLE_BLYNK=1`. To test the real Blynk Cloud with simulated sensor inputs:

1. Create a Blynk Template.
2. Create a Device from that template.
3. Create V0-V4 datastreams: V0 simulated SoundRMS, V1 acceleration magnitude, V2 gyro magnitude, V3 state integer, and V4 monitoring enabled.
4. Create the `sound_alert` event.
5. Create the `movement_alert` event.
6. Create the `combined_alert` event.
7. Enable push notifications for all three events.
8. Put the Template ID, Template Name, and Device Auth Token into local `include/secrets.h`. Do not edit `secrets.example.h` with real values and do not commit `secrets.h`.
9. Confirm the `ENABLE_BLYNK` build flag in `platformio.ini` remains set to 1.
10. Build the project.
11. Start Wokwi.
12. Verify connection to the open `Wokwi-GUEST` network; its password is empty and it is used only in simulation.
13. Verify the device appears online in Blynk.
14. Verify V0-V4 telemetry updates approximately once per second.
15. Raise the potentiometer above the simulated sound threshold.
16. Verify one sound push notification.
17. Trigger MPU6050 motion above its threshold.
18. Verify one motion push notification.
19. Trigger sound and motion together and verify one combined notification.
20. Keep each alert active and repeat transitions within/after 30 seconds to verify edge behavior and per-event cooldown.

Local LED/buzzer behavior must continue if Wi-Fi or Blynk is unavailable. The firmware uses short periodic reconnect attempts and has no permanent Wi-Fi wait loop.

V3 values: `NORMAL`, `SOUND_ALERT`, `MOTION_ALERT`, `COMBINED_ALERT`, and `MONITORING_OFF`.

