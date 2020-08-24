#include "main_thread.h"
#include "drv_gimbal.h"
#include "robodata.h"
#include "drv_remote.h"
#include "drv_strike.h"
#include "drv_aimbot.h"
/* 主控制线程句柄 */
rt_thread_t Main_Thread;
/* 是否使用按键控制 */
static rt_uint8_t key_ctrl = 0;
/* 存储yaw和pitch的增量 */
static rt_int32_t pitch_add = 0;
static rt_int32_t yaw_add = 0;
/* 云台控制状态 */
static rt_int8_t gimbal_ctrl_state = 0;
static void main_thread_entry(void * parameter)
{
	while(1)
	{
		                  /* 云台控制 */
		
		
		/********************读取拨杆************************/
		switch_action_e s1_action = Change_from_middle(S1);
		switch_action_e s2_action = Change_from_middle(S2);
		/* 该拨杆处于up状态则为键鼠控制 */
//		if(RC_data.Remote_Data.s1 == 1)
//		{key_ctrl = 1;}
//		else{key_ctrl = 0;}
		
#if key_ctrl
#else
		/* 如果不自瞄使用遥控器控制 */
		if(gimbal_ctrl_state == 0)
		{
			/* 读取遥控器的ch0及ch1 */
		    pitch_add = (RC_data.Remote_Data.ch1-1024)/100;
		    yaw_add = (RC_data.Remote_Data.ch0-1024)/100;
		    /*TODO: 需要限位 */
		
		    gimbal_addangle_set(yaw_add,pitch_add);
		}
		/* 自瞄情况下 */
		if(gimbal_ctrl_state == 1)
		{
			if(get_pitchusetime()==0)
			{
				pitch_add = get_pitch_add();
			}
			if(get_yawusetime() == 0)
			{
				yaw_add = get_yaw_add();
			}
			 gimbal_addangle_set(yaw_add,pitch_add);
		}
		/* 取弹模式 */
		if(gimbal_ctrl_state == 2)
		{
			/*TODO: */
		}
		
		/*开关摩擦轮 */
		if(s1_action == middle_to_up)
		{
			if(gun1.speed <11)
			{Gun_speed_set(&gun1,100);}
			else{Gun_speed_set(&gun1,10);}
		}
		
		/* 拨弹 */
		if(s1_action == middle_to_down)
		{
			strike_fire(&m_launch,&gun1,1);
		}
#endif
		
		rt_thread_mdelay(20);
	}
}
rt_err_t main_thread_init(void)
{
	Main_Thread = rt_thread_create("main_task", main_thread_entry, RT_NULL, 1024, 5, 2);
	if (Main_Thread != RT_NULL)
	{
			return rt_thread_startup(Main_Thread);
	}
	return RT_ERROR;
}
