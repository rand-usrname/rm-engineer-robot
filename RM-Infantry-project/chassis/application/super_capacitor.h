#ifndef __SUPER_CAPACITOR_H__
#define __SUPER_CAPACITOR_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "stdbool.h"

#define SUPER_CAPACITOR_MESSAGE_ID 0x019

extern bool refresh_super_capacitor_data(struct rt_can_msg* message);
extern rt_uint16_t 	access_electricity(void);

#endif
