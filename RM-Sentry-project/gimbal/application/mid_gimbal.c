#include "drv_strike.h"
#include "drv_motor.h"
#include "mid_gimbal.h"
#include "can_receive.h"
#include "robodata.h"

//第一部分:云台电机
/**
 * @brief  云台电机pid设置
 */
static void pid_gimbal_init(void)
{
	/*pit轴位置环速度环pid参数初始化*/
	pid_init(&m_pitch.ang, 
					8,0.05,5,
					150,2000,-2000);
	pid_init(&m_pitch.spe, 
					30,0,0,
					6000,28000,-28000);
	/*yaw轴位置环速度环pid参数初始化*/
	pid_init(&m_yaw.ang, //8 10 80 600 30
					2,0.05,0,
					50,600,-600);
	pid_init(&m_yaw.spe, 
					40,0,0,
					300,28000,-28000);
	/**********测试用***********/
	m_pitch.ang.set=3200;//1303;			//2650
	m_yaw.ang.set=2000;		//4000
}

/**
 * @brief  云台电机初始化（pid初始化）
 */
void motor_gimbal_init(void)
{
	pid_gimbal_init();
	m_pitch.dji.motorID = Pitch_ID;
	m_pitch.dji.ratio = 1;																/*减速比<=1*/
	
	m_yaw.dji.motorID = Yaw_ID;
	m_yaw.dji.ratio = 1;
}


//第二部分:发弹电机
void launch_reinit()
{
    motor_init(&m_launch,0x206,1);
	pid_init(&m_launch.ang, 
					8,0.05,5,
					150,2000,-2000);
	pid_init(&m_launch.spe, 
					30,0,0,
					6000,28000,-28000);
}


//第三部分:云台与底盘通信

