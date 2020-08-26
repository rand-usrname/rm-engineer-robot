#ifndef __CAN_RECEIVE_H__
#define __CAN_RECEIVE_H__

#include "drv_canthread.h"
#include "drv_motor.h"
#include "robodata.h"

/*电机ID（can消息接收ID）*/
typedef enum 
{
	Yaw_ID = 0x205,		//
	Pitch_ID = 0x208,
	Launch_ID = 0x201,
}RecID_e;

extern Motor_t m_yaw;
extern Motor_t m_pitch;

#endif
