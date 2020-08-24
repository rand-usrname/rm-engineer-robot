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
static int angpid_cal(gimbalmotor_t* motor,int gyroangle)
{
	switch (motor->angdata_source)
	{
	case GYRO:
		motor->angpid_gyro.set = motor->setang;
		return pid_output_motor(&motor->angpid_gyro,motor->angpid_gyro.set,gyroangle);

	case DJI_MOTOR:
		motor->angpid_dji.set = motor->setang;
		return pid_output_motor(&motor->angpid_dji,motor->angpid_dji.set,motor->motordata.angle);

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
static int gimbalpid_cal(gimbalmotor_t* motor,int gyroangle,int gyropal,rt_uint8_t angle_time)
{
	switch (motor->control_mode)
	{
	case ANGLE:
		if(angle_time > 9)
		{	
			//�����ﵽ10���ٽ��м���
			motor->palpid.set = angpid_cal(motor,gyroangle);
		}
	case PALSTANCE:
		//���ٶȻ���һ���ᱻ�����
		pid_output_calculate(&(motor->palpid),motor->palpid.set,gyropal);
		break;
	
	default://���������������
		motor->palpid.out = 0;
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
	//��ʼ��CAN����֡
	struct rt_can_msg gimctl_msg;
	gimctl_msg.id	= GIMBAL_CTL;	//����ID
	gimctl_msg.ide	= RT_CAN_STDID;	//��׼֡
	gimctl_msg.rtr	= RT_CAN_DTR;	//����֡
	gimctl_msg.priv = 0;			//�������ȼ����
	gimctl_msg.len = 8;				//����8
	
	//������������
	for(int a = 0;a<8;a++)
	{
		gimctl_msg.data[a] = 0;
	}

	rt_uint8_t angle_time = 0;		//��¼�ٶȻ�������ִ�нǶȻ�
	int yawang,yawpal,pitchang,pitchpal;
	
	while(1)
	{
		rt_sem_take(&gimbal_1ms_sem, RT_WAITING_FOREVER);

		//����ִ�нǶȻ���1-10��10��״̬
		if(angle_time > 9)
		{
			angle_time = 0;
		}
		angle_time++;
		
		//��������ת��
		yawang = (rt_uint16_t)((gimbal_atti.yaw + 180.0f)*8192.0f/360.0f);
		yawpal = (int)((gimbal_atti.yaw_speed)*8192.0f/360.0f);
		pitchang = (rt_uint16_t)((gimbal_atti.pitch + 180.0f)*8192.0f/360.0f);
		pitchpal = (int)((gimbal_atti.pitch_speed)*8192.0f/360.0f);
		
		//pitch����λ
		if(pitch.setang < ((rt_uint16_t)PITCH_MIN_ANGLE + 4096) % 8192)
		{
			pitch.setang = ((rt_uint16_t)PITCH_MIN_ANGLE + 4096) % 8192;
		}
		else if(pitch.setang > ((rt_uint16_t)PITCH_MAX_ANGLE + 4096) % 8192)
		{
			pitch.setang = ((rt_uint16_t)PITCH_MAX_ANGLE + 4096) % 8192;
		}
		//������̨����PID
		gimbalpid_cal(&yaw,yawang,yawpal,angle_time);
		gimbalpid_cal(&pitch,pitchang,pitchpal,angle_time);

		//�ڶ�Ӧλ��д����������
		gimctl_msg.data[(rt_uint16_t)(PITCH_ID - 0x205)*2] = pitch.palpid.out>>8;
		gimctl_msg.data[(rt_uint16_t)(PITCH_ID - 0x205)*2 + 1] = pitch.palpid.out;
		gimctl_msg.data[(rt_uint16_t)(YAW_ID - 0x205)*2] = yaw.palpid.out>>8;
		gimctl_msg.data[(rt_uint16_t)(YAW_ID - 0x205)*2 + 1] = yaw.palpid.out;

		//������ڵڶ�����̨���
		#ifdef DUAL_PITCH_MOTOR
			gimctl_msg.DATA[(rt_uint16_t)(DUAL_PITCH_ID - 0x205)*2] = (-yaw.palpid.out)>>8;
			gimctl_msg.DATA[(rt_uint16_t)(DUAL_PITCH_ID - 0x205)*2 + 1] = (-yaw.palpid.out);
		#endif

		if(!rt_device_write(can1_dev,0,&gimctl_msg,sizeof(gimctl_msg)))
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
	//��ʼ���ṹ������
	yaw.control_mode = ANGLE;//Ĭ������λ�ÿ���
	pitch.control_mode = ANGLE;

	yaw.motorID = YAW_ID;
	yaw.angdata_source = GYRO;//Ĭ������Դ������
	yaw.setang = 4095;//��ʼ��Ĭ�ϽǶ�

	pitch.motorID = PITCH_ID;
	pitch.angdata_source = GYRO;//Ĭ������Դ������
	pitch.setang = 4096;//��ʼ��Ĭ�ϽǶ�

	//��ʼ��PID
	pid_init(&yaw.palpid,10,0.1,20,200,0X7FFF,-0X7FFF);
	pid_init(&pitch.palpid,4,0,0,3000,0X7FFF,-0X7FFF);

	pid_init(&yaw.angpid_gyro,15,0.01,20,3,20000,-20000);
	pid_init(&yaw.angpid_dji,8,0.01,0,3,2000,-2000);
	pid_init(&pitch.angpid_gyro,17,0.05,0,3,20000,-20000);
	pid_init(&pitch.angpid_dji,8,0,0,5,2000,-2000);
	
	//��ʼ���ж��ͷŵ��ź���
	rt_sem_init(&gimbal_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	
	//��ʼ����̨�߳�
	gimbal_control = rt_thread_create(
	"gimbal_control",		//�߳���
	gimbal_contral_thread,	//�߳����
	RT_NULL,				//��ڲ�����
	2048,					//�߳�ջ
	1,	                    //�߳����ȼ�
	1);						//�߳�ʱ��Ƭ��С


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
	rt_uint16_t temang;
	//��������
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&yaw.motordata,message);
			//ת���Ƕ���ֵ������ϵ
			temang = (PITCH_ZERO_ANGLE + 4096) % 8192;
			if(yaw.motordata.angle < temang)
			{
				yaw.motordata.angle = 8191 - temang + yaw.motordata.angle;
			}
			else
			{
				yaw.motordata.angle = yaw.motordata.angle - temang;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&pitch.motordata,message);
			//ת���Ƕ���ֵ������ϵ
			temang = (PITCH_ZERO_ANGLE + 4096) % 8192;
			if(pitch.motordata.angle < temang)
			{
				pitch.motordata.angle = 8191 - temang + pitch.motordata.angle;
			}
			else
			{
				pitch.motordata.angle = pitch.motordata.angle - temang;
			}
			return 1;
			
		default:
			
			break;
	}
	return 0;
}

/**
* @brief��ֱ��������̨���ԽǶ�
* @param [in]	yawset��yaw�����ýǶȣ���ֵ��Ч��Χ0-8191����������Χ�ᱻ��������Χ��
				pitchset��pitch���ýǶȣ���ֵ��Ч��Χ PITCH_MIN_ANGLE - PITCH_MAX_ANGLE
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int gimbal_absangle_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.setang = yawset;
	pitch.setang = pitchset;
	
	//��֤���ݲ�������Χ
	yaw.setang %= 8192;
	pitch.setang %= 8192;

	return 1;
}
/**
* @brief������ʽ������̨�Ƕ�
* @param [in]	yawset��yaw�����ýǶȣ���ֵ��Ч��Χ0-8191����������Χ�ᱻ��������Χ��
				pitchset��pitch���ýǶȣ���ֵ��Ч��Χ PITCH_MIN_ANGLE - PITCH_MAX_ANGLE
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int gimbal_addangle_set(rt_int16_t yawset,rt_int16_t pitchset)
{
	yaw.setang += yawset;
	pitch.setang += pitchset;
	
	yaw.setang %= 8192;
	pitch.setang %= 8192;
	
	return 1;
}
/**
* @brief����̨����ģʽ����
* @param [in]	yawset��yaw�����ģʽ
				pitchset��pitch�����ģʽ
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int gimbal_ctlmode_set(control_mode_t yawset,control_mode_t pitchset)
{
	yaw.control_mode = yawset;
	pitch.control_mode = pitchset;
	//TODO:��֤�ڴ�һ��ģʽ�л�Ϊ��һ��ģʽʱ����̨����ͻȻת��һ���Ƕ�
	return 1;
}
/**
* @brief�����ü��ٶ�
* @param [in]	yawset��yaw�����ýǶȣ���ֵ��Ч��Χ0-8191����������Χ�ᱻ��������Χ��
				pitchset��pitch���ýǶȣ���ֵ��Ч��Χ PITCH_MIN_ANGLE - PITCH_MAX_ANGLE
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int gimbal_palstance_set(rt_int16_t yawset,rt_int16_t pitchset)
{
	//ֻ��Ϊ���ٶȿ���ģʽʱ������ֱ�����ý��ٶ�
	if(yaw.control_mode == PALSTANCE)
	{
		yaw.palpid.set = yawset;
	}
	if(pitch.control_mode == PALSTANCE)
	{
		pitch.palpid.set = pitchset;
	}
	return 1;
}
/**
* @brief��������̨����Ԫ
* @param [in]	yawset��yaw������Դ����ȡֵ��ö��
				pitchset��pitch������Դ����ȡֵ��ö��
* @return��		yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
int angle_datasource_set(data_source_t yawset,data_source_t pitchset)
{
	//��ͬʱ���и�ֵ
	if(yaw.angdata_source != yawset)
	{
		yaw.angdata_source = yawset;
		yaw.setang = get_yawangle();
	}
	if(pitch.angdata_source != pitchset)
	{
		pitch.angdata_source = pitchset;
		pitch.setang = get_pitchangle();
	}
	return 1;
}
/**
* @brief����ȡyaw��Ƕ�
* @param [in]	��
* @return��		��������Դ����yaw��Ƕȣ���ʽ0-8191
* @author��mqy
*/
rt_uint16_t get_yawangle(void)
{
	switch(yaw.angdata_source)
	{
		case GYRO:
			return (rt_uint16_t)((gimbal_atti.yaw + 180.0f)*8192.0f/360.0f);
		case DJI_MOTOR:
			return yaw.motordata.angle;
		default:
			return 0;
	}
}
/**
* @brief����ȡpitch��Ƕ�
* @param [in]	��
* @return��		��������Դ����pitch��Ƕȣ���ʽ0-8191
* @author��mqy
*/
rt_uint16_t get_pitchangle(void)
{
	switch(pitch.angdata_source)
	{
		case GYRO:
			return (rt_uint16_t)((gimbal_atti.pitch + 180.0f)*8192.0f/360.0f);
		case DJI_MOTOR:
			return pitch.motordata.angle;
		default:
			return 0;
	}
}
