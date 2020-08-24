#ifndef __DRV_REMOTE_H__
#define __DRV_REMOTE_H__
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
/* ���˶���ö�� */
typedef enum
{
	/* ���� ���� */
	no_action          =  0x00,
	up_to_middle       =  0x01,
	down_to_middle     =  0x02,
	middle_to_up       =  0x03,
	middle_to_down     =  0x04,
	/* ���� ���� */
	PRESS_ACTION       =  0x05,
	LOOSEN_ACTION      =  0x06,
	
	
	S1             =  0x07,
	S2             =  0x08,
	
}switch_action_e;

typedef struct  __Remote
{
	/* ң������ */
	uint16_t ch0;
	uint16_t ch1;
	uint16_t ch2;
	uint16_t ch3;
	/* �������� */
	uint8_t s1;
	uint8_t s2;
}Remote_t;
/* ������� */
typedef struct  __Mouse
{
	int16_t x_speed;//����
	int16_t y_speed;//������
	int16_t z_speed;
	int8_t press_l;
	int8_t press_r;
}Mouse_t;
/* �������� */
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
/* �ⲿ���ȡң����ĳ�����򲦸�״̬ʱ���� ֱ�Ӷ��ṹ���ֵ */
extern RC_Ctrl_t RC_data;
/* ֮�����ⲿ��������Ϊ��Ҫ�ֶ����� */
extern rt_uint16_t key_change;
/* �ⲿ��main.c�е��� */
extern int remote_uart_init(void);

/* ��ȡ���˶���(ֻ�ܶ�ȡ�м������߲��Ķ���) */
extern switch_action_e Change_from_middle(switch_action_e sx);
/* ��ȡ���˶���(ֻ�ܶ�ȡ�������м䲦�Ķ���) */
extern switch_action_e Change_to_middle(switch_action_e sx);
/* ��ȡ��������(����������) */
extern switch_action_e Key_action_read(rt_uint8_t *targetdata);
#endif

