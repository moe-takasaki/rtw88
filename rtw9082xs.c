// SPDX-License-Identifier: GPL-2.0-only
/* SDIO binding for the Tilk/SCI 9082XS. */

#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/module.h>
#include "main.h"
#include "rtw8188e.h"
#include "sdio.h"

static bool experimental_hw_init;
module_param(experimental_hw_init, bool, 0600);
MODULE_PARM_DESC(experimental_hw_init,
		 "Allow unfinished 9082XS firmware and PHY initialization");

static int rtw_9082xs_probe(struct sdio_func *func,
			    const struct sdio_device_id *id)
{
	if (!experimental_hw_init)
		return dev_err_probe(&func->dev, -EOPNOTSUPP,
				     "9082XS firmware and PHY adaptation is incomplete\n");

	return rtw_sdio_probe(func, id);
}

static const struct sdio_device_id rtw_9082xs_id_table[] = {
	{
		SDIO_DEVICE(0x02e7, 0x9082),
		.driver_data = (kernel_ulong_t)&rtw8188e_hw_spec,
	},
	{}
};
MODULE_DEVICE_TABLE(sdio, rtw_9082xs_id_table);

static struct sdio_driver rtw_9082xs_driver = {
	.name = KBUILD_MODNAME,
	.id_table = rtw_9082xs_id_table,
	.probe = rtw_9082xs_probe,
	.remove = rtw_sdio_remove,
	.drv = {
		.pm = &rtw_sdio_pm_ops,
		.shutdown = rtw_sdio_shutdown,
	}
};
module_sdio_driver(rtw_9082xs_driver);

MODULE_DESCRIPTION("Experimental 9082XS SDIO binding for rtw88");
MODULE_LICENSE("GPL");
