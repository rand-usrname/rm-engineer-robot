#ifndef __ROBOCONTROL_H__
#define __ROBOCONTROL_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_gimbal.h"
#include "drv_remote.h"
#include "drv_aimbot.h"
#include "drv_strike.h"

extern int remote_ctrl(RC_Ctrl_t *remote);

#endif
