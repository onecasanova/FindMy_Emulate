# Apple Find My Emulator on nRF54L15 — Turning a Nordic DK into an AirTag

Apple's Find My network spans billions of iPhones, silently relaying encrypted location
reports from lost trackers back to their owners. The protocol is proprietary and
undocumented — but reverse-engineered. This project is the first open implementation of
Apple's Offline Finding advertisement on Nordic Semiconductor hardware running Zephyr RTOS.

A P-224 key pair is generated offline and baked into the firmware. The nRF54L15 DK
broadcasts a spec-compliant Offline Finding BLE advertisement every 1–2 seconds. Any
passing iPhone picks it up, encrypts the device's location, and silently uploads it to
Apple's servers — no pairing, no app, no Apple account on the device. Location reports
are retrieved and decrypted via OpenHaystack on macOS using the matching private key.

Validated end-to-end: the advertisement is detected and correctly classified as an AirTag
by AirGuard (Android) and nRF Connect, and location reports appear in OpenHaystack within
minutes of the board being left in a populated area. Total firmware footprint is under 50 KB.