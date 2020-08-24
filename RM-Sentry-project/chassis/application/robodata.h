#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>

#ifdef MECANUM_WHEEL

    //ǰ���ּ�࣬��λmm
    #define VEHICLE_WIDTH		390//��λ����
    //�����ּ�࣬��λmm
    #define VEHICLE_LONG		390//��λ����
	
#endif
#ifdef OMNI_WHEEL

    //ȫ������תֱ��
    #define VEHICLE_DIAMETER    469//��λ����

#endif

#define YAW_ZERO_ANGLE          3360
#define PITCH_ZERO_ANGLE        0

typedef enum
{
    //����
    CHASSIS_CTLID = 0x200,

    //����
	LEFT 	= 0x201,    //�������̵����ID
	RIGHT	= 0x202,


    GYRO_ANGLE_ID = 0x101,  //���������ݽ���ID
    GYRO_SPEED_ID = 0x102
    
}drv_can1ID_e;
//����CAN1�豸ID


typedef enum
{
    //����
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//����CAN2�豸ID

#endif
