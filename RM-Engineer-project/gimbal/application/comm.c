#include "comm.h"

ammo_data_t ammo_ctrl_data;
chassis_data_t chassis_ctrl_data;

rt_size_t Send_ammo_data(rt_device_t dev)
{
	struct rt_can_msg txmsg;
	txmsg.id=AMMO_CTL;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	rt_memcpy(&txmsg.data,&ammo_ctrl_data,8);
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}

rt_size_t Send_chassis_data(rt_device_t dev,chassis_data_t *chassis_ctrl_data)
{
	struct rt_can_msg txmsg;
	txmsg.id=CHASSIS_CTL;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0] = (rt_int8_t)(chassis_ctrl_data->x_speed >> 8);
	txmsg.data[1] = (rt_int8_t)(chassis_ctrl_data->x_speed);
	txmsg.data[2] = (rt_int8_t)(chassis_ctrl_data->y_speed >> 8);
	txmsg.data[3] = (rt_int8_t)(chassis_ctrl_data->y_speed);
	txmsg.data[4] = chassis_ctrl_data->chassis_ctrl;
	if(chassis_ctrl_data->chassis_ctrl == 1)
	{
		txmsg.data[5] = (rt_uint8_t)(chassis_ctrl_data->follow_angle >> 8);
		txmsg.data[6] = (rt_uint8_t)(chassis_ctrl_data->follow_angle);
	}
	else if(chassis_ctrl_data->chassis_ctrl == 0)
	{
		txmsg.data[5] = 0;
		txmsg.data[6] = 0;
	}
	txmsg.data[7] = chassis_ctrl_data->rescue_cmd;
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}
void Comm_Deinit(void)
{
	chassis_ctrl_data.angular_velocity = 0;
	chassis_ctrl_data.chassis_ctrl = 0;
	chassis_ctrl_data.follow_angle = 0;
	chassis_ctrl_data.x_speed = 0;
	chassis_ctrl_data.y_speed = 0;
	chassis_ctrl_data.rescue_cmd = 0;
	
	ammo_ctrl_data.auto_ammo_one = 0;
	ammo_ctrl_data.auto_ammo_three = 0;
	ammo_ctrl_data.magazine_turn = 0;
	ammo_ctrl_data.raise_cmd = 0;
	ammo_ctrl_data.Reserved = 0;
}
