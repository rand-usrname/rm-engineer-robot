#include "can_receive.h"

void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取底盘数据
        case LEFT_FRONT:
        case RIGHT_FRONT:
        case LEFT_BACK:
        case RIGHT_BACK:
            refresh_chassis_motor_data(msg);
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
        case YAW_ID:
        case PITCH_ID:
            refresh_gimbal_motor_data(msg);
            return;
        case CHASSIS_CTL:
            refresh_ctldata(msg);
            return;
    }
}
