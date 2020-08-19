#include "ammo.h"

Motor_t Raise[2];
Motor_t Grap[2];

/*********************对于步进形式的抬升和机械爪的接口**********************/
/**
  * @brief  设置抬升机构的角度
  * @param  angle 抬升机构角度设置
  * @param  step  抬升每10ms的步进
  * @retval none
 **/
void raise_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Raise[0],step);
	motor_angle_set(&Raise[1],-step);
}

/**
  * @brief  设置取弹机构的角度
  * @param  angle 取弹机械爪角度设置
  * @param  step  取弹每10ms的步进
  * @retval none
 **/
void grap_angle_set(rt_int16_t angle,rt_uint8_t step)
{
	motor_angle_set(&Grap[0],step);
	motor_angle_set(&Grap[1],-step);
}
/******************如果抬升机构采取速度环*****************/
void raise_speed_set(rt_int16_t speed)
{
	Raise[0].spe.set = speed;
	Raise[1].spe.set = -speed;
}

/**************设定输入减速比之后的机械爪的角度设定****************/
void grap_radio_angle_set(rt_int16_t angle)
{
	motor_angle_set(&Grap[0],angle);
	motor_angle_set(&Grap[1],-angle);
}



/* 定义定时器来计算PID */
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
		#ifdef RAISE_SPEED
		pid_output_calculate(&Raise[0].spe,Raise[0].spe.set,Raise[0].dji.speed);
		pid_output_calculate(&Raise[1].spe,Raise[1].spe.set,Raise[1].dji.speed);
		#endif
		#ifdef RAISE_ANGLE
		pid_output_calculate(&Raise[0].ang,Raise[0].ang.out,Raise[0].dji.speed);
		pid_output_calculate(&Raise[1].ang,Raise[1].ang.out,Raise[1].dji.speed);
		#endif
		pid_output_calculate(&Grap[0].spe,Grap[0].ang.out,Grap[0].dji.speed);
		pid_output_calculate(&Grap[1].spe,Grap[1].ang.out,Grap[1].dji.speed);
		pid_output_calculate(&m_magazine.spe,m_magazine.ang.out,m_magazine.dji.speed);
		/* 发送电流 */
	}
}
static void task_10ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_10ms_sem, RT_WAITING_FOREVER);
		/* 计算角度环 */
		#ifdef RAISE_ANGLE
		pid_output_motor(&Raise[0].ang,Raise[0].ang.set,Raise[0].dji.angle);
		pid_output_motor(&Raise[1].ang,Raise[1].ang.set,Raise[1].dji.angle);
		#endif
		pid_output_motor(&Grap[0].ang,Grap[0].ang.set,Grap[0].dji.angle);
		pid_output_motor(&Grap[1].ang,Grap[1].ang.set,Grap[1].dji.angle);
		pid_output_motor(&m_magazine.ang,m_magazine.ang.set,m_magazine.dji.angle);
	}
}

/**
 * @brief  任务创建
 */
static void Ammo_start(void)
{
	/*定时器处理线程*/
	rt_thread_t thread;
	rt_sem_init(&task_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("1ms_sem", task_1ms_emtry, RT_NULL, 1024, 2, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	rt_sem_init(&task_10ms_sem, "10ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("10ms_sem", task_10ms_emtry, RT_NULL, 1024, 2, 1);
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
void Ammo_init(void)
{
	
	motor_init(&Raise[0],UPLIFT_LEFT,1);
	motor_init(&Raise[1],UPLIFT_RIGHT,1);
	motor_init(&Grap[0],GRAP_LEFT,1);
	motor_init(&Grap[1],GRAP_RIGHT,1);
	/* PID 初始化 */
	/* ...... */
	Ammo_start();
}
