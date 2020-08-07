#include "chassis.h"

	
//���̵��ͨ���豸���
static rt_device_t 		chassis_communication	= RT_NULL;

//���̵��PID�߳̾��
static rt_thread_t 		contral_speed_thread 	= RT_NULL;

//������ƽṹ�壬����0��Ӧ��СID
static motor_t				chassis_motor[4];

//�˶����ݣ�������̨����Ƕȣ������ٶȵ�
static motion_data_t	motion_data;


#define ABS(a) (((a)>0)?(a):(-a))
/**
* @brief���ú������ݲ�����ʼ��pid�ṹ�岢����״̬����
* @param [in]	����˵����� motorpid_t �ṹ��
* @return��		��
* @author��mqy
*/
static void pid_init(motorpid_t* motorpid,float kp,float ki,float kd,
					float i_limit,rt_int16_t out_limit_up,rt_int16_t out_limit_down)
{
	//��������
	motorpid->kp = kp;
	motorpid->ki = ki;
	motorpid->kd = kd;
	motorpid->i_limit = i_limit;
	motorpid->out_limit_up = out_limit_up;
	motorpid->out_limit_down = out_limit_down;
	//״̬����
	motorpid->err = 0;
	motorpid->err_old = 0;
	motorpid->i_value = 0;
	motorpid->out = 0;
}
/**
* @brief���ú��������ĸ�������趨�����ֱ����������
* @param [in]	target:Ҫ�����pid�ṹ��
							error:����ֵ��ʵ��ֵ������λrpm
* @return��		��
* @author��mqy
*/
static void pid_output_calculate(motorpid_t* target,rt_int16_t error)
{
	target->err_old = target->err;
	target->err = error;
	target->i_value += target->ki * error;
	
	//�����޷�
	if(target->i_value < 0)
	{
		if(target->i_value < -target->i_limit)
		{
			target->i_value = -target->i_limit;
		}
	}
	else
	{
		if(target->i_value > target->i_limit)
		{
			target->i_value = target->i_limit;
		}
	}
	target->out = target->kp*(float)error + target->i_value + target->kd*(float)(target->err - target->err_old);
	
	//����޷�
	if(target->out > target->out_limit_up)
	{
		target->out = target->out_limit_up;
	}
	else if(target->out < target->out_limit_down)
	{
		target->out = target->out_limit_down;
	}
}
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
static void motor_speed_set(rt_uint16_t angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed)
{
	while(angle > 8191)
	{
		angle -= 8191;
	}
	angle = 8191 - angle;
	//����Ƕȱ任֮������ٶ�
	float y = yspeed*cos((double)(angle)/4096*3.1415926) + xspeed*cos((double)(angle - 6144)/4096*3.1415926);
	float x = -yspeed*cos((double)(angle + 2048)/4096*3.1415926) + xspeed*cos((double)(angle)/4096*3.1415926);
	
	float v1 = (float)1.41421*(y + x) + angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	float v2 = (float)1.41421*(y - x) - angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	float v3 = (float)1.41421*(y - x) + angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	float v4 = (float)1.41421*(y + x) - angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	
	//2.40946Ϊ��λת����ϵ����vΪ�������������ٶȣ������ٶ�ʹ��rpm��Ϊ��λ
	chassis_motor[(rt_uint8_t)(LEFT_FRONT	- MIN_ID)].expected_speed = (rt_int16_t)((float)2.40946*v1);
	chassis_motor[(rt_uint8_t)(RIGHT_FRONT- MIN_ID)].expected_speed = -(rt_int16_t)((float)2.40946*v2);
	chassis_motor[(rt_uint8_t)(LEFT_BACK	- MIN_ID)].expected_speed = (rt_int16_t)((float)2.40946*v3);
	chassis_motor[(rt_uint8_t)(RIGHT_BACK	- MIN_ID)].expected_speed = -(rt_int16_t)((float)2.40946*v4);
}
/**
* @brief���ú��������趨�˶�ģʽ���趨���ٶȽ��ٶȵ��ú���motor_speed_set�趨����ٶ�
					�ڳ�ʼ��֮��ú���ÿ����x ms��ִ��һ��
* @param [in]	��
* @return��		��
* @author��mqy
*/
static void chassis_contral(void)
{
	switch(motion_data.sport_mode)
	{
		case NO_FOLLOW:
		{
			motor_speed_set(
			motion_data.yaw_data.angle,			//���뵱ǰ�ǶȲ���
			motion_data.angular_velocity,		//������ʱ���뵱ǰ���ٶ�
			motion_data.xspeed,
			motion_data.yspeed
			);
			//chassis_speed_set();
			break;
		}
		case FOLLOW:
		{
			//�ڸ���ģʽ����Ҫ�����趨���ٶ�
			rt_int16_t error = motion_data.follow_angle - motion_data.yaw_data.angle;
			if(ABS(error) > (8191 - ABS(error)))
			{
				if(error > 0)
				{
					error = 8191 - error;
				}
				else
				{
					error = 8191 + error;
				}
			}
			pid_output_calculate(&motion_data.anglepid,error);
			
			motor_speed_set(
			motion_data.yaw_data.angle,//���뵱ǰ�ǶȲ���
			motion_data.anglepid.out,//����ʱ����ǶȻ��ջ��Ĳ���
			motion_data.xspeed,
			motion_data.yspeed
			);
			break;
		}
		default:
			
			break;
	}
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
static void chassis_speed_contral(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= MOTOR_MESSAGE_ID;	//����ID
	wheelc_message.ide	= RT_CAN_STDID;		//��׼֡
	wheelc_message.rtr	= RT_CAN_DTR;		//����֡
	wheelc_message.priv = 0;				//�������ȼ����
	wheelc_message.len = 8;					//����8
	rt_int16_t motor_error;					//��ʱ�洢���ƫ��
	rt_uint16_t send_failure = 0;			//���ڼ�¼����ʧ�ܴ���
	
	while(1)
	{
		rt_sem_take(&chassis_2ms_sem, RT_WAITING_FOREVER);
		chassis_contral();
		for(int a = 0;a<4;a++)
		{
			motor_error = chassis_motor[a].expected_speed - chassis_motor[a].motordata.speed;
			pid_output_calculate(&chassis_motor[a].motorpid,motor_error);
			//chassis_motor[a].motorpid.out = 200;
		}
		
		//��������
		wheelc_message.data[0] = chassis_motor[0].motorpid.out>>8;
		wheelc_message.data[1] = chassis_motor[0].motorpid.out;
		wheelc_message.data[2] = chassis_motor[1].motorpid.out>>8;
		wheelc_message.data[3] = chassis_motor[1].motorpid.out;
		wheelc_message.data[4] = chassis_motor[2].motorpid.out>>8;
		wheelc_message.data[5] = chassis_motor[2].motorpid.out;
		wheelc_message.data[6] = chassis_motor[3].motorpid.out>>8;
		wheelc_message.data[7] = chassis_motor[3].motorpid.out;
		
		if(!rt_device_write(chassis_communication,0,&wheelc_message,sizeof(wheelc_message)))
		{
			//�����������Ϊ0����һ�η���ʧ�ܣ�ʧ�ܴ������෢������
			send_failure++;
			if(send_failure > 500)//����ʧ��һ��ʱ����;���
			{
				rt_kprintf("cant send chassis message,please find mqy\n");
			}
		}
		else
		{
			send_failure = 0;
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
bool chassis_init(void)
{
	chassis_communication = rt_device_find("can1");
	//�����Ҳ����豸���ʱ��ʼ��ʧ��
	if(!chassis_communication)
	{
		return false;
	}
	
	if(RT_EOK != rt_device_control(chassis_communication, RT_CAN_CMD_SET_BAUD, (void *)CAN1MBaud))
	{
		return false;
	}
	
	if(RT_EOK != rt_device_control(chassis_communication, RT_CAN_CMD_SET_MODE, (void *)RT_CAN_MODE_NORMAL))
	{
		return false;
	}
//	//���Դ��豸������һ������״̬����ʱ����i���ñ���ֻ��ʹ����һ��
//	rt_err_t i = rt_device_open(chassis_communication,RT_DEVICE_FLAG_INT_TX 
//																									| RT_DEVICE_FLAG_INT_RX );
//	//����ʧ��ʱ����false
//	if(!((i == RT_EOK)||(i == -RT_EBUSY)))
//	{
//		return false;
//	}
	
	rt_sem_init(&chassis_2ms_sem, "2ms_sem", 0, RT_IPC_FLAG_FIFO);
	//��û�д������߳��򴴽�PID�����߳�
	if(contral_speed_thread == RT_NULL)
	{
		contral_speed_thread = rt_thread_create(
		"contral_speed_thread",		//�߳���
		chassis_speed_contral,		//�߳����
		RT_NULL,									//��ڲ�����
		2048,											//�߳�ջ
		CHASSIS_CONTRAL_PRIORITY,	//�߳����ȼ�
		2);												//�߳�ʱ��Ƭ��С
	}
	
	//�̴߳���ʧ�ܷ���false
	if(contral_speed_thread == RT_NULL)
	{
		return false;
	}
	
	//�߳�����ʧ�ܷ���false
	if(rt_thread_startup(contral_speed_thread) != RT_EOK)
	{
		return false;
	}
	
	//�����̶߳�ʱ��
	rt_timer_init(&task_2ms,
                 "2ms_task",
                 task_2ms_IRQHandler,
								 RT_NULL,
                 2, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	
	//������ʱ��
	rt_timer_start(&task_2ms);
	
	//�����ɹ���ʼ��ʼ������
	for(uint16_t a = 0;a<4;a++)
	{
		chassis_motor[a].expected_speed = 0;
		chassis_motor[a].motorID = (motorID_e)(MIN_ID + a);//��ʼ��ID��ֵ
	}
	pid_init(&chassis_motor[0].motorpid,7,0.005,5,500,8000,-8000);
	pid_init(&chassis_motor[1].motorpid,7,0.005,5,500,8000,-8000);
	pid_init(&chassis_motor[2].motorpid,7,0.005,5,500,8000,-8000);
	pid_init(&chassis_motor[3].motorpid,7,0.005,5,500,8000,-8000);
	pid_init(&motion_data.anglepid,2,0,10,10,1000,-1000);
	motion_data.follow_angle = 0;
	motion_data.angular_velocity = 0;
	motion_data.xspeed = 0;
	motion_data.yspeed = 0;
	motion_data.sport_mode = NO_FOLLOW;//Ĭ�ϲ�����
	return true;
}
/**
* @brief�����ղ����趨����̨Ϊǰ�����ٶ�
* @param [in]	follow_angle:�趨����Ƕȣ���λ�� ��Χ0-360��������Χ�Ļ�ת�����÷�Χ��
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
	motion_data.follow_angle = follow_angle;
	motion_data.angular_velocity = angular_velocity;
	motion_data.xspeed = xspeed;
	motion_data.yspeed = yspeed;
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
	motordata->current = (message->data[4]<<8) + message->data[5];//ʵ�ʵ���
	motordata->temperature = message->data[6];										//�¶�
}
/**
* @brief�����øú������µ��̵����ǰת�ٵ�����
* @param [in]	message:���յ�������ָ֡��
* @return��		true:���³ɹ�
							false:id��ƥ�����ʧ��
* @author��mqy
*/
bool refresh_chassis_motor_data(struct rt_can_msg* message)
{
	rt_uint8_t coordinate = message->id - MIN_ID;
	
	//�����ĸ��������
	if(coordinate < 4)
	{
		assign_motor_data(&chassis_motor[coordinate].motordata,message);
		return true;//���������ݾͷ���
	}
	return false;
}
bool refresh_yuntai_motor_data(struct rt_can_msg* message)
{
	//��������
	switch(message->id)
	{
		case YAW:
			assign_motor_data(&motion_data.yaw_data,message);
			//ת���Ƕ���ֵ������ϵ
			if(motion_data.yaw_data.angle < YAW_START_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAW_START_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAW_START_ANGLE;
			}
			return true;
			
		case PITCH:
			assign_motor_data(&motion_data.pitch_data,message);
			return true;
			
		default:
			
			
			break;
	}
	return false;
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
