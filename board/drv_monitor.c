/************************************** Copyright ****************************** 
  *                 (C) Copyright 2020,China, HITwh.
  *                            All Rights Reserved
  *
  *                     HITwh Excellent Robot Organization
  *                     https://github.com/HERO-ECG
  *                     https://gitee.com/HIT-718LC
  *
  * FileName   : drv_monitor.c
  * Version    : v1.0
  * Author     : mqy
  * Date       : 2020-02-14
  * Description:
********************************************************************************/

#include "drv_monitor.h"

//监视线程句柄
static rt_thread_t 	monitor_device 	= RT_NULL;

//报警线程句柄
static rt_thread_t 	alarm_device 		= RT_NULL;

//小看门狗链表头指针
swdg_dev_t*         monitor_hp      = RT_NULL;

#ifdef BSP_USING_WDT
	//硬件看门狗设备句柄
	static rt_device_t 	wdg_dev 				= RT_NULL;
#endif

/**
* @brief：  监视器线程
* @param：  无	
* @return：	无
* @author： mqy
*/
static void monitor_thread(void* parameter)
{
  while(1)
  {
    swdg_dev_t *swdg_dev_tem = monitor_hp;
    while(swdg_dev_tem != RT_NULL)
    {
    	if(swdg_dev_tem->time_residue < MONITOR_PERIOD)
		{
			swdg_dev_tem->alarm_flag = 1;
		}
		else
		{
			swdg_dev_tem->alarm_flag = 0;
			swdg_dev_tem->time_residue -= MONITOR_PERIOD;
		}
		swdg_dev_tem = swdg_dev_tem->next;
    }
		
	#ifdef BSP_USING_WDT
		//定时喂狗不然就gg了w
		rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, RT_NULL);
	#endif
		
		//延时
    rt_thread_mdelay(MONITOR_PERIOD);
  }
}
/**
* @brief：  报警线程
* @param：  无	
* @return：	无
* @author： mqy
*/
static void raise_the_alarm(alarm_horn_e alarm_horn);
static void alarm_thread(void* parameter)
{
  	while(1)
  	{
		swdg_dev_t *swdg_dev_tem = monitor_hp;
		while(swdg_dev_tem != RT_NULL)
		{
			if(swdg_dev_tem->alarm_flag)
			{
				raise_the_alarm(swdg_dev_tem->alarm_horn);
			}
			swdg_dev_tem = swdg_dev_tem->next;
		}
		rt_thread_mdelay(MONITOR_PERIOD);
  	}
}

/**
* @brief：  该函数初始化监视器
* @param：  无	
* @return：	0：初始化失败
* @author： mqy
*/
static int monitor_init(void)
{
	//监视器线程创建
	monitor_device = rt_thread_create(
	"monitor",
	monitor_thread,	//入口函数
	RT_NULL,				//入口参数
	1024,						//线程栈
	25,							//优先级
	2);
	
	//查看是否创建成功
	if(monitor_device != RT_NULL)
	{
		rt_thread_startup(monitor_device);
	}
	
	//报警线程创建
	alarm_device = rt_thread_create(
	"monitor",
	alarm_thread,		//入口函数
	RT_NULL,				//入口参数
	1024,						//线程栈
	26,							//优先级
	2);
	
	//查看是否创建成功
	if(alarm_device != RT_NULL)
	{
		rt_thread_startup(alarm_device);
	}
	
	#ifdef BSP_USING_WDT
		//查找启动硬件看门狗IWDG timer
		wdg_dev = rt_device_find("wdt");
		
		//初始化同时启动IWDG（这里跟emmmRTT对看门狗的描述不太一样）
		rt_device_init(wdg_dev);
	#endif
	
	
	return 1;
}
INIT_APP_EXPORT(monitor_init);

/**
* @brief：  创建一个看门狗
* @param：  name：监视名称
            time_threshold：报警时间，超过该时间不反馈则会报警（单位ms）
            alarm_horn：报警形式，可取值为枚举值
* @return：	0：初始化失败
* @author： mqy
*/
swdg_dev_t* swdg_creaat(const char* name,
                      rt_uint32_t time_threshold,
                      alarm_horn_e alarm_horn)
{
  swdg_dev_t* swdg_dev_tem = (swdg_dev_t*)rt_malloc(sizeof(swdg_dev_t));//申请空间
	if(swdg_dev_tem == RT_NULL)
	{
		return RT_NULL;
	}
	//拷贝字符串
	rt_strncpy(swdg_dev_tem->name, name, RT_NAME_MAX);
	
	//构建链表
	swdg_dev_tem->next = monitor_hp;
	monitor_hp = swdg_dev_tem;
	
	//赋值
	swdg_dev_tem->alarm_horn = alarm_horn;
	swdg_dev_tem->time_threshold = time_threshold;
	swdg_dev_tem->time_residue = time_threshold;
	swdg_dev_tem->alarm_flag = 0;
	return swdg_dev_tem;
}
/**
* @brief：  移除一个看门狗
* @param：  看门狗句柄
* @return：	0：初始化失败
* @author： mqy
*/
int swdg_delete(swdg_dev_t* swdg_dev)
{
	//遍历链表，清除该设备
	swdg_dev_t **swdg_dev_tem = &monitor_hp;
	while((*swdg_dev_tem) != RT_NULL)
	{
		//判断相等
		if((*swdg_dev_tem) == monitor_hp)
		{
			//变量a临时储存一下
			swdg_dev_t *a = (*swdg_dev_tem)->next;
			(*swdg_dev_tem)->next = RT_NULL;
			(*swdg_dev_tem) = a;
			return 1;
		}
		swdg_dev_tem = (&(*swdg_dev_tem)->next);
	}
	return 0;
}

/**
* @brief：  给看门狗喂食
* @param：  无
* @return：	无
* @author： mqy
*/
void swdg_feed(swdg_dev_t* swdg_dev)
{
	swdg_dev->time_residue = swdg_dev->time_threshold;
	return;
}
/**
* @brief：  报警函数
* @param：  alarm_horn：报警形式
* @return：	无
* @author： mqy
*/
static void raise_the_alarm(alarm_horn_e alarm_horn)
{
	switch(alarm_horn)
	{
		case NO_ALARM:
			break;
		case BUZZER_100HZ_300MS:
			set_buzzer(100);
			rt_thread_mdelay(300);
			set_buzzer(0);
			break;
		case BUZZER_200HZ_600MS:
			set_buzzer(200);
			rt_thread_mdelay(600);
			set_buzzer(0);
			break;
		case RGB_WHITE_900MS:
			set_RGB(CORE_WHITE);
			rt_thread_mdelay(900);
			set_RGB(CORE_BLACK);
			break;
		default:
			break;
	}
	return;
}
