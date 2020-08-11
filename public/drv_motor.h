#ifndef __DRV_MOTOR_H
#define __DRV_MOTOR_H

#include <rtthread.h>
#include <rtdevice.h>
#include "pid.h"
#include "robodata.h"
#define ABS(x)						((x)<0?-(x):(x))
/* ������ķ���ID */
typedef enum 
{
	STDID_Pantilt = 0x1FF,
	STDID_launch = 0x200
}SendID_e;


typedef struct
{
	rt_int16_t set_angle;                                       /* �趨�Ƕ� */
	rt_int16_t set_speed;                                       /* �趨�ٶ� */
	rt_uint16_t angle;											 /*�Ƕ� 0~8191*/
	rt_int16_t speed;                                            /* ���ٶ� */
	rt_int16_t current;                                          /* ���� */ 
	rt_uint8_t temperature;                                      /* �¶� */
    rt_uint16_t old_angle;										 /*��һ�α�������ֵ*/
	rt_uint32_t loop;											 /*һ����תȦ��*/
	float ratio;												 /*������ļ��ٱ�<=1*/
	rt_uint32_t motorID;										 /*�����������ID*/
	
	pid_t spe;                                                   /* �ٶȻ� */
	pid_t ang;                                                   /* �ǶȻ� */
}Motor_t;


void motor_readmsg(struct rt_can_msg* rxmsg,Motor_t* motor);							/*��ȡcan�еĵ������*/
rt_size_t motor_current_send(rt_device_t dev, 											/*�����������*/
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
