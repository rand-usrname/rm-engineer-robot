#ifndef __DRV_GYRO_H__
#define __DRV_GYRO_H__
#include <rtthread.h>
#include <rtdevice.h>
#include "robodata.h"
//#include <arm_math.h>



typedef struct
{
	//IMU,大地坐标系
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

	
}ATTI_t;//姿态结构体


/*陀螺仪数据读取*/
//CAN1设备	GYRO_ANGLE_ID	0x101
extern void gyro_read_speed(struct rt_can_msg* rxmsg);	
//CAN1设备	GYRO_ANGLE_ID	0x101
extern void gyro_read_angle(struct rt_can_msg* rxmsg);
extern void IMU_transfer2_gm(void);



extern IMU_t HERO_IMU;//单位m/s^2,rad/s
extern ATTI_t gimbal_atti;//
#endif

