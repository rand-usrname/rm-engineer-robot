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
#include "drv_gyro.h"

int beginyaw;//记录初始yaw值

int main(void)
{
	chassis_init();
	remote_uart_init();
	
	int angular_velocity = 0;
	int xspeed = 0;
	int yspeed = 0;
	int bs = 500;
	while(1)
	{
		sport_mode_set(NO_FOLLOW);
		if(Change_from_middle(S1))
		{
			if(RC_data.Remote_Data.s1 == 1)
			{
				bs += 100;
			}
			else if(RC_data.Remote_Data.s1 == 2)
			{
				bs -= 100;
			}
		}
		angular_velocity = ((RC_data.Remote_Data.ch0 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch0 - 1024) < -50)*bs;
		xspeed = ((RC_data.Remote_Data.ch2 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch2 - 1024) < -50)*bs;
		yspeed = ((RC_data.Remote_Data.ch3 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch3 - 1024) < -50)*bs;
		chassis_speed_set(0,angular_velocity,xspeed,yspeed);
	}
	
	//功率限制测试主函数
//	while(1)
//	{
//		sport_mode_set(NO_FOLLOW);
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
//		sport_mode_set(NO_FOLLOW);
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
