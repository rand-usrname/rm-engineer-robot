/************************************** Copyright ****************************** 
  *
  *                 (C) Copyright 2020,China, HITwh.
  *                            All Rights Reserved
  *                              
  *                     By  哈尔滨工业大学（威海）HERO战队
  *                     HITwh Excellent Robot Organization
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : drv_sentrychas.c  
  * Version    : v1.0		
  * Author     : yi_lu			
  * Date       : 2020-08-23         
  * Description:    哨兵的底盘不同于一般底盘,单独写一个底盘文件

  ****************************************************************************** */


#include "drv_remote.h"
#include "drv_motor.h"

#include "mid_chassis.h"

#include "task_chassis.h"

Motor_t m_chassis_3508[2];

rt_uint8_t remote_mode = 0;		//模式0：底盘


/**
 * @brief 底盘pid初始化
 */
static void pid_chassis_init(void)
{
	pid_init(&m_chassis_3508[0].spe, 
					8,0,0,
					300,8000,-8000);
	pid_init(&m_chassis_3508[1].spe, 
					8,0,0,
					300,8000,-8000);
	
}
/**
 * @brief  底盘电机初始化（pid初始化+can初始化）
 */
void chassis_init(void)
{
	pid_chassis_init();
	m_chassis_3508[0].dji.motorID = LEFT;
	m_chassis_3508[0].dji.ratio = 1;				/*减速比<=1*/
	m_chassis_3508[1].dji.motorID = RIGHT;
	m_chassis_3508[1].dji.ratio = 1;
}

/**
 * @brief  底盘控制模式
 */
void chassis_control(void)
{
	if(remote_mode==0)
	{
		m_chassis_3508[0].spe.set = (rt_int16_t)((RC_data.Remote_Data.ch2 - 1018)*10);
		m_chassis_3508[1].spe.set = (rt_int16_t)((RC_data.Remote_Data.ch2 - 1018)*10);
		
	}
	
}

rt_size_t chassis_send_message(rt_device_t dev, rt_int16_t *msg)
{
	struct rt_can_msg txmsg;
	
	txmsg.id = STDID_CHASSIS;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0]=(rt_uint8_t)(msg[0]>>8);
	txmsg.data[1]=(rt_uint8_t)(msg[0]);
	txmsg.data[2]=(rt_uint8_t)(msg[1]>>8);
	txmsg.data[3]=(rt_uint8_t)(msg[1]);
	txmsg.data[4]=(rt_uint8_t)(msg[2]>>8);
	txmsg.data[5]=(rt_uint8_t)(msg[2]);
	txmsg.data[6]=(rt_uint8_t)(msg[3]>>8);
	txmsg.data[7]=(rt_uint8_t)(msg[3]);
	
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}


#ifdef mqy
//底盘电机PID线程句柄
static rt_thread_t 	chassis_ctrl_handler = RT_NULL;

//电机控制结构体，索引0对应最小ID
static motor_t	chassis_motor[4] = {0};

//运动数据，包括云台两轴角度，期望速度等
static chassis_data_t	motion_data;

#define ABS(a) (((a)>0)?(a):(-a))

