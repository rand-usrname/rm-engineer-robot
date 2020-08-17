#include "stm32f4xx.h"
#include "drv_gyro.h"
//#include <arm_math.h>
//#include "ahrs.h"

IMU_t HERO_IMU;//IMU????,??m/s^2,rad/s
gimbal_atti_t HERO_IMU_atti;

float invSqrt(float x);	//快速开平方

/***
  * @Name     gyro_read_angle
  * @brief    陀螺仪角度帧读取,CAN ID:101
  * @param    rxmsg: [输入] ,接收到的CAN报文
  * @retval   
  * @author   yi_lu
  * @Date     2020-08-11
***/
void gyro_read_angle(struct rt_can_msg* rxmsg)
{
	HERO_IMU.atti_ready = rxmsg->data[7];		//姿态数据正常标志

	HERO_IMU.pitch  = ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;// pitch    -180----180,上正下负,单位:°
	HERO_IMU.yaw 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;// yaw      -180----180,右正左负
	HERO_IMU.roll 	= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;// roll     -90----90,

}



/***
  * @Name     gyro_read_speed
  * @brief    陀螺仪角速度帧读取,CAN ID:102
  * @param    rxmsg: [输入] ,接收到的CAN报文
  * @retval   
  * @author   yi_lu
  * @Date     2020-08-11
***/
void gyro_read_speed(struct rt_can_msg* rxmsg)
{
	HERO_IMU.speed_ready = rxmsg->data[7];		//角速度数据正常标志

	HERO_IMU.pitch_speed	= ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;//单位:°/s,1rpm=6°/s
	HERO_IMU.roll_speed 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;
	HERO_IMU.yaw_speed 		= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;
	
}

//如果云台控制线程存在
#ifdef THREAD_GIMBAL_CONTROL

//大地系(IMU)转到云台电机系
void IMU_transfer2_gimbal()
{
	gimbal_atti.pitch = HERO_IMU.pitch - ;
	gimbal_atti.roll = HERO_IMU.roll - 


	gimbal_atti.pitch_speed = 
	
	HERO_IMU.gx = Gryo_data.x_speed;
	HERO_IMU.gy = Gryo_data.y_speed;
	HERO_IMU.gz = Gryo_data.z_speed;
	
	Gryo_data.yaw_speed 	= -sqrtf(Gryo_data.y_speed*Gryo_data.y_speed + Gryo_data.z_speed*Gryo_data.z_speed);
	Gryo_data.pitch_speed 	= Gryo_data.x_speed;

	if(Gryo_data.z_speed > Gryo_data.y_speed)//符号取决于较大值的符号
	{
		if(Gryo_data.z_speed > 0)
			Gryo_data.yaw_speed = -fabsf(Gryo_data.yaw_speed);
	}

}

#endif



// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}
