#include "drv_remote.h"

//两个储存数据的容器用于交替
static RC_Ctrl_t ctrl_data_container[2]={1024,1024,1024,1024,3,3,0};

//对外部的指针
RC_Ctrl_t *RC_data;

//上次数据指针
static RC_Ctrl_t *RC_data_last;

//获取s1 s2是否改变 并获取当前值
static rt_uint8_t remote_sx_data[2];
//获取按键按下, 低16为每位暂时存储一个按键信息，高16位为保留位
static rt_uint32_t Key_Press_Data = 0;
//函数原型
//判断某个键是否产生松开动作（！只能判断按键和鼠标数据，若传入其他参数会产生未知结果！）
static rt_uint8_t _RCD_loosen(rt_uint8_t *targetdata,rt_uint8_t length);
//判断某个键是否产生按下动作（！只能判断按键和鼠标数据，若传入其他参数会产生未知结果！）
static rt_uint16_t _RCD_press(rt_uint8_t *targetdata,rt_uint8_t length);
//使用时将要比较的数据传入函数，if(RCD_change(RC_data->Key_Data.A)) ，即可判断键盘数据A是否改变
static rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length);

//函数宏定义
#define RCD_change(targetdata)	_RCD_change((rt_uint8_t*)(&targetdata),sizeof(targetdata))
#define RCD_press(targetdata)	_RCD_press((rt_uint8_t*)(&targetdata),sizeof(targetdata))	
#define RCD_loosen(targetdata)	_RCD_loosen((rt_uint8_t*)(&targetdata),sizeof(targetdata))
	
//数据解算函数
static int remote_data_process(rt_uint8_t *pData, RC_Ctrl_t *RC_CtrlData)
{
	//判断数据指针是否是空指针
	if (pData == NULL)
	{
		return 0;
	}

	//提取一部分数据
	rt_uint8_t s1 = ((pData[5] >> 4) & 0x000c) >> 2;
	rt_uint8_t s2 = ((pData[5] >> 4) & 0x0003);
	rt_uint16_t ch0 = ((rt_int16_t)pData[0] | ((rt_int16_t)pData[1] << 8)) & 0x07FF;
	rt_uint16_t ch1 = (((rt_int16_t)pData[1] >> 3) | ((rt_int16_t)pData[2] << 5)) & 0x07FF;
	rt_uint16_t ch2 = (((rt_int16_t)pData[2] >> 6) | ((rt_int16_t)pData[3] << 2) | ((rt_int16_t)pData[4] << 10)) & 0x07FF;
	rt_uint16_t ch3 = (((rt_int16_t)pData[4] >> 1) | ((rt_int16_t)pData[5] << 7)) & 0x07FF;

	//判断部分数据有效性
	if (
		((s1 == 1) || (s1 == 2) || (s1 == 3)) &&
		((s2 == 1) || (s2 == 2) || (s2 == 3)) &&
		((pData[12] == 0) || (pData[12] == 1)) && //判断鼠标左键数据
		((pData[13] == 0) || (pData[13] == 1)) && //判断鼠标右键数据
		(ch0 < 1684) && (ch0 > 364) &&
		(ch1 < 1684) && (ch1 > 364) &&
		(ch2 < 1684) && (ch2 > 364) &&
		(ch3 < 1684) && (ch3 > 364))
	{
		//将解算好的数据赋值
		RC_CtrlData->Remote_Data.ch0 = ch0;
		RC_CtrlData->Remote_Data.ch1 = ch1;
		RC_CtrlData->Remote_Data.ch2 = ch2;
		RC_CtrlData->Remote_Data.ch3 = ch3;
		RC_CtrlData->Remote_Data.s1 = s1;
		RC_CtrlData->Remote_Data.s2 = s2;

		//解算鼠标数据
		RC_CtrlData->Mouse_Data.x_speed = ((rt_int16_t)pData[6]) | ((rt_int16_t)pData[7] << 8);
		RC_CtrlData->Mouse_Data.y_speed = ((rt_int16_t)pData[8]) | ((rt_int16_t)pData[9] << 8);
		RC_CtrlData->Mouse_Data.z_speed = ((rt_int16_t)pData[10]) | ((rt_int16_t)pData[11] << 8);
		RC_CtrlData->Mouse_Data.press_l = pData[12];
		RC_CtrlData->Mouse_Data.press_r = pData[13];

		//解算键盘数据
		rt_uint16_t keydata = ((rt_uint16_t)pData[14]) | ((rt_uint16_t)pData[15] << 8);
		RC_CtrlData->Key_Data.W = (keydata & 0x0001) == 0x0001;
		RC_CtrlData->Key_Data.S = (keydata & 0x0002) == 0x0002;
		RC_CtrlData->Key_Data.A = (keydata & 0x0004) == 0x0004;
		RC_CtrlData->Key_Data.D = (keydata & 0x0008) == 0x0008;
		RC_CtrlData->Key_Data.shift = (keydata & 0x0010) == 0x0010;
		RC_CtrlData->Key_Data.ctrl = (keydata & 0x0020) == 0x0020;
		RC_CtrlData->Key_Data.Q = (keydata & 0x0040) == 0x0040;
		RC_CtrlData->Key_Data.E = (keydata & 0x0080) == 0x0080;
		RC_CtrlData->Key_Data.R = (keydata & 0x0100) == 0x0100;
		RC_CtrlData->Key_Data.F = (keydata & 0x0200) == 0x0200;
		RC_CtrlData->Key_Data.G = (keydata & 0x0400) == 0x0400;
		RC_CtrlData->Key_Data.Z = (keydata & 0x0800) == 0x0800;
		RC_CtrlData->Key_Data.X = (keydata & 0x1000) == 0x1000;
		RC_CtrlData->Key_Data.C = (keydata & 0x2000) == 0x2000;
		RC_CtrlData->Key_Data.V = (keydata & 0x4000) == 0x4000;
		RC_CtrlData->Key_Data.B = (keydata & 0x8000) == 0x8000;
	}
	return 0;
}

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
	if (result == -RT_EFULL)
	{
		/* 消息队列满 */
		rt_kprintf("message queue full！\n");
	}
	return result;
}

