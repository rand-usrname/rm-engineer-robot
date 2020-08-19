#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "comm.h"

/* ��̨����ģʽ or ��������ģʽ */ /* �������ƿ������ʱʹ�� */
#define   SINGLE_CTRL     //SINGLE_CTRL   GIMBAL_CTRL


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
	TRAILER_LEFT = 0x201,
	TRAILER_RIGHT = 0x202,
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//����CAN2�豸ID

//����ˢ��
#define REVIVE_CARD_PIN    1
#define REVIVE_CARD_INIT   rt_pin_mode(REVIVE_CARD_PIN,PIN_MODE_INPUT_PULLUP)
#define REVIVE_CARD_SET    rt_pin_write(REVIVE_CARD_PIN,1)
#define REVIVE_CARD_RESET  rt_pin_write(REVIVE_CARD_PIN,0)

#endif
