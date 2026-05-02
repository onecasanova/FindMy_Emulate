# FindMy Emulate

## Introduction

### Problem statement

Apple's Find My network is deployed on billions of devices worldwide, yet the underlying
protocol is entirely proprietary and closed. Reverse-engineering efforts (Heinrich et al., 2021)
have documented the Offline Finding advertisement format, but practical emulation tools remain
scarce: OpenHaystack targets Linux and ESP32, and no open implementation exists on Nordic
Semiconductor hardware running Zephyr RTOS.

The nRF54L15 is a strong candidate for this protocol — its Bluetooth 5.4 radio, Cortex-M33
core, and low power profile match the requirements of a real tracker — yet no reference
implementation existed before this project.

This project fills that gap: a fully functional Apple Offline Finding emulator on the nRF54L15 DK
running Zephyr RTOS, with no Apple hardware, no MFi certification, and no iCloud account on
the device itself.

### Target application

- Demonstrating the Apple Offline Finding protocol on Nordic/Zephyr hardware for the first time.
- Studying BLE advertisement timing and payload correctness with real iPhones as passive relays.
- Providing a reproducible baseline for further research (key rotation, energy profiling, range testing, etc.).

### High-level architecture

```
┌─────────────────────────┐        BLE ADV_NONCONN_IND        ┌──────────────┐
│   nRF54L15 DK           │ ─────────────────────────────────▶ │  iPhone      │
│   (Zephyr firmware)     │   Apple OF payload + P-224 key     │  (relay)     │
│                         │                                     └──────┬───────┘
│  • static P-224 key     │                                            │ HTTPS (encrypted)
│  • BLE addr from key    │                                            ▼
│  • 1–2 s adv interval   │                                     ┌──────────────┐
└─────────────────────────┘                                      │ Apple servers│
                                                                 └──────┬───────┘
                                                                        │
                                                               ┌────────▼────────┐
                                                               │  OpenHaystack   │
                                                               │  (macOS app)    │
                                                               │  decrypts +     │
                                                               │  shows location │
                                                               └─────────────────┘
```

### Key features

| Feature | Detail |
|---------|--------|
| Protocol | Apple Offline Finding (type `0x12`, company ID `0x004C`) |
| Crypto | P-224 (secp224r1) — key pair generated with `scripts/generate_key.py` |
| BLE role | Non-connectable, non-scannable broadcaster |
| Advertising interval | 1–2 s (configurable) |
| Key management | Static key baked into `firmware/src/keys.h` |
| Local validation | `host/bleak_scan.py` — passive BLE scan confirms payload spec compliance |
| End-to-end validation | OpenHaystack (macOS) — fetches and decrypts Apple location reports |

### Performance summary

| Metric | Value |
|--------|-------|
| BLE advertising interval | 1–2 s (1000–2000 ms) |
| Time to first advertisement after boot | < 1 s |
| BLE range (indoors) | ~10 m |
| BLE range (line-of-sight) | ~50 m |
| TX power | +3 dBm |
| Active current (advertising) | ~5–8 mA @ 3.3 V |
| Estimated battery runtime (200 mAh) | > 24 h |
| Location report latency | Minutes to hours (depends on iPhone traffic near the board) |
| Payload spec compliance | Verified — 27-byte OF payload, correct type/length/status bytes |

---

## Hardware details

### Board and MCU

| Item | Detail |
|------|--------|
| Board | [Nordic nRF54L15 DK (PCA10156)](https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK) |
| MCU | nRF54L15 — ARM Cortex-M33 @ 128 MHz |
| Flash | 1.5 MB NVM |
| RAM | 256 KB |
| BLE | Bluetooth 5.4 |
| USB | USB-C (power + onboard J-Link) |

### Additional peripherals

None required. The DK is used bare with its onboard BLE radio and J-Link debugger.

### Hardware modifications

None. No rework, jumpers, or external components needed.

### Power subsystem

The DK is powered over USB-C from a PC or USB charger. There is no battery; the board
draws approximately 5–8 mA at 3.3 V during BLE advertising (varies with interval).

