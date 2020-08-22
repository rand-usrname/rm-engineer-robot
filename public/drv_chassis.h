#ifndef __DRV_CHASSIS_H__
#define __DRV_CHASSIS_H__

#include <rtthread.h>
#include <rtdevice.h>

#include "pid.h"
#include "robodata.h"
#include "math.h"
#include "can_receive.h"

typedef enum
{
    NO_FOLLOW,							//���̲�������̨��������̨����Ϊǰ��
    FOLLOW_GIMBAL,						//���̸�����̨

    ONLY_CHASSIS,						//�����ڵ��̣��޽��ٶȱջ�

} sport_mode_e;
//�����˶�ģʽ

typedef struct
{
    rt_uint16_t 	angle;				//ת�ӽǶȣ���С��Χ0~8191
    rt_int16_t 		speed;				//ת��ת�٣���λrpm
    rt_int16_t 		current;			//ʵ��ת�ص���
    rt_uint8_t 		temperature;		//�����ʵ���¶�

} motordata_t;
//����ĵ�ǰת�ٵ�����

typedef struct
{
    drv_can1ID_e 	motorID;			//ö�ٶ�����ID
    motordata_t		motordata;			//������ݽṹ��
    pid_t		    speedpid;			//pid�����ṹ��

} motor_t;
//�ýṹ�������ĸ����ֵĿ��ƺ����ݴ洢

typedef struct
{
    sport_mode_e	sport_mode;			//��ǰ�˶�ģʽ
    pid_t		    anglepid;			//�ǶȻ�pid������Ҫ����ʱ������ٶ�
    motordata_t		pitch_data;			//pitch�ᵱǰ���ݣ���ˮƽΪ0��
    motordata_t		yaw_data;			//yaw������
    int				powermax;			//���������

    //�����������ݴ�������̨Ϊ������������ٶ�
    rt_uint16_t		follow_angle;		//����Ƕȣ���Χ0-8191
    rt_int16_t 		angular_velocity;	//��λ0.1 ��/s
    rt_int16_t 		xspeed;				//��λmm/s
    rt_int16_t		yspeed;				//��λmm/s

} chassis_data_t;

extern void chassis_speed_set(rt_uint16_t follow_angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed);
extern void sport_mode_set(sport_mode_e sport_mode);
extern int refresh_chassis_motor_data(struct rt_can_msg* message);
extern int refresh_gimbal_motor_data(struct rt_can_msg* message);
extern int chassis_init(void);

#endif
