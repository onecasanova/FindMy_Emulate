# Host Validation Scanner

This folder contains `bleak_scan.py`, a Python BLE scanner used as a second-hand validation tool for the nRF54L15 DK firmware. The firmware reports Apple AirTag-class BLE advertisements over the serial monitor; this host script scans from a Linux or Windows computer and prints nearby Apple manufacturer data so you can confirm those devices are actually present.

The script uses the `bleak` Python package and a Bluetooth LE adapter on the host computer. A virtual environment is recommended.

## Linux Setup

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install bleak
python bleak_scan.py
```

## Windows PowerShell Setup

```powershell
py -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install bleak
python bleak_scan.py
```

## How To Use

1. Flash and run the nRF54L15 DK firmware.
2. Open the DK serial monitor and wait for an AirGuard report.
3. Put the Linux or Windows host computer near the DK.
4. Run `python bleak_scan.py` from this `host/` directory.
5. Compare the Apple devices, RSSI values, payload lengths, and payload bytes printed by the host scanner with the devices printed by the DK serial monitor.

The host script is only a validation aid. It does not replace the firmware scanner and does not identify an AirTag owner. It simply confirms that Apple BLE manufacturer-data packets are visible from a nearby computer.

macOS is not recommended for this script because CoreBluetooth filters some Apple manufacturer data before Python applications can inspect it.
