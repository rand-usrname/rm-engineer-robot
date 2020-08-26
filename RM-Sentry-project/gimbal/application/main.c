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
#include "drv_motor.h"
#include "drv_gyro.h"
#include "drv_strike.h"

#include "mid_gimbal.h"
#include "task_create.h"
#include "task_gimbal.h"



int main(void)
{

	/*ң������ʼ��*/
	remote_uart_init();
	/*��̨�����ʼ��(PID��ʼ��)*/
    motor_gimbal_init();
	/*ͨ�÷���������ʼ��(����snail��ʼ����m_launch��ʼ��)*/
	strike_init(&gun1,100);
	/*�ڱ�6020����������������ֲ�ͬ,���³�ʼ��ID��PID����*/
	launch_reinit();
	/*�������Ƴ�ʼ��*/
	heatctrl_start();
	/*��ʱ�����񴴽�*/
	task_create();
	
	while(1)
	{
		if(Change_from_middle(S1))
		{
			;
		}
	}
	
	//�������Ʋ���������
//	while(1)
//	{
//		sport_mode_set(ONLY_CHASSIS);
//		chassis_speed_set(7176,(RC_data.Remote_Data.ch0 - 1024)*3,(RC_data.Remote_Data.ch2 - 1024)*8,(RC_data.Remote_Data.ch3 - 1024)*8);
//		rt_thread_mdelay(10);
//	}


	//�����ǽǶȱջ�������
//	pid_t angpid_gyro;
//	pid_init(&angpid_gyro,2,0,5,100,5000,-5000);
//	while(!HERO_IMU.atti_ready)
//	{
//		rt_thread_mdelay(10);
//	}
//	beginyaw = (int)((HERO_IMU.yaw + 180.0)*8192.0/360.0);//��¼��ʼλ��
//	while(1)
//	{
//		sport_mode_set(ONLY_CHASSIS);
//		beginyaw -= (RC_data.Remote_Data.ch0 - 1024)/50;
//		while(beginyaw > 8191)
//		{
//			beginyaw -= 8191;
//		}
//		while(beginyaw < 0)
//		{
//			beginyaw += 8191;
//		}
//		pid_output_motor(&angpid_gyro,beginyaw,(int)((HERO_IMU.yaw + 180.0)*8192.0/360.0));
//		if(RC_Ctrl.Remote_Data.s1 == 1)
//		{
//			chassis_speed_set(0,0,0,0);
//		}
//		else
//		{
//			chassis_speed_set(7176,-angpid_gyro.out,(RC_data.Remote_Data.ch2 - 1024)*12,(RC_data.Remote_Data.ch3 - 1024)*12);
//		}
//		rt_thread_mdelay(10);
//	}
	
}