/**
* @brief：按照参数设定底盘的运动角速度与速度，速度结果将会被保存在本文件的四个轮子的数据中
* @param [in]	angle:设定本次操作的正方向（即本次操作的水平坐标系的y轴正方向
				该值大小范围0~8191，对应了角度范围0~359.95度，
				angular_velocity:设定底盘自转角速度（单位 0.1 °/s 即 pi/1800 rad/s
				xspeed:期望的x轴速度分量（单位mm/s
				yspeed:期望的y轴速度分量（单位mm/s
* @return：		无
* @author：mqy
*/
static void motor_speed_set(rt_uint16_t angle, rt_int16_t speed)
{
	while(angle > 8191)
	{
		angle -= 8191;
	}
	angle = 8191 - angle;
	
	//2.40946为单位转换的系数，v为期望的轮子线速度，期望速度使用rpm作为单位
	chassis_motor[(rt_uint8_t)(LEFT- 0x201)].speedpid.set = (rt_int16_t)((float)speed);
	chassis_motor[(rt_uint8_t)(RIGHT- 0x201)].speedpid.set = (rt_int16_t)((float)speed);
}
/**
* @brief：该函数根据设定运动模式和设定线速度角速度调用函数motor_speed_set设定电机速度
			    在初始化之后该函数每经过x ms会执行一次
* @param [in]	无
* @return：		无
* @author：mqy
*/
static void chassis_control(void)
{
	switch(motion_data.sport_mode)
	{
		case NO_FOLLOW:
			chassis_speed_set(0, 0);
			break;

		default:
			motor_speed_set(0, 0);//在没有模式的情况下，速度置零
			break;
	}
}
/**
* @brief：将地盘功率限制在期望范围内
* @param [in]	powermax:限制的功率大小，单位 mW - 毫瓦
				该函数还通过直接读写的方式获取当前四个电机的电流
* @return：计划为1时限制，0时未作出限制
* @author：mqy
*/
static int power_limit(int powermax)
{
	//进行电流求和
	int allcurrent = 0;
	allcurrent += ABS(chassis_motor[0].motordata.current);
	allcurrent += ABS(chassis_motor[1].motordata.current);
	allcurrent += ABS(chassis_motor[2].motordata.current);
	allcurrent += ABS(chassis_motor[3].motordata.current);

	//通过功率得出步兵当前可行的最大电流，量纲与allcurrent相同
	int Imax = powermax/25;		//电流最大值，单位 mA，实际供电电压25.5
	Imax = Imax*8192/10000;		//转换至电机发送数据的量纲
	if(Imax < allcurrent)		//若电流超限
	{
		chassis_motor[0].speedpid.out = ((int)chassis_motor[0].speedpid.out)*Imax/allcurrent;
		chassis_motor[1].speedpid.out = ((int)chassis_motor[1].speedpid.out)*Imax/allcurrent;
		chassis_motor[2].speedpid.out = ((int)chassis_motor[2].speedpid.out)*Imax/allcurrent;
		chassis_motor[3].speedpid.out = ((int)chassis_motor[3].speedpid.out)*Imax/allcurrent;
		return 1;
	}
	return 0;					//不超限返回0
}
/**
* @brief：根据四个电机的设定参数分别计算出其输出并发送数据
				在初始化之后该函数每经过x ms会执行一次
* @param [in]	parameter:该参数不会被使用
* @return：		无
* @author：mqy
*/
//2ms任务
static struct rt_timer task_2ms;
static struct rt_semaphore chassis_2ms_sem;
static void task_2ms_IRQHandler(void *parameter)
{
	rt_sem_release(&chassis_2ms_sem);
}
static void chassis_control_thread(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= CHASSIS_CTLID;//设置ID
	wheelc_message.ide	= RT_CAN_STDID;	//标准帧
	wheelc_message.rtr	= RT_CAN_DTR;	//数据帧
	wheelc_message.priv = 0;			//报文优先级最高
	wheelc_message.len = 8;				//长度8
	
	while(1)
	{
		rt_sem_take(&chassis_2ms_sem, RT_WAITING_FOREVER);
		chassis_control();
		for(int a = 0;a<4;a++)
		{
			pid_output_calculate(&chassis_motor[a].speedpid,chassis_motor[a].speedpid.set,chassis_motor[a].motordata.speed);
			//chassis_motor[a].motorpid.out = 200;
		}

		//测试代码进行功率限制
		//power_limit(2000);
		
		//发送数据
		wheelc_message.data[0] = chassis_motor[0].speedpid.out>>8;
		wheelc_message.data[1] = chassis_motor[0].speedpid.out;
		wheelc_message.data[2] = chassis_motor[1].speedpid.out>>8;
		wheelc_message.data[3] = chassis_motor[1].speedpid.out;
		wheelc_message.data[4] = chassis_motor[2].speedpid.out>>8;
		wheelc_message.data[5] = chassis_motor[2].speedpid.out;
		wheelc_message.data[6] = chassis_motor[3].speedpid.out>>8;
		wheelc_message.data[7] = chassis_motor[3].speedpid.out;
		
		if(!rt_device_write(can1_dev,0,&wheelc_message,sizeof(wheelc_message)))
		{
			//如果发送数据为0计数一次发送失败，失败次数过多发出警告
		}
		else
		{

		}
	}
}
/**
* @brief：初始化底盘电机
* @param [in]	无
* @return：		true:初始化成功
				false:初始化失败
* @author：mqy
*/
int chassis_init(void)
{
    rt_sem_init(&chassis_2ms_sem, "2ms_sem", 0, RT_IPC_FLAG_FIFO);
	
    //初始化底盘线程
	chassis_ctrl_handler = rt_thread_create(
	"chassis_control",		//线程名
	chassis_control_thread,	//线程入口
	RT_NULL,				//入口参数无
	2048,					//线程栈
	1,	                    //线程优先级
	2);						//线程时间片大小

	//线程创建失败返回false
	if(chassis_ctrl_handler == RT_NULL)
	{
		return 0;
	}

    //线程启动失败返回false
	if(rt_thread_startup(chassis_ctrl_handler) != RT_EOK)
	{
		return 0;
	}

    //创建线程定时器
	rt_timer_init(&task_2ms,
                "2ms_task",
                task_2ms_IRQHandler,
				RT_NULL,
                2, 
                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    //启动定时器
	rt_timer_start(&task_2ms);

    //若均成功则开始初始化数据
	for(rt_uint8_t a = 0;a<4;a++)
	{
		chassis_motor[a].motorID = (drv_can1ID_e)(0x201 + a);//初始化ID数值
	}
	pid_init(&chassis_motor[0].speedpid,6,0.004,0.2,500,8000,-8000);
	pid_init(&chassis_motor[1].speedpid,6,0.004,0.2,500,8000,-8000);

	
	return 1;
}
/**
* @brief：按照参数设定以云台为前方的速度
* @param [in]	follow_angle:设定跟随角度（单位° 范围0-8191，超出范围的值会转化到该范围内
				在 ONLY_CHASSIS 模式下该参数为前方
				angular_velocity:设定底盘自转角速度（单位 0.1 °/s 即 pi/1800 rad/s
				xspeed:期望的x轴速度分量（单位mm/s
				yspeed:期望的y轴速度分量（单位mm/s
* @return：		无
* @author：mqy
*/
void chassis_speed_set(rt_uint16_t follow_angle,rt_int16_t angular_velocity,rt_int16_t xspeed,rt_int16_t yspeed)
{
	while(follow_angle > 8191)
	{
		follow_angle -= 8191;
	}

	motion_data.xspeed = xspeed;
	motion_data.yspeed = yspeed;
}
/**
* @brief：设定运动模式
* @param [in]	sport_mode:期望的运动模式
* @return：		无
* @author：mqy
*/
void sport_mode_set(sport_mode_e sport_mode)
{
	motion_data.sport_mode = sport_mode;
}
/**
* @brief：refresh_motor_data函数会使用该函数来更新数据
* @param [in]	message:接收到的数据帧指针
				motordata:要更新的电机数据结构体指针
* @return：		无
* @author：mqy
*/
static void assign_motor_data(motordata_t* motordata,struct rt_can_msg* message)
{
	motordata->angle = (message->data[0]<<8) + message->data[1];	//转子角度
	motordata->speed = (message->data[2]<<8) + message->data[3];	//转子转速
	motordata->current = (message->data[4]<<8) + message->data[5];  //实际电流
	motordata->temperature = message->data[6];						//温度
}
/**
* @brief：利用该函数更新底盘电机当前转速等数据
* @param [in]	message:接收到的数据帧指针
* @return：		true:更新成功
				false:id不匹配更新失败
* @author：mqy
*/
int refresh_chassis_motor_data(struct rt_can_msg* message)
{
	rt_uint8_t coordinate = message->id - 0x201;
	
	//底盘四个电机数据
	if(coordinate < 4)
	{
		assign_motor_data(&chassis_motor[coordinate].motordata,message);
		return 1;//更新完数据就返回
	}

	return 0;
}
int refresh_gimbal_motor_data(struct rt_can_msg* message)
{
	//其他数据
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&motion_data.yaw_data,message);
			//转换角度数值的坐标系
			if(motion_data.yaw_data.angle < YAW_ZERO_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAW_ZERO_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAW_ZERO_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&motion_data.pitch_data,message);
			return 1;
			
		default:
			
			break;
	}
	return 0;
}

#endif
