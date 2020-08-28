#ifndef __DRV_STRIKE_H__
#define __DRV_STRIKE_H__
#include <rtthread.h>
#include "drv_motor.h"
#include "drv_refsystem.h"
#include "drv_remote.h"
#include "drv_canthread.h"
#include "robodata.h"

#define LOCAL_HEAT_ENABLE		0					            /*本地热量使能*/



#define HEAT_PERIOD		        100								/*热量检测周期，单位ms*/
/*发弹模式*/
#define STRICK_NOLIMITE			0x01							/*无限制开火*/
#define	STRICK_SINGLE           0x02							/*点射*/
#define	STRICK_TRIPLE           0x04							/*三连发*/

#define	STRICK_LOWSPEED 		0x08							/*低射速*/
#define	STRICK_MIDDLESPEED	    0x10							/*中射速*/
#define	STRICK_HIGHTSPEED       0x20							/*高射速*/
/*机构状态*/
#define STRICK_STUCK		    0x01							/*卡弹*/
#define STRICK_STOP				0x02							/*禁止发射*/
/*热量控制结构体*/
typedef struct _Heatctrl_t
{
	rt_int32_t 					now;							/*现在热量*/
	rt_uint32_t 				max;							/*热量上限*/
	rt_int8_t 					rate;							/*当前剩余热量百分比*/
	rt_uint8_t                  unlimit_heat;                   /* 无视热量发弹 */
	struct _Heatctrl_t *next;							        /*下个热量控制块*/
	rt_uint8_t 				cool;							    /*枪口冷却值*/
}Heatctrl_t;


/*发射机构结构体*/
typedef struct
{
	rt_int16_t    speed;										    /*摩擦轮速度*/
	rt_uint8_t    mode;											/*射击模式，如点射，3弹连发（之后需要改成枚举）*/
	rt_uint8_t	  status;										/*状态，如速度环卡弹，位置卡弹*/
	Heatctrl_t 	  heat;										/*枪口热量控制块*/
}Strike_t;
extern Motor_t m_rub[2];
extern Motor_t m_launch;
extern Strike_t gun1;

/*热量控制函数，由用户根据实际编写*/
void heat_control(Heatctrl_t *p_temp);
/*热量参数设置*/
void heatctrl_init(Heatctrl_t *heat, rt_uint32_t max);
/*开始热量控制*/
void heatctrl_start(void);

void motor_servo_set(uint16_t duty);

void Gun_speed_set(Strike_t *strike, rt_int16_t speed);

void motor_rub_set(uint16_t duty);

void Gun_mode_set(Strike_t *strike, rt_base_t mode);
/*发射机构初始化*/
void strike_init(Strike_t *gun, rt_uint32_t max);

__weak void strike_pid_init(void);

/*卡弹判定*/
void strike_stuck(Motor_t *motor, Strike_t *gun);
/*对于多枪管，需要重新再定义一个相同作用函数，待修改*/
/*设置模式并开火*/
void strike_fire(Motor_t *motor, Strike_t *gun, rt_uint8_t if_fire);
#endif
