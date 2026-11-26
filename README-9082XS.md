# 9082XS port status

This branch starts from Igor Listopad's RTL8189ES `rtw88` work, PR #465 head `475584a581cb449a8616e0339ecb85bcb88c67aa`. The target device is SDIO `02e7:9082`, exposed by Lunar as `mmc2:0001:1`.

The first patch adds a separate `rtw_9082xs` SDIO module and maps its WLAN register window to `0x14000` instead of RTL8189ES's `0x10000`. Six read-only register comparisons on hardware showed that the old window returns `0xea`, while the new window returns live register values. Other SDIO address spaces are left unchanged. The module builds against Lunar `6.18.41-lunar`.

A second patch gives 9082XS its own chip description and MAC table callback. The callback reverses the bits in each byte of Igor's RTL8188E MAC table, except at register `0x458`, where the vendor value is already `0x41`. Hardware readback after the diagnostic module replayed the vendor table confirmed `0x4026=0x82`, `0x4027=0xac`, and `0x4421=0xf0`, matching the transformed entries. This covers 91 of 92 shared byte addresses; the vendor table also contains 40 byte addresses absent from Igor's table. The callback is scoped to MAC table replay; runtime register access still needs separate validation. The new modules built against Lunar `6.18.41-lunar`, bound to the device, hit the default guard, and unloaded cleanly.

The 9082XS chip description now also has its own power-on sequence. It encodes the raw SDIO and MAC control bits from the diagnostic sequence that completed stage 1 on hardware, including the final `0x4004` and `0x4100` enables. It also carries the vendor-derived low-power and close tables. These compile in `rtw_8188e` but have not run through the guarded `rtw88` probe. The power-off routine has additional register operations around those tables that are not yet implemented, and eFuse access still uses Igor's RTL8188E code.

The chip now requests the privately installed `tilk/9082xs-nic.bin` instead of Igor's RTL8188E firmware. The loader treats it as a raw image without a legacy 32-byte header, selects its 4 KiB pages through register `0x82`, and follows the checksum and MCU-ready sequence validated by the diagnostic module. The image itself is absent from this repository. This code path compiles but has not been executed by `rtw88` on the device; the default probe guard remains in place.

The 9082XS physical eFuse reader now uses byte-width SDIO register accesses and bit-reverses the address byte, matching the diagnostic path that decoded 17 records on hardware. The common RTL8188E logical-map decoder is retained. The variant checks logical ID `0x9082` before accepting calibration and MAC data. This `rtw88` reader has not yet run through full probe.

Initialization is guarded by `experimental_hw_init=0` by default because runtime register access, PHY and radio calibration are incomplete. A boot or Wi-Fi claim from this binding would be premature. The diagnostic module in `../experimental/` has independently completed chip power, firmware download, MAC table replay and eFuse decode on the target.

Next: validate runtime register bit order and power-off, add the missing MAC entries, adapt PHY/AGC/RF tables and board calibration, then test staged `rtw88` firmware readiness, interface registration and traffic on hardware. Keep the extracted firmware outside the repository until redistribution rights are established.
