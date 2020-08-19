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
#include "trailer.h"
#include "main_thread.h"
#include "robodata.h"
int main(void)
{
	/* 底盘线程初始化 */
	chassis_init();
#ifdef SINGLE_CTRL
	/* 遥控器初始化 */
	remote_uart_init();
#endif
	/* 拖车初始化 */
	trailer_start();
	/* 电磁阀初始化 */
	REVIVE_CARD_INIT;
	/* 云台控制初始化 */
	gimbal_ctrl_init();
	/* 主控制线程初始化 */
	main_thread_init();
	
	return RT_EOK;
}
