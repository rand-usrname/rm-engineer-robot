#ifndef __DRV_GYRO_H__
#define __DRV_GYRO_H__
#include <rtthread.h>
#include <rtdevice.h>
//#include <arm_math.h>

#define GYRO_ANGLE_ID			0x101
#define GYRO_SPEED_ID			0x102

typedef struct
{
	//�������ϵ
	float pitch;										/*pitch*///�밲װ�����й�
	float yaw;											/*yaw*/
	float roll;											/*roll*///�밲װ�����й�

	float pitch_speed;											/*PITCH����ٶ�*/
	float roll_speed;											/*ROLL����ٶ�*/
	float yaw_speed;											/*YAW����ٶ�*/

	char speed_ready;										//���ٶ�����������־
	char atti_ready;										//��̬����������־

	
}IMU_t;//IMU�������ṹ��

typedef struct
{
	//��̨���ϵ,
	float pitch;										/*pitch*/
	float yaw;											/*yaw*/
	float roll;											/*roll*/

	float pitch_speed;											/*PITCH����ٶ�*/
	float roll_speed;											/*ROLL����ٶ�*/
	float yaw_speed;											/*YAW����ٶ�*/

	
}gimbal_atti_t;//IMU��̬�ṹ��


/*���������ݶ�ȡ*/
extern void gyro_read_speed(struct rt_can_msg* rxmsg);
extern void gyro_read_angle(struct rt_can_msg* rxmsg);
extern void IMU_transfer2_gimbal();



extern struct IMU_t HERO_IMU;//��λm/s^2,rad/s
extern struct gimbal_atti_t gimbal_atti;
#endif

