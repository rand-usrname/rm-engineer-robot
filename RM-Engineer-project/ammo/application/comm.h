#ifndef __COMM_H
#define __COMM_H

#include <rtthread.h>
#include <rtdevice.h>
typedef struct __gimbal_data{
	rt_uint8_t   raise_cmd;         /* 抬升至取弹位置 */
	rt_uint8_t   auto_ammo_one;     /* 单箱自动取弹 */
	rt_uint8_t   auto_ammo_three;   /* 三箱自动取弹 */
	rt_uint8_t   magazine_turn;     /* 弹仓转向 */
	rt_uint32_t  Reserved;          /* 保留位 */ 
}gimbal_data_t;



extern gimbal_data_t gimbal_ctrl_data;

rt_bool_t read_gimbal_data(struct rt_can_msg* msg);
void gimbal_ctrl_init(void);





#endif

