#include "stm32f4xx.h"
#include "drv_gyro.h"
#include "math.h"
//#include <arm_math.h>
//#include "ahrs.h"

IMU_t HERO_IMU;//IMU传回数据,unit:m/s^2,rad/s
ATTI_t gimbal_atti;

float invSqrt(float x);	//快速开平方
void IMU_transfer2gm(void);

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

	HERO_IMU.pitch  = ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;// pitch    -90----90,在"建议安装坐标系"下,面对枪口,上正下负,单位:°
	HERO_IMU.yaw 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;// yaw      -180----180,在"建议安装坐标系"下,面对枪口,左负右正
	HERO_IMU.roll 	= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;// roll     -180----180,在"建议安装坐标系"下,面对枪口,逆时针负,顺时针正

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

	HERO_IMU.pitch_speed	= ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;//建议安装坐标系下,面对枪口,上正下负,单位:°/s,1rpm=6°/s
	HERO_IMU.yaw_speed 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;
	HERO_IMU.roll_speed 	= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;
	
	//如果云台控制线程存在
	#ifdef THREAD_GIMBAL_CONTROL

	IMU_transfer2gm();

	#endif

}



//大地系(IMU)转到云台电机系
void IMU_transfer2gm(void)
{
	float pitch_ecd_offset = 0;
	int dir = 0;	//融合roll角速度后的方向

	gimbal_atti.pitch = HERO_IMU.pitch - pitch_ecd_offset;
	gimbal_atti.yaw = HERO_IMU.yaw;
	gimbal_atti.roll = HERO_IMU.roll;

	gimbal_atti.pitch_speed = HERO_IMU.pitch_speed;
	
	//gimbal_atti.yaw_speed = HERO_IMU.yaw_speed;

	if(HERO_IMU.yaw_speed > 0)	//融合后角速度方向同原始yaw_speed
		dir = 1;
	else
		dir = -1;

	gimbal_atti.yaw_speed = dir * sqrtf(HERO_IMU.yaw_speed * HERO_IMU.yaw_speed + HERO_IMU.roll_speed * HERO_IMU.roll_speed);
	//gimbal_atti.roll_speed = HERO_IMU.roll_speed;

	

}

void can_cali_send()
{
    struct rt_can_msg tx_msg = {0};
	rt_device_t can1_dev;            /* CAN 设备句柄 */
	
    rt_size_t  size;


    tx_msg.id = 0x103;             /* ID 为 0x101 */
    tx_msg.ide = RT_CAN_STDID;     /* 标准格式 */
    tx_msg.rtr = RT_CAN_DTR;       /* 数据帧 */
    tx_msg.len = 0x08;;                /* 数据长度为 8 */

    /* 待发送的 8 字节数据 */
	tx_msg.data[0] = 0x00;
    tx_msg.data[1] = 0x00;
    tx_msg.data[2] = 0x00;
    tx_msg.data[3] = 0x00;
    tx_msg.data[4] = 0x00;
    tx_msg.data[5] = 0x00;
	tx_msg.data[6] = 0x00;
    tx_msg.data[7] = 0x00;


	can1_dev = rt_device_find("can1");
    /* 发送一帧 CAN 数据 */
    size = rt_device_write(can1_dev, 0, &tx_msg, sizeof(tx_msg)); 
	
}



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
