/************************************** Copyright ****************************** 
  *                 (C) Copyright 2020,mqy,China,HITwh.
  *                            All Rights Reserved
  *
  *                     HITwh Excellent Robot Organization
  *                     https://github.com/HERO-ECG
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : drv_monitor.h
  * Version    : v1.0
  * Author     : mqy
  * Date       : 2020-02-14
  * Description:
********************************************************************************/

#ifndef __DRV_MONITOR_H__
#define __DRV_MONITOR_H__

#include <rtthread.h>
#include <rtdevice.h>

#define MONITOR_PERIOD	500	//设定监控周期，单位ms

typedef enum
{
	NO_ALARM 						= 0,
  BUZZER_100HZ_300MS	= 1,
	BUZZER_200HZ_600MS	= 2,
	RGB_WHITE_900MS			= 3,
}alarm_horn_e;

//看门狗句柄
typedef struct swdg_dev
{
	char            	name[RT_NAME_MAX];  //小看门狗名
  rt_uint32_t     	time_threshold;     //时间阈值，超过该时间未响应则报警，单位ms
	rt_uint32_t				time_residue;				//剩余时间，若超过改时间仍不更新就报警）
  alarm_horn_e			alarm_horn;					//报警形式
	rt_uint8_t				alarm_flag;					//报警标志位
  struct swdg_dev*	next;               //指向下一个要监视的看门狗句柄
}swdg_dev_t;

extern swdg_dev_t* swdg_creaat(const char* name,rt_uint32_t time_threshold,alarm_horn_e alarm_horn);
extern int swdg_delete(swdg_dev_t* swdg_dev);
extern void swdg_feed(swdg_dev_t* swdg_dev);

#endif
