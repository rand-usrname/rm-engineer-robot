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
	rt_int16_t x_speed; //���X���ٶ�
	rt_int16_t y_speed; //���Y���ٶ�
	rt_int16_t z_speed; //��ʱ����
	rt_int8_t press_l;	//������
	rt_int8_t press_r; //����Ҽ�

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
	Remote_t 	Remote_Data;	//ң������������
	Mouse_t 	Mouse_Data;		//��겿������
	Key_Data_t 	Key_Data;		//���̲�������
} RC_Ctrl_t;

extern RC_Ctrl_t *RC_data;
extern int remote_uart_init(void);


rt_uint8_t Obtain_Sx_Data(rt_uint8_t num);
rt_uint8_t Obtain_Key_Press(rt_uint8_t *targetdata);
//ʹ��ʱ��Ҫ�Ƚϵ����ݴ��뺯����
//if(RCD_change(RC_data->Key_Data.A))
//�����жϼ�������A�Ƿ�ı�
//#define RCD_change(targetdata)	_RCD_change((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length);

////�ж�ĳ�����Ƿ�������¶�������ֻ���жϰ�����������ݣ��������������������δ֪�������
//#define RCD_press(targetdata)	_RCD_press((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_press(rt_uint8_t *targetdata,rt_uint8_t length);

////�ж�ĳ�����Ƿ�����ɿ���������ֻ���жϰ�����������ݣ��������������������δ֪�������
//#define RCD_loosen(targetdata)	_RCD_loosen((rt_uint8_t*)(&targetdata),sizeof(targetdata))
//extern rt_uint8_t _RCD_loosen(rt_uint8_t *targetdata,rt_uint8_t length);

#endif
