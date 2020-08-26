#include "task_chassis.h"
#include "drv_motor.h"
#include "drv_canthread.h"
#include "drv_remote.h"
#include "mid_chassis.h"
//#include "drv_computer.h"

/*��ʱ����ʹ��*/
rt_uint8_t iffire = 0;														/*�����־*/


static void pid_1ms_calculate(void);
static void pid_5ms_calculate(void);
static void remote_ctrl(RC_Ctrl_t *remote);
static void auto_move(void);
static void master_send_gimbal(rt_int16_t pitch_angle, rt_int16_t yaw_angle);
/**
 * @brief  101ms����ִ�к���
 */
 rt_uint8_t test_dutystatus=0;
void task_fun101ms(void)
{
	remote_ctrl(&RC_data);
}
/**
 * @brief  5ms����ִ�к���
 */
void task_fun5ms(void)
{

	/*����̨ͨ��*/
	//chassis_send_message();
	/*����ģʽ����*/
	chassis_control();
	pid_5ms_calculate();
}
/**
 * @brief  1ms����ִ�к���
 */
void task_fun1ms(void)
{
	pid_1ms_calculate();
}
/*��ʱ����ʹ��*/
/**
 * @brief  ң�ؿ���
 * @param  remote��ң�������ݽṹ��ָ��
 */
static void remote_ctrl(RC_Ctrl_t *remote)
{
	static rt_uint8_t s1_old;
	static rt_uint8_t s2_old;
	/*δ�ɹ�ͨ�Ÿ�ֵ��ֱ�ӷ���*/
	if(remote->Remote_Data.ch2 == 0 || remote->Remote_Data.ch3 == 0)
	{
		return ;
	}
	/***********************״̬����********************************/
	if(remote->Remote_Data.s2 == 3)
	{

	}
	else if(remote->Remote_Data.s2 == 2)
	{
		iffire = 1;
	}
	else if(remote->Remote_Data.s2 == 1)	
	{

	}
	if(remote->Remote_Data.s1 == 3)
	{}
	else if(remote->Remote_Data.s1 == 2)
	{}
	else if(remote->Remote_Data.s1 == 1)	
	{}
	/*********************************************end*********************************************************/
	/*********************************���м����������򲦶�,��������*******************************************/
	/*��ʱ����ʹ��*/
	iffire = 0;																								/*����ָ������*/

	if(s1_old == 3)
	{
		if(remote->Remote_Data.s1 == 2)				/*����*/
		{
			
		}
		else if(remote->Remote_Data.s1 == 1)/*����*/
		{

		}
	}
	if(s2_old == 3)
	{
		if(remote->Remote_Data.s2 == 1)
		{
			
			
		}
		else if(remote->Remote_Data.s2 == 2)
		{
			
		}
	}
	
}
/*��ʱ����ʹ��*/

#ifdef TEST
/*
/*���ض�*/
void auto_move(void)
{
	static rt_int8_t auto_move_pitch=10;
	static rt_int8_t auto_move_yaw=3;
	m_yaw.ang.set+=auto_move_yaw;
	if(m_yaw.ang.set>YAW_UP)
	{
		auto_move_yaw*=-1;
		m_yaw.ang.set=YAW_UP;
	}
	else if(m_yaw.ang.set<YAW_DOWN)
	{
		auto_move_yaw*=-1;
		m_yaw.ang.set=YAW_DOWN;
	}
	m_pitch.ang.set+=auto_move_pitch;
	if(m_pitch.ang.set>PITCH_UP)
	{
		auto_move_pitch*=-1;
		m_pitch.ang.set=PITCH_UP;
	}
	else if(m_pitch.ang.set<PITCH_DOWN)
	{
		auto_move_pitch*=-1;
		m_pitch.ang.set=PITCH_DOWN;
	}
}
/**
* @brief  ��tx2������̨����ָ��
 */
void tx2_send(void)
{
	rt_uint8_t txdata[8];
	txdata[0]=tx2_data.head;
	txdata[1]=1;
	txdata[2]=m_yaw.dji.angle>>8;txdata[3]=(rt_uint8_t)m_yaw.dji.angle;
	txdata[4]=m_pitch.dji.angle>>8;txdata[5]=(rt_uint8_t)m_pitch.dji.angle;
	txdata[6]=0;txdata[7]=tx2_data.tail;
	rt_device_write(tx2_serial, 0, txdata, 8);
}

/**
* @brief  ��ӻ�������̨����ָ��
* @param  pitch_angle��pitch�Ƕ�
* @param  yaw_angle: yaw�Ƕ�
 */
void master_send_gimbal(rt_int16_t pitch_angle, rt_int16_t yaw_angle)
{
	struct rt_can_msg txmsg;
	
	txmsg.id=STDID_SLAVE_GIMBAL;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0]=(rt_uint8_t)(pitch_angle>>8);txmsg.data[1]=(rt_uint8_t)(pitch_angle);
	txmsg.data[2]=(rt_uint8_t)(yaw_angle>>8);txmsg.data[3]=(rt_uint8_t)(yaw_angle);
	txmsg.data[4]=0;txmsg.data[5]=0;
	txmsg.data[6]=0;txmsg.data[7]=0;
	rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg));
}
/**
* @brief  ��ӻ�����ǹ�ڿ���ָ��
* @param  gun_speed: Ħ�����ٶ�
* @param  if_fire: �����־ 1������ 0��������
* @param  gun_mode:ǹ��ģʽ
 */
void master_send_strike(rt_int16_t gun_speed, rt_uint8_t if_fire, rt_uint8_t gun_mode)
{
	struct rt_can_msg txmsg;
	
	txmsg.id=STDID_SLAVE_STRIKE;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0]=(rt_uint8_t)(gun_speed>>8);txmsg.data[1]=(rt_uint8_t)(gun_speed);
	txmsg.data[2]=if_fire;txmsg.data[3]=gun_mode;
	txmsg.data[4]=0;txmsg.data[5]=0;
	txmsg.data[6]=0;txmsg.data[7]=0;
	rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg));
}

#endif




/**
 * @brief  5ms��pid����
 */
static void pid_5ms_calculate(void)
{
	/*pidλ�û�����*/
	
}
/**
 * @brief  1ms��pid����
 */
static void pid_1ms_calculate(void)
{
	/*pid�ٶȻ�����*/
	pid_output_calculate(&m_chassis_3508[0].spe, m_chassis_3508[0].spe.set, m_chassis_3508[0].dji.speed);
	pid_output_calculate(&m_chassis_3508[1].spe, m_chassis_3508[1].spe.set, m_chassis_3508[1].dji.speed);
	
	/*�����������*/
	motor_current_send(can1_dev, 0x200, m_chassis_3508[0].spe.out, m_chassis_3508[0].spe.out, 0, 0);
	
}
