#!/usr/bin/env python3
"""Find My emulation validator — bleak-based BLE scanner.

Run on Linux or Windows (not macOS — CoreBluetooth filters Apple manufacturer data).

Usage:
    pip install bleak
    python3 bleak_scan.py

Looks for Apple manufacturer data (0x004C) and flags Find My Offline
Finding advertisements (type byte 0x12) specifically.

Expected output for our board:
    FOUND  F1:22:7A:41:21:A5  RSSI -XX  27 bytes  12 19 00 <22 key bytes> 02 00
"""

import asyncio
from bleak import BleakScanner

# BLE address our board should be using (from serial output).
# Update this if you regenerate the key.
TARGET_ADDR = "F1:22:7A:41:21:A5".lower()

APPLE_ID = 0x004C
OF_TYPE  = 0x12   # Offline Finding type byte (first byte after company ID)


def classify(data: bytes) -> str:
    """Return a short label for Apple manufacturer data payloads."""
    if len(data) < 2:
        return "unknown"
    t = data[0]
    if t == 0x12:
        return "Find My (Offline Finding)"
    if t == 0x02 and len(data) >= 2 and data[1] == 0x15:
        return "iBeacon"
    if t == 0x10:
        return "Nearby"
    if t == 0x07:
        return "AirPods"
    return f"Apple type=0x{t:02X}"


def validate_findmy(data: bytes) -> "list[str]":
    """Check a Find My payload for spec compliance. Returns list of issues."""
    issues = []
    if len(data) != 27:
        issues.append(f"length {len(data)}, expected 27")
    if len(data) >= 2 and data[1] != 0x19:
        issues.append(f"OF length byte=0x{data[1]:02X}, expected 0x19")
    if len(data) >= 3 and data[2] != 0x00:
        issues.append(f"status byte=0x{data[2]:02X}, expected 0x00")
    return issues


async def scan(duration: float = 30.0):
    print(f"Scanning for {duration}s — Apple manufacturer data (0x004C) only\n")
    seen = {}

    def callback(device, adv):
        if APPLE_ID not in adv.manufacturer_data:
            return
        raw = adv.manufacturer_data[APPLE_ID]
        label = classify(raw)
        addr = device.address.lower()
        key = addr

        if key in seen:
            return  # deduplicate
        seen[key] = True

        target = " <-- OUR BOARD" if addr == TARGET_ADDR else ""
        hex_str = raw.hex(" ")
        print(f"{'FOUND' if addr == TARGET_ADDR else 'other':5s}  "
              f"{device.address:<20s}  RSSI {adv.rssi:4d}  "
              f"{len(raw):2d} bytes  [{label}]{target}")
        print(f"       data: {hex_str}")

        if raw[0] == OF_TYPE:
            issues = validate_findmy(raw)
            if issues:
                print(f"       WARN: {'; '.join(issues)}")
            else:
                print(f"       OK: payload passes spec check")
        print()

    scanner = BleakScanner(detection_callback=callback, scanning_mode="passive")
    await scanner.start()
    await asyncio.sleep(duration)
    await scanner.stop()

    if not seen:
        print("No Apple manufacturer data found. Is Bluetooth on?")
    else:
        print(f"Done. {len(seen)} unique Apple device(s) found.")
        if TARGET_ADDR not in seen:
            print(f"NOTE: target board {TARGET_ADDR.upper()} not seen — "
                  "try increasing scan duration or moving board closer.")


if __name__ == "__main__":
    asyncio.run(scan(duration=15.0))
