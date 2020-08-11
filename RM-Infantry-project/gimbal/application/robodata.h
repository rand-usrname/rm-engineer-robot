#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>


typedef enum
{
    //发送
    GIMBAL_MCTL = 0x200,

    //接收
    GYRO_ID     = 0x1315,
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
