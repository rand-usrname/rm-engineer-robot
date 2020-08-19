#ifndef __TRAILER_H
#define __TRAILER_H

#include <rtthread.h>
#include <drv_motor.h>
#include <board.h>
#include "robodata.h"
#define     TRAILER_RADIO   1
/* ������λ���� */
#define     TRAILER_PINL    GET_PIN(A,1)
#define     TRAILER_PINR    GET_PIN(A,2)

#define     prepare_angle   120
#define     catch_angle     80
typedef enum
{
	rescue_unable   =  0x01,     /* ��û��ʼ��Ԯ���ϳ����ڳ�ʼ״̬ */
	rescue_prepare  =  0x02,     /* ׼����Ԯ �ϳ�Ԥ��*/
	rescue_going    =  0x04,     /* ���ھ�Ԯ */
	rescue_reset    =  0x08         /* ��λ */
}trailer_state_e;


typedef struct __trailer_t
{
	Motor_t trailer_motor;
	rt_uint8_t state;
	rt_uint16_t Pin_num;
}trailer_t;

extern trailer_t trailerl;
extern trailer_t trailerr;
void trailer_start(void);
void trailer_go(trailer_t *trailer);

#endif
