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
#include "drv_remote.h"
#include "comm.h"
#include "magazine.h"
#include "ammo.h"
#include "main_thread.h"

int main(void)
{
	/* 云台数据初始化 */
	gimbal_ctrl_init();
	/* 弹仓电机初始化 */
	magazine_init();
	/* 抬升及机械爪初始化 */
	Ammo_init();
	/* 主控制线程初始化 */
	Main_Thread_init();
	return RT_EOK;
}
	

