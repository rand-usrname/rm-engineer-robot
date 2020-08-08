#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        case LEFT_FRONT:
        case RIGHT_FRONT:
        case LEFT_BACK:
        case RIGHT_BACK:
            refresh_chassis_motor_data(msg);
            return;
        default:

        return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        case YAW:
        case PITCH:
            refresh_yuntai_motor_data(msg);
            return;
        case CHASSIS_CTL:

            return;
    }
}
