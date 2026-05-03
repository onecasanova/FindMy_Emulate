# FindMy Emulate / AirGuard nRF54L15 DK

FindMy Emulate is a Zephyr/Nordic Connect SDK firmware project for detecting nearby Apple AirTag-class Bluetooth Low Energy advertisements with an nRF54L15 DK. The target application is a portable embedded scanner that can warn when the same AirTag-class advertiser is observed repeatedly over time.

The current architecture is intentionally simple and reproducible: firmware on the nRF54L15 DK performs passive BLE scanning, parses Apple manufacturer-specific advertising data, stores recent observations in RAM, and prints a serial report after each scan window. There is no required host, edge, or cloud component for the baseline demo.

## Key Features and Performance Summary

- Passive BLE observer for Apple manufacturer data using company ID `0x004c`.
- AirTag-class Find My payload filter using marker `0x12` and status mask/value `0x18`/`0x10`.
- Serial reports with advertiser address, RSSI, battery field, status byte, report count, and observation duration.
- Repeated-presence heuristic: a device is marked as a possible follower after at least 3 reports spanning at least 10 minutes.
- Default scan schedule: 15 second scan window every 180 seconds.
- Detection latency: normally one scan period or less once the firmware is running, so up to about 3 minutes with default settings.
- Range: dependent on antenna orientation, environment, advertiser power, and interference; measure with the procedure in `docs/measurements/`.
- Energy: not yet characterized. The DK is USB-powered by default; battery runtime requires a separate power measurement and battery design.

## Repository Structure

