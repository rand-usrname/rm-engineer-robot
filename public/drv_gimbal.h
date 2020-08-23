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
	drv_can1ID_e 	motorID;			//ö�ٶ�����ID
	motordata_t		motordata;			//������ݽṹ��
	rt_uint16_t		setang;				//�Ƕ��趨ֵ

	pid_t		    palpid;				//���ٶȽṹ��
	data_source_t	angdata_source;		//�Ƕȱջ�����Դ
	pid_t			angpid_dji;			//����Դ���ʱpid
	pid_t			angpid_gyro;		//����Դ������ʱpid

} gimbalmotor_t;
//�ýṹ�������ĸ����ֵĿ��ƺ����ݴ洢

//�ⲿ���õĿ��ƺ���
extern int gimbal_absangle_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern int gimbal_addangle_set(rt_int16_t yawset,rt_int16_t pitchset);
extern int gimbal_palstance_set(rt_uint16_t yawset,rt_uint16_t pitchset);
extern int gimbal_ctlmode_set(control_mode_t yawset,control_mode_t pitchset);
extern int angle_datasource_set(data_source_t yawset,data_source_t pitchset);

//������õĺ���
//Ĭ�� CAN1 �豸��Ĭ��ID YAW_ID = 0X205 PITCH_ID = 0X206
extern int refresh_gimbal_motor_data(struct rt_can_msg* message);
extern int gimbal_init(void);

//��ȡ��̨���ݺ���
extern rt_uint16_t get_yawangle(void);
extern rt_uint16_t get_pitchangle(void);

#endif
