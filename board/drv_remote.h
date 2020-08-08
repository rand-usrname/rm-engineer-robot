#ifndef __DRV_REMOTE_H__
#define __DRV_REMOTE_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

typedef struct{
	rt_uint16_t ch0;
	rt_uint16_t ch1;
	rt_uint16_t ch2;
	rt_uint16_t ch3;
	rt_uint8_t s1;
	rt_uint8_t s2;

} Remote_t;

typedef struct{
	rt_int16_t x_speed; //鼠标X轴速度
	rt_int16_t y_speed; //鼠标Y轴速度
	rt_int16_t z_speed; //暂时无用
	rt_int8_t press_l;	//鼠标左键
	rt_int8_t press_r; //鼠标右键

} Mouse_t;

typedef struct{
	rt_uint8_t W;
	rt_uint8_t S;
	rt_uint8_t A;
	rt_uint8_t D;
	rt_uint8_t Q;
	rt_uint8_t E;
	rt_uint8_t shift;
	rt_uint8_t ctrl;
	rt_uint8_t R;
	rt_uint8_t F;
	rt_uint8_t G;
	rt_uint8_t Z;
	rt_uint8_t X;
	rt_uint8_t C;
	rt_uint8_t V;
	rt_uint8_t B;

} Key_Data_t;

typedef struct __RC_Ctrl
{
	Remote_t 	Remote_Data;	//遥控器部分数据
	Mouse_t 	Mouse_Data;		//鼠标部分数据
	Key_Data_t 	Key_Data;		//键盘部分数据
} RC_Ctrl_t;

extern RC_Ctrl_t *RC_data;
extern int remote_uart_init(void);


rt_uint8_t Obtain_Sx_Data(rt_uint8_t num);
rt_uint8_t Obtain_Key_Press(rt_uint8_t *targetdata);
//使用时将要比较的数据传入函数，
//if(RCD_change(RC_data->Key_Data.A))
//即可判断键盘数据A是否改变
//#define RCD_change(targetdata)	_RCD_change((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length);

////判断某个键是否产生按下动作（！只能判断按键和鼠标数据，若传入其他参数会产生未知结果！）
//#define RCD_press(targetdata)	_RCD_press((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_press(rt_uint8_t *targetdata,rt_uint8_t length);

////判断某个键是否产生松开动作（！只能判断按键和鼠标数据，若传入其他参数会产生未知结果！）
//#define RCD_loosen(targetdata)	_RCD_loosen((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_loosen(rt_uint8_t *targetdata,rt_uint8_t length);

#endif
