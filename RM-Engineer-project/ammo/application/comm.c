#include "comm.h"

gimbal_data_t gimbal_ctrl_data;

rt_bool_t read_gimbal_data(struct rt_can_msg* msg)
{
	if(msg->id == 0x101)
	{
		return RT_FALSE;
	}
	else
	{
		rt_memcpy(&gimbal_ctrl_data,&msg->data,8);
		return RT_TRUE;
	}
}
void gimbal_ctrl_init(void)
{
	gimbal_ctrl_data.raise_cmd = 0;
	gimbal_ctrl_data.auto_ammo_one = 0;
	gimbal_ctrl_data.auto_ammo_three = 0;
	gimbal_ctrl_data.magazine_turn = 0;
	gimbal_ctrl_data.Reserved = 0;
}

