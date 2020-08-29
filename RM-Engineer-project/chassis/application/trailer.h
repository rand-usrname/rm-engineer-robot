#ifndef __TRAILER_H
#define __TRAILER_H

#include <rtthread.h>
#include <drv_motor.h>
#include <board.h>
#include "robodata.h"
#define     TRAILER_RADIO   0.02009
/* ������λ���� */
#define     TRAILER_PINL    GET_PIN(A,1)
#define     TRAILER_PINR    GET_PIN(A,2)

#define     prepare_anglel   110.75
#define     catch_anglel     38.65
#define     prepare_angler   -127.75
#define     catch_angler     -38.65
typedef enum
{
	rescue_unable   =  0x01,     /* ��û��ʼ��Ԯ���ϳ����ڳ�ʼ״̬ */
	rescue_prepare  =  0x02,     /* ׼����Ԯ �ϳ�Ԥ��*/
	rescue_going    =  0x04,     /* ���ھ�Ԯ */
	rescue_prereset =  0x08,     /* ��׼��λ�ûص�ԭ�� */
	rescue_goreset  =  0x10,     /* �Ӽ�ȡλ�ûص�ԭ�� */
}trailer_state_e; 


typedef struct __trailer_t
{
	Motor_t trailer_motor;
	rt_uint8_t state;
	rt_uint16_t Pin_num;
}trailer_t;

extern trailer_t trailerl;
extern trailer_t trailerr;

/* ÿ��У������ƫ��ֵ */
extern float trailerl_offset;
extern float trailerr_offset;
/* �궨��ʼ�Ƕ� */
extern rt_uint16_t origin_anglel;
extern rt_uint16_t origin_angler;

void trailer_start(void);
void trailer_go(trailer_t *trailer);

#endif
