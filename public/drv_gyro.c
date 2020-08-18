#include "stm32f4xx.h"
#include "drv_gyro.h"
#include "math.h"
//#include <arm_math.h>
//#include "ahrs.h"

IMU_t HERO_IMU;//IMU��������,unit:m/s^2,rad/s
ATTI_t gimbal_atti;

float invSqrt(float x);	//���ٿ�ƽ��
void IMU_transfer2gm(void);

/***
  * @Name     gyro_read_angle
  * @brief    �����ǽǶ�֡��ȡ,CAN ID:101
  * @param    rxmsg: [����] ,���յ���CAN����
  * @retval   
  * @author   yi_lu
  * @Date     2020-08-11
***/
void gyro_read_angle(struct rt_can_msg* rxmsg)
{
	HERO_IMU.atti_ready = rxmsg->data[7];		//��̬����������־

	HERO_IMU.pitch  = ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;// pitch    -180----180,�����¸�,��λ:��
	HERO_IMU.yaw 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;// yaw      -180----180,������
	HERO_IMU.roll 	= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;// roll     -90----90,

}



/***
  * @Name     gyro_read_speed
  * @brief    �����ǽ��ٶ�֡��ȡ,CAN ID:102
  * @param    rxmsg: [����] ,���յ���CAN����
  * @retval   
  * @author   yi_lu
  * @Date     2020-08-11
***/
void gyro_read_speed(struct rt_can_msg* rxmsg)
{
	HERO_IMU.speed_ready = rxmsg->data[7];		//���ٶ�����������־

	HERO_IMU.pitch_speed	= ((rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1])) / 100.0f;//��λ:��/s,1rpm=6��/s
	HERO_IMU.roll_speed 	= ((rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3])) / 100.0f;
	HERO_IMU.yaw_speed 		= ((rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5])) / 100.0f;
	
	//�����̨�����̴߳���
	#ifdef THREAD_GIMBAL_CONTROL

	IMU_transfer2gm();

	#endif

}



//���ϵ(IMU)ת����̨���ϵ
void IMU_transfer2gm(void)
{
	float pitch_ecd_offset;
	int dir = 0;	//���ٶȷ���

	gimbal_atti.pitch = HERO_IMU.pitch - pitch_ecd_offset;
	gimbal_atti.yaw = HERO_IMU.yaw;
	gimbal_atti.roll = HERO_IMU.roll;

	gimbal_atti.pitch_speed = HERO_IMU.pitch_speed;

	if(fabs(HERO_IMU.yaw_speed) > fabs(HERO_IMU.roll_speed))//����ȡ���ڽϴ�ֵ�ķ���
	{
		if(HERO_IMU.yaw_speed > 0)
			dir = 1;
		else
			dir = -1;
	}
	else
	{
		if(HERO_IMU.roll_speed > 0)
			dir = 1;
		else
			dir = -1;
	}
	
	gimbal_atti.yaw_speed = dir * sqrtf(HERO_IMU.yaw_speed * HERO_IMU.yaw_speed + HERO_IMU.roll_speed * HERO_IMU.roll_speed);
	//gimbal_atti.roll_speed = HERO_IMU.roll_speed;

	

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
