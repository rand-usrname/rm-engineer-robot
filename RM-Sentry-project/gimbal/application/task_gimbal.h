#ifndef __TASK_GIMBAL_H__
#define __TASK_GIMBAL_H__
#include <rtdevice.h>
#define  STDID_SLAVE_GIMBAL  0x100
#define  STDID_SLAVE_STRIKE  0x101

/*5ms任务执行函数*/
void task_fun5ms(void);
/*1ms任务执行函数*/
void task_fun1ms(void);
/*101ms任务执行函数*/
void task_fun101ms(void);
void slave_readmsg(struct rt_can_msg *rxmsg);
#endif



