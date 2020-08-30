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
#include "main_thread.h"
#include "drv_strike.h"
#include "drv_aimbot.h"
int main(void)
{
	/* 等待陀螺仪就绪 */
	//while(!HERO_IMU.atti_ready);
	/* 遥控器初始化 */
	remote_uart_init();
	/* 云台初始化 */
	gimbal_init();
	/* 主控制线程初始化 */
	main_thread_init();
	/* 发射机构初始化 */
	strike_init(&gun1,1000);
	/* 视觉通信组件初始化 */
	//vision_init();
	return RT_EOK;
}
