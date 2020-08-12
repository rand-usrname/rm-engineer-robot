#include "drv_strike.h"

#define snail   0
#define fire_angle 60


#define PWM_DEV_NAME        "pwm3"  	  /* PWM�豸���� */
#define PWM_DEV_CHANNEL_1     1       															/* PWMͨ�� Ħ���� */
#define PWM_DEV_CHANNEL_2     2       															/* PWMͨ�� Ħ����*/
#define PWM_DEV_CHANNEL_3     3																	/* PWMͨ�� ���ֵ��*/
static struct rt_device_pwm *pwm_dev;	



/***********************************�û�����******************************************/
static Strike_t gun1={0};

/* �������ƺ��� �����±�*/
void heat_control(Heatctrl_t *p_temp)
{
	
}

/***********************************��������ģ��******************************************/



/**
 * @brief  ������������
 */
static void heat_max_set(Heatctrl_t *heat,Refdata_t *Refdata)
{
	if(Refdata->robot_id == 1 || Refdata->robot_id == 101)
	{
		heat->max = Refdata->heat_limit_42;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(((3<=Refdata->robot_id)&&(Refdata->robot_id<=5))||((103<=Refdata->robot_id)&&(Refdata->robot_id<=105)))
	{
		heat->max = Refdata->heat_limit_17;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	
}
/**
 * @brief  ����ʵʱ����
 */
static void refresh_heat(Heatctrl_t *heat,Refdata_t *Refdata)
{
	if(Refdata->robot_id == 1 || Refdata->robot_id == 101)
	{
		heat->now = Refdata->heat_17;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(((3<=Refdata->robot_id)&&(Refdata->robot_id<=5))||((103<=Refdata->robot_id)&&(Refdata->robot_id<=105)))
	{
		heat->now = Refdata->heat_42;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(Refdata->robot_id==2||Refdata->robot_id==102)
	{
		heat->now = Refdata->heat_mobile;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
}
static struct rt_semaphore refresh_heat_sem;
static struct rt_timer refresh_heat_timer;
static void refresh_heat_IRQHandler(void *parameter)
{
	rt_sem_release(&refresh_heat_sem);
}
static void refresh_heat_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&refresh_heat_sem, RT_WAITING_FOREVER);
		refresh_heat(&gun1.heat,Refdata);
	}
}
static void refresh_heat_init(void)
{
	rt_thread_t thread;
	rt_sem_init(&refresh_heat_sem, "refresh_heat_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("refresh_heat_sem", refresh_heat_emtry, RT_NULL, 1024, 3, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	rt_timer_init(&refresh_heat_timer,
                   "refresh_heat_task",
                   refresh_heat_IRQHandler,
                   RT_NULL,
                   20, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_start(&refresh_heat_timer);
}
/**
 * @brief  ���������߳�
 */
static void heatctrl_thread(void *parameter)
{
	Heatctrl_t *p_temp;
	while(1)
	{
		p_temp = &gun1.heat;
	 while(!(p_temp == RT_NULL))
	 {
		 /*������������*/
		 heat_max_set(p_temp,Refdata);
		 heat_control(p_temp);			/*�������ƺ���*/
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  ��ʼ���������߳�
 */
static void heatctrl_start(void)
{
	refresh_heat_init();
	rt_thread_t thread;
	thread = rt_thread_create("heatctrl", heatctrl_thread, RT_NULL, 1024, 2, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
}
/**
 * ��ʼ������ģ��
 */
static void heatctrl_init(Heatctrl_t *heat,Refdata_t *Refdata)
{
	heat_max_set(heat,Refdata);
	heat->now = heat->max;
	heat->rate = 100;
	heatctrl_start();
}
/************************************ End **************************************************/
/*********************************** snail��������� **************************************/
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
/************************************ End *************************************************/
/***********************************�������*****************************************/
Motor_t m_rub[2];//��Ħ����  snail����ٿ���
Motor_t m_load;

/**
 * @brief ���������ʼ��
*/
static void Gun_init(Strike_t *gun,rt_base_t mode)
{
	gun->mode = mode;			
	gun->speed = 0;
	gun->status = 0;
	heatctrl_init(&gun->heat,Refdata);
	motor_servo_init();
	#if snail
	//��ʼ��PWM
	motor_rub_init();
	#else
	//�˴�Ӧ������̨��Robodata.h�и�Ħ���ַ����id
	motor_init(&m_rub[0],0x201,1);
	motor_init(&m_rub[1],0x202,1);
	motor_init(&m_load,0x203,1);
	#endif
}
static void Gun_speed_set(Refdata_t *Refdata,Strike_t *strike,rt_base_t mode)
{
	
	switch(mode)
	{
		case STRICK_LOWSPEED: 
			break;
		case STRICK_MIDDLESPEED:
			break;
		case STRICK_HIGHTSPEED:
			break;
	}
	if(Refdata->robot_id == 1 || Refdata->robot_id == 101)
	{
		if(strike->speed>Refdata->heat_limit_17)
			strike->speed = Refdata->heat_limit_17;
	}
	else if(((3<=Refdata->robot_id)&&(Refdata->robot_id<=5))||((103<=Refdata->robot_id)&&(Refdata->robot_id<=105)))
	{
		if(strike->speed>Refdata->heat_limit_42)
			strike->speed = Refdata->heat_limit_42;
	}
	#if snail
	motor_rub_set(strike->speed);
	#else
	//��Ҫ���㣿
	m_rub[0].spe.set = strike->speed;
	m_rub[1].spe.set = -strike->speed;
	#endif
}
void Gun_mode_set(Strike_t *strike,rt_base_t mode)
{
	switch (mode)
	{
		case STRICK_NOLIMITE:
			strike->mode = STRICK_NOLIMITE;
			break;
		case STRICK_SINGLE:
			strike->mode = STRICK_SINGLE;
			break;
		case STRICK_TRIPLE:
			strike->mode = STRICK_TRIPLE;
			break;
	}
}
/**
 * @brief  �����ж�
 */
static void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;						/*ϵͳʱ��*/
	static rt_uint8_t temp = 0;
	
	if(gun->status & STRICK_STUCK)
	{	
		if(temp == 1)
		{
			if(rt_tick_get() > tick)			    
			{
				gun->status &= ~STRICK_STUCK;
				stuck_time = 0;
				temp = 0;
			}
		}
		else
		{
			//motor->ang.set = motor->angle + 3000;			/*��ת ��Ҫ�ֶ�����һ��*/
			tick = rt_tick_get()+1000;
			temp=1;
		}
	}
	else if(~(gun->status & STRICK_STOP))					/*���û�н�ֹ����*/
	{
		/*����������*/
		if(ABS(motor->spe.out) > 7777)
		{
			stuck_time++;
		}
		else {stuck_time=0;}
		
		/*�����ж�*/
		if(stuck_time > 111)
		{
			gun->status |= STRICK_STUCK;
		}
	}
}

/**
 * @brief   ������
 * @param   if_fire=1 ����
 */
static void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;										/*��ȡϵͳʱ��*/
	static rt_tick_t tick_sleep = 0;								/*������Ϣʱ��*/
	
	if(gun->status & STRICK_STOP)		   
	{
		motor->ang.set = motor->angle;
		return;
	}
	if(rt_tick_get() - tick < tick_sleep)
    {return;}					                                    /*������Ϣ�ڼ�ֱ�ӷ���*/
	else
	{
		if(if_fire==1)
		{
			/*����ģʽ*/
			if(gun->mode & STRICK_NOLIMITE)														/*ȫ�Զ�*/
			{
				tick_sleep=0;																	/*������*/
				motor_angle_set(motor, fire_angle);
			}
			else if(gun->mode & STRICK_SINGLE)													/*����*/
			{
				tick_sleep=500;
				motor_angle_set(motor, fire_angle);
			}
			else if(gun->mode & STRICK_TRIPLE)	                                               /*������*/
			{
				tick_sleep=1000;
				motor_angle_set(motor, fire_angle*3);
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************��������߳�*******************************************/
static struct rt_semaphore task_1ms_sem;     													/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore task_10ms_sem;     													/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore task_101ms_sem;     													/* ���ڽ�����Ϣ���ź��� */
/*1ms����*/
static struct rt_timer task_1ms;
/*10ms����*/
static struct rt_timer task_10ms;
/*101ms����*/
static struct rt_timer task_101ms;

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
/* task_101ms ��ʱ���� */
static void task_101ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_101ms_sem);
}

static void task_1ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		//pid�ٶȻ�
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].speed);
		pid_output_calculate(&m_load.spe,m_load.ang.out,m_load.speed);
		//���͵���
		
	}
}
static void task_10ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_10ms_sem, RT_WAITING_FOREVER);
		//pid�ǶȻ�
		pid_output_calculate(&m_load.ang,m_load.ang.set,m_load.angle);
	}
}
static void task_101ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_101ms_sem, RT_WAITING_FOREVER);
		//�����ж�
		
		//�����ж�
		if(Obtain_Sx_Data(2)&&(RC_data->Remote_Data.s2==1))
		{
			strike_fire(&m_load,&gun1,1);
		}
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
	
	rt_sem_init(&task_101ms_sem, "101ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("101ms_sem", task_101ms_emtry, RT_NULL, 1024, 1, 1);
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
	rt_timer_init(&task_101ms,
								 "101mstask",
								 task_101ms_IRQHandler,
								 RT_NULL,
								 101, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_10ms);
	rt_timer_start(&task_101ms);
	Gun_init(&gun1,STRICK_NOLIMITE);
}


