#include "drv_strike.h"

#define snail   0
#define fire_angle 60


#define PWM_DEV_NAME        "pwm3"  	  /* PWM设备名称 */
#define PWM_DEV_CHANNEL_1     1       															/* PWM通道 摩擦轮 */
#define PWM_DEV_CHANNEL_2     2       															/* PWM通道 摩擦轮*/
#define PWM_DEV_CHANNEL_3     3																	/* PWM通道 弹仓电机*/
static struct rt_device_pwm *pwm_dev;	



/***********************************用户代码******************************************/
static Strike_t gun1={0};

/* 热量控制函数 得重新编*/
void heat_control(Heatctrl_t *p_temp)
{
	
}

/***********************************热量控制模块******************************************/



/**
 * @brief  热量上限设置
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
 * @brief  更新实时热量
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
 * @brief  热量控制线程
 */
static void heatctrl_thread(void *parameter)
{
	Heatctrl_t *p_temp;
	while(1)
	{
		p_temp = &gun1.heat;
	 while(!(p_temp == RT_NULL))
	 {
		 /*设置热量上限*/
		 heat_max_set(p_temp,Refdata);
		 heat_control(p_temp);			/*热量控制函数*/
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  开始热量控制线程
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
 * 初始化热量模块
 */
static void heatctrl_init(Heatctrl_t *heat,Refdata_t *Refdata)
{
	heat_max_set(heat,Refdata);
	heat->now = heat->max;
	heat->rate = 100;
	heatctrl_start();
}
/************************************ End **************************************************/
/*********************************** snail电机及弹仓 **************************************/
/**
 * @brief  摩擦电机初始化（tim的pwm初始化）
 * @retval RT_EOK or RT_ERROR(成功或失败)
 */
int motor_rub_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!pwm_dev){return RT_ERROR;}
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_2, 20000000,1000000);
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_1, 20000000,1000000);
	/* 使能设备 */
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_2);
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_1);
	return RT_EOK;
}
/**
 * @brief  舵机（弹仓门开关）初始化
 * @param  duty 占空比（0-1000对于0%-%100）
 * @retval RT_EOK or RT_ERROR(成功或失败)
 */
int motor_servo_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!pwm_dev){return RT_ERROR;}
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_3, 10000,5000);
	/* 使能设备 */
	rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL_3);
	return RT_EOK;
}

void motor_rub_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_1, 20000000,20000000*duty/1000);
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_2, 20000000,20000000*duty/1000);
}

/**
 * @brief  弹仓舵机速度设定
 * @param  duty 占空比（0-1000对于0%-%100）
 * @retval None
 */
void motor_servo_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL_3, 10000,10000*duty/1000);
}
/************************************ End *************************************************/
/***********************************发射机构*****************************************/
Motor_t m_rub[2];//两摩擦轮  snail电机再考虑
Motor_t m_load;

/**
 * @brief 发射机构初始化
*/
static void Gun_init(Strike_t *gun,rt_base_t mode)
{
	gun->mode = mode;			
	gun->speed = 0;
	gun->status = 0;
	heatctrl_init(&gun->heat,Refdata);
	motor_servo_init();
	#if snail
	//初始化PWM
	motor_rub_init();
	#else
	//此处应传入云台的Robodata.h中给摩擦轮分配的id
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
	//需要换算？
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
 * @brief  卡弹判定
 */
static void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;						/*系统时间*/
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
			//motor->ang.set = motor->angle + 3000;			/*反转 需要手动设置一下*/
			tick = rt_tick_get()+1000;
			temp=1;
		}
	}
	else if(~(gun->status & STRICK_STOP))					/*如果没有禁止发射*/
	{
		/*电机输出过大*/
		if(ABS(motor->spe.out) > 7777)
		{
			stuck_time++;
		}
		else {stuck_time=0;}
		
		/*卡弹判定*/
		if(stuck_time > 111)
		{
			gun->status |= STRICK_STUCK;
		}
	}
}

/**
 * @brief   开火函数
 * @param   if_fire=1 开火
 */
static void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;										/*获取系统时间*/
	static rt_tick_t tick_sleep = 0;								/*发弹休息时间*/
	
	if(gun->status & STRICK_STOP)		   
	{
		motor->ang.set = motor->angle;
		return;
	}
	if(rt_tick_get() - tick < tick_sleep)
    {return;}					                                    /*处于休息期间直接返回*/
	else
	{
		if(if_fire==1)
		{
			/*发弹模式*/
			if(gun->mode & STRICK_NOLIMITE)														/*全自动*/
			{
				tick_sleep=0;																	/*不休眠*/
				motor_angle_set(motor, fire_angle);
			}
			else if(gun->mode & STRICK_SINGLE)													/*单发*/
			{
				tick_sleep=500;
				motor_angle_set(motor, fire_angle);
			}
			else if(gun->mode & STRICK_TRIPLE)	                                               /*三连发*/
			{
				tick_sleep=1000;
				motor_angle_set(motor, fire_angle*3);
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************发射机构线程*******************************************/
static struct rt_semaphore task_1ms_sem;     													/* 用于接收消息的信号量 */
static struct rt_semaphore task_10ms_sem;     													/* 用于接收消息的信号量 */
static struct rt_semaphore task_101ms_sem;     													/* 用于接收消息的信号量 */
/*1ms任务*/
static struct rt_timer task_1ms;
/*10ms任务*/
static struct rt_timer task_10ms;
/*101ms任务*/
static struct rt_timer task_101ms;

/* task_1ms 超时函数 */
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_1ms_sem);
}
/* task_10ms 超时函数 */
static void task_10ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_10ms_sem);
}
/* task_101ms 超时函数 */
static void task_101ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_101ms_sem);
}

static void task_1ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		//pid速度环
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].speed);
		pid_output_calculate(&m_load.spe,m_load.ang.out,m_load.speed);
		//发送电流
		
	}
}
static void task_10ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_10ms_sem, RT_WAITING_FOREVER);
		//pid角度环
		pid_output_calculate(&m_load.ang,m_load.ang.set,m_load.angle);
	}
}
static void task_101ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_101ms_sem, RT_WAITING_FOREVER);
		//卡弹判断
		
		//发弹判定
		if(Obtain_Sx_Data(2)&&(RC_data->Remote_Data.s2==1))
		{
			strike_fire(&m_load,&gun1,1);
		}
	}
}

/**
 * @brief  任务创建
 */
void strike_start(void)
{
	/*定时器处理线程*/
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
	
	/*定时器中断*/
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
	 /* 启动定时器 */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_10ms);
	rt_timer_start(&task_101ms);
	Gun_init(&gun1,STRICK_NOLIMITE);
}


