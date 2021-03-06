#include "drv_remote.h"
/* 存储遥控器当前数据 */
RC_Ctrl_t RC_data;
/*存储遥控器上次数据 */
static RC_Ctrl_t RC_data_last;
/* 存储s1 s2 的动作 */
static rt_int16_t remote_s1_data = 0;
static rt_int16_t remote_s2_data = 0;
/* 在从两边往中间拨时存储RC_last中拨杆s1 s2的值 */
/* 当拨杆从中间拨到两边时，可以读到RC_data的值来判断上下拨 */
/* 但当拨杆为两边往中间拨时，RC_data=RC_lastdata,所以读到动作的时候存一下 */
static rt_uint16_t temp_s_data = 0;
/* 按键有按下或松开动作时，置其中一位为1 */
static rt_uint16_t key_change = 0;
/* 鼠标左键有动作，高4位置一，右键低四位置一 */
static rt_uint8_t mouse_key_data = 0;

void RCReadKeyBoard_Data(RC_Ctrl_t *RC_CtrlData)
{
	RC_CtrlData->Key_Data.W = (RC_CtrlData->v&0x0001)==0x0001;
	RC_CtrlData->Key_Data.S = (RC_CtrlData->v&0x0002)==0x0002;
	RC_CtrlData->Key_Data.A = (RC_CtrlData->v&0x0004)==0x0004;
	RC_CtrlData->Key_Data.D = (RC_CtrlData->v&0x0008)==0x0008;
	RC_CtrlData->Key_Data.shift = (RC_CtrlData->v&0x0010)==0x0010;
	RC_CtrlData->Key_Data.ctrl = (RC_CtrlData->v&0x0020)==0x0020;
	RC_CtrlData->Key_Data.Q = (RC_CtrlData->v&0x0040)==0x0040;
	RC_CtrlData->Key_Data.E = (RC_CtrlData->v&0x0080)==0x0080;
	RC_CtrlData->Key_Data.R = (RC_CtrlData->v&0x0100)==0x0100;
	RC_CtrlData->Key_Data.F = (RC_CtrlData->v&0x0200)==0x0200;
	RC_CtrlData->Key_Data.G = (RC_CtrlData->v&0x0400)==0x0400;
	RC_CtrlData->Key_Data.Z = (RC_CtrlData->v&0x0800)==0x0800;
	RC_CtrlData->Key_Data.X = (RC_CtrlData->v&0x1000)==0x1000;
	RC_CtrlData->Key_Data.C = (RC_CtrlData->v&0x2000)==0x2000;
	RC_CtrlData->Key_Data.V = (RC_CtrlData->v&0x4000)==0x4000;
	RC_CtrlData->Key_Data.B = (RC_CtrlData->v&0x8000)==0x8000;
}
void RemoteDataProcess(uint8_t *pData, RC_Ctrl_t *RC_CtrlData)
{
<<<<<<< HEAD
	int i=0;
=======
	int i = 0;
>>>>>>> 2847d183ebe255e6e65b8f62f146679a52becd9d
	uint8_t s1 = 0;
	uint8_t s2 = 0;
	uint16_t ch0 = 0;
	uint16_t ch1 = 0;
	uint16_t ch2 = 0;
	uint16_t ch3 = 0;
    if(pData==NULL)
	{
	   return;
	}

	s1=((pData[5]>>4)&0x000c)>>2;
	s2=((pData[5]>>4)&0x0003);
	ch0=((int16_t)pData[0]|((int16_t)pData[1]<<8))&0x07FF;
	ch1=(((int16_t)pData[1]>>3)|((int16_t)pData[2]<<5))&0x07FF;
	ch2=(((int16_t)pData[2]>>6)|((int16_t)pData[3]<<2)|((int16_t)pData[4]<<10))&0x07FF;
	ch3=(((int16_t)pData[4]>>1)|((int16_t)pData[5]<<7))&0x07FF;					
	if(
		((s1==1) || (s1==2) || (s1==3)) && 
		((s2==1) || (s2==2) || (s2==3))&&
		((pData[i+12]==0)||(pData[i+12]==1))&&
		((pData[i+13]==0)||(pData[i+13]==1))&&
		(ch0<1684)&&(ch0>364)&&
		(ch1<1684)&&(ch1>364)&&
		(ch2<1684)&&(ch2>364)&&
		(ch3<1684)&&(ch3>364)
	  )																	
	 {
		 RC_CtrlData->Remote_Data.ch0=ch0;
		 RC_CtrlData->Remote_Data.ch1=ch1;
		 RC_CtrlData->Remote_Data.ch2=ch2;
		 RC_CtrlData->Remote_Data.ch3=ch3;
		 RC_CtrlData->Remote_Data.s1=s1;
		 RC_CtrlData->Remote_Data.s2=s2;
		 RC_CtrlData->Mouse_Data.x_speed=((int16_t)pData[6])|((int16_t)pData[7]<<8);
		 RC_CtrlData->Mouse_Data.y_speed=((int16_t)pData[8])|((int16_t)pData[9]<<8);
		 RC_CtrlData->Mouse_Data.z_speed=((int16_t)pData[10])|((int16_t)pData[11]<<8);
	   	 RC_CtrlData->Mouse_Data.press_l=pData[12];
		 RC_CtrlData->Mouse_Data.press_r=pData[13];
		 RC_CtrlData->v=((int16_t)pData[14])|((int16_t)pData[15]<<8);
	 } 
}
#define SAMPLE_UART_NAME       "uart1"      /* 串口设备名称 */

