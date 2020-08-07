#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "stdbool.h"
#include "math.h"

//四个电机的控制报文ID
#define MOTOR_MESSAGE_ID		0X200

//电机控制线程的优先级
#define CHASSIS_CONTRAL_PRIORITY 1

//前后轮间距，单位mm
#define VEHICLE_WIDTH			390
//左右轮间距，单位mm
#define VEHICLE_LONG			390

//定义yaw轴和pitch轴的初始角度
#define YAW_START_ANGLE			3360//1300
#define PITCH_START_ANGLE		0

//电机最小ID值（数值上最小
#define MIN_ID					0x201
typedef enum 
{
	LEFT_FRONT	= 0x201,				//在此修改四个轮子的ID
	RIGHT_FRONT	= 0x202,
	LEFT_BACK	= 0x203,
	RIGHT_BACK	= 0x204,
	
	YAW					= 0x206,		//云台两轴ID
	PITCH				= 0x205
}motorID_e;
//四个轮子的反馈报文ID，并将该值作为四个轮子的标识ID

typedef enum
{
	NO_FOLLOW,							//底盘不跟随
	FOLLOW								//小陀螺运动模式
	
}sport_mode_e;
//

typedef struct
{
	float 			kp;					//pid三个参数
	float 			ki;
	float 			kd;
	int 			err;				//偏差值
	int 			err_old;			//上次偏差值
	float 			i_value;			//现在的积分值（该积分已经乘了ki参数了
	float  			i_limit;			//积分限幅
	int				out;				//pid的输出
	int				out_limit_up;		//输出上限幅
	int				out_limit_down;		//输出下限幅
	
}motorpid_t;
//电机PID闭环结构体

typedef struct
{
	rt_uint16_t 	angle;				//转子角度，大小范围0~8191
	rt_int16_t 		speed;				//转子转速，单位rpm
	rt_int16_t 		current;			//实际转矩电流
	rt_uint8_t 		temperature;		//电机的实际温度
	
}motordata_t;
//电机的实时数据，通过与chassis_communication设备通信更新

typedef struct
{
	motorID_e 		motorID;			//枚举定义电机ID
	motordata_t		motordata;			//电机数据结构体
	motorpid_t		motorpid;			//pid参数结构体
	int 			expected_speed;		//电机的期望转速

}motor_t;
//该结构体用于四个麦轮的控制和数据存储

typedef struct
{
	sport_mode_e	sport_mode;			//当前运动模式
	motorpid_t		anglepid;			//角度环pid，在需要跟随时计算角速度
	motordata_t		pitch_data;			//pitch轴当前数据（以水平为0度
	motordata_t		yaw_data;			//yaw轴数据
	
	//以下三个数据储存以云台为正方向的期望速度
	rt_uint16_t		follow_angle;		//跟随角度，范围0-8191
	rt_int16_t 		angular_velocity;	//单位0.1 °/s
	rt_int16_t 		xspeed;				//单位mm/s
	rt_int16_t		yspeed;				//单位mm/s
}motion_data_t;


extern void chassis_speed_set(rt_uint16_t follow_angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed);
extern void sport_mode_set(sport_mode_e sport_mode);
extern bool refresh_chassis_motor_data(struct rt_can_msg* message);
extern bool refresh_yuntai_motor_data(struct rt_can_msg* message);
extern bool chassis_init(void);

#endif
