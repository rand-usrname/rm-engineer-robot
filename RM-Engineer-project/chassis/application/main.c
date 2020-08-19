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
	/* �����̳߳�ʼ�� */
	chassis_init();
#ifdef SINGLE_CTRL
	/* ң������ʼ�� */
	remote_uart_init();
#endif
	/* �ϳ���ʼ�� */
	trailer_start();
	/* ��ŷ���ʼ�� */
	REVIVE_CARD_INIT;
	/* ��̨���Ƴ�ʼ�� */
	gimbal_ctrl_init();
	/* �������̳߳�ʼ�� */
	main_thread_init();
	
	return RT_EOK;
}
