#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"
#include "drv_gyro.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取底盘数据
        case LEFT_FRONT:
        case RIGHT_FRONT:
        case LEFT_BACK:
        case RIGHT_BACK:
            refresh_chassis_motor_data(msg);
            return;
        //默认return
        default:
            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        case YAW_ID:
        case PITCH_ID:
            refresh_gimbal_motor_data(msg);
            return;
        case CHASSIS_CTL:
			read_gimbal_data(msg);
            return;
    }
}