static void serial_thread_entry(void *parameter)
{
	struct rx_msg msg;
	rt_err_t result;
	rt_uint32_t rx_length;
	rt_uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
	//初始化数据指针
	RC_data = ctrl_data_container;
	RC_data_last = ctrl_data_container + 1;
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

			//切换数据指针
			RC_Ctrl_t *a = RC_data_last;//切换用的临时数据
			RC_data_last = RC_data;
			RC_data = a;
			//解算数据到本次数据容器
			remote_data_process(rx_buffer, RC_data);
			
			//判断s1 s2是否有改变并保存
			remote_sx_data[0] |= RCD_change(RC_data->Remote_Data.s1);
			remote_sx_data[1] |= RCD_change(RC_data->Remote_Data.s2);
			//获取按键按下信息
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.W));
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.S) << 1);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.A) << 2);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.D) << 3);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.Q) << 4);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.E) << 5);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.shift) << 6);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.ctrl) << 7);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.R) << 8);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.F) << 9);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.G) << 10);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.Z) << 11);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.X) << 12);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.C) << 13);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.V) << 14);
			Key_Press_Data |= (RCD_press(RC_data->Key_Data.B) << 15);
		}
	}
}

int remote_uart_init(void)
{
	rt_err_t ret = RT_EOK;
	static char msg_pool[256];

	//step1：查找串口设备
	serial = rt_device_find("uart1");

	//初始化配置参数
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 

	//step2：修改串口配置参数
	config.baud_rate = 100000;		//修改波特率为 100000
	config.data_bits = DATA_BITS_9; //数据位 9
	config.stop_bits = STOP_BITS_1; //停止位 1
	config.bufsz = 128;				//修改缓冲区 buff size 为 128
	config.parity = PARITY_EVEN;	//偶校验

	//step3：控制串口设备。通过控制接口传入命令控制字，与控制参数
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

	//初始化消息队列
	rt_mq_init(&rx_mq, "rx_mq",
			   msg_pool,			  //存放消息的缓冲区
			   sizeof(struct rx_msg), //一条消息的最大长度
			   sizeof(msg_pool),	  //存放消息的缓冲区大小
			   RT_IPC_FLAG_FIFO);	 //如果有多个线程等待，按照先来先得到的方法分配消息

	//以 DMA 接收及轮询发送方式打开串口设备
	rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
	//设置接收回调函数
	rt_device_set_rx_indicate(serial, uart_input);

	//创建 serial 线程
	rt_thread_t thread = rt_thread_create(
		"remotr_serial", 
		serial_thread_entry, 
		RT_NULL, 
		1024, 
		2, 
		1);

	//创建成功则启动线程
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
static rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length)
{
	//算出对应的历史数据地址
	rt_uint8_t *targetdatalast = (rt_uint8_t*)RC_data_last + ((rt_uint32_t)targetdata - (rt_uint32_t)RC_data);
	while(length)
	{
		if (*targetdata != *targetdatalast) {
			return 1;
		}
		length--;
		targetdata++;
		targetdatalast++;
	}
	return 0;
}
static rt_uint16_t _RCD_press(rt_uint8_t *targetdata,rt_uint8_t length)
{
	//算出对应的历史数据地址
	rt_uint8_t *targetdatalast = (rt_uint8_t*)RC_data_last + ((rt_uint32_t)targetdata - (rt_uint32_t)RC_data);
	if ((*targetdata == 1)&&(*targetdatalast == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
static rt_uint8_t _RCD_loosen(rt_uint8_t *targetdata,rt_uint8_t length)
{
	//算出对应的历史数据地址
	rt_uint8_t *targetdatalast = (rt_uint8_t*)RC_data_last + ((rt_uint32_t)targetdata - (rt_uint32_t)RC_data);
	if ((*targetdata == 0)&&(*targetdatalast == 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
rt_uint8_t Obtain_Sx_Data(rt_uint8_t num)
{
	rt_uint8_t temp;
	temp = num == 1 ? *remote_sx_data : *(remote_sx_data+1);
	if(num==1){remote_sx_data[0]=0;}else{remote_sx_data[1]=0;}
	return temp;
}
rt_uint8_t Obtain_Key_Press(rt_uint8_t *targetdata)
{
	rt_uint8_t temp =  (Key_Press_Data >> (((rt_uint32_t)targetdata - (rt_uint32_t)RC_data) -18)) & 0x0001;
	Key_Press_Data &= ~(1 << (((rt_uint32_t)targetdata - (rt_uint32_t)RC_data) -18));
	return temp;
}
