# FindMy Emulate

Emulates an Apple Find My (Offline Finding) tracker on the **nRF54L15 DK** using Zephyr RTOS.

The device broadcasts a valid Offline Finding advertisement and rotates its P-224 public key (and derived BLE address) on a configurable interval, matching the behaviour of a real AirTag.

## Repository layout

```
firmware/       Zephyr application (CMakeLists.txt, prj.conf, src/, boards/)
scripts/        Host-side utilities (key generation)
docs/           Background reading and design notes
```

## Quick start

### 1. Generate keys

```bash
pip install cryptography
python scripts/generate_key.py --count 10
# writes firmware/src/keys.h
```

### 2. Build and flash

```bash
west build -b nrf54l15dk/nrf54l15/cpuapp firmware/
west flash
```

### 3. Observe

Open a serial terminal (115200 8N1) to watch key rotations.
Use **nRF Connect for Mobile** or **AirGuard** to see the advertisement.

## Key rotation interval

Edit `ROTATION_INTERVAL_MS` in [firmware/src/main.c](firmware/src/main.c):

| Value | Use case |
|-------|----------|
| `30000` | Development / testing (30 s) |
| `900000` | Production (15 min, matching AirTag) |
