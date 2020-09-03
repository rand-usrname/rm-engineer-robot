#include "drv_strike.h"
#include "robodata.h"
#include "drv_refsystem.h"
#include "robocontrol.h"     //��������Ƿ���������

#define PWM_DEV_NAME        "pwm8"  	  /* PWM�豸���� */
static struct rt_device_pwm *pwm_dev;	
static struct rt_device_pwm *servo_dev;   /* ����PWM�豸���� */
Motor_t m_rub[2];
Motor_t m_launch;
#define BULLET_17_Q 10
#define BULLET_42_Q 100
static rt_tick_t tick = 0;
static rt_tick_t tick_sleep = 0;		  /*����������ʱ��*/
uint16_t overheat = 0;        //���������Ա�־λ
/**
 * @brief  Ħ�������ʼ����tim��pwm��ʼ����
 * @retval RT_EOK or RT_ERROR(�ɹ���ʧ��)
 * @note   ����Ƶ��Ϊ50hz��ʵ�����100hz��(RT-THread�߼���ʱ��BUG)
 */
int motor_rub_init(void)
{
	pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
	if(!pwm_dev){return RT_ERROR;}
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, 3, 20000000,1000000);
	rt_pwm_set(pwm_dev, 4, 20000000,1000000);
	/* ʹ���豸 */
	rt_pwm_enable(pwm_dev, 3);
	rt_pwm_enable(pwm_dev, 4);
	return RT_EOK;
}
/**
 * @brief  ����������ſ��أ���ʼ��
 * @param  duty ռ�ձȣ�0-1000����0%-%100��
 * @retval RT_EOK or RT_ERROR(�ɹ���ʧ��)
 */
int motor_servo_init(void)
{
	servo_dev = (struct rt_device_pwm *)rt_device_find("pwm1");
	if(!servo_dev){return RT_ERROR;}
	/*�������ں�������*/
	rt_pwm_set(servo_dev, 2, 10000000,5000000);
	/* ʹ���豸 */
	rt_pwm_enable(servo_dev, 2);
	return RT_EOK;
}

void motor_rub_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*�������ں�������*/
	rt_pwm_set(pwm_dev, 3, 20000000,20000*duty);
	rt_pwm_set(pwm_dev, 4, 20000000,20000*duty);
}

/**
 * @brief  ���ֶ���ٶ��趨
 * @param  duty ռ�ձȣ�0-1000����0%-%100��
 * @retval None
 */
void motor_servo_set(uint16_t duty)
{
	RT_ASSERT(duty<=1000);
	
	/*�������ں�������*/
	rt_pwm_set(servo_dev, 2, 10000000,10000*duty);
}

Strike_t gun1 = {0};
rt_int32_t heat_est;
/*�������ƺ��������û�����ʵ�ʱ�д*/
void heat_control(Heatctrl_t *p_temp)
{
	if(p_temp == &gun1.heat)
	{
		/* ����������� */
		if(p_temp->unlimit_heat)
		{
			if(gun1.mode == STRICK_NOLIMITE)
				{tick_sleep = 100;}
	        else if(gun1.mode == STRICK_TRIPLE)
				{tick_sleep = 600;}
			else if(gun1.mode == STRICK_SINGLE)
				{tick_sleep = 100;}
		}
		/* �������ʣ�������Է�������������� */
		else
		{
            //�ȼ�����һ�η���������
            heat_est = p_temp->heat_now;     //ʵʱ����
			/* 3���� */
			if(gun1.mode == STRICK_TRIPLE)
			{
				/* Ԥ��ֵ�����ٵ��� */
				heat_est += (3*p_temp->heating_rate);
			}
			/* ȫ�Զ��򵥷� */
			else
			{
                heat_est += (p_temp->heating_rate);
			}
			/*��ǰѪ��֧���ҳ�����������*/
			rt_uint16_t more_heat = 0;
			rt_uint16_t hp_furture = 0;
//			if((now_furture < 2*(p_temp->max))&&(now_furture>p_temp->max))
//			{
//				hp_furture = Refdata.remain_HP - (now_furture-p_temp->max)/250/10*Refdata.max_HP;
//			}
//			else if(now_furture >= 2*(p_temp->max))
//			{
//				hp_furture = Refdata.remain_HP-(now_furture-p_temp->max)/250*Refdata.max_HP;
//			}
//			else
//			{
//				hp_furture = Refdata.remain_HP;
//			}
			//hp_furture ���� more_heat��������
			/* TODO */
			
			//δ�����������ж�ģʽѡ��tick_sleep
			if(gun1.mode == STRICK_NOLIMITE)
				{tick_sleep = 300;}
				else if(gun1.mode == STRICK_TRIPLE)
				{tick_sleep = 900;}
			else if(gun1.mode == STRICK_SINGLE)
				{tick_sleep = 300;}
			 //������ڳ���������δ����������� ��tick_sleep��������
			 /* TODO */

			/* �Գ���������������������� */
			if(heat_est >= p_temp->ref_heat_limit)
			{
				//gun1.status = STRICK_STOP;
                overheat = 1;
			}
			else
			{
                overheat = 0;
				//gun1.status &= ~STRICK_STOP;
			}
////			#else
////			if(gun1.mode == STRICK_SINGLE)
////			{}
////			else if(gun1.mode = STRICK_TRIPLE)
////			{}
////			#endif
		}
	}
}

