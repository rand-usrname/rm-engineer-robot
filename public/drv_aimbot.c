#include "drv_aimbot.h"
#include "math.h"
#include "drv_gyro.h"

/**********���Ӿ���ͨ�Ų���***********/

//�Ӿ�������Ϣ�ṹ��
static visual_ctl_t visual_ctl;

//�Ӿ�������Ϣ�ṹ��
static visual_rev_t visual_rev;
/**
* @brief����ʼ���Ӿ��ṹ��
* @param [in]	��
* @return��		true:��ʼ���ɹ�
							false:��ʼ��ʧ��
* @author��mqy
*/
int vision_init(void)
{
	//������Ϣ����
	visual_rev.computime = 0;
	visual_rev.aim_mode = BLANK;
	visual_rev.forcester = COMPUTER;
	visual_rev.pitchadd = 0;
	visual_rev.yawadd = 0;
	visual_rev.x = 0;
	visual_rev.y = 0;
	visual_rev.z = 0;
	visual_rev.yaw_usetime = 0;
	visual_rev.pitch_usetime = 0;

	//������Ϣ��ΪĬ��ֵ
	visual_ctl.aim_mode = BLANK;
	visual_ctl.forcester = COMPUTER;
	visual_ctl.tracolor = RED;

	return 1;
}
INIT_APP_EXPORT(vision_init);

/**
* @brief�����ղ������Ӿ�ʶ����Ϣ
* @param [in]	data:����Ϊ8������ָ��
* @return��		1:���³ɹ�
				0:����ʧ��
* @author��mqy
*/
int refresh_visual_data(rt_uint8_t* data)
{
	visual_rev.yaw_usetime = 0;
	visual_rev.pitch_usetime = 0;
	visual_rev.aim_mode = (aim_mode_e)((data[0]>>5) & 0X7);//ȡǰ��λ
	visual_rev.forcester = (forecast_e)((data[0]>>4) & 0X1);//ȡ����λ
	visual_rev.computime = (rt_uint8_t)(data[1]);
	switch (visual_rev.aim_mode)
	{
	case RUNE:
	case BIG_ARMOR:
	case SMALL_ARMOR:
	case BASE_ARMOR:
	case ENG_ARMOE:
		switch(visual_rev.forcester)
		{
		case COMPUTER:
			visual_rev.yawadd = (rt_int16_t)((data[2]<<8) + data[3]);
			visual_rev.pitchadd = (rt_int16_t)((data[4]<<8) + data[5]);
			break;
	
		case SIGNALCHIP:
			visual_rev.x = ((rt_int16_t)((data[2]<<8) + data[3]))/1000.0f;
			visual_rev.y = ((rt_int16_t)((data[4]<<8) + data[5]))/1000.0f;
			visual_rev.z = ((rt_int16_t)((data[6]<<8) + data[7]))/1000.0f;
			break;

		default:
			break;
		}
		break;

	case CAISSON:
		break;
	
	case BLANK:	//��Ϊ��ֵĬ�Ϸ���
	default:	//��Ϊ�������Ĭ�Ϸ���
		break;
	}
	return 1;
}
/**
* @brief:��ȡ���ֽڵ�����֡
* @param:data:�������ֽڿռ������ָ��
		 pitch_ang:pitch��Ƕȣ���Χ-32768 ~ 32767 
		 yaw_ang:yaw��Ƕȣ���Χ-32768 ~ 32767 
		 bullet_vel:�����ӵ��ٶȣ���λmm/s
* @return����
* @author��mqy
*/
void ctldata_get(rt_uint8_t data[],rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	data[0] = (visual_ctl.aim_mode << 5) | (visual_ctl.forcester << 4) | (visual_ctl.tracolor << 3);
	data[1] = yaw_ang >> 8;
	data[2] = yaw_ang;
	data[3] = pitch_ang >> 8;
	data[4] = pitch_ang;
	data[5] = bullet_vel >> 8;
	data[6] = bullet_vel;
	data[7] = 0;
}

/**
* @brief:ͨ��CAN���Ӿ�������Ϣ
* @param:pitch_ang:pitch��Ƕȣ���Χ-32768 ~ 32767 
		 yaw_ang:yaw��Ƕȣ���Χ-32768 ~ 32767 
		 bullet_vel:�����ӵ��ٶȣ���λmm/s
* @return��		��
* @author��mqy
*/
int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	struct rt_can_msg txmsg;
	txmsg.id = VISUAL_CTLID;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr = RT_CAN_DTR;
	txmsg.len = 8;
	ctldata_get(txmsg.data,yaw_ang,pitch_ang,bullet_vel);
	rt_device_write(can2_dev,0,&txmsg,sizeof(txmsg));
	
	return 1;
}
/**
* @brief:ͨ��UART���Ӿ�������Ϣ
* @param:dev:UART�豸���
		 pitch_ang:pitch��Ƕȣ���Χ-32768 ~ 32767
		 yaw_ang:yaw��Ƕȣ���Χ-32768 ~ 32767
		 bullet_vel:�����ӵ��ٶȣ���λmm/s
* @return��		��
* @author��mqy
*/
int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	rt_uint8_t data[11];
	data[0] = 0X3B;		//֡ͷ
	data[9] = 0;		//��У��λ
	data[10] = ~0X3B;	//֡β
	ctldata_get(&data[1],yaw_ang,pitch_ang,bullet_vel);
	for(int a = 1; a<9; a++)
	{
		data[9] += data[a];
	}
	rt_device_write(dev,0,data,11);//д��11���ֽڵ�����
	
	return 1;
}