```text
firmware/        Zephyr application source and board configuration
host/            Optional host or edge tooling; unused by the baseline demo
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
- MCU family: Nordic nRF54L15 application core with integrated 2.4 GHz radio suitable for Bluetooth LE scanning.
- Product links:
  - nRF54L15 DK: https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK
  - nRF54L15 SoC: https://www.nordicsemi.com/Products/nRF54L15
  - Nordic documentation portal: https://docs.nordicsemi.com/

![nRF54L15 DK board](docs/assets/nRF54L15dk.jpeg)

### Additional Peripherals

No external peripherals are required for the baseline demo. The DK's onboard debugger, USB serial interface, and 2.4 GHz antenna path are sufficient.

Optional peripherals to document if added later:

- Battery or power bank model and capacity.
- External antenna or RF front-end model.
- Enclosure model or custom enclosure files.
- Phone, known AirTag, or BLE beacon used as a controlled test target.

### Hardware Modifications

The baseline build assumes an unmodified nRF54L15 DK. 

### Power Subsystem

The default demo is powered over USB through the DK. Battery operation has not been implemented or measured yet.

For a battery build, document:

- Battery chemistry, nominal voltage, and capacity.
- Regulator part number and measured efficiency.
- Charger part number and charging current.
- Average current for scan and idle phases.
- Estimated runtime: `runtime_hours = battery_mAh / average_current_mA`.

### RF Specifications

- Radio band: 2.4 GHz ISM band through Bluetooth Low Energy.
- Firmware role: BLE observer/passive scanner.
- Advertisement channels: standard BLE primary advertising channels are used by the controller.
- Scan type: passive scan; the device does not transmit scan requests in the baseline configuration.
- Output power: no application BLE advertising or connection transmit path is used by the scanner.
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
- Firmware source: `firmware/src/main.c`.
- Firmware config: `firmware/prj.conf` and `firmware/Kconfig`.

### Other Software

No host or cloud software is required for the baseline demo. If host tools are added, place them under `host/` and include language versions, dependencies, and lock files in that directory.

Tested OS assumptions:

- macOS or Linux development host with Nordic Connect SDK installed.
- USB access to the DK debugger and serial port.
- `west`, CMake, Ninja, and Nordic command-line flashing tools available from the NCS environment.

### Programming and Debugging Tools

- `west build` for firmware builds.
- `west flash` for programming.
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

Configured in `firmware/prj.conf`:

```conf
CONFIG_BT=y
CONFIG_BT_OBSERVER=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
```

Runtime scan parameters in `firmware/src/main.c`:

- Passive scan: `BT_LE_SCAN_TYPE_PASSIVE`.
- Scan interval/window: `BT_GAP_SCAN_FAST_INTERVAL` and `BT_GAP_SCAN_FAST_WINDOW`.
- Application scan window: 15 seconds.
- Application scan period: 180 seconds.

## Reproducibility Guide

### 1. Hardware Assembly

1. Use an nRF54L15 DK with the default onboard antenna and debugger configuration.
2. Connect the DK to the development host over USB.
3. Confirm that the board enumerates as a debugger and serial device.
4. Keep at least one known AirTag or AirTag-class Find My accessory nearby for verification.
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

From the repository root:

```sh
west build -b nrf54l15dk/nrf54l15/cpuapp firmware --build-dir build
```

For a clean rebuild:

```sh
west build -b nrf54l15dk/nrf54l15/cpuapp firmware --build-dir build --pristine
```

If your SDK exposes a different nRF54L15 DK board target, use the exact target printed by `west boards`.

### 4. Flashing and Provisioning

Flash the DK:

```sh
west flash --build-dir build
```

No keys, pairing, phone provisioning, bootloader setup, or cloud account are required for the baseline scanner. The firmware does not join Apple's Find My network; it only observes nearby BLE advertisements.

### 5. Run the Demo

Open the serial terminal at 115200 baud and reset the board.

Expected startup logs include:

```text
AirGuard nRF54L15 DK starting
Initializing Bluetooth subsystem...
Bluetooth ready. Scanning every 180 seconds.
```

Expected report shape:

```text
========== AirGuard nRF54L15 DK report 1 ==========
Scan window: 15 seconds, scan period: 180 seconds
Apple AirTag-class devices nearby: N
```

Each matching device line includes BLE address, RSSI, battery field, reports seen, observation time, and status byte.

### 6. Testing and Measurement

Functional test:

1. Flash the firmware.
2. Place a known AirTag-class device within a few meters of the DK.
3. Capture serial logs for at least 15 minutes.
4. Confirm that repeated sightings appear in multiple reports.
5. Confirm that a device seen in at least 3 reports spanning at least 10 minutes appears in the "may be following" section.

Range test:

1. Place the DK at a fixed location and height.
2. Move the known target to measured distances such as 1 m, 3 m, 5 m, 10 m, and 20 m.
3. Capture at least 5 scan windows per distance.
4. Record detection rate and RSSI in `docs/measurements/range-results.md`.

Energy test:

1. Power the DK through a current meter or power profiler.
2. Measure current during scan windows and between scan windows.
3. Compute average current over a full 180 second period.
4. Record results in `docs/measurements/energy-results.md`.

### 7. Troubleshooting

- No serial output: confirm the serial port, 115200 baud, and that `CONFIG_UART_CONSOLE=y`.
- Build cannot find board target: run `west boards | rg nrf54l15dk` and update the build command to match your installed SDK.
- Flash fails: confirm the DK debugger is visible and no other program is using the debug probe.
- Bluetooth init fails: reflash with a pristine build and confirm the application core target is used.
- No AirTag-class detections: make sure a compatible device is nearby and wait through a full 15 second scan window.
- Apple payloads are heard but not classified: set `CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS=y` in `firmware/prj.conf`, rebuild, and inspect the printed manufacturer payloads.
- Repeated device not marked as following: the same BLE address must appear in at least 3 reports spanning at least 10 minutes. Rotating private addresses can reset the heuristic.

### 8. Offline Mode

The baseline demo is fully offline. It needs no Wi-Fi, cellular, cloud service, phone app, account, or internet connection after the Nordic toolchain is installed. If cloud or host components are added later, keep an offline log-only path available through the serial report.

## Project Limitations

This project does not decrypt Find My traffic, identify an AirTag owner, or access Apple's private network data. It uses BLE advertisement metadata and repeated-presence heuristics. Because AirTags can rotate BLE addresses, repeated-presence detection can miss a device after address rotation and should be treated as a warning signal, not a definitive tracking attribution.
