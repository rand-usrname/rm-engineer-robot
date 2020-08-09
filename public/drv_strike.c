#include "drv_strike.h"
#include "drv_refsystem.h"

#define snail   0
#define fire_angle 60

static rt_uint8_t cacel_heat_ctrl = 0;


/***********************************�û�����******************************************/
static Strike_t gun1={0};

/* �������ƺ��� �����±�*/
static void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
	}
}

/***********************************��������ģ��******************************************/



/**
 * @brief  ������������
 */
static void heat_max_set(Heatctrl_t *heat,Robodata_t *Robodata)
{
	if(Robodata->robot_id == 1 || Robodata->robot_id == 101)
	{
		heat->max = Robodata->heat_limit_42;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(((3<=Robodata->robot_id)&&(Robodata->robot_id<=5))||((103<=Robodata->robot_id)&&(Robodata->robot_id<=105))
	{
		heat->max = Robodata->heat_limit_17;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	
}
/**
 * @brief  ����ʵʱ����
 */
static void refresh_heat(Heatctrl_t *heat,Robodata_t *Robodata)
{
	if(Robodata->robot_id == 1 || Robodata->robot_id == 101)
	{
		heat->now = Robodata->heat_17;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(((3<=Robodata->robot_id)&&(Robodata->robot_id<=5))||((103<=Robodata->robot_id)&&(Robodata->robot_id<=105))
	{
		heat->now = Robodata->heat_42;
	    heat->rate = 100*(heat->max - heat->now)/(heat->max);
	}
	else if(Robodata->robot_id==2||Robodata->robot_id==102)
	{
		heat->now = Robodata->heat_mobile;
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
		refresh_heat(&gun1.heat,Robodata);
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
		 heat_max_set(p_temp,Robodata);
		 heat_control(p_temp);			/*�������ƺ���*/
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  ��ʼ�������� �ڷ��������ʼ���г�ʼ��
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
static void heatctrl_init(Heatctrl_t *heat,Robodata_t *Robodata)
{
	heat_max_set(heat,Robodata);
	heat->now = heat->max;
	heat->rate = 100;
	heatctrl_start();
}
/************************************ End **************************************************/



/***********************************�������*****************************************/
/**
 * @brief ���������ʼ��
*/
static void strike_init(Strike_t *gun,rt_base_t mode)
{
	gun->mode = mode;			
	gun->speed = 0;
	gun->status = 0;
	heatctrl_init(&gun->heat);
}
INIT_APP_EXPORT(strike_init);

void Gun_speed_set(struct Motor_t *motor,Robodata_t *Robodata,Strike_t *strike,rt_base_t mode)
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
	if(Robodata->robot_id == 1 || Robodata->robot_id == 101)
	{
		if(strike->speed>Robodata->heat_limit_17)
			strike->speed = Robodata->heat_limit_17;
	}
	else if(((3<=Robodata->robot_id)&&(Robodata->robot_id<=5))||((103<=Robodata->robot_id)&&(Robodata->robot_id<=105))
	{
		if(strike->speed>Robodata->heat_limit_42)
			strike->speed = Robodata->heat_limit_42;
	}
	else if(Robodata->robot_id==2||Robodata->robot_id==102)
	{
		if(strike->speed>Robodata->heat_limit_mobile)
			strike->speed = Robodata->heat_limit_mobile;
	}
	#if snail
	motor_rub_set(strike->speed);
	#else
	motor->spe.set = strike->speed;
	#endif
}
/**
 * @brief  �����ж�
 */
void strike_stuck(struct Motor_t *motor, Strike_t *gun)
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
			motor->ang.set = motor->dji.angle + 3000;			/*��ת ��Ҫ�ֶ�����һ��*/
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
void strike_fire(struct Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;										/*��ȡϵͳʱ��*/
	static rt_tick_t tick_sleep = 0;								/*������Ϣʱ��*/
	
	if(gun->status & STRICK_STOP)		   
	{
		motor->ang.set = motor->dji.angle;
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
