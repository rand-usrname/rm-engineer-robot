#include "robocontrol.h"
/**
* @brief  向底盘发送命令以控制底盘
* @param  xspeed：x轴速度 yspeed：y轴速度
* @param  mode: 模式（不跟随0，跟随1）
* @param  angel_or_speed：跟随时的角度，不跟随时速度
*/
static int chassis_ctl(rt_int16_t xspeed, rt_int16_t yspeed, rt_uint8_t mode,rt_int16_t angel_or_speed)
{
	struct rt_can_msg txmsg;

	txmsg.id = CHASSIS_CTL;
	txmsg.ide = RT_CAN_STDID;
	txmsg.rtr = RT_CAN_DTR;
	txmsg.len = 8;
	txmsg.data[0] = (rt_uint8_t)(xspeed>>8);
	txmsg.data[1] = (rt_uint8_t)(xspeed);
	txmsg.data[2] = (rt_uint8_t)(yspeed>>8);
	txmsg.data[3] = (rt_uint8_t)(yspeed);
	txmsg.data[4] = (rt_uint8_t)(angel_or_speed>>8);
	txmsg.data[5] = (rt_uint8_t)(angel_or_speed);
	txmsg.data[6] = (rt_uint8_t)(mode);
	txmsg.data[7] = 0;
	rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg));
}
int gun1speed = 0;
int basespeed = 800;
int sport_mode = 0;
/**
* @brief：一键回头入口函数
* @param [in]	time:回头后转换回跟随模式的时间，假装不是指针就行了
* @return：		无
* @author：mqy
*/
static void onekey_back(void* parameter)
{
	sport_mode = 0;//非跟随
	gimbal_addangle_set(4096,0);
	rt_thread_mdelay((int)parameter);//假装传入的是指针
	sport_mode = 1;//跟随
	return;
}
/**
* @brief：云台控制模式设置
* @param [in]	yawset：yaw轴控制模式
				pitchset：pitch轴控制模式
* @return：		yaw轴角度，格式0-8191
* @author：mqy
*/
int computer_ctrl(RC_Ctrl_t *remote)
{
	rt_int16_t xspeed = (remote->Key_Data.D - remote->Key_Data.A) * basespeed;
	rt_int16_t yspeed = (remote->Key_Data.W - remote->Key_Data.S) * basespeed;
	if(Key_action_read(&(remote->Key_Data.F)) == PRESS_ACTION)
	{
		//初始化回头
		rt_thread_t gimbal_control = rt_thread_create(
		"onekey_back",	//线程名
		onekey_back,	//线程入口
		(void*)(10000),	//入口参数无
		2048,			//线程栈
		1,	            //线程优先级
		1);				//线程时间片大小
		rt_thread_startup(gimbal_control);//启动线程
	}
	//发送底盘控制信息
	switch(sport_mode)
	{
		case 0://非跟随
			chassis_ctl(xspeed,yspeed,0,0);
			break;
		case 1://跟随
			chassis_ctl(xspeed,yspeed,1,0);
			break;
		case 2://小陀螺
			chassis_ctl(xspeed,yspeed,0,1000);
			break;
		default:
			chassis_ctl(0,0,0,0);
			break;
	}
	//设置云台角度
	gimbal_addangle_set( -remote->Mouse_Data.x_speed, -remote->Mouse_Data.y_speed);
	return 1;
}
/**
* @brief：云台控制模式设置
* @param [in]	yawset：yaw轴控制模式
				pitchset：pitch轴控制模式
* @return：		yaw轴角度，格式0-8191
* @author：mqy
*/
int remote_ctrl(RC_Ctrl_t *remote)
{
	rt_int16_t pitchadd = 0;
	rt_int16_t yawadd = 0;
	
	//状态触发部分
	//S2状态触发
	if(remote->Remote_Data.s2 == 1)//右侧案件在上
	{
		//使用电脑数据进行操作，屏蔽其他控制
		return 0;//computer_ctrl(remote);
	}
	else//右侧按键不在上
	{
		if(get_yawusetime() < 1){
			yawadd = -get_yaw_add()/360.0f*8192;
		}
		else{
			yawadd = -(rt_int16_t)((remote->Remote_Data.ch0 - 1024)/30);
		}
		//pitch增量设置，采用视觉数据或者遥控器数据
		if(get_pitchusetime() < 1){
			pitchadd = get_pitch_add()/360.0f*8192;
		}
		else{
			pitchadd = (rt_int16_t)((remote->Remote_Data.ch1 - 1024)/30);
		}
	}
	
	//动作触发部分
	//S1动作
	switch_action_e s_action;//动作触发临时变量
	s_action = Change_from_middle(S1);
	if(s_action == middle_to_up)//向上拨动
	{
		//调整并设置摩擦轮速度
		if(gun1speed  == 0){
			gun1speed = -3000;
		}
		else if(gun1speed < -6000){
			gun1speed = 0;
		}
		else{
			gun1speed -= 400;
		}
		Gun_speed_set(&gun1,gun1speed);
	}
	else if(s_action == middle_to_down)//向下拨动
	{
		//!!!Fire!!!
		strike_fire(&m_launch,&gun1,1);
	}
	//S2动作
	s_action = Change_from_middle(S2);//向上拨动用于切换电脑操作
	if(s_action == middle_to_down)//向下拨动
	{
		//控制运动模式
		sport_mode++;
		if(sport_mode > 2)
		{
			sport_mode = 0;
		}
	}
	
	//发送底盘控制信息
	switch(sport_mode)
	{
		case 0://非跟随
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,0,0);
			break;
		case 1://跟随
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,1,0);
			break;
		case 2://小陀螺
			chassis_ctl((remote->Remote_Data.ch2 - 1024)*3,(remote->Remote_Data.ch3 - 1024)*3,0,1000);
			break;
		default:
			chassis_ctl(0,0,0,0);
			break;
	}
	//设置云台
	if(pitchadd!=0)
	{
		pitch.setang = (pitchadd+get_pitchangle());
	}
	if(yawadd!=0)
	{
		yaw.setang = (yawadd+get_yawangle());
	}
	yawadd = 0;
	pitchadd = 0;
	//保证数据不超过范围
	yaw.setang %= 8192;
	pitch.setang %= 8192;
	return 1;
}

