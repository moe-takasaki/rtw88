# 9082XS port status

This branch starts from Igor Listopad's RTL8189ES `rtw88` work, PR #465 head `475584a581cb449a8616e0339ecb85bcb88c67aa`. The target device is SDIO `02e7:9082`, exposed by Lunar as `mmc2:0001:1`.

The first patch adds a separate `rtw_9082xs` SDIO module and maps its WLAN register window to `0x14000` instead of RTL8189ES's `0x10000`. Six read-only register comparisons on hardware showed that the old window returns `0xea`, while the new window returns live register values. Other SDIO address spaces are left unchanged. The module builds against Lunar `6.18.41-lunar`.

A second patch gives 9082XS its own chip description and MAC table callback. The callback reverses the bits in each byte of Igor's RTL8188E MAC table, except at register `0x458`, where the vendor value is already `0x41`. Hardware readback after the diagnostic module replayed the vendor table confirmed `0x4026=0x82`, `0x4027=0xac`, and `0x4421=0xf0`, matching the transformed entries. This covers 91 of 92 shared byte addresses; the vendor table also contains 40 byte addresses absent from Igor's table. The callback is scoped to MAC table replay; runtime register access still needs separate validation. The new modules built against Lunar `6.18.41-lunar`, bound to the device, hit the default guard, and unloaded cleanly.

Initialization is guarded by `experimental_hw_init=0` by default because this branch still inherits RTL8189ES's firmware and PHY tables. The tested 9082XS NIC image differs from Igor's firmware, and the MAC table values differ by a per-byte bit reversal. A boot or Wi-Fi claim from this binding would be premature. The diagnostic module in `../experimental/` has independently completed chip power, firmware download, MAC table replay and eFuse decode on the target.

Next: add the missing MAC entries, provide a private firmware path and format adapter, adapt PHY/AGC/RF tables and eFuse calibration, then test firmware readiness, interface registration and traffic on hardware. Keep the extracted firmware outside the repository until redistribution rights are established.
