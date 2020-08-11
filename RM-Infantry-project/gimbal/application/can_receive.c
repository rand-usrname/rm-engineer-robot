#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
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
            
            return;
        case CHASSIS_CTL:

            return;
    }
}
