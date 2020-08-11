#include "drv_gyro.h"

//float x_speed;		//??????,???????
//float y_speed;
//float z_speed;

//float yaw_speed;
//float pitch_speed;

Gyrodata_t Gryo_data;
/**

 * @brief  ID:101,????????????

 * @param rxmsg??can??????? gyro??????????????

 */
void gyro_read_angle(struct rt_can_msg* rxmsg)
{
	Gryo_data.angle_ready = rxmsg->data[7];
	Gryo_data.pitch = ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;// pitch    -90----90,???????,??��:??

	Gryo_data.yaw 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;// yaw      -180----180,??????

	Gryo_data.roll 	= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;// roll     -180----180,


	if(0)			//????????8192

	{
		Gryo_data.pitch *=  ( 8192.0f / 360.0f );
		Gryo_data.yaw 	*=  ( 8192.0f / 360.0f );
		Gryo_data.roll 	*=  ( 8192.0f / 360.0f );
	}
	
	//m_pitch.gyro.Pitch_real_angle =Gryo_data.Pitch_real_angle;//?????????????????,????????,??????,???????????,????????

	//m_yaw.gyro.Yaw_real_angle =Gryo_data.Yaw_real_angle;

	
}


/**

 * @brief  ID:102,?????????????

 * @param rxmsg??can??????? gyro??????????????

 */
void gyro_read_speed(struct rt_can_msg* rxmsg)
{
	Gryo_data.speed_ready = rxmsg->data[7];
	Gryo_data.x_speed = ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;//??��:??/s,1rpm=6??/s

	Gryo_data.y_speed = ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;
	Gryo_data.z_speed = ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;
	Gryo_data.yaw_speed 	= -sqrtf(Gryo_data.y_speed*Gryo_data.y_speed + Gryo_data.z_speed*Gryo_data.z_speed);
	Gryo_data.pitch_speed 	= Gryo_data.x_speed;

	if(Gryo_data.z_speed > Gryo_data.y_speed)//?????????????????

	{
		if(Gryo_data.z_speed > 0)
			Gryo_data.yaw_speed = -fabsf(Gryo_data.yaw_speed);
	}

	//m_pitch.gyro.Pitch_real_angle =G_data.Pitch_real_angle;//?????????????????,????????,??????,???????????,????????

	//m_yaw.gyro.Yaw_real_angle =G_data.Yaw_real_angle;

	
}