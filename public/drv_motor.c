#include "drv_motor.h"

/**
 * @brief  �Է����ǶȽ��л���Ϊ0-8191
 * @param  angle���Ƕ�
 * @param  motor��������ݽṹ��
 * @retval None
 */
static void motor_angle_adjust(rt_uint16_t angle,Motor_t* motor)
{
	float angletemp = 0;
	
	/*�����ܵı�����ת���Ƕ�*/
	if(motor->speed >= 0)
	{
		if(angle < motor->old_angle - 1000)														/*��1000��Ϊ��ȥ������������*/
		{
			motor->loop += 1;
		}
	}
	else
	{
		if(angle > motor->old_angle + 1000)														/*��1000��Ϊ��ȥ������������*/
		{
			motor->loop -= 1; 
		}
	}
	/*����0-8191��Χ�ڵĽǶ�*/
	angletemp = (motor->loop*8192 + angle) * motor->ratio / 8192;
	angletemp = angletemp - (int)angletemp; 
	if(angletemp < 0)
	{
		motor->angle = (angletemp + 1)* 8192;
	}
	else
	{
		motor->angle = angletemp * 8192;
	}
	motor->old_angle = angle;
}
/**
 * @brief  ��ȡcan�еĵ������
 * @param  rxmsg��rtcan�ṹ��
 * @param  moto��������ݽṹ
 * @retval None
 */
void motor_readmsg(struct rt_can_msg* rxmsg,Motor_t* motor)
{
	rt_uint16_t angle;
	
	motor->speed = (rt_int16_t)(rxmsg->data[2]<<8 | rxmsg->data[3]);
	motor->current = (rt_int16_t)(rxmsg->data[4]<<8 | rxmsg->data[5]);
	motor->temperature = rxmsg->data[6];
	
	angle = (rt_int16_t)(rxmsg->data[0]<<8 | rxmsg->data[1]);
	motor_angle_adjust(angle, motor);
}

/**
 * @brief  �����������
 * @param  dev��can�豸
 * @param  setcurn:�趨���� 
 * @retval RT_EOK or RT_ERROR(�ɹ���ʧ�ܻ���������)
 */
rt_size_t motor_current_send(rt_device_t dev, 
														SendID_e   	ID,
														rt_int16_t 	setcur1, 
														rt_int16_t 	setcur2, 
														rt_int16_t 	setcur3, 
														rt_int16_t 	setcur4)
{
	struct rt_can_msg txmsg;
	
	txmsg.id=ID;
	txmsg.ide = RT_CAN_STDID;txmsg.rtr=RT_CAN_DTR;
	txmsg.len=8;
	txmsg.data[0]=(rt_uint8_t)(setcur1>>8);txmsg.data[1]=(rt_uint8_t)(setcur1);
	txmsg.data[2]=(rt_uint8_t)(setcur2>>8);txmsg.data[3]=(rt_uint8_t)(setcur2);
	txmsg.data[4]=(rt_uint8_t)(setcur3>>8);txmsg.data[5]=(rt_uint8_t)(setcur3);
	txmsg.data[6]=(rt_uint8_t)(setcur4>>8);txmsg.data[7]=(rt_uint8_t)(setcur4);
	return rt_device_write(dev, 0, &txmsg, sizeof(txmsg));
}

/**
 * @brief  �����ʼ�� pid �Լ���ʼ��
 */
void motor_init(Motor_t *motor,rt_uint32_t ID,float radio)
{
	motor->motorID = ID;
	motor->ratio = radio;
}
/**
 * @brief  ���õ�������˶�
 * @retval ���ص���  
 */
void motor_speed_set(Motor_t *motor,int speed)
{
	motor->set_speed = speed;
}

/**
 * @brief  ���õ���Ƕ� -180-180��
 * @retval ���ص���
 */
void motor_angle_set(Motor_t *motor,int angle)
{	
//	int now = motor->angle;
//	int temp_set = now + (int)(angle/360.0*8192);
//	int err = 0;	
//	if(temp_set > 8191)
//	{temp_set -= 8191;}
//	else if(temp_set<0)
//	{temp_set += 8191;}
//	err = temp_set - now;
//	if(ABS(err)> (8191/2))
//	{
//		if(err>0){err-=8192;}
//		else {err += 8192;}
//	}
	motor->set_angle += angle;
}
rt_int16_t motor_angle_judge(Motor_t *motor)
{
	rt_int16_t err;	
	if(motor->set_angle > 8191)
	{
		motor->set_angle -= 8191;
	}
	else if(motor->set_angle < 0)
	{
		motor->set_angle += 8191;
	}
	err = motor->set_angle - motor->angle;
	if(ABS(err) > (8192/2))
	{
		if(err>0){err -= 8192;}
		else {err += 8192;}
	}
	return err;
}
