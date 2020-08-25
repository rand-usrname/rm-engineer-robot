#include "drv_aimbot.h"
#include "math.h"
#include "drv_gyro.h"

/**********与视觉的通信部份***********/

//视觉控制信息结构体
static visual_ctl_t visual_ctl;

//视觉接收信息结构体
static visual_rev_t visual_rev;
/**
* @brief：初始化视觉结构体
* @param [in]	无
* @return：		true:初始化成功
							false:初始化失败
* @author：mqy
*/
int vision_init(void)
{
	//接收信息置零
	visual_rev.computime = 0;
	visual_rev.aim_mode = BLANK;
	visual_rev.forcester = COMPUTER;
	visual_rev.pitchadd = 0;
	visual_rev.yawadd = 0;
	visual_rev.x = 0;
	visual_rev.y = 0;
	visual_rev.z = 0;
	visual_rev.yaw_usetime = 0;
	visual_rev.pitch_usetime = 0;

	//控制信息置为默认值
	visual_ctl.aim_mode = BLANK;
	visual_ctl.forcester = COMPUTER;
	visual_ctl.tracolor = RED;

	return 1;
}
INIT_APP_EXPORT(vision_init);

/**
* @brief：接收并解析视觉识别信息
* @param [in]	data:长度为8的数据指针
* @return：		1:更新成功
				0:更新失败
* @author：mqy
*/
int refresh_visual_data(rt_uint8_t* data)
{
	visual_rev.yaw_usetime = 0;
	visual_rev.pitch_usetime = 0;
	visual_rev.aim_mode = (aim_mode_e)((data[0]>>5) & 0X7);//取前三位
	visual_rev.forcester = (forecast_e)((data[0]>>4) & 0X1);//取第四位
	visual_rev.computime = (rt_uint8_t)(data[1]);
	switch (visual_rev.aim_mode)
	{
	case RUNE:
	case BIG_ARMOR:
	case SMALL_ARMOR:
	case BASE_ARMOR:
	case ENG_ARMOE:
		switch(visual_rev.forcester)
		{
		case COMPUTER:
			visual_rev.yawadd = (rt_int16_t)((data[2]<<8) + data[3]);
			visual_rev.pitchadd = (rt_int16_t)((data[4]<<8) + data[5]);
			break;
	
		case SIGNALCHIP:
			visual_rev.x = ((rt_int16_t)((data[2]<<8) + data[3]))/1000.0f;
			visual_rev.y = ((rt_int16_t)((data[4]<<8) + data[5]))/1000.0f;
			visual_rev.z = ((rt_int16_t)((data[6]<<8) + data[7]))/1000.0f;
			break;

		default:
			break;
		}
		break;

	case CAISSON:
		break;
	
	case BLANK:	//若为空值默认返回
	default:	//若为其他情况默认返回
		break;
	}
	return 1;
}
/**
* @brief:获取八字节的数据帧
* @param:data:留出八字节空间的数据指针
		 pitch_ang:pitch轴角度，范围-32768 ~ 32767 
		 yaw_ang:yaw轴角度，范围-32768 ~ 32767 
		 bullet_vel:发射子弹速度，单位mm/s
* @return：无
* @author：mqy
*/
void ctldata_get(rt_uint8_t data[],rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	data[0] = (visual_ctl.aim_mode << 5) | (visual_ctl.forcester << 4) | (visual_ctl.tracolor << 3);
	data[1] = yaw_ang >> 8;
	data[2] = yaw_ang;
	data[3] = pitch_ang >> 8;
	data[4] = pitch_ang;
	data[5] = bullet_vel >> 8;
	data[6] = bullet_vel;
	data[7] = 0;
}

/**
* @brief:通过CAN向视觉发送信息
* @param:pitch_ang:pitch轴角度，范围-32768 ~ 32767 
		 yaw_ang:yaw轴角度，范围-32768 ~ 32767 
		 bullet_vel:发射子弹速度，单位mm/s
* @return：		无
* @author：mqy
*/
int visual_ctl_CANsend(rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	struct rt_can_msg txmsg;
	txmsg.id = VISUAL_CTLID;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr = RT_CAN_DTR;
	txmsg.len = 8;
	ctldata_get(txmsg.data,yaw_ang,pitch_ang,bullet_vel);
	rt_device_write(can2_dev,0,&txmsg,sizeof(txmsg));
	
	return 1;
}
/**
* @brief:通过UART向视觉发送信息
* @param:dev:UART设备句柄
		 pitch_ang:pitch轴角度，范围-32768 ~ 32767
		 yaw_ang:yaw轴角度，范围-32768 ~ 32767
		 bullet_vel:发射子弹速度，单位mm/s
* @return：		无
* @author：mqy
*/
int visual_ctl_UARTsend(rt_device_t dev,rt_int16_t yaw_ang,rt_int16_t pitch_ang,rt_int16_t bullet_vel)
{
	rt_uint8_t data[11];
	data[0] = 0X3B;		//帧头
	data[9] = 0;		//和校验位
	data[10] = ~0X3B;	//帧尾
	ctldata_get(&data[1],yaw_ang,pitch_ang,bullet_vel);
	for(int a = 1; a<9; a++)
	{
		data[9] += data[a];
	}
	rt_device_write(dev,0,data,11);//写入11个字节的数据
	
	return 1;
}

