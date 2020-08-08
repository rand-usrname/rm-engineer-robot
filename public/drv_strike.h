#ifndef __DRV_STRIKE_H__
#define __DRV_STRIKE_H__
#include <rtthread.h>
#include "ctrl_motor.h"


#define HEAT_PERIOD											100				/*����������ڣ���λms*/
/*����ģʽ*/
#define STRICK_NOLIMITE	    0x01								        /*�����ƿ���*/
#define	STRICK_SINGLE       0x02										/*����*/
#define	STRICK_TRIPLE       0x04										/*������*/

#define	STRICK_LOWSPEED 	0x08										/*������*/
#define	STRICK_MIDDLESPEED	0x10										/*������*/
#define	STRICK_HIGHTSPEED   0x20										/*������*/
/*����״̬*/
#define STRICK_STUCK				0x01								/*����*/
#define STRICK_STOP					0x02								/*��ֹ����*/


/*�������ƽṹ��*/
typedef struct _Heatctrl_t
{
	rt_int32_t 					now;							/*����ʣ������*/
	rt_uint32_t 				max;							/*��������*/
	rt_uint8_t 					rate;							/*��ǰʣ�������ٷֱ�*/
}Heatctrl_t;


/*��������ṹ��*/
typedef struct
{
	rt_uint16_t   speed;										/*Ħ�����ٶ�*/
	rt_uint8_t    mode;											/*���ģʽ������䣬3��������֮����Ҫ�ĳ�ö�٣�*/
	rt_uint8_t	  status;										/*״̬�����ٶȻ�������λ�ÿ���*/
	Heatctrl_t 	  heat;											/*ǹ���������ƿ�*/
}Strike_t;

/*���������ʼ��*/
void strike_init(Strike_t *gun,rt_base_t mode);
/*�����ж�*/
void strike_stuck(struct Motor_t *motor, Strike_t *gun);
/*����ģʽ������*/
void strike_fire(struct Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire);
#endif