/**
* @brief:��ȡ�Ӿ�����
* @param:��
* @return�����غ�������Ӧ�ĺ���
* @author��mqy
*/
rt_int16_t get_yaw_add(void)
{
	visual_rev.yaw_usetime++;
	return visual_rev.yawadd;
}
rt_int16_t get_pitch_add(void)
{
	visual_rev.pitch_usetime++;
	return visual_rev.pitchadd;
}
rt_int16_t get_yawusetime(void)
{
	return visual_rev.yaw_usetime;
}
rt_int16_t get_pitchusetime(void)
{
	return visual_rev.pitch_usetime;
}
/**********���Ӿ���ͨ�Ų��ݽ���***********/


#define G   9.7988f 
#define t_transmit 3
#define pi_operator 0.01745f
//�ӿ� 
Aimbot_t Aimbot_data;

//����һ��������� �����ڴ洢��40ms�ڵĽ��ٶȡ�
static float Palstance_Deque[40] = {0.0f,};

static float vision_yaw_palstance = 0.0f;
//��Ҫ��ȡ�ӵ��ٶ�
static float bullet_speed = 0.0f;


//���ٿ���
static float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
 
    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       
    i  = 0x5f3759df - ( i >> 1 );               
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );  
    return y;
}

/* �������к�������ѭ ����ֵ�򴫲�
   ��λ ��
   ���� pitch����Ϊ�� yaw�����̨�Ϸ����¿���ʱ��Ϊ��
   �Ӿ�yaw�᷽��������෴ ����ת��
*/


/**
  * @brief  ������䵱ǰʱ���yaw����ٶ�(1msһ��) ��λ ��/s
  * @param  pdata is Palstance_Deque(�洢yaw���ٶȵĶ���)  
  * @param  gimbal_atti ���������ݽṹ��
  * @retval none
 **/
static void get_new_palstance(float *pdata,ATTI_t *gimbal_atti)
{
	float *temp = pdata;
	float motor_palstance =  gimbal_atti->yaw_speed;
    Palstance_Deque[0] = motor_palstance;
	rt_memcpy((Palstance_Deque+1),temp,39);
}

/* �Ӿ����Ѿ���������ϵ�任 */
//����ϵת��������ͷ����ϵתΪ�������ϵ ��ԭ��Ϊ�������Ħ����λ�ô�
//static Point_t Transformed_coordinate(Point_t* vision_point,float pitch_angle)
//{
//	Point_t motor_point;
//	motor_point.x = vision_point->x;
//    motor_point.y = -(vision_point->z) * sin(pitch_angle) + vision_point->y * cos(pitch_angle);
//    motor_point.z = vision_point->z * cos(pitch_angle) + vision_point->y * sin(pitch_angle);

//	return motor_point;
//}

/**
  * @brief   ͨ���Ӿ����ص�Ŀ���������ݼ���ԭʼ��pitch yaw ����(����������Ӱ�켰Ԥ����) 
  * @param   �Ӿ����͵�Ŀ������
  * @retval  ����һ���洢��ԭʼpitch yaw �����Ľṹ�� ����Ϊ0-360��
 **/
static Aimbot_t Calc_from_point(visual_rev_t *visual_rev)
{
	Aimbot_t calc_angle;
	calc_angle.pitch_add = atan((visual_rev->y)/(visual_rev->z))/pi_operator;
	/* yaw�᷽��ȡ�� */
	calc_angle.yaw_add = -atan((visual_rev->x)/(visual_rev->z))/pi_operator;
	return calc_angle;
}

/**
  * @brief  ��Calc_from_point�����pitch_add������������������������Ӱ��
  * @param  current_pitch  ��ǰpitch�Ƕ� ��λΪ��
  * @param  delta_pitch  Calc_from_point�����Ϊ������pitch_add ��λΪ��
  * @distance ��Ŀ���ֱ�߾��� ��λΪm 
  * @bullet_speed �ӵ��ٶ� ��λ m/s
  * @retval ���ز������pitch_add ����0-360��
 **/
