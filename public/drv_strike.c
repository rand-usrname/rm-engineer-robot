#include "drv_strike.h"

#define snail   0
#define fire_angle 60
Motor_t m_rub[2];
Motor_t m_launch;

#define PWM_DEV_NAME        "pwm3"  	  /* PWM�豸���� */
#define PWM_DEV_CHANNEL_1     1       															/* PWMͨ�� Ħ���� */
#define PWM_DEV_CHANNEL_2     2       															/* PWMͨ�� Ħ����*/
#define PWM_DEV_CHANNEL_3     3																	/* PWMͨ�� ���ֵ��*/
static struct rt_device_pwm *pwm_dev;	

/**
 * @brief  Ħ�������ʼ����tim��pwm��ʼ����
 * @retval RT_EOK or RT_ERROR(�ɹ���ʧ��)
 */
int motor_rub_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!pwm_dev){return RT_ERROR;}
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_2, 20000000,1000000);
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_1, 20000000,1000000);
	/* ʹ���豸 */
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_2);
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_1);
	return RT_EOK;
}
/**
 * @brief  ����������ſ��أ���ʼ��
 * @param  duty ռ�ձȣ�0-1000����0%-%100��
 * @retval RT_EOK or RT_ERROR(�ɹ���ʧ��)
 */
int motor_servo_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!pwm_dev){return RT_ERROR;}
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_3, 10000,5000);
	/* ʹ���豸 */
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_3);
	return RT_EOK;
}

void motor_rub_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_1, 20000000,20000000*duty/1000);
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_2, 20000000,20000000*duty/1000);
}

/**
 * @brief  ���ֶ���ٶ��趨
 * @param  duty ռ�ձȣ�0-1000����0%-%100��
 * @retval None
 */
void motor_servo_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_3, 10000,10000*duty/1000);
}

Strike_t gun1;
/*�������ƺ��������û�����ʵ�ʱ�д*/
void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
	}
}

/***********************************��������ģ��******************************************/

static Heatctrl_t *p_heat = RT_NULL;													/*�������ƿ�ָ��*/

#if LOCAL_HEAT_ENABLE
/**
 * @brief  ������ȴ
 */
static __inline void heatctrl_cool(Heatctrl_t *heat)
{
	heat->now += heat->cool * heat->buff;
	if(heat->now > heat->max){heat->now = heat->max;}
}
#endif

/**
 * @brief  ���������߳�
 */
