/**************************************************************************************************
 * @fileName     comm.c                                                                           *
 * @version      1.0                                                                              *
 * @date         2020 08 18                                                                       *
 *                                                                                                *
 * @brief        to communicate with gimbal and ammo device                                       *
 *                                                                                                *
 * @copyright  . All rights reserrved                                                             *
 **************************************************************************************************/
#include "comm.h"

gimbal_data_t gimbal_ctrl_data;


rt_bool_t read_gimbal_data(struct rt_can_msg* msg)
{
	if(msg->id != 0x100){
		return RT_FALSE;
	}
	else{
		/* 处理云台数据 */
		gimbal_ctrl_data.x_speed = (msg->data[0] << 8) + msg->data[1];
		gimbal_ctrl_data.y_speed = (msg->data[2] << 8) + msg->data[3];
		gimbal_ctrl_data.chassis_ctrl = msg->data[4];
		if(gimbal_ctrl_data.chassis_ctrl==1)
		{
			gimbal_ctrl_data.follow_angle = (msg->data[5]<<8) + msg->data[6];
		}
		else if (gimbal_ctrl_data.chassis_ctrl == 0)
		{
			gimbal_ctrl_data.angular_velocity = (msg->data[5]<<8) + msg->data[6];
		}
		gimbal_ctrl_data.rescue_cmd = msg->data[7];
		return RT_TRUE;
	}
}
void gimbal_ctrl_init(void)
{
	gimbal_ctrl_data.x_speed = 0;
	gimbal_ctrl_data.y_speed = 0;
	gimbal_ctrl_data.angular_velocity = 0;
	gimbal_ctrl_data.chassis_ctrl = Follow;
	gimbal_ctrl_data.follow_angle = 0;
	gimbal_ctrl_data.rescue_cmd = 0x01;
}
