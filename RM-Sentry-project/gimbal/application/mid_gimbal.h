#ifndef __MID_GIMBAL_H__
#define __MID_GIMBAL_H__

#include <rtthread.h>
#include <rtdevice.h>

//������̨ͨ��ID
#define  STDID_GIMBAL  0x110
#define  STDID_CHASSIS  0x111

void launch_reinit(void);	//�ڱ�����6020��ʼ��,����ͨ�÷���������ʼ�������³�ʼ��
void read_chassis_data(struct rt_can_msg *msg);	//��ȡ���̷���������

#endif
