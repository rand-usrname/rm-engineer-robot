#ifndef __ROBODATA_H__
#define __ROBODATA_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "comm.h"

/* 云台控制模式 or 独立控制模式 */ /* 独立控制可与测试时使用 */
#define   SINGLE_CTRL     //SINGLE_CTRL   GIMBAL_CTRL


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
	TRAILER_LEFT = 0x201,
	TRAILER_RIGHT = 0x202,
    YAW_ID      = 0x206,
    PITCH_ID    = 0x207,

    CHASSIS_CTL = 0x100
}drv_can2ID_e;
//底盘CAN2设备ID

//底盘刷卡
#define REVIVE_CARD_PIN    1
#define REVIVE_CARD_INIT   rt_pin_mode(REVIVE_CARD_PIN,PIN_MODE_INPUT_PULLUP)
#define REVIVE_CARD_SET    rt_pin_write(REVIVE_CARD_PIN,1)
#define REVIVE_CARD_RESET  rt_pin_write(REVIVE_CARD_PIN,0)

#endif
