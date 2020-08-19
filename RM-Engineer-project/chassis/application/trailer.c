#include "trailer.h"
/*
控制流程 trailer_thread_entry 20ms运行trailer_go读取拖车状态 设置角度期望 （是否在未按下遥控器按键时 挂起该线程）
定时器计算pid发送电流
*/

trailer_t trailerl;
trailer_t trailerr;


void trailer_init(trailer_t *trailer,rt_uint32_t ID,rt_uint16_t Pin_num)
{
	trailer->state = rescue_unable;
	motor_init(&trailer->trailer_motor,ID,TRAILER_RADIO);
	trailer->Pin_num = Pin_num;
	/* pid未初始化 */

}
rt_int16_t reset_angle = 0;
void trailer_go(trailer_t *trailer)
{
	/* 读取限位开关状态 */
	if((rt_pin_read(trailer->Pin_num))&&(trailer->state == 0x22)){trailer->state = rescue_going;}
	/* 检查拖车状态 */
	switch(trailer->state)
	{
		/* 都需要读取当前角度来到达状态 */
		case rescue_prepare:                                                       /* 进入准备状态 */
			motor_angle_set(&trailer->trailer_motor,prepare_angle);
		    trailer->state|= 0x20;                                                 /* 防止多次进入 */
		break;
		case rescue_going:                                                         /* 拖车抓取 */
			motor_angle_set(&trailer->trailer_motor,catch_angle);
		    trailer->state|= 0x40;
		break;
		case rescue_reset:                                                      /* 转回原位 */
			motor_angle_set(&trailer->trailer_motor,reset_angle);
		    trailer->state = rescue_unable;
		break;
		default:
	    break;
	}
}


static struct rt_semaphore trailer_sem;     /* 用于接收消息的信号量 */
/*1ms任务*/
static struct rt_timer trailer_timer;
/* task_1ms 超时函数 */
static void trailer_timer_IRQHandler(void *parameter)
{
	rt_sem_release(&trailer_sem);
}

static void trailer_tim_entry(void *parameter)
{
	static rt_uint8_t status = 0;
	while(1)
	{
		rt_sem_take(&trailer_sem, RT_WAITING_FOREVER);	
		if(status<9)
        {
			/* 速度环计算 */
			pid_output_calculate(&trailerl.trailer_motor.spe,trailerl.trailer_motor.ang.out,trailerl.trailer_motor.dji.speed);
			pid_output_calculate(&trailerr.trailer_motor.spe,trailerr.trailer_motor.ang.out,trailerr.trailer_motor.dji.speed);
			/* 电流发送 */
			
		}
		else if(status==9)
		{
			/* 角度环计算 */
			pid_output_motor(&trailerl.trailer_motor.ang,trailerl.trailer_motor.ang.set,trailerl.trailer_motor.dji.angle);
			pid_output_motor(&trailerr.trailer_motor.ang,trailerr.trailer_motor.ang.set,trailerr.trailer_motor.dji.angle);
		}
		status++;
		if(status>=9){status = 0;}
	}
}
void trailer_start(void)
{
	trailer_init(&trailerl,TRAILER_LEFT,TRAILER_PINL);
	trailer_init(&trailerr,TRAILER_RIGHT,TRAILER_PINR);
	rt_thread_t thread;
	
	/* 定义一个计时器来计算PID */
	rt_sem_init(&trailer_sem, "trailer_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("trailer_task", trailer_tim_entry, RT_NULL, 1024, 10, 2);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	/*定时器中断*/
	rt_timer_init(&trailer_timer,
                   "trailer_task",
                   trailer_timer_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* 启动定时器 */
	rt_timer_start(&trailer_timer);
}