static float Gravity_compensation(float current_pitch, float delta_pitch,float distance,rt_uint16_t bullet_speed)
{
	/* ת��Ϊ������ */
	float target_pitch = (delta_pitch + current_pitch)*pi_operator;
	
	/* x_0��ǰΪ�� y_0��������������ٶ�ͬ�� */
	float x_0 = distance * cos(-target_pitch);
	float y_0 = distance * sin(-target_pitch);
	
	float temp = (bullet_speed*bullet_speed*bullet_speed*bullet_speed)-G*(G*x_0*x_0-2*bullet_speed*bullet_speed*y_0);
	if(temp < 0) return 0;  /* ��̲��� */
	else
	{
		/* ���㲹����ĽǶ� ��λrad*/
		float aim_pitch = -atan((-bullet_speed*bullet_speed+Q_rsqrt(temp))/(G*x_0));
		/* ת���ɡ�󷵻����� */
		return (aim_pitch/pi_operator-current_pitch);
	}
}


/**
  * @brief   �õ�yaw�����ǰ��Ԥ��Ƕ�
  * @param   vision_palstance �Ӿ�����ͷ�жԷ�����Խ��ٶ�(�ɸ�����֡�ĶԷ�λ�øı����)
  * @param   t_lag �Ӿ�����һ֡ͼ����ʱ�������ݴ������ʱ
  * @retval  ����yaw������� 
 **/
static float Advance_yaw_angle(float vision_palstance, rt_uint8_t t_lag,
							   float distance,rt_uint16_t bullet_speed)
{
	/****************************1****************************/
	//�õ���̬ʱ��ǰ�Ľ��ٶ�
	float Gimbal_palstance_lag = *(Palstance_Deque+t_lag);
	float aim_palstance = Gimbal_palstance_lag+vision_palstance;
	/****************************2****************************/
	return aim_palstance*distance/bullet_speed;
}



/******************************************************************************************************************/
rt_thread_t aim_thread;
static struct rt_semaphore Obtain_palstance_sem;

static struct rt_semaphore Aim_bot_sem;

static struct rt_timer Obtain_palstance_task;

static void Obtain_palstance_IRQHandler(void *parameter)
{
	rt_sem_release(&Obtain_palstance_sem);
}

static void Obtain_palstance_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&Obtain_palstance_sem, RT_WAITING_FOREVER);
		get_new_palstance(Palstance_Deque,&gimbal_atti);
	}
} 

static void aim_bot_emtry(void *parameter)
{
	static rt_tick_t tick = 0;
	static float last_visual_yaw = 0;
	while(1)
	{
		/* �յ��Ӿ���Ϣʱ�ͷ��ź��� */
		rt_sem_take(&Aim_bot_sem, RT_WAITING_FOREVER);
		//����ת��
		//Point_t temp_point = Transformed_coordinate(vision_point,pitch_angle);
		
		//�����ʼ��pitch ��yaw
		Aimbot_t temp_aim_data = Calc_from_point(&visual_rev);
		
		
		//��ȡ���룬�ӵ�ˮƽ�ٶ�
		float temp_dis = (visual_rev.z)*(visual_rev.z)+(visual_rev.x)*(visual_rev.x)+(visual_rev.z)*(visual_rev.z);
		float distance = Q_rsqrt(temp_dis);
		
		float bullet_speed_horizontal = bullet_speed*cos((gimbal_atti.pitch+temp_aim_data.pitch_add)*pi_operator);
		
		//��������
		Aimbot_data.pitch_add = Gravity_compensation(gimbal_atti.pitch,temp_aim_data.pitch_add,distance,bullet_speed_horizontal);
		/* �������νǶ�����ٶ� ��λ��/s */
		vision_yaw_palstance = (temp_aim_data.yaw_add - last_visual_yaw)/((rt_tick_get()-tick)/1000.0f);
		
		//��ǰ��Ԥ��
		Aimbot_data.yaw_add = Advance_yaw_angle(vision_yaw_palstance,(visual_rev.computime+t_transmit),distance,bullet_speed_horizontal);
		
		last_visual_yaw = temp_aim_data.yaw_add;
		tick = rt_tick_get();
	}
	
}
void aim_bot_creat(void)
{
	//��ʱ�������߳�
	rt_thread_t thread;
	rt_sem_init(&Obtain_palstance_sem, "Obtain_palstance_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("Obtain_palstance_thread", Obtain_palstance_emtry, RT_NULL, 1024, 3, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	//��ʱ���ж�
	rt_timer_init(&Obtain_palstance_task,
                   "Obtain_palstance_task",
                   Obtain_palstance_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 //������ʱ��
	rt_timer_start(&Obtain_palstance_task);
	
	//��ʼ��һ���߳� �������Ӿ�����һ֡����ʱ��������
	//�Ƿ���Ҫδ�������ʱ������߳� �� ��������Żָ����̣߳�
	rt_sem_init(&Aim_bot_sem, "Aim_bot_sem", 0, RT_IPC_FLAG_FIFO);
	aim_thread = rt_thread_create("Aim_bot_sem", aim_bot_emtry, RT_NULL, 1024, 2, 1);
	if (aim_thread != RT_NULL)
	{
			rt_thread_startup(aim_thread);
	}
	rt_thread_suspend(aim_thread);
}
//INIT_APP_EXPORT(aim_bot_creat);
