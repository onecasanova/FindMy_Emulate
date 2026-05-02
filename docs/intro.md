# Introduction

## Apple Offline Finding overview

Apple's Find My network lets devices that are offline (no Wi-Fi, no cellular) be located by
passively leveraging nearby iPhones as relays. The lost device broadcasts a Bluetooth Low Energy
advertisement that any passing iPhone silently picks up and uploads — encrypted — to Apple's
servers. The owner can then decrypt the location report using their private key.

## Advertisement format

The Offline Finding advertisement uses Apple's manufacturer-specific AD type (`0xFF`, company ID
`0x004C`) with an Offline Finding payload:

| Offset | Length | Field |
|--------|--------|-------|
| 0–1 | 2 | Apple company ID (`0x4C 0x00`) |
| 2 | 1 | OF type (`0x12`) |
| 3 | 1 | Payload length (`0x19`) |
| 4 | 1 | Status byte |
| 5–26 | 22 | Public key bytes [6:28] |
| 27 | 1 | Top 2 bits of key byte [0] |
| 28 | 1 | Hint |

The BLE random static address is also derived from the first 6 bytes of the public key, so both
the address and payload change together on each rotation.

## Key rotation

To prevent long-term tracking, real AirTags rotate their public key every 15 minutes. This
project implements the same mechanism: a pre-generated set of P-224 key pairs lives in
`firmware/src/keys.h`. A background thread stops advertising, resets the BLE identity to the
next key's derived address, updates the payload, and resumes advertising.

See `scripts/generate_key.py` to regenerate the key set.

## References

- Heinrich et al., "Who Can Find My Devices?" (PETS 2021)
- OpenHaystack — open-source Find My accessory framework
- Zephyr BLE API documentation
