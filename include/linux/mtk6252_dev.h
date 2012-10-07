/*
 * mtk6252 Management Routines, mtk6252 as a modem of system
 *
 * Copyright (c) 2012-2015 Huawei. All rights reserved.
 * Author: someone
 * Email:   someone@huawei.com
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _MTK6252_DEV_H
#define _MTK6252_DEV_H

#include <linux/types.h>
#include <linux/ioctl.h>

/* devcie data */
struct msm_device_mtk6252_data{
	unsigned gpio_pwron;
	unsigned gpio_reset;
	unsigned gpio_pwrstat;
	unsigned gpio_softwarestate;
	unsigned gpio_download_en;
	unsigned gpio_usb_sel;
};

/* remove unused code */
/* ioctl */
#define MTKMODEM_MONITOR_STATE	_IOR('M', 1, int)

/* mtk runs normally */
#define MTKMODEM_MONITOR_STATE_NORMAL 0
/* mtk shutdown */
#define MTKMODEM_MONITOR_STATE_SHUTDOWN 1
/* mtk reset */
#define MTKMODEM_MONITOR_STATE_ABNORMAL 2

/* remove unused code */


int mtk6252_dev_init(void);

#endif /* _MTK6252_DEV_H */
