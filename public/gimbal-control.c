#include "gimbal-control.h"


/**
* @brief：初始化云台线程
* @param [in]	无
* @return：		1:初始化成功
				0:初始化失败
* @author：mqy
*/
int gimbal_init(void)
{
	
}

/**
* @brief：利用该函数更新陀螺仪和云台电机的转速等数据
* @param [in]	message:接收到的数据帧指针
* @return：		1:更新成功
				0:id不匹配更新失败
* @author：mqy
*/
int refresh_device_data(struct rt_can_msg* message)
{
	//其他数据
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&motion_data.yaw_data,message);
			//转换角度数值的坐标系
			if(motion_data.yaw_data.angle < YAW_START_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAW_START_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAW_START_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&motion_data.pitch_data,message);
			return 1;
			
		default:
			
			
			break;
	}
	return 0;
}
