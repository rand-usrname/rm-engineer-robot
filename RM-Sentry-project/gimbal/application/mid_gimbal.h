#ifndef __MID_GIMBAL_H__
#define __MID_GIMBAL_H__

#include <rtthread.h>
#include <rtdevice.h>

//底盘云台通信ID
#define  STDID_GIMBAL  0x110
#define  STDID_CHASSIS  0x111

void launch_reinit(void);	//哨兵拨弹6020初始化,放在通用发弹机构初始化后重新初始化
void read_chassis_data(struct rt_can_msg *msg);	//读取底盘发来的数据

#endif