static void heatctrl_thread(void *parameter)
{
	Heatctrl_t *p_temp = p_heat;
	while(1)
	{
		p_temp = p_heat;
	 while(!(p_temp == RT_NULL))
	 {
		 /*ʣ�������ٷֱ�*/
		 p_temp->rate = (100 * p_temp->now) / p_temp->max;
		 heat_control(p_temp);															/*��������*/
		 #if LOCAL_HEAT_ENABLE
		 heatctrl_cool(p_temp);																		/*��ȴ*/
		 #endif
		 p_temp = p_temp->next;
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  ������������
 * @param  heat�������ṹ��
 * @param  max����������
 */
void heatctrl_init(Heatctrl_t *heat, rt_uint32_t max)
{
	static rt_uint8_t num = 0;
	
	/*������������*/
	heat->max = max;
	/*״̬����ʼ��*/
	heat->rate = 100;
	heat->now = max;
	
	/*���ʹ�ܱ�����������*/
	#if LOCAL_HEAT_ENABLE
	heat->cool = 10;
	heat->buff = 1;
	#endif
	
	/*������ǵ�һ�γ�ʼ��������п��ƽṹ�ĵ�����������*/
	if(num > 0){p_heat->next = heat;}
	p_heat = heat;
	num++;
}
/**
 * @brief  ��ʼ�������ƣ��������������̣߳�
 */
void heatctrl_start(void)
{
	rt_thread_t thread;
	thread = rt_thread_create("heatctrl", heatctrl_thread, RT_NULL, 1024, 2, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
}
/************************************ End **************************************************/

/***********************************�������ģ��******************************************/

void Gun_speed_set(Strike_t *strike, rt_int16_t speed)
{
	strike->speed = speed;
	//	if(Refdata->robot_id == 1 || Refdata->robot_id == 101)
//	{
//		if(strike->speed>Refdata->heat_limit_17)
//			strike->speed = Refdata->heat_limit_17;
//	}
//	else if(((3<=Refdata->robot_id)&&(Refdata->robot_id<=5))||((103<=Refdata->robot_id)&&(Refdata->robot_id<=105)))
//	{
//		if(strike->speed>Refdata->heat_limit_42)
//			strike->speed = Refdata->heat_limit_42;
//	}
	#if snail
	motor_rub_set(strike->speed);
	#else
	//��Ҫ���㣿
	m_rub[0].spe.set = strike->speed;
	m_rub[1].spe.set = -strike->speed;
	#endif
}
void Gun_mode_set(Strike_t *strike, rt_base_t mode)
{
	strike->mode = mode;
}
/**
 * @brief  ���������ʼ��
 * @param  gun����������ṹ��ָ��
 */
void strike_init(Strike_t *gun, rt_uint32_t max)
{
	gun->mode = STRICK_NOLIMITE | STRICK_LOWSPEED;				/*��������+���ٸ���Ƶ*/
	gun->speed = 0;
	gun->status = 0;
	#if snail
	motor_rub_init();
	#else
	motor_init(&m_rub[0],0x201,1);
	motor_init(&m_rub[1],0x202,1);
	motor_init(&m_launch,0x203,0.027973);
	//TODO:pid�������ʼ��
	pid_init(&m_launch.ang, 
					3.5,0,0,
					500,5000,-5000);
	pid_init(&m_launch.spe, 
					8,0,0,
					300,8000,-8000);
	pid_init(&m_rub[0].spe, 
					8,0.1,0,
					1200,14000,-14000);
	pid_init(&m_rub[1].spe, 
					8,0.1,0,
					1200,14000,-14000);
	#endif
	heatctrl_init(&gun->heat, max);												/*�������Ʋ�����ʼ��*/
}
/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/**
 * @brief  �����ж�
 * @param  motor����������ṹ��ָ��
 */
void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;																			/*��¼ϵͳʱ��*/
	static rt_uint8_t temp = 0;
	/*�������*/
	if(gun->status & STRICK_STUCK)
	{	
		if(temp == 1)
		{
			if(rt_tick_get() > tick)																	/*��ת1000ms*/
			{
				gun->status &= ~STRICK_STUCK;
				stuck_time = 0;
				temp = 0;
			}
		}
		else
		{
			motor->ang.set = motor->dji.angle + 3000;									/**/
			tick = rt_tick_get()+1000;
			temp=1;
		}
		
	}
	else if(~(gun->status & STRICK_STOP))														/*���û�н�ֹת��*/
	{
		/*�ٶȻ��������*/
		if(ABS(motor->spe.out) > 7777)
		{
			stuck_time++;
		}
		//else {stuck_time=0;}
		/*����ʱ�����*/
		if(stuck_time > 111)
		{
			gun->status |= STRICK_STUCK;
		}
	}
}


/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/**
 * @brief  ����ģʽ������
 * @param  gun����������ṹ��ָ��
 * @param  if_fire���Ƿ񿪻�,1����0����
 */
void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;																			/*��¼ϵͳʱ��*/
	static rt_tick_t tick_sleep = 0;																/*����������ʱ��*/
	/*���ֹͣ����ʹλ�û�����*/
	if(gun->status & STRICK_STOP)																		/*��������ֱ�ӷ���*/
	{
		motor->ang.set = motor->dji.angle;
		return;
	}						
	if(rt_tick_get() - tick < tick_sleep){return;}					/*�����ڼ����Ϣ�ڼ䣬����������Ķ���ֱ�ӷ���**/
	else
	{
		if(if_fire==1)
		{
			/*һ�η�������*/
			if(gun->mode & STRICK_NOLIMITE)															/*��ͣת��*/
			{
				tick_sleep=0;																							/*���ʱ��*/
				motor_angle_set(motor, 60);
			}
			else if(gun->mode & STRICK_SINGLE)													/*����*/
			{
				tick_sleep=500;
				motor_angle_set(motor, 60);
			}
			else if(gun->mode & STRICK_TRIPLE)													/*������*/
			{
				tick_sleep=1000;
				motor_angle_set(motor, 175);
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************��������߳�*******************************************/
static struct rt_semaphore task_1ms_sem;     													/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore task_10ms_sem;     													/* ���ڽ�����Ϣ���ź��� */
/*1ms����*/
static struct rt_timer task_1ms;
/*10ms����*/
static struct rt_timer task_10ms;

/* task_1ms ��ʱ���� */
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_1ms_sem);
}
/* task_10ms ��ʱ���� */
static void task_10ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_10ms_sem);
}

static void task_1ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		//pid�ٶȻ�
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].dji.speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].dji.speed);
		pid_output_calculate(&m_launch.spe,m_launch.ang.out,m_launch.dji.speed);
		//���͵���
		motor_current_send(can2_dev,STDID_launch,m_rub[0].spe.out,m_rub[1].spe.out,m_launch.spe.out,0);
	}
}
static void task_10ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_10ms_sem, RT_WAITING_FOREVER);
		pid_output_motor(&m_launch.ang,m_launch.ang.set,m_launch.dji.angle);
	}
}

/**
 * @brief  ���񴴽�
 */
void strike_start(void)
{
	/*��ʱ�������߳�*/
	rt_thread_t thread;
	rt_sem_init(&task_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("1ms_sem", task_1ms_emtry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	rt_sem_init(&task_10ms_sem, "10ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("10ms_sem", task_10ms_emtry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	/*��ʱ���ж�*/
	rt_timer_init(&task_1ms,
                   "1mstask",
                   task_1ms_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_init(&task_10ms,
								 "10mstask",
								 task_10ms_IRQHandler,
								 RT_NULL,
								 10, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_10ms);
}
