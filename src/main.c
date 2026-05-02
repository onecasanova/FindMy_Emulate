/* Find My Emulation on nRF54L15 DK — Phase 4: Key Rotation
 *
 * Broadcasts a Find My Offline Finding advertisement and rotates
 * the P-224 public key (and derived BLE address) on a fixed interval.
 *
 * Keys live in src/keys.h — regenerate all of them with:
 *   python3 generate_key.py --count 10
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
 * Rotation interval.
 * 30 s for demo/testing — change to (15UL * 60UL * 1000UL) for production.
 */
#define ROTATION_INTERVAL_MS  30000UL

static int key_idx = 0;
static int adv_id  = -1;   /* set by main() before rotation thread fires */

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
	0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	0x00
};

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
};

/* Fill the advertisement payload from keys[idx]. */
static void set_payload(int idx)
{
	memcpy(&mfg_data[5], &keys[idx][6], 22);
	mfg_data[27] = keys[idx][0] >> 6;
}

/* Build a BLE random static address from keys[idx]. */
static void fill_addr(int idx, bt_addr_le_t *addr)
{
	addr->type     = BT_ADDR_LE_RANDOM;
	addr->a.val[5] = keys[idx][0] | 0xC0;
	addr->a.val[4] = keys[idx][1];
	addr->a.val[3] = keys[idx][2];
	addr->a.val[2] = keys[idx][3];
	addr->a.val[1] = keys[idx][4];
	addr->a.val[0] = keys[idx][5];
}

/* Print current key index, BLE address, and full payload. */
static void print_status(int idx)
{
	printk("Key %d/%d  addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
	       idx, NUM_KEYS - 1,
	       keys[idx][0] | 0xC0, keys[idx][1], keys[idx][2],
	       keys[idx][3], keys[idx][4], keys[idx][5]);
	printk("Payload:");
	for (int i = 0; i < (int)sizeof(mfg_data); i++) {
		printk(" %02X", mfg_data[i]);
	}
	printk("\n");
}

/* Start advertising using the current adv_id and mfg_data. */
static int start_advertising(void)
{
	struct bt_le_adv_param params = BT_LE_ADV_PARAM_INIT(
		BT_LE_ADV_OPT_USE_IDENTITY,
		0x0640,   /* 1 s min interval */
		0x0C80,   /* 2 s max interval */
		NULL);
	params.id = adv_id;

	int err = bt_le_adv_start(&params, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("bt_le_adv_start failed (err %d)\n", err);
	}
	return err;
}

/*
 * Key rotation thread.
 * Sleeps for ROTATION_INTERVAL_MS, then:
 *   1. Stops advertising
 *   2. Resets the BLE identity to the new key's address
 *   3. Updates the payload
 *   4. Restarts advertising
 */
static void rotation_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
		k_sleep(K_MSEC(ROTATION_INTERVAL_MS));

		if (adv_id < 0) {
			continue; /* main() hasn't finished init yet */
		}

		int next = (key_idx + 1) % NUM_KEYS;
		printk("Rotating key %d -> %d\n", key_idx, next);

		bt_le_adv_stop();

		bt_addr_le_t new_addr;
		fill_addr(next, &new_addr);

		int err = bt_id_reset(adv_id, &new_addr, NULL);
		if (err < 0) {
			printk("bt_id_reset failed (err %d) — skipping rotation\n", err);
			/* restart advertising with old key so we don't go dark */
			start_advertising();
			continue;
		}

		key_idx = next;
		set_payload(key_idx);

		err = start_advertising();
		if (err == 0) {
			print_status(key_idx);
			printk("Next rotation in %lu ms\n", ROTATION_INTERVAL_MS);
		}
	}
}

K_THREAD_DEFINE(rotation_tid, 2048,
		rotation_thread_fn, NULL, NULL, NULL,
		K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

int main(void)
{
	printk("Find My Emulation — key rotation every %lu ms, %d keys\n",
	       ROTATION_INTERVAL_MS, NUM_KEYS);

	int err = bt_enable(NULL);
	if (err) {
		printk("bt_enable failed (err %d)\n", err);
		return 0;
	}
	printk("Bluetooth initialized\n");

	/* Create a BLE identity with address derived from key[0] */
	bt_addr_le_t addr;
	fill_addr(0, &addr);

	adv_id = bt_id_create(&addr, NULL);
	if (adv_id < 0) {
		printk("bt_id_create failed (err %d)\n", adv_id);
		return 0;
	}

	set_payload(0);

	err = start_advertising();
	if (err) {
		return 0;
	}

	print_status(0);
	printk("Next rotation in %lu ms\n", ROTATION_INTERVAL_MS);

	/* Rotation thread handles everything from here. */
	return 0;
}
