#ifndef __DRV_REMOTE_H__
#define __DRV_REMOTE_H__
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#define key_enum_enable 1
typedef enum
{
	no_action          =  0x00,
	up_to_middle       =  0x01,
	down_to_middle     =  0x02,
	middle_to_up       =  0x03,
	middle_to_down     =  0x04,
	PRESS_ACTION       =  0x05,
	LOOSEN_ACTION      =  0x06,
	
	
	S1             =  0x07,
	S2             =  0x08,
	#if key_enum_enable
	Q_ACTION       =  0x09,
	E_ACTION       =  0x0A,
	SHIFT_ACTION   =  0x0B,
	CTRL_ACTION    =  0x0C,
	R_ACTION       =  0x0D,
	F_ACTION       =  0x0E,
	G_ACTION       =  0x10,
	Z_ACTION       =  0x20,
	X_ACTION       =  0x30,
	C_ACTION       =  0x40,
	V_ACTION       =  0x50,
	B_ACTION       =  0x60,
	#endif
	
}switch_action_e;
typedef struct  __Remote
{
	uint16_t ch0;
	uint16_t ch1;
	uint16_t ch2;
	uint16_t ch3;
	uint8_t s1;
	uint8_t s2;
}Remote_t;
typedef struct  __Mouse
{
	int16_t x_speed;//向左负
	int16_t y_speed;//向下正
	int16_t z_speed;
	int8_t press_l;
	int8_t press_r;
}Mouse_t;
typedef	struct __Key_Data
{
	    uint8_t W;
        uint8_t S;
		uint8_t A;
		uint8_t D;
		uint8_t Q;
		uint8_t E;
		uint8_t shift;
		uint8_t ctrl;
	    uint8_t R;
        uint8_t F;
		uint8_t G;
		uint8_t Z;
		uint8_t X;
		uint8_t C;
		uint8_t V;
		uint8_t B;
}Key_Data_t;

typedef struct __RC_Ctrl
{
	Remote_t Remote_Data;
	Mouse_t Mouse_Data;
	rt_uint16_t v;
	Key_Data_t Key_Data;
}RC_Ctrl_t;

extern RC_Ctrl_t RC_data;
extern rt_uint16_t key_change;
extern int remote_uart_init(void);

extern switch_action_e Change_from_middle(switch_action_e sx);
extern switch_action_e Change_to_middle(switch_action_e sx);
extern switch_action_e Key_action_read(rt_uint8_t *targetdata);
#endif

