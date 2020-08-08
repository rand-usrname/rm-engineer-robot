/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_chassis.h"
#include "drv_remote.h"

int main(void)
{
	chassis_init();
	remote_uart_init();
	while(1)
	{
		chassis_speed_set(0,(RC_data->Remote_Data.ch0 - 1024)*3,(RC_data->Remote_Data.ch2 - 1024)*3,(RC_data->Remote_Data.ch3 - 1024)*3);
		rt_thread_mdelay(10);
	}
}
