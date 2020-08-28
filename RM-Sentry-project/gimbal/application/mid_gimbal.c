/************************************** Copyright ****************************** 
  *
  *                 (C) Copyright 2020,China, HITwh.
  *                            All Rights Reserved
  *                              
  *                     By(��������ҵ��ѧ(����)HEROս��)
  *                     HITwh Excellent Robot Organization
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : mid_gimbal.c   
  * Version    : v1.0		
  * Author     : 			
  * Date       : 2020-08-26         
  * Description:    �ڱ���̨�м��

  ****************************************************************************** */

#include "drv_strike.h"
#include "drv_motor.h"
#include "mid_gimbal.h"
#include "mid_canrec.h"
#include "robodata.h"

Motor_t m_yaw={0};/*5901-185-6417-3090-4095*/ //ratio=0.285->1681-2387-4163-5550-5836*/
Motor_t m_pitch={0};

//��һ����:��̨���
/**
 * @brief  ��̨���pid����
 */
static void pid_gimbal_init(void)
{
	/*pit��λ�û��ٶȻ�pid������ʼ��*/
	pid_init(&m_pitch.ang, 
					8,0.05,5,
					150,2000,-2000);
	pid_init(&m_pitch.spe, 
					30,0,0,
					6000,28000,-28000);
	/*yaw��λ�û��ٶȻ�pid������ʼ��*/
	pid_init(&m_yaw.ang, //8 10 80 600 30
					2,0.05,0,
					50,600,-600);
	pid_init(&m_yaw.spe, 
					40,0,0,
					300,28000,-28000);
	/**********������***********/
	m_pitch.ang.set=3200;//1303;			//2650
	m_yaw.ang.set=2000;		//4000
}

/**
 * @brief  ��̨�����ʼ����pid��ʼ����
 */
void motor_gimbal_init(void)
{
	pid_gimbal_init();
	m_pitch.dji.motorID = Pitch_ID;
	m_pitch.dji.ratio = 1;																/*���ٱ�<=1*/
	
	m_yaw.dji.motorID = Yaw_ID;
	m_yaw.dji.ratio = 1;
}


//�ڶ�����:�������

/***
  * @Name     launch_reinit
  * @brief    �ڱ�����6020�����ʼ��
  * @param    : [����/��] 
  * @retval   
  * @author   yi_lu
  * @Date     2020-08-26
***/
void launch_reinit(void)
{
    motor_init(&m_launch,LAUNCH_ID,1);
	pid_init(&m_launch.ang, 
					8,0.05,5,
					150,2000,-2000);
	pid_init(&m_launch.spe, 
					30,0,0,
					6000,28000,-28000);
}


//��������:��̨�����ͨ��
rt_size_t gimbal_send_message(rt_device_t dev, rt_int16_t *msg)
{
	struct rt_can_msg txmsg;
	
	txmsg.id = STDID_CHASSIS;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0]=(rt_uint8_t)(msg[0]>>8);
	txmsg.data[1]=(rt_uint8_t)(msg[0]);
	txmsg.data[2]=(rt_uint8_t)(msg[1]>>8);
	txmsg.data[3]=(rt_uint8_t)(msg[1]);
	txmsg.data[4]=(rt_uint8_t)(msg[2]>>8);
	txmsg.data[5]=(rt_uint8_t)(msg[2]);
	txmsg.data[6]=(rt_uint8_t)(msg[3]>>8);
	txmsg.data[7]=(rt_uint8_t)(msg[3]);
	
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}
void read_chassis_data(struct rt_can_msg *msg)
{
    ;
}
