#!/usr/bin/env python3
"""Generate P-224 key pairs for Find My emulation.

Single key (default) — print C array to paste into main.c:
    python3 generate_key.py

Multiple keys — write src/keys.h for Phase 4 key rotation:
    python3 generate_key.py --count 10

Requires: pip install cryptography
"""

import argparse
import os

from cryptography.hazmat.primitives.asymmetric.ec import (
    generate_private_key, SECP224R1,
)
from cryptography.hazmat.primitives.serialization import (
    Encoding, PublicFormat,
)


def gen_key():
    """Return (pub_bytes[28], priv_bytes[28])."""
    key = generate_private_key(SECP224R1())
    pub = key.public_key().public_bytes(Encoding.X962, PublicFormat.CompressedPoint)
    priv = key.private_numbers().private_value.to_bytes(28, "big")
    return pub[1:], priv  # strip header byte → 28 bytes


def fmt_key(pub, idx=None, total=None, comment=None):
    """Format a 28-byte key as a C array initialiser."""
    rows = [pub[i:i+7] for i in range(0, 28, 7)]
    lines = []
    for r in rows:
        lines.append("        " + ", ".join(f"0x{b:02X}" for b in r))
    body = ",\n".join(lines)
    label = f"/* key {idx} */" if idx is not None else ""
    if comment:
        label += f"  /* {comment} */"
    comma = "," if (idx is not None and idx < total - 1) else ""
    return f"    {label}\n    {{\n{body}\n    }}{comma}"


def single_key():
    pub, priv = gen_key()
    print("/* 28-byte compressed P-224 public key (header byte stripped) */")
    print("static uint8_t public_key[28] = {")
    for i in range(0, 28, 7):
        chunk = pub[i:i+7]
        hex_vals = ", ".join(f"0x{b:02X}" for b in chunk)
        comma = "," if i + 7 < 28 else ""
        print(f"\t{hex_vals}{comma}")
    print("};")
    print()
    print("/* Private key (keep safe — needed to decrypt location reports) */")
    print("/* " + " ".join(f"{b:02X}" for b in priv) + " */")


def multi_key(count, out_path):
    keys = []
    privs = []
    for _ in range(count):
        pub, priv = gen_key()
        keys.append(pub)
        privs.append(priv)

    lines = []
    lines.append("/* keys.h — Pre-generated P-224 public keys for Find My key rotation.")
    lines.append(" *")
    lines.append(f" * Generated with: python3 generate_key.py --count {count}")
    lines.append(" *")
    lines.append(" * Each entry is a 28-byte compressed P-224 public key (header byte stripped).")
    lines.append(" * Key[0] is used on boot; the rotation thread cycles through the rest.")
    lines.append(" */")
    lines.append("")
    lines.append("#ifndef KEYS_H")
    lines.append("#define KEYS_H")
    lines.append("")
    lines.append("#include <stdint.h>")
    lines.append("")
    lines.append(f"#define NUM_KEYS {count}")
    lines.append("")
    lines.append(f"static const uint8_t keys[NUM_KEYS][28] = {{")
    for i, pub in enumerate(keys):
        addr = f"{pub[0] | 0xC0:02X}:{pub[1]:02X}:{pub[2]:02X}:{pub[3]:02X}:{pub[4]:02X}:{pub[5]:02X}"
        lines.append(f"    /* key {i}  BLE addr: {addr} */")
        rows = [pub[j:j+7] for j in range(0, 28, 7)]
        row_strs = [", ".join(f"0x{b:02X}" for b in r) for r in rows]
        body = ",\n        ".join(row_strs)
        comma = "," if i < count - 1 else ""
        lines.append(f"    {{ {body} }}{comma}")
    lines.append("};")
    lines.append("")
    lines.append("#endif /* KEYS_H */")

    with open(out_path, "w") as f:
        f.write("\n".join(lines) + "\n")

    print(f"Wrote {count} keys to {out_path}")
    print()
    print("Private keys (save these — needed to decrypt location reports):")
    for i, priv in enumerate(privs):
        print(f"  key {i:2d}: " + " ".join(f"{b:02X}" for b in priv))


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--count", type=int, default=None,
                        help="Generate N keys and write src/keys.h (for Phase 4 rotation)")
    args = parser.parse_args()

    if args.count is None:
        single_key()
    else:
        if args.count < 1:
            parser.error("--count must be >= 1")
        script_dir = os.path.dirname(os.path.abspath(__file__))
        out_path = os.path.join(script_dir, "..", "firmware", "src", "keys.h")
        multi_key(args.count, out_path)


if __name__ == "__main__":
    main()
