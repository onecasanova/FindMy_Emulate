# FindMy Emulate — nRF54L15 DK

FindMy Emulate is a Zephyr/Nordic Connect SDK firmware project that pairs two complementary applications running on the nRF54L15 DK:

1. **AirGuard scanner** — a passive BLE observer that detects nearby Apple AirTag-class advertisements, tracks repeated sightings, and prints a serial report after each scan window.
2. **Airtag broadcaster** — a BLE peripheral that emits an Apple Find My manufacturer-specific payload, so a second nRF54L15 DK can act as a controlled AirTag-class advertiser for testing the scanner.

Together they make a self-contained two-board demo: flash the broadcaster on one DK and the scanner on another, and the scanner should report the broadcaster as an AirTag-class device. There is no required host, edge, or cloud component for the baseline demo.

## Key Features and Performance Summary

### AirGuard scanner (`firmware/AirGuard`)

- Passive BLE observer for Apple manufacturer data using company ID `0x004c`.
- AirTag-class Find My payload filter using marker `0x12` and status mask/value `0x18`/`0x10`.
- Serial reports with advertiser address, RSSI, battery field, status byte, report count, and observation duration.
- Repeated-presence heuristic: a device is marked as a possible follower after at least 3 reports spanning at least 10 minutes.
- Default scan schedule: 15 second scan window every 180 seconds.
- Detection latency: normally one scan period or less once the firmware is running, so up to about 3 minutes with default settings.

### Airtag broadcaster (`firmware/Airtag`)

- Non-connectable, non-scannable BLE advertiser using the 30-byte Apple Find My manufacturer payload layout.
- Hardcoded compressed P-224 public key in `firmware/Airtag/src/keys.h`; the BLE random address and the key bytes inside the manufacturer payload are derived from this key at startup.
- Status byte chosen so the AirGuard scanner classifies it as AirTag-class (`(status & 0x18) == 0x10`).
- Advertisement interval: ~2 seconds.

### General

- Range: dependent on antenna orientation, environment, advertiser power, and interference; measure with the procedure in `docs/measurements/`.
- Energy: not yet characterized. The DK is USB-powered by default; battery runtime requires a separate power measurement and battery design.

## Repository Structure

```text
firmware/
  AirGuard/      Scanner application (Apple manufacturer-data observer)
  Airtag/        Broadcaster application (Find My-style advertiser)
cloud/           Optional cloud integration; unused by the baseline demo
docs/            Hardware notes, measurements, diagrams, and project notes
scripts/         Reproducibility and measurement helper scripts
README.md        Comprehensive build and reproduction guide
intro.md         Advertisement-style project introduction
```

Generated directories such as `build/` and local environments such as `emb/` are intentionally ignored.

## Hardware Details

### Board and MCU

- Board: Nordic Semiconductor nRF54L15 DK.
- Board target used by this project: `nrf54l15dk/nrf54l15/cpuapp`.
- MCU family: Nordic nRF54L15 application core with integrated 2.4 GHz radio suitable for Bluetooth LE scanning and advertising.
- Product links:
  - nRF54L15 DK: https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK
  - nRF54L15 SoC: https://www.nordicsemi.com/Products/nRF54L15
  - Nordic documentation portal: https://docs.nordicsemi.com/

![nRF54L15 DK board](docs/assets/nRF54L15dk.jpeg)

### Additional Peripherals

No external peripherals are required for the baseline demo. Each DK's onboard debugger, USB serial interface, and 2.4 GHz antenna path are sufficient. The two-board demo uses two unmodified nRF54L15 DKs connected over USB to the same development host.

Optional peripherals to document if added later:

- Battery or power bank model and capacity.
- External antenna or RF front-end model.
- Enclosure model or custom enclosure files.
- Phone, known AirTag, or BLE beacon used as a controlled test target alongside the broadcaster DK.

### Hardware Modifications

The baseline build assumes unmodified nRF54L15 DK boards.

### Power Subsystem

The default demo is powered over USB through the DK. Battery operation has not been implemented or measured yet.

For a battery build, document:

- Battery chemistry, nominal voltage, and capacity.
- Regulator part number and measured efficiency.
- Charger part number and charging current.
- Average current for scan and idle phases (scanner) or advertise and idle phases (broadcaster).
- Estimated runtime: `runtime_hours = battery_mAh / average_current_mA`.

### RF Specifications

- Radio band: 2.4 GHz ISM band through Bluetooth Low Energy.
- Firmware roles: BLE observer/passive scanner (AirGuard) and BLE broadcaster (Airtag).
- Advertisement channels: standard BLE primary advertising channels are used by the controller.
- Scan type (AirGuard): passive scan; the device does not transmit scan requests in the baseline configuration.
- Output power (Airtag): default Zephyr/NCS BLE TX power for the nRF54L15 controller; no application-level TX power override.
- Expected range: environment dependent; characterize in `docs/measurements/`.
- Compliance note: use the DK and antenna configuration as intended by the vendor. Any external antenna, enclosure, RF front-end, or intentional transmitter feature needs a separate compliance review.

