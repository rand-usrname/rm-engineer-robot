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
#include "core_board.h"
#include "chassis.h"
#include "communication.h"
#include "remote.h"

struct{
	int16_t 		xspeed;
	int16_t 		yspeed;
	sport_mode_e 	sport_mode;
	rt_uint16_t 	follow_angle;
	rt_int16_t 		angular_velocity;
	
}yuntai_data;

int main(void)
{
	set_RGB(CORE_WHITE);
	while(!chassis_init());
	communication_init();
	sport_mode_set(FOLLOW);
	
	while(1)
	{
		if(yuntai_data.sport_mode)
		{
			sport_mode_set(FOLLOW);
		}
		else
		{
			sport_mode_set(NO_FOLLOW);
		}
		chassis_speed_set(yuntai_data.follow_angle,yuntai_data.angular_velocity,yuntai_data.xspeed,yuntai_data.yspeed);
		rt_thread_mdelay(10);
		//rt_kprintf("luiyishigezz");
	}
}

bool refresh_data(struct rt_can_msg* message)
{
	//其他数据
	switch(message->id)
	{
		case 0x100:
			yuntai_data.xspeed = ((message->data[0]<<8) + message->data[1])*3;
			yuntai_data.yspeed = ((message->data[2]<<8) + message->data[3])*3;
			yuntai_data.sport_mode = message->data[6];
			if(yuntai_data.sport_mode)
			{
				yuntai_data.follow_angle = (message->data[4]<<8) + message->data[5];
			}
			else
			{
				yuntai_data.angular_velocity = (message->data[4]<<8) + message->data[5];
			}
			return true;
			
		default:
			
			break;
	}
	return false;
}

