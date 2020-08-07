#ifndef __DRV_AIM_BOT__H
#define __DRV_AIM_BOT__H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
typedef struct _Point_t
{
	float x;
	float y;
	float z;
}Point_t;
//作为留出的接口
typedef struct _Aimbot_t
{
	float pitch;
	float yaw;
}Aimbot_t;

















#endif
