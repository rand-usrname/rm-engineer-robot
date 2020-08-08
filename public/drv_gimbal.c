#include "gimbal-control.h"


/**
* @brief����ʼ����̨�߳�
* @param [in]	��
* @return��		1:��ʼ���ɹ�
				0:��ʼ��ʧ��
* @author��mqy
*/
int gimbal_init(void)
{
	
}

/**
* @brief�����øú������������Ǻ���̨�����ת�ٵ�����
* @param [in]	message:���յ�������ָ֡��
* @return��		1:���³ɹ�
				0:id��ƥ�����ʧ��
* @author��mqy
*/
int refresh_device_data(struct rt_can_msg* message)
{
	//��������
	switch(message->id)
	{
		case YAW_ID:
			assign_motor_data(&motion_data.yaw_data,message);
			//ת���Ƕ���ֵ������ϵ
			if(motion_data.yaw_data.angle < YAW_START_ANGLE)
			{
				motion_data.yaw_data.angle = 8191 - YAW_START_ANGLE + motion_data.yaw_data.angle;
			}
			else
			{
				motion_data.yaw_data.angle = motion_data.yaw_data.angle - YAW_START_ANGLE;
			}
			return 1;
			
		case PITCH_ID:
			assign_motor_data(&motion_data.pitch_data,message);
			return 1;
			
		default:
			
			
			break;
	}
	return 0;
}
