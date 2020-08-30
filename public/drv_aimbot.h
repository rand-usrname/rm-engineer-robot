#ifndef __DRV_AIM_BOT__H
#define __DRV_AIM_BOT__H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_canthread.h"
#include "robodata.h"

typedef struct _Point_t
{
	float x;  /* 与目标水平距离 */
	float y;  /* 与目标高度差 */
	float z;  /* 与目标垂直距离 */
}Point_t;
//作为留出的接口
typedef struct _Aimbot_t
{
	float pitch_add;
	float yaw_add;
}Aimbot_t;

/***********与视觉通信的部分************/

/* 自瞄模式 */
typedef enum
{
	NORMAL_AIM         = 0x01,    /* 视觉端自瞄 */
	SMALL_WINDWILL     = 0x02,    /* 小能量机关 */
	BIG_WINDWILL       = 0x03,    /* 大能量机关 */
	ENGINEER_AMMO      = 0x04,    /* 工程取弹 */
	LOB_SHOT           = 0x05,    /* 英雄吊射 */
	ELEC_AIM           = 0x06,    /* 电控端自瞄 */
	
}aim_mode_e;

/* 目标颜色 */
typedef enum
{
	RED		= 0xA0,
    BLUE	= 0xB0

}tarcolor_e;

/* 控制帧头 */
typedef struct
{
	aim_mode_e	aim_mode;	
	tarcolor_e	tracolor;		
}visual_head_t;

typedef struct
{
	rt_int16_t	yawadd;			//-32768 ~ 32767 标定数据
	rt_int16_t	pitchadd;		//-32768 ~ 32767 标定角度
	rt_uint8_t	yaw_usetime;	//标记使用次数
	rt_uint8_t	pitch_usetime;	//标记使用次数
	Point_t	    visual_point;			//三点坐标，单位均为 m
	int			computime;		//计算时间

}visual_rev_t;
//接收到的数据结构体
 
//必须要在某处调用才可以使用的函数
extern int vision_init(void);
//默认 can2 设备，接收ID VISUAL_REVID = 0X302
extern int refresh_visual_data(rt_uint8_t* data);

//模式设置
extern void tarcolor_set(tarcolor_e tarcolor);
extern void aim_mode_set(aim_mode_e aim_mode);

//向视觉发送控制信息，默认使用 CAN2 设备
extern int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,float bullet_vel);
extern int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,float bullet_vel);

//使用视觉控制数据调用的函数
extern rt_int16_t get_yaw_add(void);
extern rt_int16_t get_pitch_add(void);
extern rt_int16_t get_yawusetime(void);
extern rt_int16_t get_pitchusetime(void);

/***********与视觉通信的部分结束************/


#endif
