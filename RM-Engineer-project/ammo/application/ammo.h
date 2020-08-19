#ifndef __AMMO_H
#define __AMMO_H
#include <rtthread.h>
#include <drv_motor.h>
#include <board.h>
#include "robodata.h"




void raise_angle_set(rt_int16_t angle,rt_uint8_t step);
void grap_angle_set(rt_int16_t angle,rt_uint8_t step);
void raise_speed_set(rt_int16_t speed);
void grap_radio_angle_set(rt_int16_t angle);
#endif
