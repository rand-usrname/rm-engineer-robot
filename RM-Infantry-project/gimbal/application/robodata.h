#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>


typedef enum
{
    //����
    GIMBAL_CTL = 0x200,

    //����
    GYRO_ANGLE_ID = 0x101,  //���������ݽ���ID
    GYRO_SPEED_ID = 0x102,
    
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207

}drv_can1ID_e;
//����CAN1�豸ID

typedef enum
{
    //����
    CHASSIS_CTL = 0x100,
    
    //����

}drv_can2ID_e;
//����CAN2�豸ID

#endif
