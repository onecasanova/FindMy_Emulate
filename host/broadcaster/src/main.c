#include <stdint.h>
#include <string.h>

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

/* Compressed P-224 public key with the 0x02/0x03 header byte stripped (28 B).
 * Generate offline with the Python snippet in CLAUDE.md and replace the bytes
 * below. The values here are placeholders so the project builds out of the box. */
static const uint8_t public_key[28] = {
	0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00,
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00,
	0x11, 0x22,
};

/* Bytes 2..30 of the AD record (Zephyr prepends the AD length and AD type).
 * Layout matches the table in CLAUDE.md: company ID, OF type, OF length,
 * status, key[6..27], key[0]>>6, hint. */
static uint8_t mfg_data[29] = {
	0x4c, 0x00,  /* Apple company ID, little-endian */
	0x12,        /* Offline Finding type */
	0x19,        /* Offline Finding data length (25) */
	0x10,        /* Status byte: AirTag-class so the AirGuard scanner accepts it */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* key[6..27] filled at runtime */
	0x00,        /* key[0] >> 6 */
	0x00,        /* hint */
};

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
};

static void fill_payload_from_key(void)
{
	memcpy(&mfg_data[5], &public_key[6], 22);
	mfg_data[27] = public_key[0] >> 6;
}

static int set_random_address_from_key(void)
{
	bt_addr_le_t addr = { .type = BT_ADDR_LE_RANDOM };

	/* OpenHaystack: addr = key[0..5], with the top two bits of key[0] forced
	 * to 1 to mark the address as a BLE static random address. The HCI byte
	 * order is little-endian, so key[0] (which holds the address-type bits)
	 * goes into the most-significant byte of the address. */
	addr.a.val[5] = public_key[0] | 0xC0;
	addr.a.val[4] = public_key[1];
	addr.a.val[3] = public_key[2];
	addr.a.val[2] = public_key[3];
	addr.a.val[1] = public_key[4];
	addr.a.val[0] = public_key[5];

	return bt_id_create(&addr, NULL);
}

int main(void)
{
	int err;

	printk("Find My broadcaster starting on nRF54L15 DK\n");

	err = set_random_address_from_key();
	if (err < 0) {
		printk("bt_id_create failed: %d\n", err);
		return 0;
	}

	fill_payload_from_key();

	err = bt_enable(NULL);
	if (err != 0) {
		printk("Bluetooth init failed: %d\n", err);
		return 0;
	}

	struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
		BT_LE_ADV_OPT_USE_IDENTITY,
		0x0C80, 0x0C80,
		NULL);

	err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err != 0) {
		printk("Advertising failed to start: %d\n", err);
		return 0;
	}

	printk("Advertising Find My payload (status 0x%02x).\n", mfg_data[4]);
	printk("Manufacturer data: ");
	for (size_t i = 0; i < sizeof(mfg_data); i++) {
		printk("%02x", mfg_data[i]);
	}
	printk("\n");

	while (1) {
		k_sleep(K_SECONDS(60));
	}

	return 0;
}