/***********************************��������ģ��******************************************/
/* �����ʹ�ñ����������� ������Ҫ����һ����ʱ�� ÿ��һ������������ڴӲ���ϵͳ���µ�ǰ���� */

static Heatctrl_t *p_heat = RT_NULL;													/*�������ƿ�ָ��*/

/* ���ʹ�ܱ����������� */
#if LOCAL_HEAT_ENABLE
/**
 * @brief  ������ȴ
 */
static __inline void heatctrl_cool(Heatctrl_t *heat)
{
	heat->now += heat->cool * heat->buff;
	if(heat->now > heat->max){heat->now = heat->max;}
}
#endif
/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore heat_10ms_sem;     						
/*10ms����*/
static struct rt_timer heat_10ms;
/* task_10ms ��ʱ���� */
static void heat_10ms_IRQHandler(void *parameter)
{
	rt_sem_release(&heat_10ms_sem);
}
/**
 * @brief  ���������߳�
 */
static void heatctrl_thread(void *parameter)
{
	Heatctrl_t *p_heat_temp;
	while(1)
	{
        rt_sem_take(&heat_10ms_sem, RT_WAITING_FOREVER);
		p_heat_temp = p_heat;
        while(!(p_heat_temp == RT_NULL))
        {
            /* �������� */
            #ifdef BULLET_17
            p_heat_temp->heat_now       = Refdata->ref_power_heat_data.shooter_heat0;
            p_heat_temp->ref_heat_limit = Refdata->ref_robot_state.shooter_heat0_cooling_limit + 0.4*10*250/Refdata->ref_robot_state.max_HP;
            p_heat_temp->heating_rate   = BULLET_17_Q;	//ÿ��17mm���跢����
            p_heat_temp->cooling_rate   = Refdata->ref_robot_state.shooter_heat0_cooling_rate/10;
            #endif
            #ifdef MOBILE_17 /* ���̷������ */
            p_heat_temp->heat_now       = Refdata->ref_power_heat_data.shooter_heat0;
            p_heat_temp->ref_heat_limit = 150;
            p_heat_temp->heating_rate   = BULLET_17_Q;	//ÿ��17mm���跢����
            p_heat_temp->cooling_rate   = Refdata->ref_robot_state.shooter_heat0_cooling_rate/10;
            #endif
            #ifdef BULLET_42
            p_heat_temp->heat_now       = Refdata->ref_power_heat_data.shooter_heat1;
            p_heat_temp->ref_heat_limit = Refdata->ref_robot_state.shooter_heat1_cooling_limit;
            p_heat_temp->heating_rate   = BULLET_42_Q;	//ÿ��42mm���跢����
            p_heat_temp->cool           = Refdata->ref_robot_state.shooter_heat1_cooling_rate/10;
            #endif

            p_heat_temp->heat_remain_percent = (100 * (p_heat_temp->ref_heat_limit - p_heat_temp->heat_now)) / p_heat_temp->ref_heat_limit;


            /*��������*/
            heat_control(p_heat_temp);					
            #if LOCAL_HEAT_ENABLE
            heatctrl_cool(p_heat_temp);					/*��ȴ*/
            #endif
            p_heat_temp = p_heat_temp->next;
        }
        rt_thread_mdelay(HEAT_PERIOD);
    }
}
/**
 * @brief  ������������
 * @param  heat�������ṹ��
 * @param  max����������
 */
void heatctrl_init(Heatctrl_t *heat, rt_uint32_t max)
{
	static rt_uint8_t num = 0;
	
	/*���ó�������������*/
	//heat->ref_heat_limit = max + 0.4*10*250/Refdata->max_HP;
    heat->ref_heat_limit = max + 0.4*10*250/Refdata->ref_robot_state.max_HP;
	/*״̬����ʼ��*/
	heat->heat_remain_percent = 100;
	heat->heat_now = 0;
	heat->unlimit_heat = 0;
	heat->overheat_permit = 0;
	/*���ʹ�ܱ�����������*/
	#if LOCAL_HEAT_ENABLE
	heat->cooling_rate = 10;
	heat->buff = 1;
	#endif
	/*�������������߳�*/
	heatctrl_start();
	/*������ǵ�һ�γ�ʼ��������п��ƽṹ�ĵ�����������*/
//	if(num > 0)
//	{
//		p_heat->next = heat;
//	}
	p_heat = heat;
//	num++;
}
/**
 * @brief  ��ʼ�������ƣ��������������̣߳�
 */
