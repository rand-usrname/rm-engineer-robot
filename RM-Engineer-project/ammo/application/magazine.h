#ifndef __MAGAZINE_H
#define __MAGAZINE_H

#include <rtthread.h>
#include <drv_motor.h>
#include <board.h>
#include "robodata.h"

#define maga_start_angle    10
#define maga_end_angle      200
void magazine_init(void);
void magazine_big_turn(void);
void magazine_small_turn(void);








#endif
