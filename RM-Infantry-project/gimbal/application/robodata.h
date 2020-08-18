#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>


typedef enum
{
    //发送
    GIMBAL_CTL = 0x200,

    //接收
    GYRO_ANGLE_ID = 0x101,  //陀螺仪数据结算ID
    GYRO_SPEED_ID = 0x102,
    
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207

}drv_can1ID_e;
//底盘CAN1设备ID

typedef enum
{
    //发送
    CHASSIS_CTL = 0x100,
    
    //接收

}drv_can2ID_e;
//底盘CAN2设备ID

#endif
