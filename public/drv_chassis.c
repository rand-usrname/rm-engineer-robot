#include "drv_chassis.h"

//���̵��PID�߳̾��
static rt_thread_t 		chassis_control 	= RT_NULL;

//������ƽṹ�壬����0��Ӧ��СID
static motor_t			chassis_motor[4];

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

    //�����˶�����
	#ifdef MECANUM_WHEEL
	    float v1 = (float)1.41421*(y + x) + angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	    float v2 = (float)1.41421*(y - x) - angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	    float v3 = (float)1.41421*(y - x) + angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	    float v4 = (float)1.41421*(y + x) - angular_velocity*((float)3.1415926/(float)1800.0*(VEHICLE_WIDTH + VEHICLE_LONG));
	
	    //2.40946Ϊ��λת����ϵ����vΪ�������������ٶȣ������ٶ�ʹ��rpm��Ϊ��λ
	    chassis_motor[(rt_uint8_t)(LEFT_FRONT- 0x201)].expected_speed = (rt_int16_t)((float)2.40946*v1);
	    chassis_motor[(rt_uint8_t)(RIGHT_FRONT- 0x201)].expected_speed = -(rt_int16_t)((float)2.40946*v2);
	    chassis_motor[(rt_uint8_t)(LEFT_BACK- 0x201)].expected_speed = (rt_int16_t)((float)2.40946*v3);
	    chassis_motor[(rt_uint8_t)(RIGHT_BACK- 0x201)].expected_speed = -(rt_int16_t)((float)2.40946*v4);
    #endif
    //ȫ�����˶�����
    #ifdef OMNI_WHEEL
	    float v1 = (float)x + (float)angular_velocity*((float)3.1415926/(float)1800.0*VEHICLE_DIAMETER);
	    float v2 = (float)y - (float)angular_velocity*((float)3.1415926/(float)1800.0*VEHICLE_DIAMETER);
	    float v3 = (float)y + (float)angular_velocity*((float)3.1415926/(float)1800.0*VEHICLE_DIAMETER);
	    float v4 = (float)x - (float)angular_velocity*((float)3.1415926/(float)1800.0*VEHICLE_DIAMETER);
	
	    //2.40946Ϊ��λת����ϵ����vΪ�������������ٶȣ������ٶ�ʹ��rpm��Ϊ��λ
	    chassis_motor[(rt_uint8_t)(LEFT_FRONT- 0x201)].expected_speed = (rt_int16_t)((float)2.40946*v1);
	    chassis_motor[(rt_uint8_t)(RIGHT_FRONT- 0x201)].expected_speed = -(rt_int16_t)((float)2.40946*v2);
	    chassis_motor[(rt_uint8_t)(LEFT_BACK- 0x201)].expected_speed = (rt_int16_t)((float)2.40946*v3);
	    chassis_motor[(rt_uint8_t)(RIGHT_BACK- 0x201)].expected_speed = -(rt_int16_t)((float)2.40946*v4);
    #endif
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
static void chassis_contral_thread(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= MOTOR_CONTROL;  //����ID
	wheelc_message.ide	= RT_CAN_STDID;	//��׼֡
	wheelc_message.rtr	= RT_CAN_DTR;	//����֡
	wheelc_message.priv = 0;			//�������ȼ����
	wheelc_message.len = 8;				//����8
	rt_int16_t motor_error;				//��ʱ�洢���ƫ��
	
	while(1)
	{
		rt_sem_take(&chassis_2ms_sem, RT_WAITING_FOREVER);
		chassis_contral();
		for(int a = 0;a<4;a++)
		{
			motor_error = chassis_motor[a].expected_speed - chassis_motor[a].motordata.speed;
			pid_output_calculate(&chassis_motor[a].speedpid,motor_error);
			//chassis_motor[a].motorpid.out = 200;
		}
		
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
	chassis_control = rt_thread_create(
	"chassis_control",		//�߳���
	chassis_contral_thread,	//�߳����
	RT_NULL,				//��ڲ�����
	2048,					//�߳�ջ
	1,	                    //�߳����ȼ�
	2);						//�߳�ʱ��Ƭ��С

	//�̴߳���ʧ�ܷ���false
	if(chassis_control == RT_NULL)
	{
		return 0;
	}

    //�߳�����ʧ�ܷ���false
	if(rt_thread_startup(chassis_control) != RT_EOK)
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
		chassis_motor[a].expected_speed = 0;
		chassis_motor[a].motorID = (drv_can1ID_e)(0x201 + a);//��ʼ��ID��ֵ
	}
	pid_init(&chassis_motor[0].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&chassis_motor[1].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&chassis_motor[2].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&chassis_motor[3].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&motion_data.anglepid,2,0,10,10,1000,-1000);
	motion_data.follow_angle = 0;
	motion_data.angular_velocity = 0;
	motion_data.xspeed = 0;
	motion_data.yspeed = 0;
	motion_data.sport_mode = NO_FOLLOW;//Ĭ�ϲ�����
	return 1;
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
			if(motion_data.yaw_data.angle < YAE_ZERO_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAE_ZERO_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAE_ZERO_ANGLE;
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