## Software Environment

### Firmware

- IDE/toolchain: Nordic Connect SDK command-line workflow with `west`.
- SDK/RTOS: Nordic Connect SDK v3.2.1, Zephyr-based.
- `west` version observed in the previous local build: 1.4.0.
- Build system: Zephyr CMake/sysbuild.
- Compiler: Nordic Connect SDK toolchain bundled with NCS v3.2.1.
- Board configuration: `nrf54l15dk/nrf54l15/cpuapp`.
- Scanner source: `firmware/AirGuard/src/main.c`, configured by `firmware/AirGuard/prj.conf` and `firmware/AirGuard/Kconfig`.
- Broadcaster source: `firmware/Airtag/src/main.c` and `firmware/Airtag/src/keys.h`, configured by `firmware/Airtag/prj.conf`.

### Other Software

No host or cloud software is required. The two firmware applications run independently on their respective DKs and communicate only over the air.

Tested OS assumptions:

- macOS or Linux development host with Nordic Connect SDK installed for firmware builds.
- USB access to each DK debugger and serial port.
- `west`, CMake, Ninja, and Nordic command-line flashing tools available from the NCS environment.

### Programming and Debugging Tools

- `west build` for firmware builds.
- `west flash` for programming, with `--dev-id <serial>` to select between two attached DKs.
- `nrfjprog --ids` to list connected DK debugger serial numbers.
- Serial terminal at 115200 baud for runtime logs.
- Onboard DK debugger for programming and serial output.

Useful serial tools:

```sh
screen /dev/tty.usbmodem* 115200
```

or:

```sh
picocom -b 115200 /dev/ttyACM0
```

### Radio Stack and Protocol Configuration

Scanner — configured in `firmware/AirGuard/prj.conf`:

```conf
CONFIG_BT=y
CONFIG_BT_OBSERVER=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
```

Scanner runtime parameters in `firmware/AirGuard/src/main.c`:

- Passive scan: `BT_LE_SCAN_TYPE_PASSIVE`.
- Scan interval/window: `BT_GAP_SCAN_FAST_INTERVAL` and `BT_GAP_SCAN_FAST_WINDOW`.
- Application scan window: 15 seconds.
- Application scan period: 180 seconds.

Broadcaster — configured in `firmware/Airtag/prj.conf`:

```conf
CONFIG_BT=y
CONFIG_BT_BROADCASTER=y
CONFIG_BT_ID_MAX=2
```

Broadcaster runtime parameters in `firmware/Airtag/src/main.c`:

- Advertisement type: non-connectable, non-scannable.
- Advertisement interval: ~2 seconds.
- Identity address: BLE static random, derived from `public_key[0..5]` with the top two bits of `public_key[0]` forced to `1`.
- Manufacturer payload: 29 bytes after Zephyr's AD length and AD type prefix, matching the Find My byte layout (Apple company ID, Offline Finding type `0x12`, length `0x19`, status byte, last 22 bytes of the compressed public key, `key[0] >> 6`, hint byte).

## Reproducibility Guide

### 1. Hardware Assembly

1. Use one or two nRF54L15 DKs with the default onboard antenna and debugger configuration.
2. Connect each DK to the development host over USB.
3. Confirm that each board enumerates as a debugger and serial device.
4. For the two-board demo, run `nrfjprog --ids` and note the serial number of each DK so they can be addressed individually with `west flash --dev-id`.
5. Add any wiring diagrams, pin maps, photos, or hardware changes under `docs/hardware/`.

### 2. Environment Setup

Install Nordic Connect SDK v3.2.1 using Nordic's official installation flow, then open a shell with the NCS environment active.

Verify tools:

```sh
west --version
cmake --version
ninja --version
```

Verify the board target:

```sh
west boards | rg nrf54l15dk
```

If `rg` is unavailable, use:

```sh
west boards | grep nrf54l15dk
```

### 3. Build Firmware

Each application is built separately with its own build directory. From the repository root:

Scanner:

```sh
west build -b nrf54l15dk/nrf54l15/cpuapp firmware/AirGuard --build-dir build_airguard
```

Broadcaster:

```sh
west build -b nrf54l15dk/nrf54l15/cpuapp firmware/Airtag --build-dir build_airtag
```

For a clean rebuild of either application, append `--pristine` to the corresponding command.

If your SDK exposes a different nRF54L15 DK board target, use the exact target printed by `west boards`.

### 4. Flashing and Provisioning

List connected DKs and note their serial numbers:

```sh
nrfjprog --ids
```

