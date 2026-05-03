# AirGuard nRF54L15 DK Application

This application runs on an nRF54L15 DK and periodically scans nearby Bluetooth Low
Energy advertisements for Apple AirTag-class Find My devices. Every 3 minutes it starts
a short passive BLE scan, collects matching advertisements, and prints a serial report.
The report includes how many AirTag-class devices were seen nearby and an identifier for
each one. On this board the identifier is the BLE advertiser address because the public
Find My key cannot be resolved to a stable owner identity without Apple's private network
data.

The scanner identifies AirTag-class devices by parsing Bluetooth manufacturer-specific
advertising data. Apple uses company identifier `0x004c`. AirGuard's Android
implementation looks for the Find My payload marker `0x12` and then checks the following
status byte with the AirTag mask/value pair `0x18`/`0x10`. This embedded version applies
the same AirTag-class status-byte check. It also prints the RSSI and the battery field
encoded in the status byte when present.

The tracking section uses a time-based heuristic suitable for a development board with
no phone-grade location history. A device is marked as "possibly following" when the same
BLE address appears in at least 3 scan reports and those observations span at least 10
minutes. The Android app can make a stronger decision by comparing detections at multiple
locations; this board-only version cannot prove movement unless a location or
motion source is added. Because AirTags rotate BLE addresses, this heuristic can miss a
tracker after its address changes and should be treated as a nearby repeated-presence
warning, not a definitive attribution.
