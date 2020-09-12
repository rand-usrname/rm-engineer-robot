#include "drv_canthread.h"
#include "robodata.h"
#include "comm.h"
#include "magazine.h"
#include "ammo.h"
void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
		case UPLIFT_LEFT:
			motor_readmsg(msg,&Raise[0].dji);
		break;
		case UPLIFT_RIGHT:
			motor_readmsg(msg,&Raise[1].dji);
		break;
		case GRAP_LEFT:
			motor_readmsg(msg,&Grap[0].dji);
		break;
		case GRAP_RIGHT:
			motor_readmsg(msg,&Grap[1].dji);
		break;
		case FILTER_2006:
			motor_readmsg(msg,&m_magazine.dji);
		break;
        //Ä¬ÈÏreturn
        default:
            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
		case GIMBAL_ID:
			read_gimbal_data(msg);
		    return;
		default:
			return;
    }
}
