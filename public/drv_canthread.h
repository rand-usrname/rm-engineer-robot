#ifndef __DRV_CANTHREAD_H__
#define __DRV_CANTHREAD_H__
#include <rtdevice.h>
#include <rtthread.h>

extern rt_device_t can1_dev;      //CAN �豸���
extern rt_device_t can2_dev;      //CAN �豸���

//can1��ʼ����can1���ݴ����̺߳��ж��趨
extern int can1_init(void);
//can2��ʼ����can2���ݴ����̺߳��ж��趨
extern int can2_init(void);

#endif



