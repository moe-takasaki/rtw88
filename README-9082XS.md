# 9082XS port status

This branch starts from Igor Listopad's RTL8189ES `rtw88` work, PR #465 head `475584a581cb449a8616e0339ecb85bcb88c67aa`. The target device is SDIO `02e7:9082`, exposed by Lunar as `mmc2:0001:1`.

The first patch adds a separate `rtw_9082xs` SDIO module and maps its WLAN register window to `0x14000` instead of RTL8189ES's `0x10000`. Six read-only register comparisons on hardware showed that the old window returns `0xea`, while the new window returns live register values. Other SDIO address spaces are left unchanged. The module builds against Lunar `6.18.41-lunar`.

Initialization is guarded by `experimental_hw_init=0` by default because this branch still inherits RTL8189ES's firmware and PHY tables. The tested 9082XS NIC image differs from Igor's firmware, and the MAC table values differ by a per-byte bit reversal. A boot or Wi-Fi claim from this binding would be premature. The diagnostic module in `../experimental/` has independently completed chip power, firmware download, MAC table replay and eFuse decode on the target.

Next: provide a separate 9082XS chip description and private firmware path, map the register data transform at the correct boundary, replace or adapt MAC/PHY/AGC/RF tables, then test firmware readiness, interface registration and traffic on hardware. Keep the extracted firmware outside the repository until redistribution rights are established.