/**
* @brief:获取视觉数据
* @param:无
* @return：返回函数名对应的含义
* @author：mqy
*/
rt_int16_t get_yaw_add(void)
{
	visual_rev.yaw_usetime++;
	return visual_rev.yawadd;
}
rt_int16_t get_pitch_add(void)
{
	visual_rev.pitch_usetime++;
	return visual_rev.pitchadd;
}
rt_int16_t get_yawusetime(void)
{
	return visual_rev.yaw_usetime;
}
rt_int16_t get_pitchusetime(void)
{
	return visual_rev.pitch_usetime;
}
/**********与视觉的通信部份结束***********/


#define G   9.7988f 
#define t_transmit 3
#define pi_operator 0.01745f
//接口 
Aimbot_t Aimbot_data;

//定义一个数组队列 ，用于存储近40ms内的角速度。
static float Palstance_Deque[40] = {0.0f,};

static float vision_yaw_palstance = 0.0f;
//需要获取子弹速度
static float bullet_speed = 0.0f;


//快速开方
static float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
 
    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       
    i  = 0x5f3759df - ( i >> 1 );               
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );  
    return y;
}

/* 以下所有函数需遵循 返回值或传参
   单位 °
   方向 pitch向上为正 yaw轴从云台上方往下看逆时针为正
   视觉yaw轴方向和上述相反 需做转换
*/


/**
  * @brief  队列填充当前时间的yaw轴角速度(1ms一次) 单位 °/s
  * @param  pdata is Palstance_Deque(存储yaw角速度的队列)  
  * @param  gimbal_atti 陀螺仪数据结构体
  * @retval none
 **/
static void get_new_palstance(float *pdata,ATTI_t *gimbal_atti)
{
	float *temp = pdata;
	float motor_palstance =  gimbal_atti->yaw_speed;
    Palstance_Deque[0] = motor_palstance;
	rt_memcpy((Palstance_Deque+1),temp,39);
}

/* 视觉端已经做好坐标系变换 */
//坐标系转换：摄像头坐标系转为电机坐标系 ，原点为发射机构摩擦轮位置处
//static Point_t Transformed_coordinate(Point_t* vision_point,float pitch_angle)
//{
//	Point_t motor_point;
//	motor_point.x = vision_point->x;
//    motor_point.y = -(vision_point->z) * sin(pitch_angle) + vision_point->y * cos(pitch_angle);
//    motor_point.z = vision_point->z * cos(pitch_angle) + vision_point->y * sin(pitch_angle);

//	return motor_point;
//}

/**
  * @brief   通过视觉返回的目标坐标数据计算原始的pitch yaw 增量(不考虑重力影响及预瞄量) 
  * @param   视觉发送的目标坐标
  * @retval  返回一个存储有原始pitch yaw 增量的结构体 量纲为0-360°
 **/
static Aimbot_t Calc_from_point(visual_rev_t *visual_rev)
{
	Aimbot_t calc_angle;
	calc_angle.pitch_add = atan((visual_rev->y)/(visual_rev->z))/pi_operator;
	/* yaw轴方向取反 */
	calc_angle.yaw_add = -atan((visual_rev->x)/(visual_rev->z))/pi_operator;
	return calc_angle;
}

/**
  * @brief  对Calc_from_point求出的pitch_add进行重力补偿，即考虑重力影响
  * @param  current_pitch  当前pitch角度 单位为°
  * @param  delta_pitch  Calc_from_point求出的为补偿的pitch_add 单位为°
  * @distance 与目标的直线距离 单位为m 
  * @bullet_speed 子弹速度 单位 m/s
  * @retval 返回补偿后的pitch_add 量纲0-360°
 **/
