#include "drv_strike.h"

#define snail   0
#define fire_angle 60
Motor_t m_rub[2];
Motor_t m_launch;

#define PWM_DEV_NAME        "pwm3"  	  /* PWM设备名称 */
#define PWM_DEV_CHANNEL_1     1       															/* PWM通道 摩擦轮 */
#define PWM_DEV_CHANNEL_2     2       															/* PWM通道 摩擦轮*/
#define PWM_DEV_CHANNEL_3     3																	/* PWM通道 弹仓电机*/
static struct rt_device_pwm *pwm_dev;	

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

Strike_t gun1;
/*热量控制函数，由用户根据实际编写*/
void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
	}
}

/***********************************热量控制模块******************************************/

static Heatctrl_t *p_heat = RT_NULL;													/*热量控制块指针*/

#if LOCAL_HEAT_ENABLE
/**
 * @brief  热量冷却
 */
static __inline void heatctrl_cool(Heatctrl_t *heat)
{
	heat->now += heat->cool * heat->buff;
	if(heat->now > heat->max){heat->now = heat->max;}
}
#endif

/**
 * @brief  热量控制线程
 */
static void heatctrl_thread(void *parameter)
{
	Heatctrl_t *p_temp = p_heat;
	while(1)
	{
		p_temp = p_heat;
	 while(!(p_temp == RT_NULL))
	 {
		 /*剩余热量百分比*/
		 p_temp->rate = (100 * p_temp->now) / p_temp->max;
		 heat_control(p_temp);															/*热量控制*/
		 #if LOCAL_HEAT_ENABLE
		 heatctrl_cool(p_temp);																		/*冷却*/
		 #endif
		 p_temp = p_temp->next;
	 }
	 rt_thread_mdelay(HEAT_PERIOD);
 }
}
/**
 * @brief  热量参数设置
 * @param  heat：热量结构体
 * @param  max：热量上限
 */
void heatctrl_init(Heatctrl_t *heat, rt_uint32_t max)
{
	static rt_uint8_t num = 0;
	
	/*设置热量上限*/
	heat->max = max;
	/*状态量初始化*/
	heat->rate = 100;
	heat->now = max;
	
	/*如果使能本地热量计算*/
	#if LOCAL_HEAT_ENABLE
	heat->cool = 10;
	heat->buff = 1;
	#endif
	
	/*如果不是第一次初始化，则进行控制结构的单向链表连接*/
	if(num > 0){p_heat->next = heat;}
	p_heat = heat;
	num++;
}
/**
 * @brief  开始热量控制（创建热量控制线程）
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

/***********************************发射机构模块******************************************/

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
	//需要换算？
	m_rub[0].spe.set = strike->speed;
	m_rub[1].spe.set = -strike->speed;
	#endif
}
void Gun_mode_set(Strike_t *strike, rt_base_t mode)
{
	strike->mode = mode;
}
/**
 * @brief  发射机构初始化
 * @param  gun：发射机构结构体指针
 */
void strike_init(Strike_t *gun, rt_uint32_t max)
{
	gun->mode = STRICK_NOLIMITE | STRICK_LOWSPEED;				/*持续开火+低速高射频*/
	gun->speed = 0;
	gun->status = 0;
	#if snail
	motor_rub_init();
	#else
	motor_init(&m_rub[0],0x201,1);
	motor_init(&m_rub[1],0x202,1);
	motor_init(&m_launch,0x203,0.027973);
	//TODO:pid在外面初始化
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
	heatctrl_init(&gun->heat, max);												/*热量控制参数初始化*/
}
/*对于多枪管，需要重新再定义一个相同作用函数，待修改*/
/**
 * @brief  卡弹判定
 * @param  motor：发射机构结构体指针
 */
void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;																			/*记录系统时间*/
	static rt_uint8_t temp = 0;
	/*如果卡弹*/
	if(gun->status & STRICK_STUCK)
	{	
		if(temp == 1)
		{
			if(rt_tick_get() > tick)																	/*倒转1000ms*/
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
	else if(~(gun->status & STRICK_STOP))														/*如果没有禁止转动*/
	{
		/*速度环输出过大*/
		if(ABS(motor->spe.out) > 7777)
		{
			stuck_time++;
		}
		//else {stuck_time=0;}
		/*卡弹时间过长*/
		if(stuck_time > 111)
		{
			gun->status |= STRICK_STUCK;
		}
	}
}


/*对于多枪管，需要重新再定义一个相同作用函数，待修改*/
/**
 * @brief  设置模式并开火
 * @param  gun：发射机构结构体指针
 * @param  if_fire：是否开火,1开火，0不开
 */
void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire)
{
	static rt_tick_t tick = 0;																			/*记录系统时间*/
	static rt_tick_t tick_sleep = 0;																/*拨弹电机间隔时间*/
	/*如果停止开火，使位置环不动*/
	if(gun->status & STRICK_STOP)																		/*不允许开火，直接返回*/
	{
		motor->ang.set = motor->dji.angle;
		return;
	}						
	if(rt_tick_get() - tick < tick_sleep){return;}					/*开火期间和休息期间，参数不允许改动，直接返回**/
	else
	{
		if(if_fire==1)
		{
			/*一次发弹数量*/
			if(gun->mode & STRICK_NOLIMITE)															/*不停转动*/
			{
				tick_sleep=0;																							/*间隔时间*/
				motor_angle_set(motor, 60);
			}
			else if(gun->mode & STRICK_SINGLE)													/*单发*/
			{
				tick_sleep=500;
				motor_angle_set(motor, 60);
			}
			else if(gun->mode & STRICK_TRIPLE)													/*三连发*/
			{
				tick_sleep=1000;
				motor_angle_set(motor, 175);
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************发射机构线程*******************************************/
static struct rt_semaphore task_1ms_sem;     													/* 用于接收消息的信号量 */
static struct rt_semaphore task_10ms_sem;     													/* 用于接收消息的信号量 */
/*1ms任务*/
static struct rt_timer task_1ms;
/*10ms任务*/
static struct rt_timer task_10ms;

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

static void task_1ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		//pid速度环
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].dji.speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].dji.speed);
		pid_output_calculate(&m_launch.spe,m_launch.ang.out,m_launch.dji.speed);
		//发送电流
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
	 /* 启动定时器 */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_10ms);
}
