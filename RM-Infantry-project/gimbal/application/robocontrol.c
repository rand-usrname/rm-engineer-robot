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
int basespeed = 800;
int sport_mode = 0;
/**
* @brief��һ����ͷ��ں���
* @param [in]	time:��ͷ��ת���ظ���ģʽ��ʱ�䣬��װ����ָ�������
* @return��		��
* @author��mqy
*/
static void onekey_back(void* parameter)
{
	sport_mode = 0;//�Ǹ���
	gimbal_addangle_set(4096,0);
	rt_thread_mdelay((int)parameter);//��װ�������ָ��
	sport_mode = 1;//����
	return;
}
/**
* @brief����̨����ģʽ����
* @param [in]	yawset��yaw�����ģʽ
				pitchset��pitch�����ģʽ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int computer_ctrl(RC_Ctrl_t *remote)
{
	rt_int16_t xspeed = (remote->Key_Data.D - remote->Key_Data.A) * basespeed;
	rt_int16_t yspeed = (remote->Key_Data.W - remote->Key_Data.S) * basespeed;
	if(Key_action_read(&(remote->Key_Data.F)) == PRESS_ACTION)
	{
		//��ʼ����ͷ
		rt_thread_t gimbal_control = rt_thread_create(
		"onekey_back",	//�߳���
		onekey_back,	//�߳����
		(void*)(10000),	//��ڲ�����
		2048,			//�߳�ջ
		1,	            //�߳����ȼ�
		1);				//�߳�ʱ��Ƭ��С
		rt_thread_startup(gimbal_control);//�����߳�
	}
	//���͵��̿�����Ϣ
	switch(sport_mode)
	{
		case 0://�Ǹ���
			chassis_ctl(xspeed,yspeed,0,0);
			break;
		case 1://����
			chassis_ctl(xspeed,yspeed,1,0);
			break;
		case 2://С����
			chassis_ctl(xspeed,yspeed,0,1000);
			break;
		default:
			chassis_ctl(0,0,0,0);
			break;
	}
	//������̨�Ƕ�
	gimbal_addangle_set( -remote->Mouse_Data.x_speed, -remote->Mouse_Data.y_speed);
	return 1;
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
	//S2״̬����
	if(remote->Remote_Data.s2 == 1)//�Ҳస������
	{
		//ʹ�õ������ݽ��в�����������������
		return 0;//computer_ctrl(remote);
	}
	else//�Ҳఴ��������
	{
		if(get_yawusetime() < 1){
			yawadd = -get_yaw_add()/360.0f*8192;
		}
		else{
			yawadd = -(rt_int16_t)((remote->Remote_Data.ch0 - 1024)/30);
		}
		//pitch�������ã������Ӿ����ݻ���ң��������
		if(get_pitchusetime() < 1){
			pitchadd = get_pitch_add()/360.0f*8192;
		}
		else{
			pitchadd = (rt_int16_t)((remote->Remote_Data.ch1 - 1024)/30);
		}
	}
	
	//������������
	//S1����
	switch_action_e s_action;//����������ʱ����
	s_action = Change_from_middle(S1);
	if(s_action == middle_to_up)//���ϲ���
	{
		//����������Ħ�����ٶ�
		if(gun1speed  == 0){
			gun1speed = -3000;
		}
		else if(gun1speed < -6000){
			gun1speed = 0;
		}
		else{
			gun1speed -= 400;
		}
		Gun_speed_set(&gun1,gun1speed);
	}
	else if(s_action == middle_to_down)//���²���
	{
		//!!!Fire!!!
		strike_fire(&m_launch,&gun1,1);
	}
	//S2����
	s_action = Change_from_middle(S2);//���ϲ��������л����Բ���
	if(s_action == middle_to_down)//���²���
	{
		//�����˶�ģʽ
		sport_mode++;
		if(sport_mode > 2)
		{
			sport_mode = 0;
		}
	}
	
	//���͵��̿�����Ϣ
	switch(sport_mode)
	{
		case 0://�Ǹ���
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,0,0);
			break;
		case 1://����
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,1,0);
			break;
		case 2://С����
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,0,1000);
			break;
		default:
			chassis_ctl(0,0,0,0);
			break;
	}
	//������̨
	if(pitchadd!=0)
	{
		pitch.setang = (pitchadd+get_pitchangle());
	}
	if(yawadd!=0)
	{
		yaw.setang = (yawadd+get_yawangle());
	}
	yawadd = 0;
	pitchadd = 0;
	//��֤���ݲ�������Χ
	yaw.setang %= 8192;
	pitch.setang %= 8192;
	return 1;
}

