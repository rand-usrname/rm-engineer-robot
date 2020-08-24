#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>

#define YAW_ZERO_ANGLE			5430
#define PITCH_ZERO_ANGLE		1330

#define PITCH_MIN_ANGLE			(-540)
#define PITCH_MAX_ANGLE			460

#define FIRE_ANGLE 60
#define HERO

typedef enum
{
	//����
	GIMBAL_CTL = 0x1FF,
	CHASSIS_CTL = 0x100,
	
	//����
	GYRO_ANGLE_ID = 0x101,  //���������ݽ���ID
	GYRO_SPEED_ID = 0x102,
    
	YAW_ID      = 0x208,	//��̨��������ݸ���ID
	PITCH_ID    = 0x205

}drv_can1ID_e;
//����CAN1�豸ID

typedef enum
{
	//����
	VISUAL_CTLID = 0X301,	//�Ӿ�ͨ�ŷ���
	
	//����
	VISUAL_REVID = 0X302	//�Ӿ�ͨ�Ž���
	
}drv_can2ID_e;
//����CAN2�豸ID

#endif