void heatctrl_start(void)
{
	rt_thread_t thread;
	rt_sem_init(&heat_10ms_sem, "heat_10ms_sem", 0, RT_IPC_FLAG_FIFO);
	rt_timer_init(&heat_10ms,
				  "heat_10mstask",
				   heat_10ms_IRQHandler,
				   RT_NULL,
				   10, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&heat_10ms);
	
	thread = rt_thread_create("heatctrl", heatctrl_thread, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
		rt_thread_startup(thread);
	}
}
/************************************ End **************************************************/

/***********************************�������ģ��******************************************/

void Gun_speed_set(Strike_t *strike, rt_int16_t speed)
{
	strike->speed = speed;
	#ifdef RUB_SNAIL
	motor_rub_set(strike->speed);
	#else
	//��Ҫ���㣿
	m_rub[0].spe.set = strike->speed;
	m_rub[1].spe.set = -strike->speed;
	#endif
}
void Gun_mode_set(Strike_t *strike, rt_base_t mode)
{
	strike->mode = mode;
}
/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/**
 * @brief  �����ж�
 * @param  motor����������ṹ��ָ��
 */
void strike_stuck(Motor_t *motor, Strike_t *gun)
{
	static rt_uint16_t stuck_time = 0;
	static rt_tick_t tick = 0;								/*��¼ϵͳʱ��*/
	static rt_uint8_t temp = 0;
	/*�������*/
	if(gun->status & STRICK_STUCK)
	{	
		if(temp == 1)
		{
			if(rt_tick_get() > tick)						/*��ת1000ms*/
			{
				gun->status &= ~STRICK_STUCK;
				stuck_time = 0;
				temp = 0;
			}
		}
		else
		{
			motor->ang.set = motor->dji.angle + 3000;	
			tick = rt_tick_get()+1000;
			temp=1;
		}
		
	}
	else if(~(gun->status & STRICK_STOP))					/*���û�н�ֹת��*/
	{
		/*�ٶȻ��������*/
		if(ABS(motor->spe.out) > 7777)
		{
			stuck_time++;
		}
		//else {stuck_time=0;}
		/*����ʱ�����*/
		if(stuck_time > 111)
		{
			gun->status |= STRICK_STUCK;
		}
	}
}


/*���ڶ�ǹ�ܣ���Ҫ�����ٶ���һ����ͬ���ú��������޸�*/
/**
 * @brief  ����ģʽ������
 * @param  gun����������ṹ��ָ��
 * @param  if_fire���Ƿ񿪻�,1����0����
 */
void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t *if_fire)
{
	/* ���Ħ�����ٶ�С��һ��ֵ */
	if(ABS(gun->speed) < 11)
	{
		gun->status = STRICK_STOP;
	}
	else
	{
		gun->status &= ~STRICK_STOP;
	}
	/*���ֹͣ����ʹλ�û�����*/
	if(gun->status & STRICK_STOP)									/*��������ֱ�ӷ���*/
	{
		motor->ang.set = motor->dji.angle;
		return;
	}
    
	if(rt_tick_get() - tick < tick_sleep){return;}					/*�����ڼ����Ϣ�ڼ䣬����������Ķ���ֱ�ӷ���**/
	else
	{
		if(*if_fire && overheat == 0)
		{
			/*һ�η�������*/
			if(gun->mode /*&*/== STRICK_NOLIMITE)							/*��ͣת��*/
			{									/*���ʱ��*/
				motor_angle_set(motor, FIRE_ANGLE);
                if(*if_fire > 1)
                {
                    *if_fire = 0;
                }
			}
			else if(gun->mode /*&*/== STRICK_SINGLE)						/*����*/
			{
				motor_angle_set(motor, FIRE_ANGLE);
                *if_fire = 0;
			}
			else if(gun->mode /*&*/== STRICK_TRIPLE)						/*������*/
			{
				motor_angle_set(motor, FIRE_ANGLE*3);
                *if_fire = 0;
			}
			tick = rt_tick_get();
		}
	}
}
/************************************ End **************************************************/
/************************************��������߳�*******************************************/
static struct rt_semaphore task_1ms_sem;     						/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore task_10ms_sem;     						/* ���ڽ�����Ϣ���ź��� */
/*1ms����*/
static struct rt_timer task_1ms;
/*10ms����*/
static struct rt_timer task_10ms;

