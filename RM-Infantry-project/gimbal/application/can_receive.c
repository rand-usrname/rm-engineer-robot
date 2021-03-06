#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"
#include "drv_gyro.h"
#include "drv_aimbot.h"
#include "drv_strike.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取陀螺仪数据
        case GYRO_ANGLE_ID:
            gyro_read_angle(msg);
            return;
        case GYRO_SPEED_ID:
            gyro_read_speed(msg);
            return;
		
		//更新云台电机数据
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
		//视觉通信数据接收ID
		case VISUAL_REVID:
			refresh_visual_data(msg->data);
		return;
		case RUB0_ID:
			motor_readmsg(msg,&m_rub[0].dji);
		return;
		case RUB1_ID:
			motor_readmsg(msg,&m_rub[1].dji);
		return;
		case LAUNCH_ID:
			motor_readmsg(msg,&m_launch.dji);
			return;
        default:

            return;
    }
}
