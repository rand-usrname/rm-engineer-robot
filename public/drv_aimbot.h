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
//��Ϊ�����Ľӿ�
typedef struct _Aimbot_t
{
	float pitch;
	float yaw;
}Aimbot_t;

/***********���Ӿ�ͨ�ŵĲ���************/


typedef enum
{
    RUNE		= 0,//��糵
    BIG_ARMOR	= 1,//��װ�װ�
    SMALL_ARMOR = 2,//Сװ�װ�
	BASE_ARMOR	= 3,//����װ��
	ENG_ARMOE	= 4,//����װ��
	CAISSON		= 5,//��ҩ��

	BLANK		= 0X7//��ֵ

}aim_mode_e;
//���Ŀ��

typedef enum
{
	RED		= 0,
    BLUE	= 1

}tarcolor_e;
//Ŀ����ɫ

typedef enum
{
    COMPUTER	= 0,
    SIGNALCHIP	= 1

}forecast_e;
//Ԥ����

typedef struct
{
	aim_mode_e	aim_mode;		//���Ŀ��ID
	forecast_e	forcester;		//������Ԥ����
	tarcolor_e	tracolor;		//������Ŀ����ɫ

}visual_ctl_t;
//���͵Ŀ�����Ϣ�洢����

typedef struct
{
	aim_mode_e	aim_mode;		//���Ŀ��ID
	forecast_e	forcester;		//ʵ�ʵ�Ԥ����
	int			computime;		//����ʱ��
	rt_int16_t	yawadd;			//-32768 ~ 32767 �궨����
	rt_int16_t	pitchadd;		//-32768 ~ 32767 �궨�Ƕ�
	rt_int16_t	x,y,z;			//�������꣬��λ��Ϊ mm/s
	rt_uint8_t	yaw_usetime;	//���ʹ�ô���
	rt_uint8_t	pitch_usetime;	//���ʹ�ô���

}visual_rev_t;
//���յ������ݽṹ��
 
//����Ҫ��ĳ�����òſ���ʹ�õĺ���
extern int vision_init(void);
//Ĭ�� can2 �豸������ID VISUAL_REVID = 0X302
extern int refresh_visual_data(rt_uint8_t* data);

//ģʽ����
extern void tarcolor_set(tarcolor_e tarcolor);
extern void forecast_set(forecast_e forecast);
extern void aim_mode_set(aim_mode_e aim_mode);

//���Ӿ����Ϳ�����Ϣ��Ĭ��ʹ�� CAN2 �豸
extern int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel);
extern int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel);

//ʹ���Ӿ��������ݵ��õĺ���
extern rt_int16_t get_yaw_add(void);
extern rt_int16_t get_pitch_add(void);
extern rt_int16_t get_yawusetime(void);
extern rt_int16_t get_pitchusetime(void);

/***********���Ӿ�ͨ�ŵĲ��ֽ���************/


#endif
