#include "drv_canthread.h"
#include "robodata.h"
#include "comm.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //Ä¬ÈÏreturn
        default:
            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
		case 0x101:
			read_gimbal_data(msg);
		    return;
		default:
			return;
    }
}
