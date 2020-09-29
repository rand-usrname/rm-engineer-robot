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
#include "drv_strike.h"
#include "robocontrol.h"
void strike_pid_init(void)
{
	//自己初始化发射机构(摩擦轮+发弹)的PID

	//举例1:snail摩擦轮+2006拨弹

	//举例2:3508摩擦轮+2006拨弹
	// pid_init(&m_rub[0].spe,  
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	// pid_init(&m_rub[1].spe, 
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	 pid_init(&m_launch.ang, 
	 				3.5,0,0,
	 				500,5000,-5000);
	 pid_init(&m_launch.spe, 
	 				7.5,0,0,
	 				350,8000,-8000);

}

int main(void)
{
	remote_uart_init();
	while(!HERO_IMU.atti_ready);
	gimbal_init();
	strike_init(&gun1,1000);
	vision_init();
<<<<<<< HEAD
	strike_pid_init();
=======
>>>>>>> 2847d183ebe255e6e65b8f62f146679a52becd9d
	while(1)
	{
		
		remote_ctrl(&RC_data);
<<<<<<< HEAD
		visual_ctl_CANsend((8191 - yaw.motordata.angle)*36000/8192,gimbal_atti.pitch*100,20.0f);
=======
		visual_ctl_CANsend((8191 - yaw.motordata.angle)*36000/8191,gimbal_atti.pitch*36000/360,20000);
>>>>>>> 2847d183ebe255e6e65b8f62f146679a52becd9d
		rt_thread_mdelay(10);
	}
}
