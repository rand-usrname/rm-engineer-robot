#include "ammo.h"

Motor_t Raise[2];
Motor_t Grap[2];
void Ammo_init(void)
{
	motor_init(&Raise[0],UPLIFT_LEFT,1);
	motor_init(&Raise[1],UPLIFT_RIGHT,1);
	motor_init(&Grap[0],GRAP_LEFT,1);
	motor_init(&Grap[1],GRAP_RIGHT,1);
	/* PID ��ʼ�� */
	/* ...... */
}
/*********************���ڲ�����ʽ��̧���ͻ�еצ�Ľӿ�**********************/
/**
  * @brief  ����̧�������ĽǶ�
  * @param  angle ̧�������Ƕ�����
  * @param  step  ̧��ÿ10ms�Ĳ���
  * @retval none
 **/
void raise_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Raise[0],step);
	motor_angle_set(&Raise[1],-step);
}

/**
  * @brief  ����ȡ�������ĽǶ�
  * @param  angle ȡ����еצ�Ƕ�����
  * @param  step  ȡ��ÿ10ms�Ĳ���
  * @retval none
 **/
void grap_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Grap[0],step);
	motor_angle_set(&Grap[1],-step);
}
/******************���̧��������ȡ�ٶȻ�*****************/
void raise_speed_set(rt_int16_t speed)
{
	Raise[0].spe.set = speed;
	Raise[1].spe.set = -speed;
}

/**************�趨������ٱ�֮��Ļ�еצ�ĽǶ��趨****************/
void grap_radio_angle_set(rt_int16_t angle)
{
	motor_angle_set(&Grap[0],angle);
	motor_angle_set(&Grap[1],-angle);
}
/* ���嶨ʱ��������PID */


