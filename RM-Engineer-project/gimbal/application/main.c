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
	/* �ȴ������Ǿ��� */
	//while(!HERO_IMU.atti_ready);
	/* ң������ʼ�� */
	remote_uart_init();
	/* ��̨��ʼ�� */
	gimbal_init();
	/* �������̳߳�ʼ�� */
	main_thread_init();
	/* ���������ʼ�� */
	strike_init(&gun1,1000);
	/* �Ӿ�ͨ�������ʼ�� */
	//vision_init();
	return RT_EOK;
}
