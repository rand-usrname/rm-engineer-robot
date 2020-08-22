#include "robocontrol.h"
/**
* @brief  向底盘发送命令以控制底盘
* @param  xspeed：x轴速度 yspeed：y轴速度
* @param  mode: 模式（不跟随0，跟随1）
* @param  angel_or_speed：跟随时的角度，不跟随时速度
*/
static int chassis_ctl(rt_int16_t xspeed, rt_int16_t yspeed, rt_uint8_t mode,rt_int16_t angel_or_speed)
{
    xspeed *= 3;
    yspeed *= 3;

    struct rt_can_msg txmsg;

    txmsg.id = CHASSIS_CTL;
    txmsg.ide = RT_CAN_STDID;
    txmsg.rtr=RT_CAN_DTR;
    txmsg.len=8;
    txmsg.data[0]=(rt_uint8_t)(xspeed>>8);
    txmsg.data[1]=(rt_uint8_t)(xspeed);
    txmsg.data[2]=(rt_uint8_t)(yspeed>>8);
    txmsg.data[3]=(rt_uint8_t)(yspeed);
    txmsg.data[4]=(rt_uint8_t)(angel_or_speed>>8);
    txmsg.data[5]=(rt_uint8_t)(angel_or_speed);
    txmsg.data[6]=(rt_uint8_t)(mode);
    txmsg.data[7]=0;
    rt_device_write(can1_dev, 0, &txmsg, sizeof(txmsg));
}
int gun1speed = 0;

int remote_ctrl(RC_Ctrl_t *remote)
{
    rt_int16_t pitchadd = 0;
    rt_int16_t yawadd = 0;
    //状态触发部分

    //S2状态触发
    if(remote->Remote_Data.s2 == 3)//右侧按键在中
    {
        pitchadd = (rt_int16_t)((remote->Remote_Data.ch1 - 1024)/30);
        yawadd = -(rt_int16_t)((remote->Remote_Data.ch0 - 1024)/30);
    }
    else if(remote->Remote_Data.s2 == 2)//右侧按键在下
    {
        return 1;//填充电脑控制函数
    }
    else if(remote->Remote_Data.s2 == 1)//右侧案件在上
    {
        if(get_yawusetime() < 1)
        {
            yawadd = -get_yaw_add()/16;
        }
        if(get_pitchusetime() < 1)
        {
            pitchadd = get_pitch_add()/16;
        }
    }

    //S1状态触发
    if(remote->Remote_Data.s1 == 3)//左侧按键在中
    {

    }
    else if(remote->Remote_Data.s1 == 2)//左侧按键在上
    {

    }
    else if(remote->Remote_Data.s1 == 1)//左侧按键在下
    {

    }

    //动作触发部分
    //S1动作
    switch_action_e s_action;//动作触发临时变量
    s_action = Change_from_middle(S1);
    if(s_action == middle_to_up)//向上拨动
    {
        strike_fire(&m_launch,&gun1,1);
    }
    else if(s_action == middle_to_down)//向下拨动
    {
        if(gun1speed  == 0)
        {
            gun1speed = -3000;
        }
        else if(gun1speed < -6000)
        {
            gun1speed = 0;
        }
        else
        {
            gun1speed -= 400;
        }
        Gun_speed_set(&gun1,gun1speed);
    }

    //S2动作
    s_action = Change_from_middle(S2);
    if(s_action == middle_to_up)//向上拨动
    {

    }
    else if(s_action == middle_to_down)//向下拨动
    {

    }

    gimbal_addangle_set(yawadd,pitchadd);
}
