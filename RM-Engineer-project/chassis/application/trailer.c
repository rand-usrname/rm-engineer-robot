#include "trailer.h"
#include "drv_motor.h"
#include "drv_canthread.h"
/*
�������� trailer_thread_entry 20ms����trailer_go��ȡ�ϳ�״̬ ���ýǶ����� ���Ƿ���δ����ң��������ʱ ������̣߳�
��ʱ������pid���͵���
*/

trailer_t trailerl;
trailer_t trailerr;


/* ÿ��У������ƫ��ֵ */
float trailerl_offset;
float trailerr_offset;
/* �궨��ʼ�Ƕ� */
 rt_uint16_t origin_anglel;
 rt_uint16_t origin_angler;
void trailer_init(trailer_t *trailer,rt_uint32_t ID,rt_uint16_t Pin_num)
{
	trailer->state = rescue_unable;
	motor_init(&trailer->trailer_motor,ID,TRAILER_RADIO);
	trailer->Pin_num = Pin_num;
	/* pidδ��ʼ�� */
	/* �궨��ʼ�Ƕ� */

}
void trailer_go(trailer_t *trailer)
{
	/* ����ϳ�״̬ */
	if(trailer == &trailerl)
	{
		switch(trailer->state)
		{
			/* ����Ҫ��ȡ��ǰ�Ƕ�������״̬ */
			case rescue_prepare:                                                       /* ����׼��״̬ */
				motor_angle_set(&trailer->trailer_motor,(prepare_anglel+trailerl_offset));
				trailer->state|= 0x20;                                                 /* ��ֹ��ν��� */
			break;
			case rescue_going:                                                         /* �ϳ�ץȡ */
				motor_angle_set(&trailer->trailer_motor,catch_anglel);
				trailer->state|= 0x40;
			break;
			case rescue_prereset :
				motor_angle_set(&trailer->trailer_motor,-(prepare_anglel+trailerl_offset));
				trailer->state |= 0x80;
			break;
			case rescue_goreset :
				motor_angle_set(&trailer->trailer_motor,-(prepare_anglel+catch_anglel+trailerl_offset));
				trailer->state |= 0x01;
			break;
			default:
			break;
		}
	}
	else
	{
		switch(trailer->state)
		{
			/* ����Ҫ��ȡ��ǰ�Ƕ�������״̬ */
			case rescue_prepare:                                                       /* ����׼��״̬ */
				motor_angle_set(&trailer->trailer_motor,(prepare_angler-trailerr_offset));
				trailer->state|= 0x20;                                                 /* ��ֹ��ν��� */
			break;
			case rescue_going:                                                         /* �ϳ�ץȡ */
				motor_angle_set(&trailer->trailer_motor,catch_angler);
				trailer->state|= 0x40;
			break;
			case rescue_prereset :
				motor_angle_set(&trailer->trailer_motor,-(prepare_angler-trailerr_offset));
				trailer->state |= 0x80;
			break;
			case rescue_goreset :
				motor_angle_set(&trailer->trailer_motor,-(prepare_angler+catch_angler-trailerr_offset));
				trailer->state |= 0x01;
			break;
			default:
			break;
		}
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
			if(status==9)
			{
				/* �ǶȻ����� */
				pid_output_motor(&trailerl.trailer_motor.ang,trailerl.trailer_motor.ang.set,trailerl.trailer_motor.dji.angle);
				pid_output_motor(&trailerr.trailer_motor.ang,trailerr.trailer_motor.ang.set,trailerr.trailer_motor.dji.angle);
			}
			/* �ٶȻ����� */
			pid_output_calculate(&trailerl.trailer_motor.spe,trailerl.trailer_motor.ang.out,trailerl.trailer_motor.dji.speed);
			pid_output_calculate(&trailerr.trailer_motor.spe,trailerr.trailer_motor.ang.out,trailerr.trailer_motor.dji.speed);
			/* �������� */
			motor_current_send(can1_dev,0x1FF,trailerl.trailer_motor.spe.out,trailerr.trailer_motor.spe.out,0,0);
			status++;
			if(status>9){status = 0;}
	}
}
void trailer_start(void)
{
	trailer_init(&trailerl,TRAILER_LEFT,TRAILER_PINL);
	trailer_init(&trailerr,TRAILER_RIGHT,TRAILER_PINR);
	pid_init(&trailerl.trailer_motor.ang, 
 				10.5,0,0,
 				500,5000,-5000);
	pid_init(&trailerl.trailer_motor.spe, 
 				10,0,0,
 				350,8000,-8000);
	pid_init(&trailerr.trailer_motor.ang, 
 				7.5,0,0,
 				500,5000,-5000);
	pid_init(&trailerr.trailer_motor.spe, 
 				9,0,0,
 				350,8000,-8000);
	rt_thread_t thread;
	
	/* ����һ����ʱ��������PID */
	rt_sem_init(&trailer_sem, "trailer_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("trailer_task", trailer_tim_entry, RT_NULL, 1024, 5, 4);
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




