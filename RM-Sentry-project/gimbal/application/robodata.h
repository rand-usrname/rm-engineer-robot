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
#define LAUNCH_ID 0x205
#define FIRE_ANGLE 	(360.0f/10.0f)	//发弹角度

typedef enum
{
    //发送
    GIMBAL_CTLID = 0x200,

    //接收
	LEFT_FRONT  = 0x201,    //四个底盘轮子的ID
	RIGHT_FRONT	= 0x202,
	LEFT_BACK	= 0x203,
	RIGHT_BACK	= 0x204,

    GYRO_ANGLE_ID = 0x101,  //陀螺仪数据结算ID
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
