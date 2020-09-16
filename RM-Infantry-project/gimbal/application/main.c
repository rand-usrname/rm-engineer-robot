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
#include "drv_aimbot.h"
#include "drv_strike.h"
#include "robocontrol.h"

int main(void)
{
	remote_uart_init();
	while(!HERO_IMU.atti_ready);
	gimbal_init();
	strike_init(&gun1,1000);
	vision_init();
	while(1)
	{
		
		remote_ctrl(&RC_data);
		visual_ctl_CANsend((8191 - yaw.motordata.angle)*36000/8191,gimbal_atti.pitch*36000/360,20000);
		rt_thread_mdelay(10);
	}
}
