# 9082XS Android-driver gaps

The supplied Android `9082xs.ko` is not an RTL8189ES module with only a new
SDIO ID. Several Android register values are bytewise bit reversals of the
corresponding RTL8189ES values. For example:

| RTL8189ES value | Android 9082XS value | Register or operation |
| --- | --- | --- |
| `0x100a` | `0x0850` | Four SIFS registers |
| `0x005ea42b` | `0x007a25d4` | EDCA best-effort parameters |
| `0x40` | `0x02` | ACK timeout |
| `0x3030` | `0x0c0c` | Default retry limit |
| `0xcdf0` | `0xb30f` | TX-report timer |
| `0x02` | `0x40` | Maximum reported MACID |

The [public RTL8189ES branch](https://github.com/ChalesYu/rtl8822bs-aml/tree/rtl8188e_usb_sdio_5.13.3-17-test)
only adds `02e7:9082` to the SDIO ID table; it contains no SCI-specific HAL
changes. Its README says the ID *should* work. That claim does not establish
that this board's 9082XS uses an unmodified RTL8189ES register interface.
The stock values above come from its
[`sdio_halinit.c`](https://github.com/ChalesYu/rtl8822bs-aml/blob/rtl8188e_usb_sdio_5.13.3-17-test/hal/rtl8188e/sdio/sdio_halinit.c);
the Android values come from the supplied module's `WMACSetting_init`,
`AdaptiveCtrl_init`, `EDCA_init`, `RetryFunction_init` and HAL initialization.

The conversion is not safe to apply to every bus operation. Android's masked
BB accessor reverses bits on both sides of a read-modify-write, while its
direct BB writes take already converted constants. MACID uses a 32-bit write
for the first four address bytes followed by two ordinary byte writes;
BSSID uses the same 32-bit write followed by two *bit-reversed* byte writes.
SDIO-local counters use a different nibble conversion. The current driver
implements these distinctions in the paths that have been traced and tested.
It also matches the observed Android RCR value `0x0e000673`, RX driver-info
size `0x20`, RX aggregation timeout `0x60`, TX queue-page allocation, MAC/BSSID
write widths, management TX descriptor, control-frame filter map and the main
IQK register sequences. SIFS, EDCA, rate fallback and initial ACK/retry
values now match observed Android readback; association still times out.

## Still different

| Area | Android module | Current `s9082c` branch | Evidence and limit |
| --- | --- | --- | --- |
| Join-time MAC transition | Temporarily adjusts RCR, RX filter map and retry limit while joining | Initial SIFS, EDCA, rate fallback and ACK/retry values now match; no Android-style join transition | The combined initial register sequence was tested and did not establish association. The join transition remains untested. |
| TX reports | Enables bits `0xc0` in `TX_RPT_CTRL`, sets `TX_RPT_CTRL+1=0x40` and timer `0xb30f` | Inherited bits `0x03`, maximum MACID `0x20`, timer `0x927c` | A completed test verified Android register readback, enabled mac80211 ACK-status requests and observed numbered report requests, but no RX report type 1/2 arrived. The branch therefore still does not advertise ACK-report support. |
| Calibration and power tracking | Runs IQK, power-tracking setup, then LCK during HAL initialization; uses SCI tables and BB access | Runs LCK then the adapted IQK at first managed TX; retains RTL8188E power-tracking tables and runtime matrix writes | IQK now produces changing TX values and RX shots. Their effect on TX quality, and the power-tracking path, remain unverified. |
| Firmware and low power | Android firmware initialization, reserved pages, H2C/C2H and power-mode transitions | Reuses the 8188E firmware interface with 9082XS adaptations | The expected TX report is absent; WPA and data traffic have not been established. A no-LPS test could not evaluate the associated state. |

## What is established

`wlan0` registers, passive and active scans find nearby BSSs, and the AP
answers authentication. Several earlier runs received successful association
responses, followed by beacon loss and disconnect. Association is not
repeatable; no WPA handshake or IP data was observed. The 9082XS RX length
excludes FCS, and clearing the inherited `RX_INCLUDES_FCS` capability removed
mac80211's `corrupt beacon` message. SDIO writes of association and probe
requests return success, but this does not establish that the AP received
them. No independent radio or AP log is available. The exact point of loss
is therefore still unknown.
