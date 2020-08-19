#ifndef __AMMO_H
#define __AMMO_H
#include <rtthread.h>
#include <drv_motor.h>
#include <board.h>
#include "robodata.h"
#include "magazine.h"
#define RAISE_ANGLE     //RAISE_SPEED/RAISE_ANGLE ����̧�������Ƿ�ʹ���ٶȻ�/�ǶȻ�
#define AMMO_RADIO       //AMMO_STEP/AMMO_RADIO    ������еצͨ�����ٱ���ֱ�����ýǶȻ��ǲ���ʽ�����Ƕ�

#define RAISE_PRE_POSITION  26636
#define RAISE_MOST_POSITION 28000

void raise_angle_set(rt_int8_t step);
void grap_angle_set(rt_int8_t step);
void raise_speed_set(rt_int16_t speed);
void grap_radio_angle_set(rt_int16_t angle);
void Ammo_init(void);
extern Motor_t Raise[2];
extern Motor_t Grap[2];
#endif