/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 串口设备句柄 */
static rt_device_t serial;
/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}
uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
	//初始化
	RC_data.Remote_Data.ch0 = 1024;RC_data.Remote_Data.ch1 = 1024;RC_data.Remote_Data.ch2 = 1024;RC_data.Remote_Data.ch3 = 1024;
	RC_data.Remote_Data.s1 = 3;RC_data.Remote_Data.s2 = 3;
	RC_data_last.Remote_Data.ch0 = 1024;RC_data_last.Remote_Data.ch1 = 1024;RC_data_last.Remote_Data.ch2 = 1024;RC_data_last.Remote_Data.ch3 = 1024;
	RC_data_last.Remote_Data.s1 = 3;RC_data_last.Remote_Data.s2 = 3;
    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            rx_buffer[rx_length] = '\0';
			
			RemoteDataProcess(rx_buffer,&RC_data);
			RCReadKeyBoard_Data(&RC_data);
			/* remote_sx_data = 1 中间往两边拨 =2 两边往中间拨 */
			if(RC_data_last.Remote_Data.s1==3&&RC_data.Remote_Data.s1!=3)
			{
				remote_s1_data = 1;
			}
			else if(RC_data_last.Remote_Data.s1!=3&&RC_data.Remote_Data.s1==3)
			{
				remote_s1_data = 2;
				temp_s_data = (RC_data_last.Remote_Data.s1<<8)|RC_data_last.Remote_Data.s2;
			}
			
			if(RC_data_last.Remote_Data.s2==3&&RC_data.Remote_Data.s2!=3)
			{
				remote_s2_data = 1;
			}
			else if(RC_data_last.Remote_Data.s2!=3&&RC_data.Remote_Data.s2==3)
			{
				remote_s2_data = 2;
				temp_s_data = (RC_data_last.Remote_Data.s1<<8)|RC_data_last.Remote_Data.s2;
			}
			
			mouse_key_data = ((RC_data_last.Mouse_Data.press_l==RC_data.Mouse_Data.press_l)<<1)|(RC_data_last.Mouse_Data.press_r==RC_data.Mouse_Data.press_r);
			if(RC_data_last.v != RC_data.v)
			{
				key_change |= ((RC_data_last.v)^(RC_data.v));
			}
			
			rt_memcpy(&RC_data_last,&RC_data,sizeof(RC_data));
        }
    }
}

