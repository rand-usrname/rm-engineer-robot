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

#define YAE_ZERO_ANGLE          3360
#define PITCH_ZERO_ANGLE        0

//tsymq
#define PITCH_UP	4094
#define PITCH_DOWN	3080
#define YAW_UP 5832
#define YAW_DOWN 1685

#define GIMBAL_CTL 		
#define RUB_SNAIL
#define LAUNCH_6020
#define LAUNCH_ID 0x201
#define FIRE_ANGLE 	(360.0f/10.0f)	//�����Ƕ�

typedef enum
{
    //����
    GIMBAL_CTLID = 0x200,

    //����
	LEFT_FRONT  = 0x201,    //�ĸ��������ӵ�ID
	RIGHT_FRONT	= 0x202,
	LEFT_BACK	= 0x203,
	RIGHT_BACK	= 0x204,

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
