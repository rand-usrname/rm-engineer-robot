#include "drv_aimbot.h"
#include "math.h"

#define G   9.7988f 
#define t_transmit 3
#define pi_operator 0.01745f
//�ӿ�
Aimbot_t *Aimbot_data;

//�Ӿ����͵�Ŀ������
static Point_t *vision_point;

//����һ��������� �����ڴ洢��40ms�ڵĽ��ٶȡ�
static float Palstance_Deque[40] = {2.0f,0};

//���ٶȵĻ�ȡ�ӿ���ʱδȷ�� Ŀǰ�ٶ���֪
static float current_yaw_palstance = 1.0f;
static float current_roll_palstance = 1.0f;
static float vision_palstance = 1.0f;
//��Ҫ ��ȡ�ӵ��ٶ�   ��ǰpitch yaw�Ƕ�
static float bullet_speed = 0.0f;
static float pitch_angle = 30.0f;
static float yaw_angle = 10.0f;
//�Ӿ�����ͼ��ʱ��
static rt_uint8_t t_camera = 14;//��λms��
//���ٿ���
static float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
 
    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       
    i  = 0x5f3759df - ( i >> 1 );               
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );  
    return y;
}
static rt_uint8_t Isgreater_than_zero(float data)
{
	if(data >= 0)
		return 1;
	else
		return -1;
}


//�������
static void get_new_palstance(float *pdata,float yaw_palstance, float roll_palstance, float current_pitch)
{
	float *temp = pdata;
	float motor_palstance =  Q_rsqrt(yaw_palstance*yaw_palstance+roll_palstance*roll_palstance);
	//�жϷ���
	if((-45 < current_pitch)&& (current_pitch<45))
	{motor_palstance*= Isgreater_than_zero(yaw_palstance);}
	else if((current_pitch>=45) && (current_pitch<135))
	{motor_palstance*= Isgreater_than_zero(roll_palstance);}
    Palstance_Deque[0] = motor_palstance;
	rt_memcpy((Palstance_Deque+1),temp,39);
}
//����ϵת��������ͷ����ϵתΪ�������ϵ ��ԭ��Ϊ�������Ħ����λ�ô�
static Point_t Transformed_coordinate(Point_t* vision_point,float pitch_angle)
{
	Point_t motor_point;
	motor_point.x = vision_point->x;
    motor_point.y = -(vision_point->z) * sin(pitch_angle) + vision_point->y * cos(pitch_angle);
    motor_point.z = vision_point->z * cos(pitch_angle) + vision_point->y * sin(pitch_angle);

	return motor_point;
}
//ͨ���������δ�Ӳ���ʱ�Ħ�pitch,��yaw Ϊʹ�÷��� ������Aimbot_t���͵Ľṹ���ݴ�
static Aimbot_t Calc_from_point(Point_t *motor_point)
{
	Aimbot_t calc_angle;
	calc_angle.pitch = atan((motor_point->y)/(motor_point->z))/pi_operator;
	calc_angle.yaw = atan((motor_point->x)/(motor_point->z))/pi_operator;
	return calc_angle;
}

//���������� ���뵱ǰpitch ��pitch,�ӵ��ٶȣ���Ŀ����룬 ���ز������pitch;
static float Gravity_compensation(float current_pitch, float delta_pitch,float distance,rt_uint16_t bullet_speed)
{
	float target_pitch = (delta_pitch + current_pitch)*pi_operator;
	float x_0 = distance * cos(-target_pitch);
	float y_0 = distance * sin(-target_pitch);
	float temp = (bullet_speed*bullet_speed*bullet_speed*bullet_speed)-G*(G*x_0*x_0-2*bullet_speed*bullet_speed*y_0);
	if(temp < 0) return delta_pitch;
	else
	{
		float aim_pitch = -atan((-bullet_speed*bullet_speed+Q_rsqrt(temp))/(G*x_0));
		return aim_pitch/pi_operator;
	}
}
/*
     ��ǰ�� ��ȡyaw����ǰ�ǲ���
    1�� ��ȡĿ����ٶȣ��Ӿ����ص�Ŀ����ٶȣ���̨���Խ��ٶȣ���̬ʱ��t_lag
    2�� ��ȡ��ǰ�ǣ��ӵ�����ʱ�䣺���� �ӵ��ٶ�; Ŀ����ٶȣ�
*/
static float Advance_yaw_angle(float vision_palstance, rt_uint8_t t_lag,
							   float distance,rt_uint16_t bullet_speed)
{
	/****************************1****************************/
	//�õ���̬ʱ��ǰ�Ľ��ٶ�
	float Gimbal_palstance_lag = *(Palstance_Deque+t_lag);
	float aim_palstance = Gimbal_palstance_lag+vision_palstance;
	/****************************2****************************/
	return ((aim_palstance*distance/bullet_speed)/pi_operator + yaw_angle);
}

