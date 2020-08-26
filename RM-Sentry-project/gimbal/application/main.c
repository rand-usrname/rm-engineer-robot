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

	/*遥控器初始化*/
	remote_uart_init();
	/*云台电机初始化(PID初始化)*/
    motor_gimbal_init();
	/*通用发弹机构初始化(包括snail初始化和m_launch初始化)*/
	strike_init(&gun1,100);
	/*哨兵6020发弹电机与其他兵种不同,重新初始化ID和PID参数*/
	launch_reinit();
	/*热量控制初始化*/
	heatctrl_start();
	/*定时器任务创建*/
	task_create();
	
	while(1)
	{
		if(Change_from_middle(S1))
		{
			;
		}
	}
	
	//功率限制测试主函数
//	while(1)
//	{
//		sport_mode_set(ONLY_CHASSIS);
//		chassis_speed_set(7176,(RC_data.Remote_Data.ch0 - 1024)*3,(RC_data.Remote_Data.ch2 - 1024)*8,(RC_data.Remote_Data.ch3 - 1024)*8);
//		rt_thread_mdelay(10);
//	}


	//陀螺仪角度闭环主函数
//	pid_t angpid_gyro;
//	pid_init(&angpid_gyro,2,0,5,100,5000,-5000);
//	while(!HERO_IMU.atti_ready)
//	{
//		rt_thread_mdelay(10);
//	}
//	beginyaw = (int)((HERO_IMU.yaw + 180.0)*8192.0/360.0);//记录初始位置
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
