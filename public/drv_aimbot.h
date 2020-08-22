#ifndef __DRV_AIM_BOT__H
#define __DRV_AIM_BOT__H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_canthread.h"
#include "robodata.h"

typedef struct _Point_t
{
	float x;
	float y;
	float z;
}Point_t;
//作为留出的接口
typedef struct _Aimbot_t
{
	float pitch;
	float yaw;
}Aimbot_t;

/***********与视觉通信的部分************/


typedef enum
{
    RUNE		= 0,//大风车
    BIG_ARMOR	= 1,//大装甲板
    SMALL_ARMOR = 2,//小装甲板
	BASE_ARMOR	= 3,//基地装甲
	ENG_ARMOE	= 4,//工程装甲
	CAISSON		= 5,//弹药箱

	BLANK		= 0X7//空值

}aim_mode_e;
//打击目标

typedef enum
{
	RED		= 0,
    BLUE	= 1

}tarcolor_e;
//目标颜色

typedef enum
{
    COMPUTER	= 0,
    SIGNALCHIP	= 1

}forecast_e;
//预测者

typedef struct
{
	aim_mode_e	aim_mode;		//打击目标ID
	forecast_e	forcester;		//期望的预测者
	tarcolor_e	tracolor;		//期望的目标颜色

}visual_ctl_t;
//发送的控制信息存储部分

typedef struct
{
	aim_mode_e	aim_mode;		//打击目标ID
	forecast_e	forcester;		//实际的预测者
	int			computime;		//计算时间
	rt_int16_t	yawadd;			//-32768 ~ 32767 标定数据
	rt_int16_t	pitchadd;		//-32768 ~ 32767 标定角度
	rt_int16_t	x,y,z;			//三点坐标，单位均为 mm/s
	rt_uint8_t	yaw_usetime;	//标记使用次数
	rt_uint8_t	pitch_usetime;	//标记使用次数

}visual_rev_t;
//接收到的数据结构体
 
//必须要在某处调用才可以使用的函数
extern int vision_init(void);
//默认 can2 设备，接收ID VISUAL_REVID = 0X302
extern int refresh_visual_data(rt_uint8_t* data);

//模式设置
extern void tarcolor_set(tarcolor_e tarcolor);
extern void forecast_set(forecast_e forecast);
extern void aim_mode_set(aim_mode_e aim_mode);

//向视觉发送控制信息，默认使用 CAN2 设备
extern int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel);
extern int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel);

//使用视觉控制数据调用的函数
extern rt_int16_t get_yaw_add(void);
extern rt_int16_t get_pitch_add(void);
extern rt_int16_t get_yawusetime(void);
extern rt_int16_t get_pitchusetime(void);

/***********与视觉通信的部分结束************/


#endif
