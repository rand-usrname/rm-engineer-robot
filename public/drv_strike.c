#include "drv_strike.h"

#define Infantry
//#define Hero
//#define Sentry
//#define Mobile

#define snail   0
#define fire_angle 60

static rt_uint8_t cacel_heat_ctrl = 0;

//����һЩ��֪���������д����
rt_uint8_t level;
rt_uint16_t now_heat;
/***********************************�û�����******************************************/
static Strike_t gun1={0};

/*�������ƺ��������û�����ʵ�ʱ�д*/
static void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
		/*�������̫�٣�ֹͣ����*/
		if(gun1.heat.now<(gun1.speed/100)){gun1.status |= STRICK_STOP;}
		else{gun1.status &= ~STRICK_STOP;}
		if(gun1.heat.rate > 75){}
		else if(gun1.heat.rate > 10){}
	}
}

/***********************************��������ģ��******************************************/



/**
 * @brief  �������� ��ȴ����
 */
static void heat_max_set(Heatctrl_t *heat,rt_uint8_t level)
{
	#ifdef Infantry
	switch (level)
	{
		case 2:
			heat->max = 240;
		    heat->rate = (100*(heat->now))/(heat->max);
			break;
		case 3:
			heat->max = 360;
		    heat->rate = (100*(heat->now))/(heat->max);
			break;
	}
	#endif
	#ifdef Hero
	switch (level)
	{
		case 2:
			heat->max = 300;
		    heat->rate = (100*(heat->now))/(heat->max);
			break;
		case 3:
			heat->max = 400;
		    heat->rate = (100*(heat->now))/(heat->max);
			break;
	}
	#endif
}
/**
 * @brief  ����ʵʱ����
 */
static void refresh_heat(Heatctrl_t *heat,rt_uint16_t now_heat)
{
	heat->now = now_heat;
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
		refresh_heat(&gun1.heat,now_heat);
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
		 /*�������޼�ʣ�������ٷֱ�*/
		 heat_max_set(p_temp,level);
		 heat_control(p_temp);			/*��������*/
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  ��ʼ�������ƣ��������������̣߳�
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
 * @brief  ������������
 * @param  heat�������ṹ��  �����ڵ�һ�γ�ʼ��
 * @param  max���������� 
 */
static void heatctrl_init(Heatctrl_t *heat)
{
	#ifdef Infantry
	heat->max = 120;
	heat->now = 120;
	heat->rate = 20;
	#endif
	#ifdef Hero
	heat->max = 200;
	heat->now = 200
	heat->rate = 100;
	#endif
	#ifdef Sentry
	heat->max = 320;
	heat->now = 320;
	heat->rate = 100;
	#endif
	#ifdef Mobile
	heat->max = 150;
	heat->now = 150;
	heat->rate = 100;
	#endif
	heatctrl_start();
}
/************************************ End **************************************************/



/***********************************�������ģ��******************************************/
/**
 * @brief  ���������ʼ��
 * @param  gun����������ṹ��ָ��
 */
void strike_init(Strike_t *gun,rt_base_t mode)
{
	gun->mode = mode;			
	gun->speed = 0;
	gun->status = 0;
	heatctrl_init(&gun->heat);
}
INIT_APP_EXPORT(strike_init);
void Gun_speed_set(struct Motor_t *motor,rt_uint16_t speed)
{
	//������Ҫ��m/s ת����Ħ����ת���ٶ�
	gun1.speed = speed;
	#if snail
	motor_rub_set(speed);
	#else
	motor->spe.set = speed;
	#endif
}
/**
 * @brief  �����ж�
 * @param  motor����������ṹ��ָ��
 */
void strike_stuck(struct Motor_t *motor, Strike_t *gun)
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

/**
 * @brief  ����ģʽ������
 * @param  gun����������ṹ��ָ��
 * @param  if_fire���Ƿ񿪻�,1����0����
 */
void strike_fire(struct Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;										/*��¼ϵͳʱ��*/
	static rt_tick_t tick_sleep = 0;								/*����������ʱ��*/
	
	if((gun->status & STRICK_STOP)&&(!cacel_heat_ctrl))									/*����������ֱ�ӷ���*/
	{
		motor->ang.set = motor->dji.angle;
		return;
	}
	if(rt_tick_get() - tick < tick_sleep)
    {return;}					                                    /*�����ڼ����Ϣ�ڼ䣬�����������Ķ���ֱ�ӷ���**/
	else
	{
		if(if_fire==1)
		{
			/*һ�η�������*/
			if(gun->mode & STRICK_NOLIMITE)														/*��ͣת��*/
			{
				tick_sleep=0;																	/*���ʱ��*/
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