For battery-powered deployment, connect a 3.3 V supply to the VDD pins on the DK header.
A 200 mAh LiPo would give an estimated runtime of > 24 h at the default advertising interval.

### RF specifications

| Parameter | Value |
|-----------|-------|
| Band | 2.4 GHz ISM |
| BLE version | 5.4 |
| PHY | 1M PHY (coded PHY not used) |
| TX power | +3 dBm (nRF54L15 default) |
| Expected range | ~10 m indoors, ~50 m line-of-sight |
| Advertising type | `ADV_NONCONN_IND` (non-connectable, non-scannable) |
| Advertising interval | 1000–2000 ms (0x0640–0x0C80 × 0.625 ms) |

Compliance: This is a research/educational prototype. It is not CE/FCC certified for
commercial use.

---

## Software environment

### i. Firmware

#### 1. IDE and toolchain

| Tool | Version |
|------|---------|
| IDE | VS Code |
| Extension | nRF Connect for VS Code (extension pack) |
| Toolchain | nRF Connect Toolchain Manager (installs compiler automatically) |

#### 2. SDK, RTOS, and compiler

| Component | Version |
|-----------|---------|
| nRF Connect SDK | 2.6.x (or later) |
| Zephyr RTOS | bundled with nRF Connect SDK |
| Compiler | arm-zephyr-eabi-gcc (bundled with toolchain) |
| CMake | ≥ 3.20.0 (bundled) |
| west | bundled with nRF Connect SDK |

#### 3. Build system and board configuration

Build system: CMake via `west`.

**Board target:** `nrf54l15dk/nrf54l15/cpuapp`

**Kconfig** (`firmware/prj.conf`):

```
CONFIG_BT=y
CONFIG_BT_BROADCASTER=y
CONFIG_BT_ID_MAX=2
```

Build command:

```bash
west build -b nrf54l15dk/nrf54l15/cpuapp firmware/
```

### ii. Other software

#### 1. Language, version, dependencies, and lock files

| Item | Detail |
|------|--------|
| Language | Python 3.8+ |
| Scripts | `host/bleak_scan.py`, `scripts/generate_key.py` |
| Dependencies | `bleak`, `cryptography`, `requests`, `srp`, `truststore` |
| Lock file | `requirements.txt` (repo root) |

Install:

```bash
pip install -r requirements.txt
```

#### 2. OS compatibility and special drivers

| OS | Status | Notes |
|----|--------|-------|
| Windows 10/11 | Supported | Requires `winrt` backend — install bleak via `conda install -c conda-forge bleak` |
| Linux | Supported | BlueZ required (`sudo apt install bluez`) |
| macOS | Not supported | CoreBluetooth filters Apple manufacturer data from third-party apps |

No additional drivers needed on Windows — the onboard J-Link uses the standard nRF Connect driver installed with the VS Code extension.

### Programming and debugging tools

| Tool | Purpose |
|------|---------|
| nRF Connect for VS Code | Build, flash, debug |
| J-Link (onboard) | SWD programming and RTT/serial output |
| nRF Connect for Mobile | BLE advertisement verification on phone |
| AirGuard (Android) | Classifies board as AirTag — confirms OF format is correct |
| OpenHaystack (macOS) | End-to-end Find My location verification |
| Serial terminal | 115200 8N1 — startup and status logs |

### Radio stack configuration

| Parameter | Value |
|-----------|-------|
| Stack | Zephyr BLE (`CONFIG_BT=y`) |
| Role | Broadcaster only (`CONFIG_BT_BROADCASTER=y`) |
| Identities | 2 (`CONFIG_BT_ID_MAX=2`) |
| PHY | 1M (default) |
| Channels | 37, 38, 39 (standard BLE advertising channels) |
| PDU type | `ADV_NONCONN_IND` |

---

## Reproducibility guide

### 1. Hardware assembly

No assembly required. Connect the nRF54L15 DK to your PC via USB-C. The onboard J-Link
handles both power and programming.

```
PC  ──USB-C──▶  nRF54L15 DK
```

### 2. Environment setup

#### Install nRF Connect SDK

