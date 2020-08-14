#ifndef __GIMBAL_CONTROL_H__
#define __GIMBAL_CONTROL_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "pid.h"
#include "robodata.h"
#include "can_receive.h"
#include "drv_gyro.h"

typedef enum
{
	ANGLE,			//����λ��
	PALSTANCE,		//���ٶ�
	CURRENT			//���õ���

} control_mode_t;
//����ģʽö�٣���ѡ��ֱ�ӿ�����

typedef enum
{
	DJI_MOTOR,		//����Դ�󽮵��
	GYRO			//����Դ������

} data_source_t;
//�ջ�����Դö��

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
	control_mode_t	control_mode;		//����ģʽ
	drv_can2ID_e 	motorID;			//ö�ٶ�����ID
	motordata_t		motordata;			//������ݽṹ��
	int				set;				//�趨ֵ

	pid_t		    speedpid;			//pid�����ṹ��
	data_source_t	angledata_source;	
	pid_t			anglepid_dji;		//����Դ���ʱpid
	pid_t			anglepid_gyro;		//����Դ������ʱpid

} gimbalmotor_t;
//�ýṹ�������ĸ����ֵĿ��ƺ����ݴ洢

//���ú���
extern void gimbal_current_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern void gimbal_absangle_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern void gimbal_addangle_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern void gimbal_palstance_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern void angle_datasource_set(data_source_t yawset,data_source_t pitchset);

//������õĺ���
extern int refresh_gimbal_motor_data(struct rt_can_msg* message);
extern int gimbal_init(void);

//��ȡ����
extern int get_yawangle(void);
extern int get_pitchangle(void);

#endif
