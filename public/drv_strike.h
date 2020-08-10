#ifndef __DRV_STRIKE_H__
#define __DRV_STRIKE_H__
#include <rtthread.h>
#include "drv_motor.h"
#include "drv_refsystem.h"
#include "drv_remote.h"
#include "drv_canthread.h"

#define HEAT_PERIOD											100				/*热量控制周期 100ms*/
/*发弹模式*/
#define STRICK_NOLIMITE	    0x01								        /*全自动*/
#define	STRICK_SINGLE       0x02										/*单发*/
#define	STRICK_TRIPLE       0x04										/*三连发*/

#define	STRICK_LOWSPEED 	0x08										/*低速*/
#define	STRICK_MIDDLESPEED	0x10										/*中速*/
#define	STRICK_HIGHTSPEED   0x20										/*高速*/

#define STRICK_STUCK				0x01								/*卡弹*/
#define STRICK_STOP					0x02								/*停止发弹*/


/*热量模块*/
typedef struct _Heatctrl_t
{
	rt_int32_t 					now;							/*当前热量*/
	rt_uint32_t 				max;							/*热量上限*/
	rt_uint8_t 					rate;							/*剩余热量百分比*/
}Heatctrl_t;


/*发射模块*/
typedef struct
{
	rt_uint16_t   speed;										/*发弹速度 建议m/s*/
	rt_uint8_t    mode;											/*发弹模式*/
	rt_uint8_t	  status;										/*发弹状态*/
	Heatctrl_t 	  heat;											/*热量模块*/
}Strike_t;
void motor_servo_set(uint16_t duty);
void Gun_mode_set(Strike_t *strike,rt_base_t mode);
static void Gun_speed_set(Refdata_t *Refdata,Strike_t *strike,rt_base_t mode);
#endif
