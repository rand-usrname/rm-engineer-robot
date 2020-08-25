#include "drv_canthread.h"
#include "robodata.h"
#include "drv_sentrychas.h"
#include "drv_gyro.h"
#include "drv_motor.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取底盘电机数据
        case LEFT:
			motor_readmsg(msg,&m_chassis_3508[0].dji);
			return;
        case RIGHT:
			motor_readmsg(msg,&m_chassis_3508[1].dji);
            return;

        //读取陀螺仪数据
//        case GYRO_ANGLE_ID:
//            gyro_read_angle(msg);
//            return;
//        case GYRO_SPEED_ID:
//            gyro_read_speed(msg);
//            return;
//        
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
            //refresh_gimbal_motor_data(msg);
            return;
        case CHASSIS_CTL:

            return;
    }
}
