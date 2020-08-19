#ifndef __COMM_H
#define __COMM_H

#include <rtthread.h>
#include <rtdevice.h>
typedef enum {
	No_Follow     = 0,        /* 不跟随 */
	Follow        = 1,        /* 跟随 */
	visual_ctrl   = 2,        /* 视觉控制 */
}chassis_mode_e;

typedef struct __gimbal_data{
	rt_int16_t         x_speed;            
	rt_int16_t         y_speed;
	chassis_mode_e     chassis_ctrl;
	rt_uint16_t        follow_angle;             /* 跟随角度 */
	rt_int16_t         angular_velocity;         /* 小陀螺转速(仅在不跟随模式设置） */
	rt_uint8_t         rescue_cmd;               /* 拖车就绪状态 */
}gimbal_data_t;
extern gimbal_data_t gimbal_ctrl_data;

rt_bool_t read_gimbal_data(struct rt_can_msg* msg);
void gimbal_ctrl_init(void);





#endif
