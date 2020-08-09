#ifndef __DRV_STRIKE_H__
#define __DRV_STRIKE_H__
#include <rtthread.h>
#include "ctrl_motor.h"


#define HEAT_PERIOD											100				/*������������ 100ms*/
/*����ģʽ*/
#define STRICK_NOLIMITE	    0x01								        /*ȫ�Զ�*/
#define	STRICK_SINGLE       0x02										/*����*/
#define	STRICK_TRIPLE       0x04										/*������*/

#define	STRICK_LOWSPEED 	0x08										/*����*/
#define	STRICK_MIDDLESPEED	0x10										/*����*/
#define	STRICK_HIGHTSPEED   0x20										/*����*/

#define STRICK_STUCK				0x01								/*����*/
#define STRICK_STOP					0x02								/*ֹͣ����*/


/*����ģ��*/
typedef struct _Heatctrl_t
{
	rt_int32_t 					now;							/*��ǰ����*/
	rt_uint32_t 				max;							/*��������*/
	rt_uint8_t 					rate;							/*ʣ�������ٷֱ�*/
}Heatctrl_t;


/*����ģ��*/
typedef struct
{
	rt_uint16_t   speed;										/*�����ٶ� ����m/s*/
	rt_uint8_t    mode;											/*����ģʽ*/
	rt_uint8_t	  status;										/*����״̬*/
	Heatctrl_t 	  heat;											/*����ģ��*/
}Strike_t;

/*�ӵ��ٶ��趨*/
void Gun_speed_set(struct Motor_t *motor,Robodata_t *Robodata,Strike_t *strike,rt_base_t mode)
/*�����ж�*/
void strike_stuck(struct Motor_t *motor, Strike_t *gun);
/*��������*/
void strike_fire(struct Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire);
#endif
