#include "drv_canthread.h"
#include "robodata.h"
#include "drv_chassis.h"
#include "drv_motor.h"
#include "trailer.h"
static rt_uint8_t origin_l_init;
static rt_uint8_t origin_r_init;
static rt_uint16_t encoder_l;
static rt_uint16_t encoder_r;
static rt_uint8_t  encoderl_count;
static rt_uint8_t  encoderr_count;
void can1_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        //读取底盘数据
        case LEFT_FRONT:
        case RIGHT_FRONT:
        case LEFT_BACK:
        case RIGHT_BACK:
            refresh_chassis_motor_data(msg);
            return;
        //默认return
		
		case TRAILER_LEFT:
			motor_readmsg(msg,&trailerl.trailer_motor.dji);
			/* 标定初始位置 */
			if(origin_l_init==0&&trailerl.trailer_motor.dji.angle!=0)
			{origin_l_init=1;trailerl.trailer_motor.ang.set = trailerl.trailer_motor.dji.angle;origin_anglel = trailerl.trailer_motor.dji.angle;}
			/* 每次回到原点时计算偏差 */
			if((origin_l_init==1)&&((trailerl.state==0x88||trailerl.state==0x11)))
			{
				if(encoder_l==trailerl.trailer_motor.dji.angle){encoderl_count++;}
				if(encoderl_count > 100)
				{
					/* 计算偏差 */
					trailerl_offset = origin_anglel - encoder_l;
					/* 对不合理的偏差进行判断 */
					if(ABS(trailerl_offset) > 1000)
					{
						if(trailerl_offset>0){trailerl_offset -= 8192;}
						else {trailerl_offset += 8192;}
					}
					trailerl_offset = trailerl_offset/8192.0f*360;
					trailerl.state = rescue_unable;
					encoderl_count = 0;
				}
			}
			encoder_l = trailerl.trailer_motor.dji.angle;
			return;
		case TRAILER_RIGHT:
			motor_readmsg(msg,&trailerr.trailer_motor.dji);
			/* 标定初始位置 */
			if(origin_r_init==0&&trailerr.trailer_motor.dji.angle!=0)
			{origin_r_init=1;trailerr.trailer_motor.ang.set = trailerr.trailer_motor.dji.angle;origin_angler = trailerr.trailer_motor.dji.angle;}
			/* 每次回到原点时计算偏差 */
			if((origin_r_init==1)&&((trailerr.state==0x88||trailerr.state==0x11)))
			{
				if(encoder_r==trailerr.trailer_motor.dji.angle){encoderr_count++;}
				if(encoderr_count > 100)
				{
					/* 计算偏差 */
					trailerr_offset = origin_angler - encoder_r;
					/* 对不合理的偏差进行判断 */
					if(ABS(trailerr_offset) > 4096)
					{
						if(trailerr_offset>0){trailerr_offset -= 8192;}
						else {trailerr_offset += 8192;}
					}
					trailerr_offset = trailerr_offset/8192.0f*360;
					trailerr.state = rescue_unable;
					encoderr_count = 0;
				}
			}
			encoder_r = trailerr.trailer_motor.dji.angle;
			return;
        default:
            return;
    }
}

void can2_rec(struct rt_can_msg *msg)
{
    switch(msg->id)
    {
        case YAW_ID:
        case PITCH_ID:
            refresh_gimbal_motor_data(msg);
            return;
        case CHASSIS_CTL:
			read_gimbal_data(msg);
            return;
    }
}
