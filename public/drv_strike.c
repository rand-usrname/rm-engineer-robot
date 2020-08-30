#include "drv_strike.h"
#include "robodata.h"
#include "drv_refsystem.h"

#define PWM_DEV_NAME        "pwm8"  	  /* PWM设备名称 */
static struct rt_device_pwm *pwm_dev;	
static struct rt_device_pwm *servo_dev;   /* 弹仓PWM设备名称 */
Motor_t m_rub[2];
Motor_t m_launch;
static rt_tick_t tick_sleep = 0;		  /*拨弹电机间隔时间*/
/**
 * @brief  摩擦电机初始化（tim的pwm初始化）
 * @retval RT_EOK or RT_ERROR(成功或失败)
 * @note   设置频率为50hz，实际输出100hz。(RT-THread高级定时器BUG)
 */
int motor_rub_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
	if(!pwm_dev){return RT_ERROR;}
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, 3, 20000000,1000000);
	rt_pwm_set(pwm_dev, 4, 20000000,1000000);
	/* 使能设备 */
	rt_pwm_enable(pwm_dev, 3);
	rt_pwm_enable(pwm_dev, 4);
	return RT_EOK;
}
/**
 * @brief  舵机（弹仓门开关）初始化
 * @param  duty 占空比（0-1000对于0%-%100）
 * @retval RT_EOK or RT_ERROR(成功或失败)
 */
int motor_servo_init(void)
{
	servo_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!servo_dev){return RT_ERROR;}
	/*设置周期和脉冲宽度*/
	rt_pwm_set(servo_dev, 2, 10000000,5000000);
	/* 使能设备 */
	rt_pwm_enable(servo_dev, 2);
	return RT_EOK;
}

void motor_rub_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*设置周期和脉冲宽度*/
	rt_pwm_set(pwm_dev, 3, 20000000,20000*duty);
	rt_pwm_set(pwm_dev, 4, 20000000,20000*duty);
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
	rt_pwm_set(servo_dev, 2, 10000000,10000*duty);
}

Strike_t gun1;
/*热量控制函数，由用户根据实际编写*/
void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
		/* 如果无视热量 */
		if(p_temp->unlimit_heat)
		{
			if(gun1.mode == STRICK_NOLIMITE)
				{tick_sleep = 50;}
	        else if(gun1.mode == STRICK_TRIPLE)
				{tick_sleep = 600;}
			else if(gun1.mode == STRICK_SINGLE)
				{tick_sleep = 100;}
		}
		/* 否则根据剩余热量对发弹间隔做出限制 */
		else
		{
			/* 17mm */
			#ifndef BULLET_42
			rt_uint16_t now_furture = p_temp->now;
			/* 3连发 */
			//先计算下一次发射后的热量
			if(gun1.mode == STRICK_TRIPLE)
			{
				/* 预估值可以再调整 */
				now_furture += 30;
			}
			/* 全自动 */
			else if(gun1.mode == STRICK_NOLIMITE)
			{
				//接下来100ms内如果一直发弹
				now_furture += 1000/tick_sleep - p_temp->cool/10;
			}
			/* 当前血量支持我超出多少热量*/
			rt_uint16_t more_heat = 0;
			rt_uint16_t hp_furture = 0;
//			if((now_furture < 2*(p_temp->max))&&(now_furture>p_temp->max))
//			{
//				hp_furture = Refdata.remain_HP - (now_furture-p_temp->max)/250/10*Refdata.max_HP;
//			}
//			else if(now_furture >= 2*(p_temp->max))
//			{
//				hp_furture = Refdata.remain_HP-(now_furture-p_temp->max)/250*Refdata.max_HP;
//			}
//			else
//			{
//				hp_furture = Refdata.remain_HP;
//			}
			//hp_furture 良好 more_heat留出余量
			/* TODO */
			
			//未超出热量仅判断模式选择tick_sleep
			if(gun1.mode == STRICK_NOLIMITE)
				{tick_sleep = 50;}
	        else if(gun1.mode == STRICK_TRIPLE)
				{tick_sleep = 600;}
			else if(gun1.mode == STRICK_SINGLE)
				{tick_sleep = 100;}
			 //如果处于超出热量但未到余量的情况 对tick_sleep做出限制
			 /* TODO */
			 
			/* 对超出的热量的情况做出控制 */
			if(now_furture >= p_temp->max+more_heat)
			{
				gun1.status = STRICK_STOP;
			}
			else
			{
				gun1.status &= ~STRICK_STOP;
			}
			#else
			if(gun1.mode == STRICK_SINGLE)
			{}
			else if(gun1.mode = STRICK_TRIPLE)
			{}
			#endif
		}
	}
}

/***********************************热量控制模块******************************************/
/* 如果不使用本地热量计算 可能需要定义一个定时器 每隔一个热量检测周期从裁判系统更新当前热量 */

static Heatctrl_t *p_heat = RT_NULL;													/*热量控制块指针*/

