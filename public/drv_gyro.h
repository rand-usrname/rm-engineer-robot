#ifndef __DRV_GYRO_H__
#define __DRV_GYRO_H__
#include <rtthread.h>
#include <rtdevice.h>
//#include <arm_math.h>

#define GYRO_ANGLE_ID			0x101
#define GYRO_SPEED_ID			0x102

typedef struct
{
	//大地坐标系
	float pitch;										/*pitch*///与安装方向有关
	float yaw;											/*yaw*/
	float roll;											/*roll*///与安装方向有关

	float pitch_speed;											/*PITCH轴角速度*/
	float roll_speed;											/*ROLL轴角速度*/
	float yaw_speed;											/*YAW轴角速度*/

	char speed_ready;										//角速度数据正常标志
	char atti_ready;										//姿态数据正常标志

	
}IMU_t;//IMU传感器结构体

typedef struct
{
	//云台电机系,
	float pitch;										/*pitch*/
	float yaw;											/*yaw*/
	float roll;											/*roll*/

	float pitch_speed;											/*PITCH轴角速度*/
	float roll_speed;											/*ROLL轴角速度*/
	float yaw_speed;											/*YAW轴角速度*/

	
}gimbal_atti_t;//IMU姿态结构体


/*陀螺仪数据读取*/
extern void gyro_read_speed(struct rt_can_msg* rxmsg);
extern void gyro_read_angle(struct rt_can_msg* rxmsg);
extern void IMU_transfer2_gimbal();



extern struct IMU_t HERO_IMU;//单位m/s^2,rad/s
extern struct gimbal_atti_t gimbal_atti;
#endif

