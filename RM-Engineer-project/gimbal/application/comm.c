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

rt_size_t Send_chassis_data(rt_device_t dev)
{
	struct rt_can_msg txmsg;
	txmsg.id=CHASSIS_CTL;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0] = (rt_int8_t)(chassis_ctrl_data.x_speed >> 8);
	txmsg.data[1] = (rt_int8_t)(chassis_ctrl_data.x_speed);
	txmsg.data[2] = (rt_int8_t)(chassis_ctrl_data.y_speed >> 8);
	txmsg.data[3] = (rt_int8_t)(chassis_ctrl_data.y_speed);
	txmsg.data[4] = chassis_ctrl_data.chassis_ctrl;
	if(chassis_ctrl_data.chassis_ctrl == 1)
	{
		txmsg.data[5] = (rt_uint8_t)(chassis_ctrl_data.follow_angle >> 8);
		txmsg.data[6] = (rt_uint8_t)(chassis_ctrl_data.follow_angle);
	}
	else if(chassis_ctrl_data.chassis_ctrl == 0)
	{
		txmsg.data[5] = (rt_int8_t)(chassis_ctrl_data.angular_velocity >> 8);
		txmsg.data[6] = (rt_int8_t)(chassis_ctrl_data.angular_velocity);
	}
	txmsg.data[7] = chassis_ctrl_data.rescue_cmd;
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}