/* task_1ms ��ʱ���� */
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_1ms_sem);
}
/* task_10ms ��ʱ���� */
static void task_10ms_IRQHandler(void *parameter)
{
	rt_sem_release(&task_10ms_sem);
}
static void task_1ms_entry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_1ms_sem, RT_WAITING_FOREVER);
		
		rt_int16_t send_current[4] = {0};
		rt_int16_t launch_6020_cur[4] = {0};
		
		pid_output_calculate(&m_launch.spe,m_launch.ang.out,m_launch.dji.speed);
		
		#ifndef RUB_SNAIL /* �����3508Ħ���� */
		pid_output_calculate(&m_rub[0].spe,m_rub[0].spe.set,m_rub[0].dji.speed);
		pid_output_calculate(&m_rub[1].spe,m_rub[1].spe.set,m_rub[1].dji.speed);
		send_current[(RUB0_ID-0x201)] = m_rub[0].spe.out;
		send_current[(RUB1_ID-0x201)] = m_rub[1].spe.out;
		#endif
		//6020����,ID��ͬ
		#ifdef LAUNCH_6020
		motor_current_send(can2_dev,STDID_launch,send_current[0],send_current[1],send_current[2],send_current[3]);
		launch_6020_cur[(LAUNCH_ID-0x205)] = m_launch.spe.out;
		motor_current_send(can1_dev,0x1FF,launch_6020_cur[0],launch_6020_cur[1],launch_6020_cur[2],launch_6020_cur[3]);
		#else   //2006����
		send_current[(LAUNCH_ID-0x201)] = m_launch.spe.out;
		motor_current_send(can1_dev,LAUNCH_CTL,send_current[0],send_current[1],send_current[2],send_current[3]);
		#endif
	}
}
static void task_10ms_entry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&task_10ms_sem, RT_WAITING_FOREVER);
        strike_fire(&m_launch, &gun1, &launch_multiple);
		pid_output_motor(&m_launch.ang,m_launch.ang.set,m_launch.dji.angle);
	}
}

/**
 * @brief  ���񴴽�
 */
static void strike_start(void)
{
	/*��ʱ�������߳�*/
	rt_thread_t thread;
	rt_sem_init(&task_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("1ms_sem", task_1ms_entry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	rt_sem_init(&task_10ms_sem, "10ms_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("10ms_sem", task_10ms_entry, RT_NULL, 1024, 1, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	
	/*��ʱ���ж�*/
	rt_timer_init(&task_1ms,
                   "1mstask",
                   task_1ms_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_init(&task_10ms,
				  "10mstask",
				   task_10ms_IRQHandler,
				   RT_NULL,
				   10, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&task_1ms);
	rt_timer_start(&task_10ms);
}

//���º���Ӧ�÷���˽���ļ���,�Լ�����PID����,������Ϊʾ��
__weak void strike_pid_init(void)
{
	//�Լ���ʼ���������(Ħ����+����)��PID

	//����1:snailĦ����+2006����
	// pid_init(&m_launch.ang, 
	// 				3.5,0,0,
	// 				500,5000,-5000);
	// pid_init(&m_launch.spe, 
	// 				7.5,0,0,
	// 				350,8000,-8000);

	//����2:3508Ħ����+2006����
	// pid_init(&m_rub[0].spe,  
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	// pid_init(&m_rub[1].spe, 
	// 				8.2,0.05,0,
	// 				1200,14000,-14000);
	// pid_init(&m_launch.ang, 
	// 				3.5,0,0,
	// 				500,5000,-5000);
	// pid_init(&m_launch.spe, 
	// 				7.5,0,0,
	// 				350,8000,-8000);

}

/**
 * @brief  ���������ʼ��
 * @param  gun����������ṹ��ָ��
 */
void strike_init(Strike_t *gun, rt_uint32_t max)
{
    gun->mode = STRICK_NOLIMITE;
//	gun->mode = STRICK_NOLIMITE | STRICK_LOWSPEED;				/*��������+���ٸ���Ƶ*/
//	gun->speed = 0;
//	gun->status = 0;
//////	if(gun->mode == STRICK_NOLIMITE)
//////	{tick_sleep = 300;}
//////	else if(gun->mode == STRICK_SINGLE)
//////	{tick_sleep = 300;}
//////	else if(gun->mode == STRICK_TRIPLE)
//////	{tick_sleep = 900;}
	//Ħ���ֵ��ѡ��:
	//1. Snail
	//2. 3508ƨ��
	#ifdef RUB_SNAIL
	motor_rub_init();
	#else	//Ĭ��3508
	motor_init(&m_rub[0],RUB0_ID,1);
	motor_init(&m_rub[1],RUB1_ID,1);
	#endif

	//	motor_servo_init();
    m_launch.ang.set = m_launch.dji.angle;   //���������ֵ
	motor_init(&m_launch,LAUNCH_CTL,0.018);    //0.027973

	strike_pid_init();

	heatctrl_init(&gun->heat, max);						/*�������Ʋ�����ʼ��*/
	strike_start();
}
