/************************************** Copyright ****************************** 
  *
  *                 (C) Copyright 2020,China, HITwh.
  *                            All Rights Reserved
  *                              
  *                     By  哈尔滨工业大学（威海）HERO战队
  *                     HITwh Excellent Robot Organization
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : drv_sentrychas.h   
  * Version    : v1.0		
  * Author     : yi_lu			
  * Date       : 2020-08-23         
  * Description:    哨兵的底盘中间件

  ****************************************************************************** */


#ifndef __MID_CHASSIS_H__
#define __MID_CHASSIS_H__

#include <rtthread.h>
#include <rtdevice.h>

#include "pid.h"
#include "math.h"
#include "can_receive.h"
#include "drv_motor.h"

#include "robodata.h"

#define  STDID_GIMBAL  0x110
#define  STDID_CHASSIS  0x111

extern Motor_t m_chassis_3508[2];

void chassis_init(void);
void chassis_control(void);

void read_gimbal_data(struct rt_can_msg *msg);


#ifdef mqy
typedef enum
{
	NO_FOLLOW,							//底盘不跟随云台，但以云台朝向为前方
	FOLLOW_GIMBAL,						//底盘跟随云台

	ONLY_CHASSIS,						//仅存在底盘，无角速度闭环

}sport_mode_e;
//底盘运动模式

typedef struct
{
	rt_uint16_t 	angle;				//转子角度，大小范围0~8191
	rt_int16_t 		speed;				//转子转速，单位rpm
	rt_int16_t 		current;			//实际转矩电流
	rt_uint8_t 		temperature;		//电机的实际温度
	
} motordata_t;
//电机的当前转速等数据

typedef struct
{
	drv_can1ID_e 	motorID;			//枚举定义电机ID
	motordata_t		motordata;			//电机数据结构体
	pid_t		    speedpid;			//pid参数结构体
	
} motor_t;
//该结构体用于四个麦轮的控制和数据存储 

typedef struct
{
	sport_mode_e	sport_mode;			//当前运动模式
	pid_t		    anglepid;			//角度环pid，在需要跟随时计算角速度
	motordata_t		pitch_data;			//pitch轴当前数据（以水平为0度
	motordata_t		yaw_data;			//yaw轴数据
	int				powermax;			//最大功率限制
	
	//以下三个数据储存以云台为正方向的期望速度
	rt_uint16_t		angle;		//跟随角度，范围0-8191
	rt_int16_t 		xspeed;				//单位mm/s
	rt_int16_t		yspeed;				//单位mm/s

} chassis_data_t;

extern void chassis_speed_set(rt_uint16_t follow_angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed);
extern void sport_mode_set(sport_mode_e sport_mode);
extern int refresh_chassis_motor_data(struct rt_can_msg* message);
extern int refresh_gimbal_motor_data(struct rt_can_msg* message);
extern int chassis_init(void);
#endif

#endif
