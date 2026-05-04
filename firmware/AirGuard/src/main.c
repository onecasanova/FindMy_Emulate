#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define APPLE_COMPANY_ID 0x004c
#define FIND_MY_PREFIX_0 0x12
#define FIND_MY_STATUS_AIRTAG_MASK 0x18
#define FIND_MY_STATUS_AIRTAG_VALUE 0x10

#define MAX_TRACKED_DEVICES 32
#define SCAN_PERIOD_SECONDS (1 * 60)
#define SCAN_WINDOW_SECONDS 15
#define FOLLOWING_MIN_REPORTS 3
#define FOLLOWING_MIN_SECONDS (10 * 60)

struct airtag_record {
	bool in_use;
	bool seen_this_report;
	bt_addr_le_t addr;
	int8_t rssi;
	uint8_t battery_level;
	uint8_t status_byte;
	uint8_t reports_seen;
	uint32_t first_seen_s;
	uint32_t last_seen_s;
};

struct parse_context {
	bool matched_airtag;
	uint8_t status_byte;
	uint8_t battery_level;
#if IS_ENABLED(CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS)
	bool saw_apple_payload;
	bool saw_find_my_payload;
	uint8_t apple_payload[29];
	uint8_t apple_payload_len;
#endif
};

static struct airtag_record records[MAX_TRACKED_DEVICES];
static uint32_t report_number;

static uint32_t uptime_seconds(void)
{
	return (uint32_t)(k_uptime_get() / 1000);
}

static const char *battery_to_string(uint8_t battery_level)
{
	switch (battery_level) {
	case 0:
		return "full";
	case 1:
		return "medium";
	case 2:
		return "low";
	case 3:
		return "very low";
	default:
		return "unknown";
	}
}

static bool apple_find_my_data_cb(struct bt_data *data, void *user_data)
{
	struct parse_context *ctx = user_data;
	const uint8_t *mfg;
	uint16_t company_id;

	if (data->type != BT_DATA_MANUFACTURER_DATA || data->data_len < 5) {
		return true;
	}

	company_id = sys_get_le16(data->data);
	if (company_id != APPLE_COMPANY_ID) {
		return true;
	}

	mfg = &data->data[2];
#if IS_ENABLED(CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS)
	ctx->saw_apple_payload = true;
	ctx->apple_payload_len = MIN(data->data_len - 2, sizeof(ctx->apple_payload));
	memcpy(ctx->apple_payload, mfg, ctx->apple_payload_len);
#endif

	if (mfg[0] != FIND_MY_PREFIX_0) {
		return true;
	}

	ctx->status_byte = mfg[2];
#if IS_ENABLED(CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS)
	ctx->saw_find_my_payload = true;
#endif

	if ((ctx->status_byte & FIND_MY_STATUS_AIRTAG_MASK) == FIND_MY_STATUS_AIRTAG_VALUE) {
		ctx->matched_airtag = true;
		ctx->battery_level = (ctx->status_byte >> 6) & 0x03;
		return false;
	}

	return true;
}

static struct airtag_record *find_record(const bt_addr_le_t *addr)
{
	for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
		if (records[i].in_use && bt_addr_le_cmp(&records[i].addr, addr) == 0) {
			return &records[i];
		}
	}

	return NULL;
}

static struct airtag_record *allocate_record(void)
{
	uint32_t oldest_seen = UINT32_MAX;
	size_t oldest_index = 0;

	for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
		if (!records[i].in_use) {
			return &records[i];
		}

		if (records[i].last_seen_s < oldest_seen) {
			oldest_seen = records[i].last_seen_s;
			oldest_index = i;
		}
	}

	return &records[oldest_index];
}

static void remember_airtag(const bt_addr_le_t *addr, int8_t rssi,
			    uint8_t status_byte, uint8_t battery_level)
{
	struct airtag_record *record = find_record(addr);
	const uint32_t now_s = uptime_seconds();

	if (record == NULL) {
		record = allocate_record();
		memset(record, 0, sizeof(*record));
		record->in_use = true;
		bt_addr_le_copy(&record->addr, addr);
		record->first_seen_s = now_s;
	}

	record->rssi = rssi;
	record->status_byte = status_byte;
	record->battery_level = battery_level;
	record->last_seen_s = now_s;

	if (!record->seen_this_report) {
		record->seen_this_report = true;
		if (record->reports_seen < UINT8_MAX) {
			record->reports_seen++;
		}
	}
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	struct parse_context ctx = {
		.matched_airtag = false,
		.status_byte = 0,
		.battery_level = 0xff,
#if IS_ENABLED(CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS)
		.saw_apple_payload = false,
		.saw_find_my_payload = false,
		.apple_payload_len = 0,
#endif
	};

	if (type == BT_GAP_ADV_TYPE_SCAN_RSP) {
		return;
	}

	bt_data_parse(ad, apple_find_my_data_cb, &ctx);

	if (ctx.matched_airtag) {
		remember_airtag(addr, rssi, ctx.status_byte, ctx.battery_level);
#if IS_ENABLED(CONFIG_AIRGUARD_DEBUG_APPLE_PAYLOADS)
	} else if (ctx.saw_apple_payload) {
		char addr_string[BT_ADDR_LE_STR_LEN];

		bt_addr_le_to_str(addr, addr_string, sizeof(addr_string));
		printk("Debug Apple payload from %s RSSI %d%s: ",
		       addr_string, rssi, ctx.saw_find_my_payload ? " FindMy-like" : "");
		for (uint8_t i = 0; i < ctx.apple_payload_len; i++) {
			printk("%02x", ctx.apple_payload[i]);
		}
		printk("\n");
#endif
	}
}