Flash the scanner to one DK:

```sh
west flash --build-dir build_airguard --dev-id <serial-of-scanner-board>
```

Flash the broadcaster to the other DK:

```sh
west flash --build-dir build_airtag --dev-id <serial-of-broadcaster-board>
```

If only one DK is attached, `--dev-id` can be omitted.

No keys, pairing, phone provisioning, bootloader setup, or cloud account are required for the baseline demo. The firmware does not join Apple's Find My network; the scanner only observes nearby BLE advertisements, and the broadcaster only emits a Find My-shaped payload locally.

The broadcaster ships with a placeholder compressed P-224 public key in `firmware/Airtag/src/keys.h`. Replace it with a real key generated offline (see the Python snippet in `CLAUDE.md`) before relying on the payload for anything beyond a self-test against the AirGuard scanner.

### 5. Run the Demo

Open a serial terminal at 115200 baud for each DK and reset the boards.

Expected scanner startup logs:

```text
AirGuard nRF54L15 DK starting
Initializing Bluetooth subsystem...
Bluetooth ready. Scanning every 180 seconds.
```

Expected scanner report shape:

```text
========== AirGuard nRF54L15 DK report 1 ==========
Scan window: 15 seconds, scan period: 180 seconds
Apple AirTag-class devices nearby: N
```

Each matching device line includes BLE address, RSSI, battery field, reports seen, observation time, and status byte. With the Airtag broadcaster powered nearby, it should appear in the report as an AirTag-class device using the BLE random address derived from its hardcoded public key.

The broadcaster prints the manufacturer payload it is advertising on its own serial console at startup, which is useful for confirming the byte layout matches the expected Find My structure.

### 6. Testing and Measurement

Functional test (two-board demo):

1. Flash the scanner to one DK and the broadcaster to the other.
2. Place both DKs within a few meters of each other.
3. Capture serial logs from the scanner for at least 15 minutes.
4. Confirm that the broadcaster's address appears in successive scanner reports.
5. Confirm that a device seen in at least 3 reports spanning at least 10 minutes appears in the "may be following" section.

Functional test (against a real AirTag):

1. Flash the scanner only.
2. Place a known AirTag-class device within a few meters of the DK.
3. Capture serial logs for at least 15 minutes and confirm repeated sightings as above.

Range test:

1. Place the scanner at a fixed location and height.
2. Move the broadcaster (or a known target) to measured distances such as 1 m, 3 m, 5 m, 10 m, and 20 m.
3. Capture at least 5 scan windows per distance.
4. Record detection rate and RSSI in `docs/measurements/range-results.md`.

Energy test:

1. Power each DK through a current meter or power profiler.
2. For the scanner, measure current during scan windows and between scan windows; compute the average over a full 180 second period.
3. For the broadcaster, measure current during advertising events and between events at the ~2 second interval.
4. Record results in `docs/measurements/energy-results.md`.

### 7. Troubleshooting

- No serial output: confirm the serial port, 115200 baud, and that `CONFIG_UART_CONSOLE=y`.
- Build cannot find board target: run `west boards | rg nrf54l15dk` and update the build command to match your installed SDK.
- Flash fails or programs the wrong board: confirm both DK debuggers are visible with `nrfjprog --ids` and pass the correct `--dev-id` to `west flash`.
- Bluetooth init fails: reflash with a pristine build and confirm the application core target is used.
- No AirTag-class detections: make sure the broadcaster DK is powered, or that a compatible Apple device is nearby, and wait through a full 15 second scan window.
- Apple payloads are heard but not classified: set `CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS=y` in `firmware/AirGuard/prj.conf`, rebuild, and inspect the printed manufacturer payloads.
- Broadcaster is heard but skipped by the scanner: confirm the status byte in `firmware/Airtag/src/main.c` satisfies `(status & 0x18) == 0x10`. The OpenHaystack default of `0x00` will not pass the AirTag-class filter.
- Repeated device not marked as following: the same BLE address must appear in at least 3 reports spanning at least 10 minutes. Rotating private addresses can reset the heuristic.

### 8. Offline Mode

The baseline demo is fully offline. It needs no Wi-Fi, cellular, cloud service, phone app, account, or internet connection after the Nordic toolchain is installed. If cloud or host components are added later, keep an offline log-only path available through the serial report.

## Project Limitations

This project does not decrypt Find My traffic, identify an AirTag owner, or access Apple's private network data. It uses BLE advertisement metadata and repeated-presence heuristics. Because AirTags can rotate BLE addresses, repeated-presence detection can miss a device after address rotation and should be treated as a warning signal, not a definitive tracking attribution. The Airtag broadcaster is intended for educational use in a controlled lab environment; do not deploy it where it could interfere with real Find My users or be mistaken for a registered tracker.
