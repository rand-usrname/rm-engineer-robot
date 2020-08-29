/**************************************************************************************************
 * @fileName     main_thread                                                                      *
 * @version                                                                                       *
 * @date         2020 8 18                                                                        *
 *                                                                                                *
 * @brief        main control thread                                                              *
 *                                                                                                *
 * @copyright  . All rights reserrved                                                             *
 **************************************************************************************************/


#include "main_thread.h"
#include "comm.h"
#include "drv_chassis.h"
#include "trailer.h"
#include "robodata.h"
#include "drv_remote.h"
/* 主控制线程句柄 */


rt_thread_t Main_Thread;

static void main_thread_entry(void * parameter)
{
	static rt_uint8_t trailer_cur = 0x0F;
	while(1)
	{
#ifdef GIMBAL_CTRL
		/* 对底盘运动进行控制 */
		chassis_speed_set(gimbal_ctrl_data.follow_angle,gimbal_ctrl_data.angular_velocity,gimbal_ctrl_data.x_speed,gimbal_ctrl_data.y_speed);
		/* 对拖车进行控制 */
		if((trailerl.state == rescue_unable)&&(trailerr.state == rescue_unable))
		{
		    trailerl.state = (((gimbal_ctrl_data.rescue_cmd)>>4)&0x0E);
		    trailerr.state = (((gimbal_ctrl_data.rescue_cmd)>>4)&0x0E);
		}
		if(trailer_cur == 0x0F)
		{trailer_go(&trailerl);}
		else if(trailer_cur == 0xF0)
		{trailer_go(&trailerr);}
		trailer_cur = ~trailer_cur;
		/* 刷卡复活 */
		rt_uint8_t revive_card_cmd = ((gimbal_ctrl_data.rescue_cmd)&0x0E);
		if(revive_card_cmd){REVIVE_CARD_SET;}
		else{REVIVE_CARD_RESET;}
#endif
#ifdef SINGLE_CTRL
		/* 对底盘进行控制 */
		chassis_speed_set(0,0,RC_data.Remote_Data.ch2-1024,RC_data.Remote_Data.ch3-1024);
		/* 遥控器读取 */
		switch_action_e switch_s1_action;
		switch_action_e switch_s2_action;
		switch_s1_action = Change_from_middle(S1);
		switch_s2_action = Change_from_middle(S2);
		/* 对拖车进行控制 */
		if(switch_s1_action == middle_to_up)
        {
			if((trailerl.state == rescue_unable)&&(trailerr.state == rescue_unable))
			{trailerl.state = rescue_prepare;trailerr.state = rescue_prepare;}
		}
		if(trailer_cur == 0x0F)
		{trailer_go(&trailerl);}
		else if(trailer_cur == 0xF0)
		{trailer_go(&trailerr);}
		trailer_cur = ~trailer_cur;
		/* 刷卡控制 */
		if(switch_s2_action == middle_to_up)
		{	REVIVE_CARD_SET;}
		else if(switch_s2_action == middle_to_down)
		{	REVIVE_CARD_RESET;}
		
#endif
		rt_thread_mdelay(10);
	}
}
rt_err_t main_thread_init(void)
{
	Main_Thread = rt_thread_create("trailer_task", main_thread_entry, RT_NULL, 1024, 5, 2);
	if (Main_Thread != RT_NULL)
	{
			return rt_thread_startup(Main_Thread);
	}
	return RT_ERROR;
}
