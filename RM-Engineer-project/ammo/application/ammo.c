#include "ammo.h"

Motor_t Raise[2];
Motor_t Grap[2];
void Ammo_init(void)
{
	motor_init(&Raise[0],UPLIFT_LEFT,1);
	motor_init(&Raise[1],UPLIFT_RIGHT,1);
	motor_init(&Grap[0],GRAP_LEFT,1);
	motor_init(&Grap[1],GRAP_RIGHT,1);
	/* PID 初始化 */
	/* ...... */
}
/*********************对于步进形式的抬升和机械爪的接口**********************/
/**
  * @brief  设置抬升机构的角度
  * @param  angle 抬升机构角度设置
  * @param  step  抬升每10ms的步进
  * @retval none
 **/
void raise_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Raise[0],step);
	motor_angle_set(&Raise[1],-step);
}

/**
  * @brief  设置取弹机构的角度
  * @param  angle 取弹机械爪角度设置
  * @param  step  取弹每10ms的步进
  * @retval none
 **/
void grap_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Grap[0],step);
	motor_angle_set(&Grap[1],-step);
}
/******************如果抬升机构采取速度环*****************/
void raise_speed_set(rt_int16_t speed)
{
	Raise[0].spe.set = speed;
	Raise[1].spe.set = -speed;
}

/**************设定输入减速比之后的机械爪的角度设定****************/
void grap_radio_angle_set(rt_int16_t angle)
{
	motor_angle_set(&Grap[0],angle);
	motor_angle_set(&Grap[1],-angle);
}
/* 定义定时器来计算PID */


