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

struct
{
	rt_int16_t		xspeed;
	rt_int16_t		yspeed;
	sport_mode_e	sport_mode;
	rt_uint16_t		follow_angle;
	rt_int16_t		augular_vel;
	int				usetime;

} ctldata = {0,0,NO_FOLLOW,0,0,0};//���ó�ʼֵ

int main(void)
{
	chassis_init();
	//remote_uart_init();//��ʹ��ң����
	while(1)
	{
		//ÿ��ֵ����ʹ��ʮ��
		if(ctldata.usetime < 10)
		{
			//�ٶ���ģʽ����
			sport_mode_set(ctldata.sport_mode);
			chassis_speed_set(ctldata.follow_angle,ctldata.augular_vel,ctldata.xspeed,ctldata.yspeed);
			ctldata.usetime++;
		}
		else
		{
			//����ʹ���������ٶȹ���
			sport_mode_set(NO_FOLLOW);
			chassis_speed_set(0,0,0,0);
		}
		rt_thread_mdelay(10);//��һ�����һ��
	}
	//��ң��������������
	// int angular_velocity = 0;
	// int xspeed = 0;
	// int yspeed = 0;
	// int bs = 500;
	// while(1)
	// {
	// 	sport_mode_set(ONLY_CHASSIS);
	// 	if(Change_from_middle(S1))
	// 	{
	// 		if(RC_data.Remote_Data.s1 == 1)
	// 		{
	// 			bs += 100;
	// 		}
	// 		else if(RC_data.Remote_Data.s1 == 2)
	// 		{
	// 			bs -= 100;
	// 		}
	// 	}
	// 	angular_velocity = ((RC_data.Remote_Data.ch0 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch0 - 1024) < -50)*bs;
	// 	xspeed = ((RC_data.Remote_Data.ch2 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch2 - 1024) < -50)*bs;
	// 	yspeed = ((RC_data.Remote_Data.ch3 - 1024) > 50)*bs - ((RC_data.Remote_Data.ch3 - 1024) < -50)*bs;
	// 	chassis_speed_set(0,angular_velocity,xspeed,yspeed);
	//}
	
	//�������Ʋ���������
	// while(1)
	// {
	// 	sport_mode_set(ONLY_CHASSIS);
	// 	chassis_speed_set(7176,(RC_data.Remote_Data.ch0 - 1024)*3,(RC_data.Remote_Data.ch2 - 1024)*8,(RC_data.Remote_Data.ch3 - 1024)*8);
	// 	rt_thread_mdelay(10);
	// }


	//�����ǽǶȱջ�������
	// pid_t angpid_gyro;
	// pid_init(&angpid_gyro,2,0,5,100,5000,-5000);
	// while(!HERO_IMU.atti_ready)
	// {
	// 	rt_thread_mdelay(10);
	// }
	// while(1)
	// {
	// 	sport_mode_set(ONLY_CHASSIS);
	// 	pid_output_motor(&angpid_gyro,beginyaw,(int)((HERO_IMU.yaw + 180.0)*8192.0/360.0));
	// 	if(RC_Ctrl.Remote_Data.s1 == 1)
	// 	{
	// 		chassis_speed_set(0,0,0,0);
	// 	}
	// 	else
	// 	{
	// 		chassis_speed_set(7176,-angpid_gyro.out,(RC_data.Remote_Data.ch2 - 1024)*12,(RC_data.Remote_Data.ch3 - 1024)*12);
	// 	}
	// 	rt_thread_mdelay(10);
	// }
	
}

int refresh_ctldata(struct rt_can_msg* msg)
{
	ctldata.xspeed = ((msg->data[0]<<8) + msg->data[1]);
	ctldata.yspeed = ((msg->data[2]<<8) + msg->data[3]);
	ctldata.sport_mode = (sport_mode_e)(msg->data[6]);
	if(ctldata.sport_mode == FOLLOW_GIMBAL)
	{
		ctldata.augular_vel = (msg->data[4]<<8) + msg->data[5];
	}
	else if(ctldata.sport_mode == NO_FOLLOW)
	{
		ctldata.follow_angle = (msg->data[4]<<8) + msg->data[5];
	}
	ctldata.usetime = 0;
	return 1;
}
