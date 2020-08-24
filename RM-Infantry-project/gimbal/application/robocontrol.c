#include "robocontrol.h"
/**
* @brief  ����̷��������Կ��Ƶ���
* @param  xspeed��x���ٶ� yspeed��y���ٶ�
* @param  mode: ģʽ��������0������1��
* @param  angel_or_speed������ʱ�ĽǶȣ�������ʱ�ٶ�
*/
static int chassis_ctl(rt_int16_t xspeed, rt_int16_t yspeed, rt_uint8_t mode,rt_int16_t angel_or_speed)
{
	struct rt_can_msg txmsg;

	txmsg.id = CHASSIS_CTL;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr = RT_CAN_DTR;
	txmsg.len = 8;
	txmsg.data[0] = (rt_uint8_t)(xspeed>>8);
	txmsg.data[1] = (rt_uint8_t)(xspeed);
	txmsg.data[2] = (rt_uint8_t)(yspeed>>8);
	txmsg.data[3] = (rt_uint8_t)(yspeed);
	txmsg.data[4] = (rt_uint8_t)(angel_or_speed>>8);
	txmsg.data[5] = (rt_uint8_t)(angel_or_speed);
	txmsg.data[6] = (rt_uint8_t)(mode);
	txmsg.data[7] = 0;
	rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg));
}
int gun1speed = 0;
int basespeed = 100;
/**
* @brief����̨����ģʽ����
* @param [in]	yawset��yaw�����ģʽ
				pitchset��pitch�����ģʽ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int computer_ctrl(RC_Ctrl_t *remote)
{
	//�������������̨ת��
	
	gimbal_addangle_set(remote->Mouse_Data.x_speed,remote->Mouse_Data.y_speed);
	rt_int16_t xspeed = (remote->Key_Data.D - remote->Key_Data.A) * basespeed;
	rt_int16_t yspeed = (remote->Key_Data.W - remote->Key_Data.S) * basespeed;
	
	chassis_ctl(xspeed,yspeed,0,0);
}
/**
* @brief����̨����ģʽ����
* @param [in]	yawset��yaw�����ģʽ
				pitchset��pitch�����ģʽ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int remote_ctrl(RC_Ctrl_t *remote)
{
	rt_int16_t pitchadd = 0;
	rt_int16_t yawadd = 0;
	//״̬��������

	
	//S1״̬����
	if(remote->Remote_Data.s1 == 3)//��ఴ������
	{
		
	}
	else if(remote->Remote_Data.s1 == 2)//��ఴ������
	{
		
	}
	else if(remote->Remote_Data.s1 == 1)//��ఴ������
	{
		
	}

	//S2״̬����
	if(remote->Remote_Data.s2 == 3)//�Ҳఴ������
	{
		pitchadd = (rt_int16_t)((remote->Remote_Data.ch1 - 1024)/30);
		yawadd = -(rt_int16_t)((remote->Remote_Data.ch0 - 1024)/30);
	}
	else if(remote->Remote_Data.s2 == 2)//�Ҳఴ������
	{
		//ʹ�õ������ݽ��в�����������������
		computer_ctrl(remote);
		return 1;
	}
	else if(remote->Remote_Data.s2 == 1)//�Ҳస������
	{
		if(get_yawusetime() < 1)
		{
			yawadd = -get_yaw_add()/16;
		}
		if(get_pitchusetime() < 1)
		{
			pitchadd = get_pitch_add()/16;
		}
	}
	
	//������������
	//S1����
	switch_action_e s_action;//����������ʱ����
	s_action = Change_from_middle(S1);
	if(s_action == middle_to_up)//���ϲ���
	{
		strike_fire(&m_launch,&gun1,1);
	}
	else if(s_action == middle_to_down)//���²���
	{
		//����������Ħ�����ٶ�
		if(gun1speed  == 0)
		{
			gun1speed = -3000;
		}
		else if(gun1speed < -6000)
		{
			gun1speed = 0;
		}
		else
		{
			gun1speed -= 400;
		}
		Gun_speed_set(&gun1,gun1speed);
	}
	
	//S2����
	s_action = Change_from_middle(S2);
	if(s_action == middle_to_up)//���ϲ���
	{
		
	}
	else if(s_action == middle_to_down)//���²���
	{
	}
	chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,0,0);
	gimbal_addangle_set(yawadd,pitchadd);
	return 1;
}

