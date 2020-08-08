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
//�����߳̾��
static rt_thread_t 	monitor_device 	= RT_NULL;

//�����߳̾��
static rt_thread_t 	alarm_device 		= RT_NULL;

//С���Ź�����ͷָ��
swdg_dev_t*         monitor_hp      = RT_NULL;

#ifdef BSP_USING_WDT
	//Ӳ�����Ź��豸���
	static rt_device_t 	wdg_dev 				= RT_NULL;
#endif

#define RGB_FLASH_TIME  120
#define BUZZER_SET  set_buzzer(500);rt_thread_mdelay(40);set_buzzer(0);rt_thread_mdelay(80);
#define RGB_RED_flash  set_RGB(CORE_RED);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
#define RGB_BLUE_flash  set_RGB(CORE_BLUE);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
#define RGB_WHITE_flash  set_RGB(CORE_WHITE);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
#define RGB_YELLOW_flash  set_RGB(CORE_YELLOW);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
#define RGB_GREEN_flash  set_RGB(CORE_GREEN);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
#define RGB_PURPLE_flash  set_RGB(CORE_PURPLE);rt_thread_mdelay(RGB_FLASH_TIME);set_RGB(CORE_BLACK);
/**
* @brief��  �������߳�
* @param��  ��	
* @return��	��
* @author�� mqy
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
			//��ʱι����Ȼ��gg��w
			rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, RT_NULL);
		#endif
		
		//��ʱ
    rt_thread_mdelay(MONITOR_PERIOD);
  }
}
/**
* @brief��  �����߳�
* @param��  ��	
* @return��	��
* @author�� mqy
*/
static void raise_the_alarm(rt_base_t mode);
static void alarm_thread(void* parameter)
{
  while(1)
  {
		swdg_dev_t *swdg_dev_tem = monitor_hp;
		while(swdg_dev_tem != RT_NULL)
		{
			if(swdg_dev_tem->alarm_flag)
			{
				rt_thread_suspend(swdg_dev_tem->thread);
				if(rt_pin_read(67))
				{raise_the_alarm(swdg_dev_tem->alarm_mode);}
			}
			swdg_dev_tem = swdg_dev_tem->next;
		}
		rt_thread_mdelay(MONITOR_PERIOD);
  }
}

/**
* @brief��  �ú�����ʼ��������
* @param��  ��	
* @return��	0����ʼ��ʧ��
* @author�� mqy
*/
static int monitor_init(void)
{
	rt_pin_mode(67,PIN_MODE_INPUT_PULLDOWN);
	//�������̴߳���
	monitor_device = rt_thread_create(
	"monitor",
	monitor_thread,	//��ں���
	RT_NULL,				//��ڲ���
	1024,						//�߳�ջ
	25,							//���ȼ�
	2);
	
	//�鿴�Ƿ񴴽��ɹ�
	if(monitor_device != RT_NULL)
	{
		rt_thread_startup(monitor_device);
	}
	
	//�����̴߳���
	alarm_device = rt_thread_create(
	"alarm",
	alarm_thread,		//��ں���
	RT_NULL,				//��ڲ���
	1024,						//�߳�ջ
	26,							//���ȼ�
	2);
	
	//�鿴�Ƿ񴴽��ɹ�
	if(alarm_device != RT_NULL)
	{
		rt_thread_startup(alarm_device);
	}
	
	#ifdef BSP_USING_WDT
		//��������Ӳ�����Ź�IWDG timer
		wdg_dev = rt_device_find("wdt");
		
		//��ʼ��ͬʱ����IWDG�������emmmRTT�Կ��Ź���������̫һ����
		rt_device_init(wdg_dev);
	#endif
	
	
	return 1;
}
INIT_APP_EXPORT(monitor_init);

/**
* @brief��  ����һ�����Ź�
* @param��  name����������
            time_threshold������ʱ�䣬������ʱ�䲻������ᱨ������λms��
            alarm_horn��������ʽ����ȡֵΪö��ֵ
* @return��	0����ʼ��ʧ��
* @author�� mqy
*/
swdg_dev_t* swdg_create(const char* name,rt_thread_t thread,
                      rt_uint32_t time_threshold,
                      rt_base_t mode)
{
  swdg_dev_t* swdg_dev_tem = (swdg_dev_t*)rt_malloc(sizeof(swdg_dev_t));//����ռ�
	if(swdg_dev_tem == RT_NULL)
	{
		return RT_NULL;
	}
	//�����ַ���
  rt_strncpy(swdg_dev_tem->name, name, RT_NAME_MAX);
	
	//��������
	swdg_dev_tem->next = monitor_hp;
	monitor_hp = swdg_dev_tem;
	
	//��ֵ
	swdg_dev_tem->alarm_mode = mode;
	swdg_dev_tem->thread = thread;
	swdg_dev_tem->time_threshold = time_threshold;
	swdg_dev_tem->time_residue = time_threshold;
	swdg_dev_tem->alarm_flag = 0;
	return swdg_dev_tem;
}

/**
* @brief��  �����Ź�ιʳ
* @param��  ��
* @return��	��
* @author�� mqy
*/
void swdg_feed(swdg_dev_t* swdg_dev)
{
	swdg_dev->time_residue = swdg_dev->time_threshold;
	rt_thread_resume(swdg_dev->thread);
	return;
}
/**
* @brief��  ��������
* @param��  alarm_horn��������ʽ
* @return��	��
* @author�� mqy
*/
static void raise_the_alarm(rt_base_t mode)
{
	switch(mode)
	{
		case 0x11:
		    RGB_RED_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x21:
			RGB_BLUE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x31:
			RGB_WHITE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x41:
			RGB_YELLOW_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x51:
			RGB_GREEN_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x61:
			RGB_PURPLE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x12:
			BUZZER_SET
			RGB_RED_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x22:
			BUZZER_SET
			RGB_BLUE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x32:
			BUZZER_SET
			RGB_WHITE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x42:
			BUZZER_SET
			RGB_YELLOW_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x52:
			BUZZER_SET
			RGB_GREEN_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		case 0x62:
			BUZZER_SET
			RGB_PURPLE_flash
		    BUZZER_SET
		    rt_thread_mdelay(400);
			break;
		default:
			break;
	}
	return;
}
