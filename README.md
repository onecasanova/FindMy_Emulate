# AirGuard nRF54L15 DK

This folder is a standalone Zephyr/Nordic Connect SDK application for the nRF54L15 DK.
It scans BLE advertisements every 3 minutes, prints nearby Apple AirTag-class devices,
and marks devices as possible trackers when the same BLE address is repeatedly observed
over a sustained period.

## Build and flash

From this directory, with Nordic Connect SDK initialized:

```sh
west build -b nrf54l15dk/nrf54l15/cpuapp .
west flash
```

If your installed SDK exposes a different board target, list the available names:

```sh
west boards | rg nrf54l15dk
```

Then rebuild with that board name.

## Serial output

Open the DK serial terminal at 115200 baud. Every report includes:

- the scan number and scan duration
- the number of AirTag-class devices nearby
- each device's current BLE address, RSSI, battery field when present, and observation history
- a tracking section for devices repeatedly seen across reports for at least 30 minutes

## Debugging detection

If you have AirTags nearby but the report says zero devices, temporarily enable:

```conf
CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS=y
```

in `prj.conf`, rebuild, and flash again. The app will print Apple manufacturer
payloads that it hears but does not classify as AirTag-class Find My advertisements.
This helps distinguish "the radio is not hearing the AirTags" from "the AirTags are
advertising a different Apple payload."
