#include "super_capacitor.h"

//���泬�����ݵ���������
static rt_uint16_t super_capacitor_electric = 0;

/**
* @brief�����³������ݵ�������
* @param [in]	message:���յ�������ָ֡��
* @return��		true:���³ɹ�
							false:����ʧ��
* @author��mqy
*/
bool refresh_super_capacitor_data(struct rt_can_msg* message)
{
	if(message->id == SUPER_CAPACITOR_MESSAGE_ID)
	{
		super_capacitor_electric = 0;
		return true;
	}
	return false;
}
/**
* @brief����ȡ�������ݵ�ǰ�����ٷֱ�
* @param [in]	��
* @return��		0-100��һ���������ݣ���ֵΪʣ������ռ���ܵİٷֱ�
* @author��mqy
*/
rt_uint16_t access_electricity(void)
{
	return super_capacitor_electric;
}
