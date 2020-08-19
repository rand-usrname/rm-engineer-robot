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
int beginyaw;//记录初始yaw值

int main(void)
{
	//remote_uart_init();
	
	/* 云台数据初始化 */
	gimbal_ctrl_init();
	
	return RT_EOK;
}
	

