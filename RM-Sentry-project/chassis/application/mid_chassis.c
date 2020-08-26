/************************************** Copyright ****************************** 
  *
  *                 (C) Copyright 2020,China, HITwh.
  *                            All Rights Reserved
  *                              
  *                     By  ��������ҵ��ѧ��������HEROս��
  *                     HITwh Excellent Robot Organization
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : drv_sentrychas.c  
  * Version    : v1.0		
  * Author     : yi_lu			
  * Date       : 2020-08-23         
  * Description:    �ڱ��ĵ��̲�ͬ��һ�����,����дһ�������ļ�

  ****************************************************************************** */


#include "drv_remote.h"
#include "drv_motor.h"

#include "mid_chassis.h"

#include "task_chassis.h"

Motor_t m_chassis_3508[2];

rt_uint8_t remote_mode = 0;		//ģʽ0������


/**
 * @brief ����pid��ʼ��
 */
static void pid_chassis_init(void)
{
	pid_init(&m_chassis_3508[0].spe, 
					8,0,0,
					300,8000,-8000);
	pid_init(&m_chassis_3508[1].spe, 
					8,0,0,
					300,8000,-8000);
	
}
/**
 * @brief  ���̵����ʼ����pid��ʼ��+can��ʼ����
 */
void chassis_init(void)
{
	pid_chassis_init();
	m_chassis_3508[0].dji.motorID = LEFT;
	m_chassis_3508[0].dji.ratio = 1;				/*���ٱ�<=1*/
	m_chassis_3508[1].dji.motorID = RIGHT;
	m_chassis_3508[1].dji.ratio = 1;
}

/**
 * @brief  ���̿���ģʽ
 */
void chassis_control(void)
{
	if(remote_mode==0)
	{
		m_chassis_3508[0].spe.set = (rt_int16_t)((RC_data.Remote_Data.ch2 - 1018)*10);
		m_chassis_3508[1].spe.set = (rt_int16_t)((RC_data.Remote_Data.ch2 - 1018)*10);
		
	}
	
}

rt_size_t chassis_send_message(rt_device_t dev, rt_int16_t *msg)
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


#ifdef mqy
//���̵��PID�߳̾��
static rt_thread_t 	chassis_ctrl_handler = RT_NULL;

//������ƽṹ�壬����0��Ӧ��СID
static motor_t	chassis_motor[4] = {0};

//�˶����ݣ�������̨����Ƕȣ������ٶȵ�
static chassis_data_t	motion_data;

#define ABS(a) (((a)>0)?(a):(-a))

