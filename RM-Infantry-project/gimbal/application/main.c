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
#include "drv_gimbal.h"
#include "drv_remote.h"

int main(void)
{
	remote_uart_init();
	while(!HERO_IMU.atti_ready);
	gimbal_init();
	while(1)
	{
		
		rt_thread_mdelay(1);
	}
}