int remote_uart_init(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    static char msg_pool[256];
    char str[] = "hello RT-Thread!\r\n";

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找串口设备 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

	/* step1：查找串口设备 */
	serial = rt_device_find(SAMPLE_UART_NAME);

	/* step2：修改串口配置参数 */
	config.baud_rate = 100000;                //修改波特率为 9600
	config.data_bits = DATA_BITS_9;           //数据位 8
	config.stop_bits = STOP_BITS_1;           //停止位 1
	config.bufsz     = 128;                   //修改缓冲区 buff size 为 128
	config.parity    = PARITY_EVEN;           //

	/* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 1, 1);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}

/**
  * @brief   读取拨杆动作(只能读取中间往两边拨的动作)
  * @param   传入S1则读取S1的动作，传入S2则读取S2的动作
  * @retval  返回往上拨(middle_to_up) 或 往下拨(middle_to_down)
 **/
switch_action_e Change_from_middle(switch_action_e sx)
{
	if(sx==S1)
	{
		if(remote_s1_data == 1)
		{
			if(RC_data.Remote_Data.s1==1)
		    {remote_s1_data = 0;return middle_to_up;}
		    else if(RC_data.Remote_Data.s1==2)
		    {remote_s1_data = 0;return middle_to_down;}
		}
	}
	else if(sx==S2)
	{
		if(remote_s2_data == 1)
		{
			if(RC_data.Remote_Data.s2==1)
		    {remote_s2_data = 0;return middle_to_up;}
		    else if(RC_data.Remote_Data.s2==2)
		    {remote_s2_data = 0;return middle_to_down;}
		}
	}
	return NO_ACTION;
}
/**
  * @brief   读取拨杆动作(只能读取两边往中间拨的动作)
  * @param   传入S1则读取S1的动作，传入S2则读取S2的动作
  * @retval  返回从上往中间拨(up_to_middle) 或 从下往中间拨(down_to_middle)
 **/
switch_action_e Change_to_middle(switch_action_e sx)
{
	if(sx==S1)
	{
		if(remote_s1_data == 2)
		{
			if((rt_uint8_t)(temp_s_data>>8)==1)
		    {remote_s1_data = 0;temp_s_data&=0x1100;return up_to_middle;}
		    else if((rt_uint8_t)(temp_s_data>>8)==2)
		    {remote_s1_data = 0;temp_s_data&=0x1100;return down_to_middle;}
		}
	}
	else if(sx==S2)
	{
		if(remote_s2_data == 2)
		{
			if((rt_uint8_t)temp_s_data==1)
		    {remote_s2_data = 0;temp_s_data&=0x0011;return up_to_middle;}
		    else if((rt_uint8_t)temp_s_data==2)
		    {remote_s2_data = 0;temp_s_data&=0x0011;return down_to_middle;}
		}
	}
	return NO_ACTION;
}
/**
  * @brief  读取按键动作(仅键盘数据)
  * @param  传入&RC_data.Key_Data.x x为你想查询的按键 例:&RC_data.Key_Data.shift
  * @retval 返回PRESS_ACTION(按下) or LOOSEN_ACTION(松开) or NO_ACTION(无动作)
 **/
switch_action_e Key_action_read(rt_uint8_t *targetdata)
{
	/* 计算字节偏移量 */
	rt_uint8_t length = targetdata - (rt_uint8_t *)&RC_data.Key_Data;
	/* 查询是否该按键对应的位是否有改变 */
	if((key_change&(0x0001<<length)))
	{
	    rt_uint8_t *temp_now = (rt_uint8_t*)&RC_data.Key_Data;
	    if(*(temp_now+length)==1)
	    {
			/* 将该位置零 */
			key_change &= (~(0x0001<<length));
			return PRESS_ACTION;
		}
	    else if(*(temp_now+length)==0)
	    {
			/* 将该位置零 */
			key_change &= (~(1<<length));
			return LOOSEN_ACTION;
		}
	}
	else
	{return NO_ACTION;}
}
/**
  * @brief  读取鼠标按键动作
  * @param  mouse_x = MOUSE_LEFT(鼠标左键) or MOUSE_RIGHT(鼠标右键)
  * @retval 返回PRESS_ACTION(按下) or LOOSEN_ACTION(松开) or NO_ACTION(无动作)
 **/
switch_action_e Mouse_action_read(switch_action_e mouse_x)
{
	if(mouse_x == MOUSE_LEFT)
	{
		if(mouse_key_data&0x02)
		{
			if(RC_data.Mouse_Data.press_l == 1)
			{
				mouse_key_data&=0x01;
				return PRESS_ACTION;
			}
			else
			{
				mouse_key_data&=0x01;
				return LOOSEN_ACTION;
			}
		}
		else
		{return NO_ACTION;}
	}
	else if(mouse_x == MOUSE_RIGHT)
	{
		if(mouse_key_data&0x01)
		{
			if(RC_data.Mouse_Data.press_r == 1)
			{
				mouse_key_data&=0x02;
				return PRESS_ACTION;
			}
			else
			{
				mouse_key_data&=0x02;
				return LOOSEN_ACTION;
			}
		}
		else
		{return NO_ACTION;}
	}
	return NO_ACTION;
}
