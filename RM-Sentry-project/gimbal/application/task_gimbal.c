#include "task_gimbal.h"
#include "drv_motor.h"
//#include "ctrl_strike.h"
#include "drv_canthread.h"
#include "drv_remote.h"
//#include "drv_computer.h"

Motor_t m_yaw={0};/*5901-185-6417-3090-4095*/ //ratio=0.285->1681-2387-4163-5550-5836*/
Motor_t m_pitch={0};
Motor_t m_launch={0}; //����6020

/*��ʱ����ʹ��*/
rt_uint8_t iffire = 0;														/*�����־*/

static void pid_1ms_calculate(void);
static void pid_5ms_calculate(void);
static void tx2_gimbal_ctrl(void);
static void tx2_send(void);
static void auto_move(void);
/**
 * @brief  101ms����ִ�к���
 */
void task_fun101ms(void)
{
	motor_rub_set(gun1.speed);/*Ħ���ֿ����ر�*/
	tx2_send();
}
/**
 * @brief  5ms����ִ�к���
 */
void task_fun5ms(void)
{
	strike_fire(&m_launch,&gun1,&iffire);/*���ִ�к���*/
	iffire=0;
	tx2_gimbal_ctrl();
	pid_5ms_calculate();
}
/**
 * @brief  1ms����ִ�к���
 */
void task_fun1ms(void)
{
	pid_1ms_calculate();
}


/**
* @brief  ����������̨����ָ��
* @param  rxmsg��can��Ϣ�ṹ��ָ��
 */
void slave_readmsg(struct rt_can_msg *rxmsg)
{
	/*��̨����*/
	if(rxmsg->id == STDID_SLAVE_GIMBAL)
	{
		m_pitch.ang.set += (rt_int16_t)(rt_uint16_t)((rt_uint16_t)(rxmsg->data[0])<<8 | rxmsg->data[1]);
		if(m_pitch.ang.set > PITCH_UP){m_pitch.ang.set = PITCH_UP;}
		else if(m_pitch.ang.set < PITCH_DOWN){m_pitch.ang.set = PITCH_DOWN;}
		
		m_yaw.ang.set += (rt_int16_t)((rt_uint16_t)(rxmsg->data[2])<<8 | rxmsg->data[3]);
		if(m_yaw.ang.set > YAW_UP){m_yaw.ang.set = YAW_UP;}
		else if(m_yaw.ang.set < YAW_DOWN){m_yaw.ang.set = YAW_DOWN;}
	}
	else if(rxmsg->id == STDID_SLAVE_STRIKE)
	{
		gun1.speed = (uint16_t)(rxmsg->data[0]<<8 || rxmsg->data[1]);
		iffire = rxmsg->data[2];
		gun1.mode = rxmsg->data[3];
	}
}
/*��ʱ����ʹ��*/
/**
 * @brief  С������̨����
 */
void tx2_gimbal_ctrl(void)
{
	/*��ʧĿ�����*/
	static rt_uint8_t watch_num=0;
	if(tx2_data.pitch==0&&tx2_data.yaw==0)
	{
		watch_num=watch_num>10?11:(watch_num+1);
	}
	else{watch_num=0;}
	if(watch_num>10){/*auto_move();*/}
	else
	{
		if(tx2_data.pitch!=0){
		m_pitch.ang.set = m_pitch.dji.angle+tx2_data.pitch*0.385f/3;}
		if(tx2_data.yaw!=0){
		m_yaw.ang.set = m_yaw.dji.angle+tx2_data.yaw*-0.385f;}
		tx2_data.yaw=0;
		tx2_data.pitch=0;
	}
}
/**
* @brief  ��tx2������̨����ָ��
 */
void tx2_send(void)
{
	rt_uint8_t txdata[8];
	txdata[0]=tx2_data.head;
	txdata[1]=0;
	txdata[2]=m_yaw.dji.angle>>8;txdata[3]=(rt_uint8_t)m_yaw.dji.angle;
	txdata[4]=m_pitch.dji.angle>>8;txdata[5]=(rt_uint8_t)m_pitch.dji.angle;
	txdata[6]=0;txdata[7]=tx2_data.tail;
	rt_device_write(tx2_serial, 0, txdata, 8);
}
/*���ض�*/
void auto_move(void)
{
	static rt_int8_t auto_move_pitch=10;
	static rt_int8_t auto_move_yaw=2;
	m_yaw.ang.set+=auto_move_yaw;
	if(m_yaw.ang.set>YAW_UP)
	{
		auto_move_yaw*=-1;
		m_yaw.ang.set=YAW_UP;
	}
	else if(m_yaw.ang.set<YAW_DOWN)
	{
		auto_move_yaw*=-1;
		m_yaw.ang.set=YAW_DOWN;
	}
	m_pitch.ang.set+=auto_move_pitch;
	if(m_pitch.ang.set>PITCH_UP)
	{
		auto_move_pitch*=-1;
		m_pitch.ang.set=PITCH_UP;
	}
	else if(m_pitch.ang.set<PITCH_DOWN)
	{
		auto_move_pitch*=-1;
		m_pitch.ang.set=PITCH_DOWN;
	}
}
/**
 * @brief  5ms��pid����
 */
static void pid_5ms_calculate(void)
{
	/*pidλ�û�����*/
	pid_out_check(&m_pitch.ang, m_pitch.ang.set, m_pitch.dji.angle);
	pid_out_check(&m_yaw.ang, m_yaw.ang.set, m_yaw.dji.angle);
	pid_out_check(&m_launch.ang, m_launch.ang.set, m_launch.dji.angle);
}
/**
 * @brief  1ms��pid����
 */
static void pid_1ms_calculate(void)
{
	/*pid�ٶȻ�����*/
	pid_out_nocheck(&m_pitch.spe, m_pitch.ang.out, m_pitch.dji.speed);
	pid_out_nocheck(&m_yaw.spe, m_yaw.ang.out, m_yaw.dji.speed);
	pid_out_nocheck(&m_launch.spe, m_launch.ang.out, m_launch.dji.speed);
	/*�����������*/
	motor_current_send(can2_dev, STDID_gimbal, m_yaw.spe.out, 0, 0, m_pitch.spe.out);
	motor_current_send(can2_dev, STDID_launch, m_launch.spe.out, 0 , 0, 0);
}
