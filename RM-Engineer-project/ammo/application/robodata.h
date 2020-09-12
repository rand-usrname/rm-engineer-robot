#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>

/* ��̨����ģʽ or ��������ģʽ */ /* �������ƿ������ʱʹ�� */
#define   SINGLE_CTRL     //SINGLE_CTRL   GIMBAL_CTRL

typedef enum
{    
	GRAP_LEFT    = 0x201,
	GRAP_RIGHT   = 0x202,
	UPLIFT_LEFT  = 0x203,
	UPLIFT_RIGHT = 0x204,
	FILTER_2006  = 0x205,
}drv_can1ID_e;
//����CAN1�豸ID


typedef enum
{
	GIMBAL_ID   = 0x101,
}drv_can2ID_e;
//����CAN2�豸ID

#endif
