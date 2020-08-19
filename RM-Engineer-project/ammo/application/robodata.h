#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
typedef enum
{    
	UPLIFT_LEFT  = 0x201,
	UPLIFT_RIGHT = 0x202,
}drv_can1ID_e;
//底盘CAN1设备ID


typedef enum
{
	GRAP_LEFT   = 0x201,
	GRAP_RIGHT  = 0x202,
	FILTER_2006 = 0x203,
}drv_can2ID_e;
//底盘CAN2设备ID

#endif
