#include "main_thread.h"
#include "drv_gimbal.h"
#include "robodata.h"
#include "drv_remote.h"
#include "drv_strike.h"
#include "drv_aimbot.h"
/* �������߳̾�� */
rt_thread_t Main_Thread;
/* �Ƿ�ʹ�ð������� */
static rt_uint8_t key_ctrl = 0;
/* �洢yaw��pitch������ */
static rt_int32_t pitch_add = 0;
static rt_int32_t yaw_add = 0;
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
		    pitch_add = (RC_data.Remote_Data.ch1-1024)/100;
		    yaw_add = (RC_data.Remote_Data.ch0-1024)/100;
		    /*TODO: ��Ҫ��λ */
		
		    gimbal_addangle_set(yaw_add,pitch_add);
		}
		/* ��������� */
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
		/* ȡ��ģʽ */
		if(gimbal_ctrl_state == 2)
		{
			/*TODO: */
		}
		
		/*����Ħ���� */
		if(s1_action == middle_to_up)
		{
			if(gun1.speed <11)
			{Gun_speed_set(&gun1,100);}
			else{Gun_speed_set(&gun1,10);}
		}
		
		/* ���� */
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
