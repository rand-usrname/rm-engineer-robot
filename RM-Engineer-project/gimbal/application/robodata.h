#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#define RUB_SNAIL
#define FIRE_ANGLE              50

#define YAW_ZERO_ANGLE          6762
#define PITCH_ZERO_ANGLE        7500

#define PITCH_MIN_ANGLE			-401
#define PITCH_MAX_ANGLE			590

typedef enum
{
    //发送
    GIMBAL_CTL = 0x1FF,

    //接收
    GYRO_ANGLE_ID = 0x101,  //陀螺仪数据结算ID
    GYRO_SPEED_ID = 0x102,
    LAUNCH_ID   = 0x201,
    YAW_ID      = 0x205,	//云台两电机数据更新ID
    PITCH_ID    = 0x208

}drv_can1ID_e;
//底盘CAN1设备ID

typedef enum
{
    //发送
    CHASSIS_CTL = 0x100,
	AMMO_CTL = 0x101,
    VISUAL_CTLID = 0X301,	//视觉通信发送
	
    //接收
	VISUAL_REVID = 0X302	//视觉通信接收
	
}drv_can2ID_e;
//底盘CAN2设备ID

#endif