/**
* @brief�����ղ����趨���̵��˶����ٶ����ٶȣ��ٶȽ�����ᱻ�����ڱ��ļ����ĸ����ӵ�������
* @param [in]	angle:�趨���β����������򣨼����β�����ˮƽ����ϵ��y��������
				��ֵ��С��Χ0~8191����Ӧ�˽Ƕȷ�Χ0~359.95�ȣ�
				angular_velocity:�趨������ת���ٶȣ���λ 0.1 ��/s �� pi/1800 rad/s
				xspeed:������x���ٶȷ�������λmm/s
				yspeed:������y���ٶȷ�������λmm/s
* @return��		��
* @author��mqy
*/
static void motor_speed_set(rt_uint16_t angle, rt_int16_t speed)
{
	while(angle > 8191)
	{
		angle -= 8191;
	}
	angle = 8191 - angle;
	
	//2.40946Ϊ��λת����ϵ����vΪ�������������ٶȣ������ٶ�ʹ��rpm��Ϊ��λ
	chassis_motor[(rt_uint8_t)(LEFT- 0x201)].speedpid.set = (rt_int16_t)((float)speed);
	chassis_motor[(rt_uint8_t)(RIGHT- 0x201)].speedpid.set = (rt_int16_t)((float)speed);
}
/**
* @brief���ú��������趨�˶�ģʽ���趨���ٶȽ��ٶȵ��ú���motor_speed_set�趨����ٶ�
			    �ڳ�ʼ��֮��ú���ÿ����x ms��ִ��һ��
* @param [in]	��
* @return��		��
* @author��mqy
*/
static void chassis_control(void)
{
	switch(motion_data.sport_mode)
	{
		case NO_FOLLOW:
			chassis_speed_set(0, 0);
			break;

		default:
			motor_speed_set(0, 0);//��û��ģʽ������£��ٶ�����
			break;
	}
}
/**
* @brief�������̹���������������Χ��
* @param [in]	powermax:���ƵĹ��ʴ�С����λ mW - ����
				�ú�����ͨ��ֱ�Ӷ�д�ķ�ʽ��ȡ��ǰ�ĸ�����ĵ���
* @return���ƻ�Ϊ1ʱ���ƣ�0ʱδ��������
* @author��mqy
*/
static int power_limit(int powermax)
{
	//���е������
	int allcurrent = 0;
	allcurrent += ABS(chassis_motor[0].motordata.current);
	allcurrent += ABS(chassis_motor[1].motordata.current);
	allcurrent += ABS(chassis_motor[2].motordata.current);
	allcurrent += ABS(chassis_motor[3].motordata.current);

	//ͨ�����ʵó�������ǰ���е���������������allcurrent��ͬ
	int Imax = powermax/25;		//�������ֵ����λ mA��ʵ�ʹ����ѹ25.5
	Imax = Imax*8192/10000;		//ת��������������ݵ�����
	if(Imax < allcurrent)		//����������
	{
		chassis_motor[0].speedpid.out = ((int)chassis_motor[0].speedpid.out)*Imax/allcurrent;
		chassis_motor[1].speedpid.out = ((int)chassis_motor[1].speedpid.out)*Imax/allcurrent;
		chassis_motor[2].speedpid.out = ((int)chassis_motor[2].speedpid.out)*Imax/allcurrent;
		chassis_motor[3].speedpid.out = ((int)chassis_motor[3].speedpid.out)*Imax/allcurrent;
		return 1;
	}
	return 0;					//�����޷���0
}
/**
* @brief�������ĸ�������趨�����ֱ��������������������
				�ڳ�ʼ��֮��ú���ÿ����x ms��ִ��һ��
* @param [in]	parameter:�ò������ᱻʹ��
* @return��		��
* @author��mqy
*/
//2ms����
static struct rt_timer task_2ms;
static struct rt_semaphore chassis_2ms_sem;
static void task_2ms_IRQHandler(void *parameter)
{
	rt_sem_release(&chassis_2ms_sem);
}
static void chassis_control_thread(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= CHASSIS_CTLID;//����ID
	wheelc_message.ide	= RT_CAN_STDID;	//��׼֡
	wheelc_message.rtr	= RT_CAN_DTR;	//����֡
	wheelc_message.priv = 0;			//�������ȼ����
	wheelc_message.len = 8;				//����8
	
	while(1)
	{
		rt_sem_take(&chassis_2ms_sem, RT_WAITING_FOREVER);
		chassis_control();
		for(int a = 0;a<4;a++)
		{
			pid_output_calculate(&chassis_motor[a].speedpid,chassis_motor[a].speedpid.set,chassis_motor[a].motordata.speed);
			//chassis_motor[a].motorpid.out = 200;
		}

		//���Դ�����й�������
		//power_limit(2000);
		
		//��������
		wheelc_message.data[0] = chassis_motor[0].speedpid.out>>8;
		wheelc_message.data[1] = chassis_motor[0].speedpid.out;
		wheelc_message.data[2] = chassis_motor[1].speedpid.out>>8;
		wheelc_message.data[3] = chassis_motor[1].speedpid.out;
		wheelc_message.data[4] = chassis_motor[2].speedpid.out>>8;
		wheelc_message.data[5] = chassis_motor[2].speedpid.out;
		wheelc_message.data[6] = chassis_motor[3].speedpid.out>>8;
		wheelc_message.data[7] = chassis_motor[3].speedpid.out;
		
		if(!rt_device_write(can1_dev,0,&wheelc_message,sizeof(wheelc_message)))
		{
			//�����������Ϊ0����һ�η���ʧ�ܣ�ʧ�ܴ������෢������
		}
		else
		{

		}
	}
}
/**
* @brief����ʼ�����̵��
* @param [in]	��
* @return��		true:��ʼ���ɹ�
				false:��ʼ��ʧ��
* @author��mqy
*/
int chassis_init(void)
{
    rt_sem_init(&chassis_2ms_sem, "2ms_sem", 0, RT_IPC_FLAG_FIFO);
	
    //��ʼ�������߳�
	chassis_ctrl_handler = rt_thread_create(
	"chassis_control",		//�߳���
	chassis_control_thread,	//�߳����
	RT_NULL,				//��ڲ�����
	2048,					//�߳�ջ
	1,	                    //�߳����ȼ�
	2);						//�߳�ʱ��Ƭ��С

	//�̴߳���ʧ�ܷ���false
	if(chassis_ctrl_handler == RT_NULL)
	{
		return 0;
	}

    //�߳�����ʧ�ܷ���false
	if(rt_thread_startup(chassis_ctrl_handler) != RT_EOK)
	{
		return 0;
	}

    //�����̶߳�ʱ��
	rt_timer_init(&task_2ms,
                "2ms_task",
                task_2ms_IRQHandler,
				RT_NULL,
                2, 
                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    //������ʱ��
	rt_timer_start(&task_2ms);

    //�����ɹ���ʼ��ʼ������
	for(rt_uint8_t a = 0;a<4;a++)
	{
		chassis_motor[a].motorID = (drv_can1ID_e)(0x201 + a);//��ʼ��ID��ֵ
	}
	pid_init(&chassis_motor[0].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&chassis_motor[1].speedpid,6,0.004,0.2,500,8000,-8000);

	
	return 1;
}
/**
* @brief�����ղ����趨����̨Ϊǰ�����ٶ�
* @param [in]	follow_angle:�趨����Ƕȣ���λ�� ��Χ0-8191��������Χ��ֵ��ת�����÷�Χ��
				�� ONLY_CHASSIS ģʽ�¸ò���Ϊǰ��
				angular_velocity:�趨������ת���ٶȣ���λ 0.1 ��/s �� pi/1800 rad/s
				xspeed:������x���ٶȷ�������λmm/s
				yspeed:������y���ٶȷ�������λmm/s
* @return��		��
* @author��mqy
*/
void chassis_speed_set(rt_uint16_t follow_angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed)
{
	while(follow_angle > 8191)
	{
		follow_angle -= 8191;
	}

	motion_data.xspeed = xspeed;
	motion_data.yspeed = yspeed;
}
/**
* @brief���趨�˶�ģʽ
* @param [in]	sport_mode:�������˶�ģʽ
* @return��		��
* @author��mqy
*/
void sport_mode_set(sport_mode_e sport_mode)
{
	motion_data.sport_mode = sport_mode;
}
/**
* @brief��refresh_motor_data������ʹ�øú�������������
* @param [in]	message:���յ�������ָ֡��
				motordata:Ҫ���µĵ�����ݽṹ��ָ��
* @return��		��
* @author��mqy
*/
static void assign_motor_data(motordata_t* motordata,struct rt_can_msg* message)
{
	motordata->angle = (message->data[0]<<8) + message->data[1];	//ת�ӽǶ�
	motordata->speed = (message->data[2]<<8) + message->data[3];	//ת��ת��
	motordata->current = (message->data[4]<<8) + message->data[5];  //ʵ�ʵ���
	motordata->temperature = message->data[6];						//�¶�
}
/**
* @brief�����øú������µ��̵����ǰת�ٵ�����
* @param [in]	message:���յ�������ָ֡��
* @return��		true:���³ɹ�
				false:id��ƥ�����ʧ��
* @author��mqy
*/
int refresh_chassis_motor_data(struct rt_can_msg* message)
{
	rt_uint8_t coordinate = message->id - 0x201;
	
	//�����ĸ��������
	if(coordinate < 4)
	{
		assign_motor_data(&chassis_motor[coordinate].motordata,message);
		return 1;//���������ݾͷ���
	}

	return 0;
}
int refresh_gimbal_motor_data(struct rt_can_msg* message)
{
	//��������
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&motion_data.yaw_data,message);
			//ת���Ƕ���ֵ������ϵ
			if(motion_data.yaw_data.angle < YAW_ZERO_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAW_ZERO_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAW_ZERO_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&motion_data.pitch_data,message);
			return 1;
			
		default:
			
			break;
	}
	return 0;
}

#endif
