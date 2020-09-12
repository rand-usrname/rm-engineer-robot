
#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "comm.h"

/* ��̨����ģʽ or ��������ģʽ */ /* �������ƿ������ʱʹ�� */
#define   GIMBAL_CTRL     //SINGLE_CTRL   GIMBAL_CTRL


#ifdef MECANUM_WHEEL

    //ǰ���ּ�࣬��λmm
    #define VEHICLE_WIDTH		485//��λ����
    //�����ּ�࣬��λmm
    #define VEHICLE_LONG		510//��λ����
	
#endif
#ifdef OMNI_WHEEL

    //ȫ������תֱ��
    #define VEHICLE_DIAMETER    469//��λ����

#endif

#define YAW_ZERO_ANGLE          8186
#define PITCH_ZERO_ANGLE        1347
typedef enum
{
    //����
    CHASSIS_CTLID = 0x200,

    //����
	LEFT_FRONT  = 0x201,    //�ĸ��������ӵ�ID
	RIGHT_FRONT	= 0x202,
	LEFT_BACK	= 0x203,
	RIGHT_BACK	= 0x204,
	
	TRAILER_LEFT = 0x205,
	TRAILER_RIGHT = 0x206,
	
    GYRO_ANGLE_ID = 0x101,  //���������ݽ���ID
    GYRO_SPEED_ID = 0x102
    
}drv_can1ID_e;
//����CAN1�豸ID


typedef enum
{
    //����
    YAW_ID      = 0x205,
    PITCH_ID    = 0x206,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//����CAN2�豸ID


#endif
