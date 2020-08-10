#ifndef __DRV_MOTOR_H
#define __DRV_MOTOR_H

#include <rtthread.h>
#include <rtdevice.h>
#include "pid.h"
#include "robodata.h"
#define ABS(x)						((x)<0?-(x):(x))
/* 电机报文发送ID */
typedef enum 
{
	STDID_Pantilt = 0x1FF,
	STDID_launch = 0x200
}SendID_e;


typedef struct
{
	rt_int16_t set_angle;                                       /* 设定角度 */
	rt_int16_t set_speed;                                       /* 设定速度 */
	rt_uint16_t angle;											 /*角度 0~8191*/
	rt_int16_t speed;                                            /* 角速度 */
	rt_int16_t current;                                          /* 电流 */ 
	rt_uint8_t temperature;                                      /* 温度 */
    rt_uint16_t old_angle;										 /*上一次编码器数值*/
	rt_uint32_t loop;											 /*一共所转圈数*/
	float ratio;												 /*减速箱的减速比<=1*/
	rt_uint32_t motorID;										 /*电机反馈报文ID*/
	
	pid_t spe;                                                   /* 速度环 */
	pid_t ang;                                                   /* 角度环 */
}Motor_t;


void motor_readmsg(struct rt_can_msg* rxmsg,Motor_t* motor);							/*读取can中的电机数据*/
rt_size_t motor_current_send(rt_device_t dev, 											/*电机电流发送*/
														SendID_e   ID,
														rt_int16_t setcur1, 
														rt_int16_t setcur2, 
														rt_int16_t setcur3, 
														rt_int16_t setcur4);

void motor_init(Motor_t *motor,rt_uint32_t ID,float radio);
void motor_speed_set(Motor_t *motor,int speed);
void motor_angle_set(Motor_t *motor,int angle);
rt_int16_t motor_angle_judge(Motor_t *motor);
#endif
