#ifndef __DRV_AIM_BOT__H
#define __DRV_AIM_BOT__H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_canthread.h"
#include "robodata.h"

typedef struct _Point_t
{
	float x;  /* ��Ŀ��ˮƽ���� */
	float y;  /* ��Ŀ��߶Ȳ� */
	float z;  /* ��Ŀ�괹ֱ���� */
}Point_t;
//��Ϊ�����Ľӿ�
typedef struct _Aimbot_t
{
	float pitch_add;
	float yaw_add;
}Aimbot_t;

/***********���Ӿ�ͨ�ŵĲ���************/

/* ����ģʽ */
typedef enum
{
	NORMAL_AIM         = 0x01,    /* �Ӿ������� */
	SMALL_WINDWILL     = 0x02,    /* С�������� */
	BIG_WINDWILL       = 0x03,    /* ���������� */
	ENGINEER_AMMO      = 0x04,    /* ����ȡ�� */
	LOB_SHOT           = 0x05,    /* Ӣ�۵��� */
	ELEC_AIM           = 0x06,    /* ��ض����� */
	
}aim_mode_e;

/* Ŀ����ɫ */
typedef enum
{
	RED		= 0xA0,
    BLUE	= 0xB0

}tarcolor_e;

/* ����֡ͷ */
typedef struct
{
	aim_mode_e	aim_mode;	
	tarcolor_e	tracolor;		
}visual_head_t;

typedef struct
{
	rt_int16_t	yawadd;			//-32768 ~ 32767 �궨����
	rt_int16_t	pitchadd;		//-32768 ~ 32767 �궨�Ƕ�
	rt_uint8_t	yaw_usetime;	//���ʹ�ô���
	rt_uint8_t	pitch_usetime;	//���ʹ�ô���
	Point_t	    visual_point;			//�������꣬��λ��Ϊ m
	int			computime;		//����ʱ��

}visual_rev_t;
//���յ������ݽṹ��
 
//����Ҫ��ĳ�����òſ���ʹ�õĺ���
extern int vision_init(void);
//Ĭ�� can2 �豸������ID VISUAL_REVID = 0X302
extern int refresh_visual_data(rt_uint8_t* data);

//ģʽ����
extern void tarcolor_set(tarcolor_e tarcolor);
extern void aim_mode_set(aim_mode_e aim_mode);

//���Ӿ����Ϳ�����Ϣ��Ĭ��ʹ�� CAN2 �豸
extern int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,float bullet_vel);
extern int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,float bullet_vel);

//ʹ���Ӿ��������ݵ��õĺ���
extern rt_int16_t get_yaw_add(void);
extern rt_int16_t get_pitch_add(void);
extern rt_int16_t get_yawusetime(void);
extern rt_int16_t get_pitchusetime(void);

/***********���Ӿ�ͨ�ŵĲ��ֽ���************/


#endif
