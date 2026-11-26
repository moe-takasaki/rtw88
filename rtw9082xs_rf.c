// SPDX-License-Identifier: GPL-2.0-only
/* SCI 9082XS RF serial-write sequence. */

#include <linux/delay.h>

#include "main.h"
#include "rtw8188e_table.h"

struct rtw9082xs_rf_reg {
	u32 value;
	bool long_delay;
};

static const struct rtw9082xs_rf_reg rtw9082xs_rf[] = {
	{ 0x00c00000, false },
	{ 0x00110200, false },
	{ 0x800120e0, false },
	{ 0x80090048, false },
	{ 0x800d0021, false },
	{ 0x80170090, false },
	{ 0x800f1001, false },
	{ 0x408f0506, false },
	{ 0xc03f0000, false },
	{ 0x20040603, false },
	{ 0xa0be0000, false },
	{ 0xa0d18701, false },
	{ 0x600ea84a, false },
	{ 0x100c0000, false },
	{ 0xd0f01f3f, false },
	{ 0xd0a82200, false },
	{ 0xd0343398, false },
	{ 0xd0220cc0, false },
	{ 0xd026a97c, false },
	{ 0xd08ee318, false },
	{ 0xd00106ff, false },
	{ 0xd0190080, false },
	{ 0xd0250000, false },
	{ 0xd00d2000, false },
	{ 0xd03f0000, false },
	{ 0x30042400, false },
	{ 0x300c0090, false },
	{ 0x30223089, false },
	{ 0x309a9999, false },
	{ 0x300600c5, false },
	{ 0x301e1104, false },
	{ 0x30e13660, false },
	{ 0x30090000, false },
	{ 0x30150000, false },
	{ 0xb00f8001, false },
	{ 0x700f8005, false },
	{ 0xa0684dbe, false },
	{ 0xa0e427bb, false },
	{ 0xa00c00ce, false },
	{ 0xa0a6f8cf, false },
	{ 0xc00a0061, false },
	{ 0xc00a8061, false },
	{ 0xc00a4061, false },
	{ 0xc00638a4, false },
	{ 0xc00639a4, false },
	{ 0xc08638a4, false },
	{ 0xc08639a4, false },
	{ 0xd026a11c, false },
	{ 0x800130e0, false },
	{ 0xa025bd00, false },
	{ 0x80e99c0b, false },
	{ 0xc002b5cf, false },
	{ 0xc002b90f, false },
	{ 0xc002b1b7, false },
	{ 0xc002be57, false },
	{ 0xc002b6e7, false },
	{ 0xc0022a77, false },
	{ 0xc00222d7, false },
	{ 0xc0022c17, false },
	{ 0xc00224d6, false },
	{ 0xc0022816, false },
	{ 0xc00200b6, false },
	{ 0x00c0809a, false },
	{ 0x10624100, false },
	{ 0x10060073, false },
	{ 0x102e5100, false },
	{ 0x1067aa02, false },
	{ 0x101f0100, false },
	{ 0x700f0405, false },
	{ 0xc0fd400d, false },
	{ 0xc07def0d, false },
	{ 0xc0bdf20d, false },
	{ 0xc03d0f06, false },
	{ 0xc0dd0009, false },
	{ 0xc05d0001, false },
	{ 0xc09d0001, false },
	{ 0xc01def01, false },
	{ 0xc0ed440d, false },
	{ 0xc06def0d, false },
	{ 0xc0adf20d, false },
	{ 0xc02d0f06, false },
	{ 0xc0cd0009, false },
	{ 0xc04d0001, false },
	{ 0xc08d0001, false },
	{ 0xc00def01, false },
	{ 0x700f0005, false },
	{ 0x0080809a, false },
	{ 0x80012fe0, true },
	{ 0x801f00c0, true },
	{ 0x80070080, false },
	{ 0x801f0000, false },
	{ 0x00c07c06, false },
};

static void rtw9082xs_parse_rf(struct rtw_dev *rtwdev,
			       const struct rtw_table *tbl)
{
	const struct rtw9082xs_rf_reg *regs = tbl->data;
	u32 i;

	for (i = 0; i < tbl->size; i++) {
		rtw_write32(rtwdev, 0x840, regs[i].value);
		if (regs[i].long_delay)
			msleep(100);
		else
			udelay(1);
	}
}

const struct rtw_table rtw9082xs_rf_tbl = {
	.data = rtw9082xs_rf,
	.size = ARRAY_SIZE(rtw9082xs_rf),
	.parse = rtw9082xs_parse_rf,
};