static float Gravity_compensation(float current_pitch, float delta_pitch,float distance,rt_uint16_t bullet_speed)
{
	/* 转换为弧度制 */
	float target_pitch = (delta_pitch + current_pitch)*pi_operator;
	
	/* x_0向前为正 y_0正方向和重力加速度同向 */
	float x_0 = distance * cos(-target_pitch);
	float y_0 = distance * sin(-target_pitch);
	
	float temp = (bullet_speed*bullet_speed*bullet_speed*bullet_speed)-G*(G*x_0*x_0-2*bullet_speed*bullet_speed*y_0);
	if(temp < 0) return 0;  /* 射程不够 */
	else
	{
		/* 计算补偿后的角度 单位rad*/
		float aim_pitch = -atan((-bullet_speed*bullet_speed+Q_rsqrt(temp))/(G*x_0));
		/* 转换成°后返回增量 */
		return (aim_pitch/pi_operator-current_pitch);
	}
}


/**
  * @brief   得到yaw轴的提前量预瞄角度
  * @param   vision_palstance 视觉摄像头中对方的相对角速度(可根据两帧的对方位置改变算出)
  * @param   t_lag 视觉处理一帧图像用时加上数据传输等用时
  * @retval  返回yaw轴的增量 
 **/
static float Advance_yaw_angle(float vision_palstance, rt_uint8_t t_lag,
							   float distance,rt_uint16_t bullet_speed)
{
	/****************************1****************************/
	//得到动态时滞前的角速度
	float Gimbal_palstance_lag = *(Palstance_Deque+t_lag);
	float aim_palstance = Gimbal_palstance_lag+vision_palstance;
	/****************************2****************************/
	return aim_palstance*distance/bullet_speed;
}



/******************************************************************************************************************/
rt_thread_t aim_thread;
static struct rt_semaphore Obtain_palstance_sem;

static struct rt_semaphore Aim_bot_sem;

static struct rt_timer Obtain_palstance_task;

static void Obtain_palstance_IRQHandler(void *parameter)
{
	rt_sem_release(&Obtain_palstance_sem);
}

static void Obtain_palstance_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&Obtain_palstance_sem, RT_WAITING_FOREVER);
		get_new_palstance(Palstance_Deque,&gimbal_atti);
	}
} 

static void aim_bot_emtry(void *parameter)
{
	static rt_tick_t tick = 0;
	static float last_visual_yaw = 0;
	while(1)
	{
		/* 收到视觉消息时释放信号量 */
		rt_sem_take(&Aim_bot_sem, RT_WAITING_FOREVER);
		//坐标转换
		//Point_t temp_point = Transformed_coordinate(vision_point,pitch_angle);
		
		//计算初始Δpitch Δyaw
		Aimbot_t temp_aim_data = Calc_from_point(&visual_rev);
		
		
		//获取距离，子弹水平速度
		float temp_dis = (visual_rev.z)*(visual_rev.z)+(visual_rev.x)*(visual_rev.x)+(visual_rev.z)*(visual_rev.z);
		float distance = Q_rsqrt(temp_dis);
		
		float bullet_speed_horizontal = bullet_speed*cos((gimbal_atti.pitch+temp_aim_data.pitch_add)*pi_operator);
		
		//重力补偿
		Aimbot_data.pitch_add = Gravity_compensation(gimbal_atti.pitch,temp_aim_data.pitch_add,distance,bullet_speed_horizontal);
		/* 根据两次角度求角速度 单位°/s */
		vision_yaw_palstance = (temp_aim_data.yaw_add - last_visual_yaw)/((rt_tick_get()-tick)/1000.0f);
		
		//提前量预瞄
		Aimbot_data.yaw_add = Advance_yaw_angle(vision_yaw_palstance,(visual_rev.computime+t_transmit),distance,bullet_speed_horizontal);
		
		last_visual_yaw = temp_aim_data.yaw_add;
		tick = rt_tick_get();
	}
	
}
void aim_bot_creat(void)
{
	//定时器处理线程
	rt_thread_t thread;
	rt_sem_init(&Obtain_palstance_sem, "Obtain_palstance_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("Obtain_palstance_thread", Obtain_palstance_emtry, RT_NULL, 1024, 3, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	//定时器中断
	rt_timer_init(&Obtain_palstance_task,
                   "Obtain_palstance_task",
                   Obtain_palstance_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 //启动定时器
	rt_timer_start(&Obtain_palstance_task);
	
	//初始化一个线程 用于在视觉发出一帧数据时处理数据
	//是否需要未按自瞄键时挂起该线程 ， 按下自瞄才恢复该线程？
	rt_sem_init(&Aim_bot_sem, "Aim_bot_sem", 0, RT_IPC_FLAG_FIFO);
	aim_thread = rt_thread_create("Aim_bot_sem", aim_bot_emtry, RT_NULL, 1024, 2, 1);
	if (aim_thread != RT_NULL)
	{
			rt_thread_startup(aim_thread);
	}
	rt_thread_suspend(aim_thread);
}
//INIT_APP_EXPORT(aim_bot_creat);
