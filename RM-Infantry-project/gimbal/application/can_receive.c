#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"
#include "drv_gyro.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //��ȡ����������
        case GYRO_ANGLE_ID:
            gyro_read_angle(msg);
            return;
        case GYRO_SPEED_ID:
            gyro_read_speed(msg);
            return;
		
		//������̨�������
		case YAW_ID:
        case PITCH_ID:
            refresh_gimbal_motor_data(msg);
            return;
		
        default:

            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {


        default:

            return;
    }
}
