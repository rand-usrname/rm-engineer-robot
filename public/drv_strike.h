#ifndef __DRV_STRIKE_H__
#define __DRV_STRIKE_H__
#include <rtthread.h>
#include "drv_motor.h"
#include "drv_refsystem.h"
#include "drv_remote.h"
#include "drv_canthread.h"

#define LOCAL_HEAT_ENABLE		0					            /*��������ʹ��*/


#define HEAT_PERIOD		        100								/*����������ڣ���λms*/
/*����ģʽ*/
#define STRICK_NOLIMITE			0x01							/*�����ƿ���*/
#define	STRICK_SINGLE           0x02							/*����*/
#define	STRICK_TRIPLE           0x04							/*������*/

#define	STRICK_LOWSPEED 		0x08							/*������*/
#define	STRICK_MIDDLESPEED	    0x10							/*������*/
#define	STRICK_HIGHTSPEED       0x20							/*������*/
/*����״̬*/
#define STRICK_STUCK		    0x01							/*����*/
#define STRICK_STOP				0x02							/*��ֹ����*/
/*�������ƽṹ��*/
typedef struct _Heatctrl_t
{
	rt_int32_t 					now;							/*����ʣ������*/
	rt_uint32_t 				max;							/*��������*/
	rt_uint8_t 					rate;							/*��ǰʣ�������ٷֱ�*/
	struct _Heatctrl_t *next;							        /*�¸��������ƿ�*/

	/*���ʹ�ܱ�����������*/
	#if LOCAL_HEAT_ENABLE
	rt_uint8_t 				  cool;							    /*ǹ����ȴֵ*/
	rt_uint8_t    			buff;							    /*ǹ����ȴbuff����ǹ����ȴ������*/
	#endif
}Heatctrl_t;


/*��������ṹ��*/
typedef struct
{
	rt_int16_t    speed;										    /*Ħ�����ٶ�*/
	rt_uint8_t    mode;											/*���ģʽ������䣬3��������֮����Ҫ�ĳ�ö�٣�*/
	rt_uint8_t	  status;										/*״̬�����ٶȻ�������λ�ÿ���*/
	Heatctrl_t 	  heat;										/*ǹ���������ƿ�*/
}Strike_t;

extern Strike_t gun1;
extern Motor_t m_rub[2];
extern Motor_t m_launch;
/*�������ƺ��������û�����ʵ�ʱ�д*/
void heat_control(Heatctrl_t *p_temp);
/*������������*/
void heatctrl_init(Heatctrl_t *heat, rt_uint32_t max);
/*��ʼ��������*/
void heatctrl_start(void);
void motor_servo_set(uint16_t duty);
void Gun_speed_set(Strike_t *strike, rt_int16_t speed);
void Gun_mode_set(Strike_t *strike, rt_base_t mode);
/*���������ʼ��*/
void strike_init(Strike_t *gun, rt_uint32_t max);
void strike_start(void);
/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/*�����ж�*/
void strike_stuck(Motor_t *motor, Strike_t *gun);
/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/*����ģʽ������*/
void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire);
#endif
