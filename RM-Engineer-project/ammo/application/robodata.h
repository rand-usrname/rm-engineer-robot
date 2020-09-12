#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>

/* 云台控制模式 or 独立控制模式 */ /* 独立控制可与测试时使用 */
#define   SINGLE_CTRL     //SINGLE_CTRL   GIMBAL_CTRL

typedef enum
{    
	GRAP_LEFT    = 0x201,
	GRAP_RIGHT   = 0x202,
	UPLIFT_LEFT  = 0x203,
	UPLIFT_RIGHT = 0x204,
	FILTER_2006  = 0x205,
}drv_can1ID_e;
//底盘CAN1设备ID


typedef enum
{
	GIMBAL_ID   = 0x101,
}drv_can2ID_e;
//底盘CAN2设备ID

#endif
