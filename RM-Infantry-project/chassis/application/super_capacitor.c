#include "super_capacitor.h"

//储存超级电容电量的数据
static rt_uint16_t super_capacitor_electric = 0;

/**
* @brief：更新超级电容电量数据
* @param [in]	message:接收到的数据帧指针
* @return：		true:更新成功
							false:更新失败
* @author：mqy
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
* @brief：获取超级电容当前电量百分比
* @param [in]	无
* @return：		0-100的一个整形数据，该值为剩余能量占总能的百分比
* @author：mqy
*/
rt_uint16_t access_electricity(void)
{
	return super_capacitor_electric;
}
