#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //¶ÁÈ¡ÍÓÂÝÒÇÊý¾Ý
        case GYRO_ANGLE_ID:
            gyro_read_angle(msg);
            return;
        case GYRO_SPEED_ID:
            gyro_read_speed(msg);
            return;

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

        default:

            return;
    }
}
