// SPDX-License-Identifier: GPL-2.0-only
/* SDIO binding for the Tilk/SCI 9082XS. */

#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "main.h"
#include "hci.h"
#include "rtw8188e.h"
#include "sdio.h"

static bool experimental_hw_init;
module_param(experimental_hw_init, bool, 0600);
MODULE_PARM_DESC(experimental_hw_init,
		 "Allow unfinished 9082XS firmware and PHY initialization");

static bool experimental_register_hw;
module_param(experimental_register_hw, bool, 0600);
MODULE_PARM_DESC(experimental_register_hw,
		 "Register the unfinished 9082XS phy without bringing it up");

static bool experimental_start_once;
module_param(experimental_start_once, bool, 0600);
MODULE_PARM_DESC(experimental_start_once,
		 "Power on and off the unfinished 9082XS core during probe");

static int rtw_9082xs_probe(struct sdio_func *func,
			    const struct sdio_device_id *id)
{
	struct ieee80211_hw *hw;
	struct rtw_dev *rtwdev;
	struct rtw_chip_info *chip;
	struct sdio_device_id probe_id;
	int ret;

	if (!experimental_hw_init)
		return dev_err_probe(&func->dev, -EOPNOTSUPP,
				     "9082XS firmware and PHY adaptation is incomplete\n");
	if (experimental_start_once && !experimental_register_hw)
		return -EINVAL;

	if (!experimental_register_hw)
		return rtw_sdio_probe(func, id);

	chip = devm_kmemdup(&func->dev, &rtw9082xs_hw_spec,
			    sizeof(rtw9082xs_hw_spec), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->stop_after_efuse = false;
	probe_id = *id;
	probe_id.driver_data = (kernel_ulong_t)chip;
	dev_info(&func->dev, "experimental registration enabled\n");

	ret = rtw_sdio_probe(func, &probe_id);
	if (ret || !experimental_start_once)
		return ret;

	hw = sdio_get_drvdata(func);
	rtwdev = hw->priv;
	mutex_lock(&rtwdev->mutex);
	ret = rtw_power_on(rtwdev);
	if (!ret) {
		ret = rtw_hci_get_io_error(rtwdev);
		rtw_power_off(rtwdev);
		clear_bit(RTW_FLAG_FW_RUNNING, rtwdev->flags);
		if (!ret)
			ret = rtw_hci_get_io_error(rtwdev);
	}
	mutex_unlock(&rtwdev->mutex);
	if (ret) {
		dev_err(&func->dev, "experimental core power-on failed: %d\n", ret);
		rtw_sdio_remove(func);
		sdio_set_drvdata(func, NULL);
		return ret;
	}

	dev_info(&func->dev, "experimental core power-on/off completed\n");
	return 0;
}

static const struct sdio_device_id rtw_9082xs_id_table[] = {
	{
		SDIO_DEVICE(0x02e7, 0x9082),
		.driver_data = (kernel_ulong_t)&rtw9082xs_hw_spec,
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
