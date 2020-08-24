#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#define SNAIL
#define FIRE_ANGLE              50

#define YAW_ZERO_ANGLE          6762
#define PITCH_ZERO_ANGLE        7500

#define PITCH_MIN_ANGLE			-401
#define PITCH_MAX_ANGLE			590

typedef enum
{
    //����
    GIMBAL_CTL = 0x1FF,

    //����
    GYRO_ANGLE_ID = 0x101,  //���������ݽ���ID
    GYRO_SPEED_ID = 0x102,
    
    YAW_ID      = 0x205,	//��̨��������ݸ���ID
    PITCH_ID    = 0x208

}drv_can1ID_e;
//����CAN1�豸ID

typedef enum
{
    //����
    CHASSIS_CTL = 0x100,
	AMMO_CTL = 0x101,
    VISUAL_CTLID = 0X301,	//�Ӿ�ͨ�ŷ���
	
    //����
	VISUAL_REVID = 0X302	//�Ӿ�ͨ�Ž���
	
}drv_can2ID_e;
//����CAN2�豸ID

#endif