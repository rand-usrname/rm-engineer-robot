#include "drv_gimbal.h"

//��̨���PID�߳̾��
static rt_thread_t 		gimbal_control 	= RT_NULL;

//yaw pitch������ݽṹ��
static gimbalmotor_t	yaw;
static gimbalmotor_t	pitch;

/**
* @brief���ú�������ǶȻ������
* @param [in]	motor:��̨���ָ��
				gyrodata:��Ӧ�������������
* @return��		��
* @author��mqy
*/
static int anglepid_calculate(gimbalmotor_t* motor,int gyroangle)
{
	switch (motor->angledata_source)
	{
	case GYRO:
		return pid_output_motor(&motor->anglepid_gyro,motor->anglepid_gyro.set,gyroangle);

	case DJI_MOTOR:
		return pid_output_motor(&motor->anglepid_dji,motor->anglepid_dji.set,gyroangle);

	default:
		return 0;
	}
}
/**
* @brief���ú���������̨���pid��������pid��out��
* @param [in]	motor:��̨���ָ��
				gyrodata:��Ӧ�������������
* @return��		��
* @author��mqy
*/
static int gimbalpid_cal(gimbalmotor_t* motor,int gyroangle,int gyrospeed,rt_uint8_t angle_time)
{
	switch (motor->control_mode)
	{
	case  ANGLE:
		if(angle_time > 9)
		{
			motor->speedpid.set = anglepid_calculate(motor,gyroangle);
		}
	case PALSTANCE:
		if(motor->control_mode == PALSTANCE)
		{
			motor->speedpid.set = motor->set;
		}
		pid_output_calculate(&motor->speedpid,motor->speedpid.set,gyrospeed);

	case CURRENT://��Ϊ�����������õ���ֵ��
		if(motor->control_mode == CURRENT)
		{
			motor->speedpid.out = motor->set;
		}
		return 1;

	default://���������������
		motor->speedpid.out = 0;
		break;
	}
	return 0;
}
/**
* @brief��������̨������趨���������������������
				�ڳ�ʼ��֮��ú���ÿ����x ms��ִ��һ��
* @param [in]	parameter:�ò������ᱻʹ��
* @return��		��
* @author��mqy
*/
//2ms����
static struct rt_timer task_1ms;
static struct rt_semaphore gimbal_1ms_sem;
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&gimbal_1ms_sem);
}
static void gimbal_contral_thread(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= GIMBAL_MCTL;  //����ID
	wheelc_message.ide	= RT_CAN_STDID;	//��׼֡
	wheelc_message.rtr	= RT_CAN_DTR;	//����֡
	wheelc_message.priv = 0;			//�������ȼ����
	wheelc_message.len = 8;				//����8

	rt_uint8_t angle_time = 0;			//��¼�ٶȻ�������ִ�нǶȻ�
	while(1)
	{
		rt_sem_take(&gimbal_1ms_sem, RT_WAITING_FOREVER);

		//����ִ�нǶȻ�
		if(angle_time > 9)
		{
			angle_time = 0;
		}
		angle_time++;

		//������̨�����
		gimbalpid_cal(&yaw,gimbal_atti.yaw,gimbal_atti.yaw_speed,angle_time);
		gimbalpid_cal(&pitch,gimbal_atti.pitch,gimbal_atti.pitch_speed,angle_time);

		//��������
		wheelc_message.data[0] = pitch.speedpid.out>>8;
		wheelc_message.data[1] = pitch.speedpid.out;
		wheelc_message.data[2] = yaw.speedpid.out>>8;
		wheelc_message.data[3] = yaw.speedpid.out;
		wheelc_message.data[4] = 0;
		wheelc_message.data[5] = 0;
		wheelc_message.data[6] = 0;
		wheelc_message.data[7] = 0;

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
* @brief����ʼ����̨�߳�
* @param [in]	��
* @return��		1:��ʼ���ɹ�
				0:��ʼ��ʧ��
* @author��mqy
*/
int gimbal_init(void)
{
	rt_sem_init(&gimbal_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);

	//��ʼ����̨�߳�
	gimbal_control = rt_thread_create(
	"gimbal_control",		//�߳���
	gimbal_contral_thread,	//�߳����
	RT_NULL,				//��ڲ�����
	2048,					//�߳�ջ
	1,	                    //�߳����ȼ�
	2);						//�߳�ʱ��Ƭ��С


	//�̴߳���ʧ�ܷ���false
	if(gimbal_control == RT_NULL)
	{
		return 0;
	}

    //�����̶߳�ʱ��
	rt_timer_init(&task_1ms,
                "1ms_task",
                task_1ms_IRQHandler,
				RT_NULL,
                1, 
                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

	//��ʼ���ṹ������
	yaw.control_mode = ANGLE;//Ĭ������λ�ÿ���
	pitch.control_mode = ANGLE;

	yaw.motorID = YAW_ID;
	yaw.angledata_source = GYRO;//Ĭ������Դ������

	pitch.motorID = PITCH_ID;
	pitch.angledata_source = GYRO;//Ĭ������Դ������

	//��ʼ��PID
	pid_init(&yaw.speedpid,185,0.01,100,100,0X7FFF,-0X7FFF);
	pid_init(&pitch.speedpid,150,0.01,200,3000,0X7FFF,-0X7FFF);

	pid_init(&yaw.anglepid_gyro,0.7,0.01,0,3,20000,-20000);
	pid_init(&yaw.anglepid_dji,0.18,0.01,0,3,2000,-2000);
	pid_init(&pitch.anglepid_gyro,1,0.01,0,3,20000,-20000);
	pid_init(&pitch.anglepid_dji,0.4,0.01,0,5,2000,-2000);

	//�߳�����ʧ�ܷ���false
	if(rt_thread_startup(gimbal_control) != RT_EOK)
	{
		return 0;
	}

    //������ʱ��
	if(rt_timer_start(&task_1ms) != RT_EOK)
	{
		return 0;
	}

	return 1;
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
* @brief�����øú���������̨����������
* @param [in]	message:���յ�������ָ֡��
* @return��		true:���³ɹ�
				false:id��ƥ�����ʧ��
* @author��mqy
*/
int refresh_gimbal_motor_data(struct rt_can_msg* message)
{
	//��������
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&yaw.motordata,message);
			//ת���Ƕ���ֵ������ϵ
			if(yaw.motordata.angle < YAE_ZERO_ANGLE)
			{
				yaw.motordata.angle = 8191 - YAE_ZERO_ANGLE + yaw.motordata.angle;
			}
			else
			{
				yaw.motordata.angle = yaw.motordata.angle - YAE_ZERO_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&pitch.motordata,message);
			return 1;
			
		default:
			
			break;
	}
	return 0;
}

/**
* @brief�����ú���
* @param [in]	data_source:ϣ��������Դ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
void gimbal_current_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = CURRENT;
	pitch.control_mode = CURRENT;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void gimbal_absangle_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = ANGLE;
	pitch.control_mode = ANGLE;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void gimbal_addangle_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = ANGLE;
	pitch.control_mode = ANGLE;
	yaw.set += yawset;
	pitch.set += pitchset;
}
void gimbal_palstance_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = PALSTANCE;
	pitch.control_mode = PALSTANCE;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void angle_datasource_set(data_source_t yawset,data_source_t pitchset)
{
	yaw.angledata_source = yawset;
	pitch.angledata_source = pitchset;
}
/**
* @brief����ȡyaw��Ƕ�
* @param [in]	data_source:ϣ��������Դ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int get_yawangle(void)
{
	return yaw.motordata.angle;
}
/**
* @brief����ȡpitch��Ƕ�
* @param [in]	data_source:ϣ��������Դ
* @return��		pitch��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int get_pitchangle(void)
{
	return pitch.motordata.angle;
}
