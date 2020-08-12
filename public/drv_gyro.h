#ifndef __DRV_GYRO_H__
#define __DRV_GYRO_H__

#include <rtthread.h>
#include <rtdevice.h>
//#include <arm_math.h>

#define GYRO_ANGLE_ID			0x101
#define GYRO_SPEED_ID			0x102

typedef struct
{
	float pitch;			//pitch//�밲װ�����й�
	float yaw;				//yaw
	float roll;				//roll

	float x_speed;			//x����ٶ�
	float y_speed;			//y����ٶ�
	float z_speed;			//z����ٶ�

	float pitch_speed;		//pitch���ٶ�
	float yaw_speed;		//yaw���ٶ�
	float roll_speed;		//roll���ٶ�

	char angle_ready;		//�Ƕ�����������־
	char speed_ready;		//���ٶ�����������־

}Gyrodata_t;

void gyro_read_speed(struct rt_can_msg* rxmsg);
void gyro_read_angle(struct rt_can_msg* rxmsg);
extern Gyrodata_t Gryo_data;

#endif
