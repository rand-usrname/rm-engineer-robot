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
#include "drv_rgblight.h"
#include "drv_buzzer.h"

#define MONITOR_PERIOD	500	//设定监控周期，单位ms

//可自定义的报警信息
//buzzer tick amount
#define BUZZER_TICK_ONCE    0x001
#define BUZZER_TICK_TWICE   0x002
//RGB color select
#define RGB_RED             0x010
#define RGB_BLUE            0x020
#define RGB_WHITE           0x030
#define RGB_YELLOW          0x040
#define RGB_GREEN           0x050
#define RGB_PURPLE          0x060


//看门狗句柄
typedef struct swdg_dev
{
	char            	name[RT_NAME_MAX];  //小看门狗名
	rt_uint32_t     	time_threshold;     //时间阈值，超过该时间未响应则报警，单位ms
	rt_uint32_t			time_residue;		//剩余时间，若超过改时间仍不更新就报警）
	rt_base_t           alarm_mode;         //报警形式
	rt_uint8_t			alarm_flag;			//报警标志位
	rt_thread_t         thread;            //当前线程句柄
	struct swdg_dev*	next;               //指向下一个要监视的看门狗句柄
	
}swdg_dev_t;

extern swdg_dev_t* swdg_create(const char* name,rt_thread_t thread,rt_uint32_t time_threshold,rt_base_t mode);
extern void swdg_feed(swdg_dev_t* swdg_dev);

#endif
