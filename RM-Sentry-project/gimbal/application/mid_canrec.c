#include "drv_canthread.h"
#include "robodata.h"
#include "drv_motor.h"
#include "drv_strike.h"
#include "drv_gyro.h"
#include "task_gimbal.h"
#include "mid_canrec.h"
#include "mid_gimbal.h"



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
        //发弹6020
        case Launch_ID:
			motor_readmsg(msg,&m_launch.dji);
            return;
        
        //与底盘通信    
        case STDID_CHASSIS:
            read_chassis_data(msg);
            return;
    }
}