Follow the official guide for your OS:
[nRF Connect SDK — Getting Started](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html)

Quick path (VS Code):
1. Install [VS Code](https://code.visualstudio.com/)
2. Install the **nRF Connect for VS Code** extension pack
3. Open the extension → **Toolchain Manager** → install the latest nRF Connect SDK
4. `ZEPHYR_BASE` is set automatically by the extension

#### Clone and open this repo

```bash
git clone <repo-url>
cd FindMy_Emulate
code .
```

#### Install Python dependencies

```bash
pip install -r requirements.txt
```

### 3. Generate a key pair

```bash
python scripts/generate_key.py
```

Output example:

```
/* Paste into firmware/src/keys.h */
/* BLE addr: F1:22:7A:41:21:A5 */
static const uint8_t public_key[28] = {
    0xB1, 0x22, 0x7A, 0x41, 0x21, 0xA5, 0x48,
    ...
};

/* Private key (keep safe — needed to decrypt location reports) */
/* B1 22 7A 41 ... */
```

**Save the private key hex somewhere safe** — it is only printed once and is needed by
OpenHaystack (or a collaborator) to decrypt location reports.

Alternatively, use an existing key from a collaborator's OpenHaystack accessory (see §4d).

### 4. Flash and provision

#### 4a. Update the public key

Edit `firmware/src/keys.h` and replace the `public_key` array:

```c
static const uint8_t public_key[28] = {
    0xXX, 0xXX, ...  /* paste output of generate_key.py here */
};
```

#### 4b. Update the device name (optional)

Edit `firmware/src/main.c`:

```c
#define DEVICE_NAME "YusooTag"   /* change to whatever you like */
```

#### 4c. Build

In the nRF Connect VS Code extension:
- **Application** → select `firmware/`
- **Board** → `nrf54l15dk/nrf54l15/cpuapp`
- Click **Build**

Or from the terminal:

```bash
west build -b nrf54l15dk/nrf54l15/cpuapp firmware/
```

#### 4d. Flash

```bash
west flash
```

#### 4e. Using a collaborator's OpenHaystack key

If a collaborator already has an OpenHaystack accessory on macOS:

1. In OpenHaystack: right-click the accessory → **Copy Advertisement Key (Base64)**
2. Convert to a hex byte array and paste into `firmware/src/keys.h`
3. Rebuild and reflash — the board now advertises under their key, so their OpenHaystack
   app will show the board's location

To convert base64 → C array:

```bash
python -c "
import base64
b = base64.b64decode('PASTE_BASE64_HERE')
print(', '.join(f'0x{x:02X}' for x in b))
"
```

### 5. Running the demo

#### Expected serial output (115200 8N1)

```
Find My Emulation — static key, BLE addr: F1:22:7A:41:21:A5
Advertising
```

If you see this the board is advertising correctly.

#### Verify with bleak_scan.py (PC)

```bash
python host/bleak_scan.py
```

Expected output:

```
Scanning for 30s — Apple manufacturer data (0x004C) only

FOUND  F1:22:7A:41:21:A5       RSSI  -XX  27 bytes  [Find My (Offline Finding)] <-- OUR BOARD
       data: 12 19 00 <22 key bytes> XX 00
       OK: payload passes spec check
```

Update `TARGET_ADDR` in `host/bleak_scan.py` if you regenerated the key.

#### Verify with AirGuard (Android)

1. Install [AirGuard](https://play.google.com/store/apps/details?id=de.seemoo.at_tracking_detection)
2. Start a scan — AirGuard classifies the board as an **AirTag** based on the `0x004C` / `0x12`
   manufacturer data pattern
3. The name `YusooTag` appears alongside the classification

#### Verify with nRF Connect for Mobile

1. Scan for devices
2. Find `YusooTag` or address `F1:22:7A:41:21:A5`
3. Confirm manufacturer data: company `4C00`, type byte `12`

#### Verify location reports (OpenHaystack, macOS)

1. Install [OpenHaystack](https://github.com/seemoo-lab/openhaystack)
2. Add accessory → **Deploy on custom hardware**
3. Paste the base64 public key (convert from hex with the command below)
4. Leave the board advertising near iPhones; check OpenHaystack after ~15 minutes

Convert hex public key to base64:

```bash
python -c "import base64; print(base64.b64encode(bytes.fromhex('YOUR_HEX_KEY')).decode())"
```

Current key (base64): `sSJ6QSGlSLvRyagxMmOcTClzPc8bmswaJamdWg==`

### 6. Testing and measurement

`bleak_scan.py` validates every received payload against the Offline Finding spec:

| Check | Expected |
|-------|---------|
| Payload length | 27 bytes |
| OF length byte (`[1]`) | `0x19` |
| Status byte (`[2]`) | `0x00` |

Run the board within 5 m of the PC for reliable RSSI readings.

### 7. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| `No module named 'winrt'` | bleak Windows backend missing | `conda install -c conda-forge bleak` |
| `No module named '_cffi_backend'` | cffi broken | `pip install --force-reinstall cffi` |
| Board not found in scan | Board not advertising | Check serial output first |
| `bt_le_adv_start failed` | BLE identity conflict | Power-cycle the board |
| bleak finds 0 devices | BLE adapter issue | Check Device Manager → Bluetooth |
| AirGuard shows no device | Board not in range or not advertising | Move board closer, check serial |
| OpenHaystack shows no location | No iPhone has passed by | Wait 15–30 min in a populated area |

### 8. Offline mode

If internet access is unavailable:

- Use `host/bleak_scan.py` to confirm the advertisement payload locally — no internet needed.
- Use **AirGuard** or **nRF Connect for Mobile** to visually confirm the advertisement.
- Location report fetching (OpenHaystack) requires internet and cannot function offline.

### 9. Security keys

**Never commit your private key to git.**

The private key is printed by `scripts/generate_key.py` and must be saved manually.
It is not stored anywhere in this repository. Keep it in a password manager or secure note.

The public key in `firmware/src/keys.h` is safe to commit — it is mathematically derived
from the private key but cannot be reversed.

---

## Repository layout

```
firmware/               Zephyr application
  src/
    main.c              BLE broadcaster, Offline Finding payload builder
    keys.h              Public key only — do not put private key here
  boards/               Board-specific overlays
  CMakeLists.txt
  prj.conf
scripts/
  generate_key.py       P-224 key pair generation — prints public C array + private hex
host/
  bleak_scan.py         Passive BLE scanner — validates OF advertisement locally
docs/
  intro.md              Protocol background and advertisement format reference
requirements.txt        Python dependencies
```

---

## Example reference

This README follows the structure and level of detail demonstrated by the
[uwb-sniffer](https://github.com/seemoo-lab/uwb-sniffer) project from the SEEMOO lab at TU
Darmstadt. Key differences tailored to this project:

- **Single-board setup** — no multi-device wiring diagrams needed; hardware section is
  minimal by design.
- **No custom PCB or enclosure** — the DK is used bare; hardware modifications section
  explicitly states none.
- **Protocol-first focus** — the advertisement format table and decryption math in
  `docs/intro.md` replace the physical-layer captures that uwb-sniffer documents.
- **Offline verification path** — `bleak_scan.py` and AirGuard provide local validation
  without requiring a macOS machine, unlike uwb-sniffer which requires specialised
  radio hardware for capture.
- **Security keys section** — added because this project handles cryptographic key material
  that must not be committed to the repository; uwb-sniffer does not have this concern.

---

## References

- Heinrich et al., "Who Can Find My Devices? Security and Privacy of Apple's Crowd-Sourced Bluetooth Location Tracking System" — PETS 2021
- [OpenHaystack](https://github.com/seemoo-lab/openhaystack) — open-source Find My accessory framework
- [Nordic nRF54L15 DK product page](https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK)
- [Zephyr BLE Broadcaster sample](https://docs.zephyrproject.org/latest/samples/bluetooth/broadcaster/README.html)
- [Apple Find My network accessory specification](https://developer.apple.com/find-my/)
- [uwb-sniffer](https://github.com/seemoo-lab/uwb-sniffer) — README structure reference
