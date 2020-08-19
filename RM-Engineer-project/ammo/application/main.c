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
	/* ��̨���ݳ�ʼ�� */
	gimbal_ctrl_init();
	/* ���ֵ����ʼ�� */
	magazine_init();
	/* ̧������еצ��ʼ�� */
	Ammo_init();
	/* �������̳߳�ʼ�� */
	Main_Thread_init();
	return RT_EOK;
}
	

