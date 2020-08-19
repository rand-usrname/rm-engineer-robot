#include "magazine.h"

/* ��������pid����̧����ȡ��������һ����� */
Motor_t m_magazine;


void magazine_init(void)
{
	motor_init(&m_magazine,FILTER_2006,0.027973);
	/* pid ��ʼ�� */
	pid_init(&m_magazine.ang,3,0,0,300,8000,-8000);
	pid_init(&m_magazine.spe,8,0,0,300,12000,-12000);
}

void magazine_big_turn(void)
{
	/* ���½Ƕ� */
	rt_int32_t current_angle = m_magazine.dji.angle;
	/* �趨�Ƕ� */
	motor_angle_set(&m_magazine,maga_end_angle-current_angle);
}
void magazine_small_turn(void)
{
	/* ���½Ƕ� */
	rt_int32_t current_angle = m_magazine.dji.angle;
	/* �趨�Ƕ� */
	motor_angle_set(&m_magazine,maga_start_angle-current_angle);
}

