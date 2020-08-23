#include "drv_canthread.h"
#include "robodata.h"
#include "drv_gimbal.h"
#include "drv_gyro.h"
#include "drv_aimbot.h"
#include "drv_strike.h"

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
		
		
        default:

            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
		//�Ӿ�ͨ�����ݽ���ID
		case VISUAL_REVID:
			refresh_visual_data(msg->data);
		return;
		//������̨�������
		case YAW_ID:
        case PITCH_ID:
            refresh_gimbal_motor_data(msg);
            return;
		case 0x201:
			motor_readmsg(msg,&m_launch.dji);
        default:

            return;
    }
}
