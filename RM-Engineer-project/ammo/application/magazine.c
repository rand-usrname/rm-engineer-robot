#include "magazine.h"

/* 这个电机的pid将在抬升及取弹机构中一起计算 */
Motor_t m_magazine;


void magazine_init(void)
{
	motor_init(&m_magazine,FILTER_2006,0.027973);
	/* pid 初始化 */
	pid_init(&m_magazine.ang,3,0,0,300,8000,-8000);
	pid_init(&m_magazine.spe,8,0,0,300,12000,-12000);
}

void magazine_big_turn(void)
{
	/* 更新角度 */
	rt_int32_t current_angle = m_magazine.dji.angle;
	/* 设定角度 */
	motor_angle_set(&m_magazine,maga_end_angle-current_angle);
}
void magazine_small_turn(void)
{
	/* 更新角度 */
	rt_int32_t current_angle = m_magazine.dji.angle;
	/* 设定角度 */
	motor_angle_set(&m_magazine,maga_start_angle-current_angle);
}

