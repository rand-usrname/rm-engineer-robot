#include "drv_canthread.h"
#include "robodata.h"
#include "drv_motor.h"
#include "drv_strike.h"
#include "drv_gyro.h"
#include "task_gimbal.h"
#include "can_receive.h"

Motor_t m_yaw={0};/*5901-185-6417-3090-4095*/ //ratio=0.285->1681-2387-4163-5550-5836*/
Motor_t m_pitch={0};

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取电机数据
        case Pitch_ID:
			motor_readmsg(msg,&m_pitch.dji);
			return;
        case Yaw_ID:
			motor_readmsg(msg,&m_yaw.dji);
            return;

        //读取陀螺仪数据
        case GYRO_ANGLE_ID:
            gyro_read_angle(msg);
            return;
        case GYRO_SPEED_ID:
            gyro_read_speed(msg);
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
        case Launch_ID:
			motor_readmsg(msg,&m_launch.dji);
            return;
    }
}
