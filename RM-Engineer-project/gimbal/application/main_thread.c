#include "main_thread.h"
#include "drv_gimbal.h"
#include "robodata.h"
#include "drv_remote.h"
#include "drv_strike.h"
#include "drv_aimbot.h"
#include "comm.h"
/* �������߳̾�� */
rt_thread_t Main_Thread;
/* �Ƿ�ʹ�ð������� */
static rt_uint8_t key_ctrl = 0;
/* �洢yaw��pitch������ */
static rt_int32_t pitch_add = 0;
static rt_int32_t yaw_add = 0;

/* yaw����ٶ� */
float yaw_speed_last = 0.0f;
/* Ħ�����ٶ� */
rt_uint16_t gunspeed = 115;
/* ��̨����״̬ */
static rt_int8_t gimbal_ctrl_state = 0;
static void main_thread_entry(void * parameter)
{
	while(1)
	{
		                  /* ��̨���� */
		
		
		/********************��ȡ����************************/
		switch_action_e s1_action = Change_from_middle(S1);
		switch_action_e s2_action = Change_from_middle(S2);
		/* �ò��˴���up״̬��Ϊ������� */
//		if(RC_data.Remote_Data.s1 == 1)
//		{key_ctrl = 1;}
//		else{key_ctrl = 0;}
		
#if key_ctrl
#else
		/* ���������ʹ��ң�������� */
		if(gimbal_ctrl_state == 0)
		{
			/* ��ȡң������ch0��ch1 */
		    pitch_add = (RC_data.Remote_Data.ch1-1024)/10;
		    yaw_add = -(RC_data.Remote_Data.ch0-1024)/10;
		}
		/* ��������� */
		if(gimbal_ctrl_state == 1)
		{
		}
		/* ȡ��ģʽ */
		if(gimbal_ctrl_state == 2)
		{
			/*TODO: */
		}
		
		/*����Ħ���� */
		if(s1_action == middle_to_up)
		{
			if(gun1.speed <11)
			{Gun_speed_set(&gun1,gunspeed);}
			else{Gun_speed_set(&gun1,10);}
		}
		
		/* ���� */
		if(s1_action == middle_to_down)
		{
			strike_fire(&m_launch,&gun1,1);
		}
		
		/********yaw����λ����*************/
		
		/* ��ȡ10ms֮ǰ��Ϊ0��yaw����ٶ� */
//		if(ABS(gimbal_atti.yaw_speed)>1)
//		{
//			yaw_speed_last = gimbal_atti.yaw_speed;
//		}
//		/* ���ݽ��ٶȶ�yaw����λ */
//		if(yaw_speed_last > 0)
//		{
//			if(yaw.setang > 5999)
//			{yaw.setang = 6000;}
//		}
//		else if(yaw_speed_last < 0)
//		{
//			if(yaw.setang>4096&&yaw.setang<6800)
//			{
//				yaw.setang = 6800;
//			}
//		}
		gimbal_addangle_set(yaw_add,pitch_add);
		
		chassis_ctrl_data.x_speed = (RC_data.Remote_Data.ch2 - 1024)*1.5;
		chassis_ctrl_data.y_speed = (RC_data.Remote_Data.ch3 - 1024)*1.5;
		chassis_ctrl_data.angular_velocity = 0;
		chassis_ctrl_data.follow_angle = 0;
		if(s2_action == middle_to_down)
		{
			if(chassis_ctrl_data.chassis_ctrl == No_Follow)
				chassis_ctrl_data.chassis_ctrl = Follow;
			else if(chassis_ctrl_data.chassis_ctrl == Follow)
				chassis_ctrl_data.chassis_ctrl = No_Follow;
		}
		Send_chassis_data(can2_dev);
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
