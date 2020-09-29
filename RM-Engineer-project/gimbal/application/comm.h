#ifndef __COMM_H
#define __COMM_H


#include "comm.h"

#include <rtthread.h>
#include <rtdevice.h>
#include "robodata.h"
typedef struct __ammo_data{
	rt_uint8_t   raise_cmd;         /* 抬升至取弹位置 */
	rt_uint8_t   auto_ammo_one;     /* 单箱自动取弹 */
	rt_uint8_t   auto_ammo_three;   /* 三箱自动取弹 */
	rt_uint8_t   magazine_turn;     /* 弹仓转向 */
	rt_uint32_t  Reserved;          /* 保留位 */ 
}ammo_data_t;

typedef enum {
	No_Follow     = 0,        /* 不跟随 */
	Follow        = 1,        /* 跟随 */
	visual_ctrl   = 2,        /* 视觉控制 */
}chassis_mode_e;

typedef struct __chassis_data{
	rt_int16_t         x_speed;            
	rt_int16_t         y_speed;
	chassis_mode_e     chassis_ctrl;
	rt_uint16_t        follow_angle;             /* 跟随角度 */
	rt_int16_t         angular_velocity;         /* 小陀螺转速(仅在不跟随模式设置） */
	rt_uint8_t         rescue_cmd;               /* 拖车就绪状态 */
}chassis_data_t;


extern ammo_data_t ammo_ctrl_data;
extern chassis_data_t chassis_ctrl_data;

rt_size_t Send_ammo_data(rt_device_t dev);
rt_size_t Send_chassis_data(rt_device_t dev,chassis_data_t *chassis_ctrl_data);

void Comm_Deinit(void);
#endif

