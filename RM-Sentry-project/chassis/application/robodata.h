#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>

#ifdef MECANUM_WHEEL

    //前后轮间距，单位mm
    #define VEHICLE_WIDTH		390//单位毫米
    //左右轮间距，单位mm
    #define VEHICLE_LONG		390//单位毫米
	
#endif
#ifdef OMNI_WHEEL

    //全向轮旋转直径
    #define VEHICLE_DIAMETER    469//单位毫米

#endif

#define YAW_ZERO_ANGLE          3360
#define PITCH_ZERO_ANGLE        0

typedef enum
{
    //发送
    CHASSIS_CTLID = 0x200,

    //接收
	LEFT 	= 0x201,    //两个底盘电机的ID
	RIGHT	= 0x202,


    GYRO_ANGLE_ID = 0x101,  //陀螺仪数据解算ID
    GYRO_SPEED_ID = 0x102
    
}drv_can1ID_e;
//底盘CAN1设备ID


typedef enum
{
    //接收
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//底盘CAN2设备ID

#endif
