#include "trailer.h"
/*
�������� trailer_thread_entry 20ms����trailer_go��ȡ�ϳ�״̬ ���ýǶ����� ���Ƿ���δ����ң��������ʱ ������̣߳�
��ʱ������pid���͵���
*/

trailer_t trailerl;
trailer_t trailerr;


void trailer_init(trailer_t *trailer,rt_uint32_t ID,rt_uint16_t Pin_num)
{
	trailer->state = rescue_unable;
	motor_init(&trailer->trailer_motor,ID,TRAILER_RADIO);
	trailer->Pin_num = Pin_num;
	/* pidδ��ʼ�� */

}
rt_int16_t reset_angle = 0;
void trailer_go(trailer_t *trailer)
{
	/* ��ȡ��λ����״̬ */
	if((rt_pin_read(trailer->Pin_num))&&(trailer->state == 0x22)){trailer->state = rescue_going;}
	/* ����ϳ�״̬ */
	switch(trailer->state)
	{
		/* ����Ҫ��ȡ��ǰ�Ƕ�������״̬ */
		case rescue_prepare:                                                       /* ����׼��״̬ */
			motor_angle_set(&trailer->trailer_motor,prepare_angle);
		    trailer->state|= 0x20;                                                 /* ��ֹ��ν��� */
		break;
		case rescue_going:                                                         /* �ϳ�ץȡ */
			motor_angle_set(&trailer->trailer_motor,catch_angle);
		    trailer->state|= 0x40;
		break;
		case rescue_reset:                                                      /* ת��ԭλ */
			motor_angle_set(&trailer->trailer_motor,reset_angle);
		    trailer->state = rescue_unable;
		break;
		default:
	    break;
	}
}


static struct rt_semaphore trailer_sem;     /* ���ڽ�����Ϣ���ź��� */
/*1ms����*/
static struct rt_timer trailer_timer;
/* task_1ms ��ʱ���� */
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
			/* �ٶȻ����� */
			pid_output_calculate(&trailerl.trailer_motor.spe,trailerl.trailer_motor.ang.out,trailerl.trailer_motor.dji.speed);
			pid_output_calculate(&trailerr.trailer_motor.spe,trailerr.trailer_motor.ang.out,trailerr.trailer_motor.dji.speed);
			/* �������� */
			
		}
		else if(status==9)
		{
			/* �ǶȻ����� */
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
	
	/* ����һ����ʱ��������PID */
	rt_sem_init(&trailer_sem, "trailer_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("trailer_task", trailer_tim_entry, RT_NULL, 1024, 10, 2);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	/*��ʱ���ж�*/
	rt_timer_init(&trailer_timer,
                   "trailer_task",
                   trailer_timer_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&trailer_timer);
}




