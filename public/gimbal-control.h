#ifndef __GIMBAL_CONTROL_H__
#define __GIMBAL_CONTROL_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "pid.h"

typedef enum
{
	YAW_ID		= 0x206,
	PITCH_ID 	= 0X205,
	
	GYRO_ID 	= 0x1315
}deviceID_e;
//云台电机设备与陀螺仪设备反馈报文ID


extern int refresh_device_data(struct rt_can_msg* message);

#endif
