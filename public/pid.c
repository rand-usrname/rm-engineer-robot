#include "pid.h"

/**
 * @brief  pid�����趨
 * @param  pid��pid�ṹ��ָ��
 */
void pid_init(pid_t *pid,
			float kp, float ki, float kd,
			rt_uint16_t i_limit,
			rt_int16_t 	out_limit_up,
			rt_int16_t 	out_limit_down)
{
	//��������
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->i_limit = i_limit;
	pid->out_limit_up = out_limit_up;
	pid->out_limit_down = out_limit_down;
	//״̬����
	pid->err = 0;
	pid->err_old = 0;
	pid->i_value = 0;
	pid->out = 0;
}

/**
* @brief���ú��������ĸ�������趨�����ֱ����������
* @param [in]	target:Ҫ�����pid�ṹ��
				error:����ֵ��ʵ��ֵ������λrpm
* @return��		pid�ļ�����
* @author��mqy
*/
int pid_output_calculate(pid_t* target,int nowdata)
{
	int error = target->set - nowdata;
	target->err_old = target->err;
	target->err = error;
	target->i_value += target->ki * error;
	
	//�����޷�
	if(target->i_value < 0)
	{
		if(target->i_value < -target->i_limit)
		{
			target->i_value = -target->i_limit;
		}
	}
	else
	{
		if(target->i_value > target->i_limit)
		{
			target->i_value = target->i_limit;
		}
	}
	target->out = target->kp*(float)error + target->i_value + target->kd*(float)(target->err - target->err_old);
	
	//����޷�
	if(target->out > target->out_limit_up)
	{
		target->out = target->out_limit_up;
	}
	else if(target->out < target->out_limit_down)
	{
		target->out = target->out_limit_down;
	}
	return target->out;
}

/**
* @brief�����PID��״̬
* @param [in]	target:Ҫ�����PID�ṹ��ָ��
* @return��		��
* @author��mqy
*/
void pid_clear(pid_t* target)
{
	//״̬����
	target->err = 0;
	target->err_old = 0;
	target->i_value = 0;
	target->out = 0;
}
