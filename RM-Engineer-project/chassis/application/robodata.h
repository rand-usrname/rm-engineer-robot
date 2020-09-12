
#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "comm.h"

/* 云台控制模式 or 独立控制模式 */ /* 独立控制可与测试时使用 */
#define   GIMBAL_CTRL     //SINGLE_CTRL   GIMBAL_CTRL


#ifdef MECANUM_WHEEL

    //前后轮间距，单位mm
    #define VEHICLE_WIDTH		485//单位毫米
    //左右轮间距，单位mm
    #define VEHICLE_LONG		510//单位毫米
	
#endif
#ifdef OMNI_WHEEL

    //全向轮旋转直径
    #define VEHICLE_DIAMETER    469//单位毫米

#endif

#define YAW_ZERO_ANGLE          8186
#define PITCH_ZERO_ANGLE        1347
typedef enum
{
    //发送
    CHASSIS_CTLID = 0x200,

    //接收
	LEFT_FRONT  = 0x201,    //四个底盘轮子的ID
	RIGHT_FRONT	= 0x202,
	LEFT_BACK	= 0x203,
	RIGHT_BACK	= 0x204,
	
	TRAILER_LEFT = 0x205,
	TRAILER_RIGHT = 0x206,
	
    GYRO_ANGLE_ID = 0x101,  //陀螺仪数据结算ID
    GYRO_SPEED_ID = 0x102
    
}drv_can1ID_e;
//底盘CAN1设备ID


typedef enum
{
    //接收
    YAW_ID      = 0x205,
    PITCH_ID    = 0x206,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//底盘CAN2设备ID


#endif
