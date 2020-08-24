#ifndef __CAN_RECEIVE_H__
#define __CAN_RECEIVE_H__

#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"
#include "drv_gyro.h"

extern int refresh_ctldata(struct rt_can_msg* msg);

#endif