static bool is_following_candidate(const struct airtag_record *record)
{
	uint32_t observed_for_s;

	if (!record->in_use || !record->seen_this_report) {
		return false;
	}

	if (record->reports_seen < FOLLOWING_MIN_REPORTS) {
		return false;
	}

	observed_for_s = record->last_seen_s - record->first_seen_s;
	return observed_for_s >= FOLLOWING_MIN_SECONDS;
}

static void reset_report_marks(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
		records[i].seen_this_report = false;
	}
}

static void print_report(void)
{
	size_t nearby_count = 0;
	size_t following_count = 0;
	char addr_string[BT_ADDR_LE_STR_LEN];

	for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
		if (records[i].in_use && records[i].seen_this_report) {
			nearby_count++;
			if (is_following_candidate(&records[i])) {
				following_count++;
			}
		}
	}

	printk("\n========== AirGuard nRF54L15 DK report %u ==========\n", report_number);
	printk("Scan window: %u seconds, scan period: %u seconds\n",
	       SCAN_WINDOW_SECONDS, SCAN_PERIOD_SECONDS);
	printk("Apple AirTag-class devices nearby: %u\n", (unsigned int)nearby_count);

	if (nearby_count == 0) {
		printk("No AirTag-class advertisements were detected in this scan.\n");
	} else {
		for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
			const struct airtag_record *record = &records[i];
			uint32_t observed_for_s;

			if (!record->in_use || !record->seen_this_report) {
				continue;
			}

			bt_addr_le_to_str(&record->addr, addr_string, sizeof(addr_string));
			observed_for_s = record->last_seen_s - record->first_seen_s;

			printk("- ID/address: %s | RSSI: %d dBm | battery: %s | reports: %u | observed: %u min | status: 0x%02x\n",
			       addr_string, record->rssi,
			       battery_to_string(record->battery_level),
			       record->reports_seen,
			       (unsigned int)(observed_for_s / 60),
			       record->status_byte);
		}
	}

	printk("\nAirTags that may be following you:\n");
	if (following_count == 0) {
		printk("None based on the repeated-presence heuristic.\n");
	} else {
		for (size_t i = 0; i < ARRAY_SIZE(records); i++) {
			const struct airtag_record *record = &records[i];

			if (!is_following_candidate(record)) {
				continue;
			}

			bt_addr_le_to_str(&record->addr, addr_string, sizeof(addr_string));
			printk("- %s has appeared in %u reports over %u minutes.\n",
			       addr_string, record->reports_seen,
			       (unsigned int)((record->last_seen_s - record->first_seen_s) / 60));
		}
	}

	printk("====================================================\n");
}

int main(void)
{
	int err;
	const struct bt_le_scan_param scan_param = {
		.type = BT_LE_SCAN_TYPE_PASSIVE,
		.options = BT_LE_SCAN_OPT_NONE,
		.interval = BT_GAP_SCAN_FAST_INTERVAL,
		.window = BT_GAP_SCAN_FAST_WINDOW,
	};

	printk("AirGuard nRF54L15 DK starting\n");
	printk("Initializing Bluetooth subsystem...\n");

	err = bt_enable(NULL);
	if (err != 0) {
		printk("Bluetooth init failed: %d\n", err);
		return 0;
	}

	printk("Bluetooth ready. Scanning every %u seconds.\n", SCAN_PERIOD_SECONDS);

	while (true) {
		report_number++;
		reset_report_marks();

		printk("\nStarting AirTag scan %u...\n", report_number);
		err = bt_le_scan_start(&scan_param, device_found);
		if (err != 0) {
			printk("Failed to start BLE scan: %d\n", err);
			k_sleep(K_SECONDS(SCAN_PERIOD_SECONDS));
			continue;
		}

		k_sleep(K_SECONDS(SCAN_WINDOW_SECONDS));

		err = bt_le_scan_stop();
		if (err != 0) {
			printk("Failed to stop BLE scan cleanly: %d\n", err);
		}

		print_report();
		k_sleep(K_SECONDS(SCAN_PERIOD_SECONDS - SCAN_WINDOW_SECONDS));
	}

	return 0;
}
