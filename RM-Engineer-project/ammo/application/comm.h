#ifndef __COMM_H
#define __COMM_H

#include <rtthread.h>
#include <rtdevice.h>
typedef struct __gimbal_data{
	rt_uint8_t   raise_cmd;         /* ̧����ȡ��λ�� */
	rt_uint8_t   auto_ammo_one;     /* �����Զ�ȡ�� */
	rt_uint8_t   auto_ammo_three;   /* �����Զ�ȡ�� */
	rt_uint8_t   magazine_turn;     /* ����ת�� */
	rt_uint32_t  Reserved;          /* ����λ */ 
}gimbal_data_t;



extern gimbal_data_t gimbal_ctrl_data;

rt_bool_t read_gimbal_data(struct rt_can_msg* msg);
void gimbal_ctrl_init(void);





#endif

