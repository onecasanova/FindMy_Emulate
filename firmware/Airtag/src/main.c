/* Find My Emulation on nRF54L15 DK — Static Key
 *
 * Broadcasts a Find My Offline Finding advertisement using a fixed
 * P-224 public key. No key rotation.
 *
 * To use a different key, regenerate with:
 *   python3 scripts/generate_key.py
 * and paste the output into firmware/src/keys.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include "keys.h"

/*
 * Manufacturer-specific advertising data (29 bytes).
 * Zephyr prepends the AD length (0x1e) and type (0xff).
 *
 *   [0-1]  Apple company ID  0x4C 0x00
 *   [2]    OF type           0x12
 *   [3]    OF data length    0x19 (25)
 *   [4]    Status byte       0x00
 *   [5-26] key[6:28]         last 22 bytes of public key
 *   [27]   key[0] >> 6       top 2 bits of first key byte
 *   [28]   Hint              0x00
 */
static uint8_t mfg_data[29] = {
	0x4c, 0x00,
	0x12, 0x19,
	0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	0x00
};

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
};

#define DEVICE_NAME     "YusooTag"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

int main(void)
{
	printk("Find My Emulation — static key, BLE addr: "
	       "%02X:%02X:%02X:%02X:%02X:%02X\n",
	       public_key[0] | 0xC0, public_key[1], public_key[2],
	       public_key[3], public_key[4], public_key[5]);

	int err = bt_enable(NULL);
	if (err) {
		printk("bt_enable failed (err %d)\n", err);
		return 0;
	}

	/* Derive BLE random static address from public key bytes [0:5] */
	bt_addr_le_t addr = {
		.type = BT_ADDR_LE_RANDOM,
		.a.val = {
			public_key[5],
			public_key[4],
			public_key[3],
			public_key[2],
			public_key[1],
			public_key[0] | 0xC0,
		},
	};

	int id = bt_id_create(&addr, NULL);
	if (id < 0) {
		printk("bt_id_create failed (err %d)\n", id);
		return 0;
	}

	/* Fill Offline Finding payload */
	memcpy(&mfg_data[5], &public_key[6], 22);
	mfg_data[27] = public_key[0] >> 6;

	struct bt_le_adv_param params = BT_LE_ADV_PARAM_INIT(
		BT_LE_ADV_OPT_USE_IDENTITY,
		0x0640,   /* 1 s min interval */
		0x0C80,   /* 2 s max interval */
		NULL);
	params.id = id;

	err = bt_le_adv_start(&params, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		printk("bt_le_adv_start failed (err %d)\n", err);
		return 0;
	}

	printk("Advertising\n");
	return 0;
}
