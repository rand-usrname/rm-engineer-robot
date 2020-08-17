#include "drv_gimbal.h"

//云台电机PID线程句柄
static rt_thread_t 		gimbal_control 	= RT_NULL;

//yaw pitch电机数据结构体
static gimbalmotor_t	yaw;
static gimbalmotor_t	pitch;

/**
* @brief：该函数计算角度环并输出
* @param [in]	motor:云台电机指针
				gyrodata:对应轴的陀螺仪数据
* @return：		无
* @author：mqy
*/
static int anglepid_calculate(gimbalmotor_t* motor,int gyroangle)
{
	switch (motor->angledata_source)
	{
	case GYRO:
		return pid_output_motor(&motor->anglepid_gyro,motor->anglepid_gyro.set,gyroangle);

	case DJI_MOTOR:
		return pid_output_motor(&motor->anglepid_dji,motor->anglepid_dji.set,gyroangle);

	default:
		return 0;
	}
}
/**
* @brief：该函数计算云台电机pid并保存在pid的out中
* @param [in]	motor:云台电机指针
				gyrodata:对应轴的陀螺仪数据
* @return：		无
* @author：mqy
*/
static int gimbalpid_cal(gimbalmotor_t* motor,int gyroangle,int gyrospeed,rt_uint8_t angle_time)
{
	switch (motor->control_mode)
	{
	case  ANGLE:
		if(angle_time > 9)
		{
			motor->speedpid.set = anglepid_calculate(motor,gyroangle);
		}
	case PALSTANCE:
		if(motor->control_mode == PALSTANCE)
		{
			motor->speedpid.set = motor->set;
		}
		pid_output_calculate(&motor->speedpid,motor->speedpid.set,gyrospeed);

	case CURRENT://若为电流环则设置电流值，
		if(motor->control_mode == CURRENT)
		{
			motor->speedpid.out = motor->set;
		}
		return 1;

	default://不存在则电流归零
		motor->speedpid.out = 0;
		break;
	}
	return 0;
}
/**
* @brief：根据云台电机的设定参数计算输出并发送数据
				在初始化之后该函数每经过x ms会执行一次
* @param [in]	parameter:该参数不会被使用
* @return：		无
* @author：mqy
*/
//2ms任务
static struct rt_timer task_1ms;
static struct rt_semaphore gimbal_1ms_sem;
static void task_1ms_IRQHandler(void *parameter)
{
	rt_sem_release(&gimbal_1ms_sem);
}
static void gimbal_contral_thread(void* parameter)
{
	struct rt_can_msg wheelc_message;
	
	wheelc_message.id	= GIMBAL_MCTL;  //设置ID
	wheelc_message.ide	= RT_CAN_STDID;	//标准帧
	wheelc_message.rtr	= RT_CAN_DTR;	//数据帧
	wheelc_message.priv = 0;			//报文优先级最高
	wheelc_message.len = 8;				//长度8

	rt_uint8_t angle_time = 0;			//记录速度环次数以执行角度环
	while(1)
	{
		rt_sem_take(&gimbal_1ms_sem, RT_WAITING_FOREVER);

		//计数执行角度环
		if(angle_time > 9)
		{
			angle_time = 0;
		}
		angle_time++;

		//计算云台电机等
		gimbalpid_cal(&yaw,gimbal_atti.yaw,gimbal_atti.yaw_speed,angle_time);
		gimbalpid_cal(&pitch,gimbal_atti.pitch,gimbal_atti.pitch_speed,angle_time);

		//发送数据
		wheelc_message.data[0] = pitch.speedpid.out>>8;
		wheelc_message.data[1] = pitch.speedpid.out;
		wheelc_message.data[2] = yaw.speedpid.out>>8;
		wheelc_message.data[3] = yaw.speedpid.out;
		wheelc_message.data[4] = 0;
		wheelc_message.data[5] = 0;
		wheelc_message.data[6] = 0;
		wheelc_message.data[7] = 0;

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
* @brief：初始化云台线程
* @param [in]	无
* @return：		1:初始化成功
				0:初始化失败
* @author：mqy
*/
int gimbal_init(void)
{
	rt_sem_init(&gimbal_1ms_sem, "1ms_sem", 0, RT_IPC_FLAG_FIFO);

	//初始化云台线程
	gimbal_control = rt_thread_create(
	"gimbal_control",		//线程名
	gimbal_contral_thread,	//线程入口
	RT_NULL,				//入口参数无
	2048,					//线程栈
	1,	                    //线程优先级
	2);						//线程时间片大小


	//线程创建失败返回false
	if(gimbal_control == RT_NULL)
	{
		return 0;
	}

    //创建线程定时器
	rt_timer_init(&task_1ms,
                "1ms_task",
                task_1ms_IRQHandler,
				RT_NULL,
                1, 
                RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

	//初始化结构体数据
	yaw.control_mode = ANGLE;//默认设置位置控制
	pitch.control_mode = ANGLE;

	yaw.motorID = YAW_ID;
	yaw.angledata_source = GYRO;//默认数据源陀螺仪

	pitch.motorID = PITCH_ID;
	pitch.angledata_source = GYRO;//默认数据源陀螺仪

	//初始化PID
	pid_init(&yaw.speedpid,185,0.01,100,100,0X7FFF,-0X7FFF);
	pid_init(&pitch.speedpid,150,0.01,200,3000,0X7FFF,-0X7FFF);

	pid_init(&yaw.anglepid_gyro,0.7,0.01,0,3,20000,-20000);
	pid_init(&yaw.anglepid_dji,0.18,0.01,0,3,2000,-2000);
	pid_init(&pitch.anglepid_gyro,1,0.01,0,3,20000,-20000);
	pid_init(&pitch.anglepid_dji,0.4,0.01,0,5,2000,-2000);

	//线程启动失败返回false
	if(rt_thread_startup(gimbal_control) != RT_EOK)
	{
		return 0;
	}

    //启动定时器
	if(rt_timer_start(&task_1ms) != RT_EOK)
	{
		return 0;
	}

	return 1;
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
* @brief：利用该函数更新云台电机相关数据
* @param [in]	message:接收到的数据帧指针
* @return：		true:更新成功
				false:id不匹配更新失败
* @author：mqy
*/
int refresh_gimbal_motor_data(struct rt_can_msg* message)
{
	//其他数据
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&yaw.motordata,message);
			//转换角度数值的坐标系
			if(yaw.motordata.angle < YAE_ZERO_ANGLE)
			{
				yaw.motordata.angle = 8191 - YAE_ZERO_ANGLE + yaw.motordata.angle;
			}
			else
			{
				yaw.motordata.angle = yaw.motordata.angle - YAE_ZERO_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&pitch.motordata,message);
			return 1;
			
		default:
			
			break;
	}
	return 0;
}

/**
* @brief：设置函数
* @param [in]	data_source:希望的数据源
* @return：		yaw轴角度，格式0-8191
* @author：mqy
*/
void gimbal_current_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = CURRENT;
	pitch.control_mode = CURRENT;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void gimbal_absangle_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = ANGLE;
	pitch.control_mode = ANGLE;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void gimbal_addangle_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = ANGLE;
	pitch.control_mode = ANGLE;
	yaw.set += yawset;
	pitch.set += pitchset;
}
void gimbal_palstance_set(rt_uint16_t yawset,rt_uint16_t pitchset)
{
	yaw.control_mode = PALSTANCE;
	pitch.control_mode = PALSTANCE;
	yaw.set = yawset;
	pitch.set = pitchset;
}
void angle_datasource_set(data_source_t yawset,data_source_t pitchset)
{
	yaw.angledata_source = yawset;
	pitch.angledata_source = pitchset;
}
/**
* @brief：获取yaw轴角度
* @param [in]	data_source:希望的数据源
* @return：		yaw轴角度，格式0-8191
* @author：mqy
*/
int get_yawangle(void)
{
	return yaw.motordata.angle;
}
/**
* @brief：获取pitch轴角度
* @param [in]	data_source:希望的数据源
* @return：		pitch轴角度，格式0-8191
* @author：mqy
*/
int get_pitchangle(void)
{
	return pitch.motordata.angle;
}
