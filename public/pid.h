#ifndef __PID_H__
#define __PID_H__

#include <rtthread.h>


typedef struct
{
	float 	kp;					//pid��������
	float 	ki;
	float 	kd;
	int 	err;				//ƫ��ֵ
	int 	err_old;			//�ϴ�ƫ��ֵ
	float 	i_value;			//���ڵĻ���ֵ���û����Ѿ�����ki������
	float  	i_limit;			//�����޷�
	int 	set;				//pid���趨ֵ
	int		out;				//pid�����
	int		out_limit_up;		//������޷�
	int		out_limit_down;		//������޷�
	
}pid_t;
//���PID�ջ��ṹ��

extern void pid_init(pid_t *pid,
	float kp, float ki, float kd, 	//pid����
	rt_uint16_t i_limite, 			//�����޷�
	rt_int16_t out_limite_up, 		//������޷�
	rt_int16_t out_limite_down);	//������޷�
		
extern int pid_output_calculate(pid_t* target,int nowdata);
extern void pid_clear(pid_t* target);
	
#endif
