#ifndef __DRV_GYRO_H__
#define __DRV_GYRO_H__

#include <rtthread.h>
#include <rtdevice.h>
//#include <arm_math.h>

#define GYRO_ANGLE_ID			0x101
#define GYRO_SPEED_ID			0x102

typedef struct
{
	float pitch;			//pitch//与安装方向有关
	float yaw;				//yaw
	float roll;				//roll

	float x_speed;			//x轴角速度
	float y_speed;			//y轴角速度
	float z_speed;			//z轴角速度

	float pitch_speed;		//pitch角速度
	float yaw_speed;		//yaw角速度
	float roll_speed;		//roll角速度

	char angle_ready;		//角度数据正常标志
	char speed_ready;		//角速度数据正常标志

}Gyrodata_t;

void gyro_read_speed(struct rt_can_msg* rxmsg);
void gyro_read_angle(struct rt_can_msg* rxmsg);
extern Gyrodata_t Gryo_data;

#endif
