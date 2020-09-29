#ifndef __COMM_H
#define __COMM_H


#include "comm.h"

#include <rtthread.h>
#include <rtdevice.h>
#include "robodata.h"
typedef struct __ammo_data{
	rt_uint8_t   raise_cmd;         /* ̧����ȡ��λ�� */
	rt_uint8_t   auto_ammo_one;     /* �����Զ�ȡ�� */
	rt_uint8_t   auto_ammo_three;   /* �����Զ�ȡ�� */
	rt_uint8_t   magazine_turn;     /* ����ת�� */
	rt_uint32_t  Reserved;          /* ����λ */ 
}ammo_data_t;

typedef enum {
	No_Follow     = 0,        /* ������ */
	Follow        = 1,        /* ���� */
	visual_ctrl   = 2,        /* �Ӿ����� */
}chassis_mode_e;

typedef struct __chassis_data{
	rt_int16_t         x_speed;            
	rt_int16_t         y_speed;
	chassis_mode_e     chassis_ctrl;
	rt_uint16_t        follow_angle;             /* ����Ƕ� */
	rt_int16_t         angular_velocity;         /* С����ת��(���ڲ�����ģʽ���ã� */
	rt_uint8_t         rescue_cmd;               /* �ϳ�����״̬ */
}chassis_data_t;


extern ammo_data_t ammo_ctrl_data;
extern chassis_data_t chassis_ctrl_data;

rt_size_t Send_ammo_data(rt_device_t dev);
rt_size_t Send_chassis_data(rt_device_t dev,chassis_data_t *chassis_ctrl_data);

void Comm_Deinit(void);
#endif

