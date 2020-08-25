#include "task_create.h"
#include "task_chassis.h"

#include <rtthread.h>

static struct rt_semaphore task_1ms_sem;     														/* 用于接收消息的信号量 */
static struct rt_semaphore task_5ms_sem;     													/* 用于接收消息的信号量 */
static struct rt_semaphore task_101ms_sem;     													/* 用于接收消息的信号量 */
/*1ms任务*/
static struct rt_timer task_1ms;
/*5ms任务*/
static struct rt_timer task_5ms;
/*101ms任务*/
static struct rt_timer task_101ms;

/* task_1ms 超时函数 */
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_1ms_sem);
}
/* task_5ms 超时函数 */
static void task_5ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_5ms_sem);
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
		task_fun1ms();
	}
}
static void task_5ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_5ms_sem, RT_WAITING_FOREVER);
		task_fun5ms();
	}
}
static void task_101ms_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_101ms_sem, RT_WAITING_FOREVER);
		task_fun101ms();
	}
}

/**
 * @brief  任务创建
 */
void task_create(void)
{
	/*定时器处理线程*/
	rt_thread_t thread;
	rt_sem_init(&task_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("1ms_sem", task_1ms_emtry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	rt_sem_init(&task_5ms_sem, "5ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("5ms_sem", task_5ms_emtry, RT_NULL, 1024, 1, 1);
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
	rt_timer_init(&task_5ms,
								 "5mstask",
								 task_5ms_IRQHandler,
								 RT_NULL,
								 5, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_init(&task_101ms,
								 "101mstask",
								 task_101ms_IRQHandler,
								 RT_NULL,
								 101, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* 启动定时器 */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_5ms);
	rt_timer_start(&task_101ms);
}

/* 自动初始化 */
INIT_APP_EXPORT(task_create);