/******************************************************************************************************************/
rt_thread_t aim_thread;
static struct rt_semaphore Obtain_palstance_sem;

static struct rt_semaphore Aim_bot_sem;

static struct rt_timer Obtain_palstance_task;

static void Obtain_palstance_IRQHandler(void *parameter)
{
	rt_sem_release(&Obtain_palstance_sem);
}

static void Obtain_palstance_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&Obtain_palstance_sem, RT_WAITING_FOREVER);
		get_new_palstance(Palstance_Deque,current_yaw_palstance,current_roll_palstance,pitch_angle);
	}
} 

static void aim_bot_emtry(void *parameter)
{
	while(1)
	{
		rt_sem_take(&Aim_bot_sem, RT_WAITING_FOREVER);
		//����ת��
		Point_t temp_point = Transformed_coordinate(vision_point,pitch_angle);
		//�����ʼ��pitch ��yaw
		Aimbot_t temp_aim_data = Calc_from_point(&temp_point);
		//��ȡ���룬�ӵ�ˮƽ�ٶ�
		float temp_dis = (temp_point.z)*(temp_point.z)+(temp_point.x)*(temp_point.x);
		float distance = Q_rsqrt(temp_dis);
		float bullet_speed_horizontal = bullet_speed*cos(pitch_angle);
		//��������
		Aimbot_data->pitch = Gravity_compensation(pitch_angle,temp_aim_data.pitch,distance,bullet_speed_horizontal);
		//��ǰ��Ԥ��
		Aimbot_data->yaw = Advance_yaw_angle(vision_palstance,t_camera+t_transmit,distance,bullet_speed_horizontal);
		
	}
	
}
void aim_bot_creat(void)
{
	/*��ʱ�������߳�*/
	rt_thread_t thread;
	rt_sem_init(&Obtain_palstance_sem, "Obtain_palstance_sem", 0, RT_IPC_FLAG_FIFO);
	thread = rt_thread_create("Obtain_palstance_thread", Obtain_palstance_emtry, RT_NULL, 1024, 3, 1);
	if (thread != RT_NULL)
	{
			rt_thread_startup(thread);
	}
	/*��ʱ���ж�*/
	rt_timer_init(&Obtain_palstance_task,
                   "Obtain_palstance_task",
                   Obtain_palstance_IRQHandler,
                   RT_NULL,
                   1, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
	 /* ������ʱ�� */
	rt_timer_start(&Obtain_palstance_task);
	
	//��ʼ��һ���߳� �������Ӿ�����һ֡����ʱ��������
	//�Ƿ���Ҫδ�������ʱ������߳� �� ��������Żָ����̣߳�
	rt_sem_init(&Aim_bot_sem, "Aim_bot_sem", 0, RT_IPC_FLAG_FIFO);
	aim_thread = rt_thread_create("Aim_bot_sem", aim_bot_emtry, RT_NULL, 1024, 2, 1);
	if (aim_thread != RT_NULL)
	{
			rt_thread_startup(aim_thread);
	}
	rt_thread_suspend(aim_thread);
}
INIT_APP_EXPORT(aim_bot_creat);