/* 如果使能本地热量计算 */
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
		 /* 更新热量 */
		 #ifdef BULLET_17
		 p_temp->now = Refdata.heat_17;
		 p_temp->max = Refdata.heat_limit_17;
		 p_temp->cool = Refdata.cooling_rate_17;
		 #endif
		 #ifdef MOBILE_17 /* 工程发射机构 */
		 p_temp->now = Refdata.heat_17;
		 p_temp->cool = Refdata.cooling_rate_17;
		 p_temp->max = 150;
		 #endif
		 #ifdef BULLET_42
		 p_temp->now = Refdata.heat_42;
		 p_temp->max = Refdata.heat_limit_42;
		 p_temp->cool = Refdata.cooling_rate_42;
		 #endif
		 
		 p_temp->rate = (100 * (p_temp->max-p_temp->now)) / p_temp->max;
		 
		 
		 /*热量控制*/
		 heat_control(p_temp);					
		 #if LOCAL_HEAT_ENABLE
		 heatctrl_cool(p_temp);					/*冷却*/
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
	heat->now = 0;
	heat->unlimit_heat = 0;
	
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
	#ifdef RUB_SNAIL
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
/*对于多枪管，需要重新再定义一个相同作用函数，待修改*/
/**
 * @brief  卡弹判定
 * @param  motor：发射机构结构体指针
 */
void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;								/*记录系统时间*/
	static rt_uint8_t temp = 0;
	/*如果卡弹*/
	if(gun->status & STRICK_STUCK)
	{	
		if(temp == 1)
		{
			if(rt_tick_get() > tick)						/*倒转1000ms*/
			{
				gun->status &= ~STRICK_STUCK;
				stuck_time = 0;
				temp = 0;
			}
		}
		else
		{
			motor->ang.set = motor->dji.angle + 3000;	
			tick = rt_tick_get()+1000;
			temp=1;
		}
		
	}
	else if(~(gun->status & STRICK_STOP))					/*如果没有禁止转动*/
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
	static rt_tick_t tick = 0;										/*记录系统时间*/
	/* 如果摩擦轮速度小于一定值 */
	if(ABS(gun->speed) < 11)
	{
		gun->status = STRICK_STOP;
	}
	else
	{
		gun->status &= ~STRICK_STOP;
	}
	/*如果停止开火，使位置环不动*/
	if(gun->status & STRICK_STOP)									/*不允许开火，直接返回*/
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
			if(gun->mode & STRICK_NOLIMITE)							/*不停转动*/
			{									/*间隔时间*/
				motor_angle_set(motor, FIRE_ANGLE);
			}
			else if(gun->mode & STRICK_SINGLE)						/*单发*/
			{
				motor_angle_set(motor, FIRE_ANGLE);
			}
			else if(gun->mode & STRICK_TRIPLE)						/*三连发*/
			{
				motor_angle_set(motor, FIRE_ANGLE*3);
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************发射机构线程*******************************************/
static struct rt_semaphore task_1ms_sem;     						/* 用于接收消息的信号量 */
static struct rt_semaphore task_10ms_sem;     						/* 用于接收消息的信号量 */
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
static void task_1ms_entry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		
		rt_int16_t send_current[4] = {0};
		rt_int16_t launch_6020_cur[4] = {0};
		
		pid_output_calculate(&m_launch.spe,m_launch.ang.out,m_launch.dji.speed);
		
		#ifndef RUB_SNAIL /* 如果是3508摩擦轮 */
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].dji.speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].dji.speed);
		send_current[(RUB0_ID-0x201)] = m_rub[0].spe.out;
		send_current[(RUB1_ID-0x201)] = m_rub[1].spe.out;
		#endif
		//6020拨弹,ID不同
		#ifdef LAUNCH_6020
		motor_current_send(can2_dev,STDID_launch,send_current[0],send_current[1],send_current[2],send_current[3]);
		launch_6020_cur[(LAUNCH_ID-0x205)] = m_launch.spe.out;
		motor_current_send(can1_dev,0x1FF,launch_6020_cur[0],launch_6020_cur[1],launch_6020_cur[2],launch_6020_cur[3]);
		#else   //2006拨弹
		send_current[(LAUNCH_ID-0x201)] = m_launch.spe.out;
		motor_current_send(can1_dev,STDID_launch,send_current[0],send_current[1],send_current[2],send_current[3]);
		#endif
	}
}
static void task_10ms_entry(void *parameter)
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
static void strike_start(void)
{
	/*定时器处理线程*/
	rt_thread_t thread;
	rt_sem_init(&task_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("1ms_sem", task_1ms_entry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	rt_sem_init(&task_10ms_sem, "10ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("10ms_sem", task_10ms_entry, RT_NULL, 1024, 1, 1);
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

//以下函数应该放在私有文件中,自己调整PID参数,以下作为示例
__weak void strike_pid_init(void)
{
	//自己初始化发射机构(摩擦轮+发弹)的PID

	//举例1:snail摩擦轮+2006拨弹

	//举例2:3508摩擦轮+2006拨弹
	// pid_init(&m_rub[0].spe,  
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	// pid_init(&m_rub[1].spe, 
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	// pid_init(&m_launch.ang, 
	// 				3.5,0,0,
	// 				500,5000,-5000);
	// pid_init(&m_launch.spe, 
	// 				7.5,0,0,
	// 				350,8000,-8000);

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
	tick_sleep = 50;
	//摩擦轮电机选择:
	 pid_init(&m_launch.ang, 
	 				5.5,0,0,
	 				500,5000,-5000);
	 pid_init(&m_launch.spe, 
	 				7.5,0,0,
	 				350,8000,-8000);
	//1. Snail
	//2. 3508屁股
	#ifdef RUB_SNAIL
	motor_rub_init();
	#else	//默认3508
	motor_init(&m_rub[0],RUB0_ID,1);
	motor_init(&m_rub[1],RUB1_ID,1);
	#endif

	//	motor_servo_init();
	motor_init(&m_launch,LAUNCH_ID,0.027973);

	strike_pid_init();

	heatctrl_init(&gun->heat, max);						/*热量控制参数初始化*/
	strike_start();
}
