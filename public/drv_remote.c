#include "drv_remote.h"

//�����������ݵ��������ڽ���
static RC_Ctrl_t ctrl_data_container[2]={1024,1024,1024,1024,3,3,0};

//���ⲿ��ָ��
RC_Ctrl_t *RC_data;

//�ϴ�����ָ��
static RC_Ctrl_t *RC_data_last;

//��ȡs1 s2�Ƿ�ı� ����ȡ��ǰֵ
static rt_uint8_t remote_sx_data[2];
//��ȡ��������, ��16Ϊÿλ��ʱ�洢һ��������Ϣ����16λΪ����λ
static rt_uint32_t Key_Press_Data = 0;
//����ԭ��
//�ж�ĳ�����Ƿ�����ɿ���������ֻ���жϰ�����������ݣ��������������������δ֪�������
static rt_uint8_t _RCD_loosen(rt_uint8_t *targetdata,rt_uint8_t length);
//�ж�ĳ�����Ƿ�������¶�������ֻ���жϰ�����������ݣ��������������������δ֪�������
static rt_uint16_t _RCD_press(rt_uint8_t *targetdata,rt_uint8_t length);
//ʹ��ʱ��Ҫ�Ƚϵ����ݴ��뺯����if(RCD_change(RC_data->Key_Data.A)) �������жϼ�������A�Ƿ�ı�
static rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length);

//�����궨��
#define RCD_change(targetdata)	_RCD_change((rt_uint8_t*)(&targetdata),sizeof(targetdata))
#define RCD_press(targetdata)	_RCD_press((rt_uint8_t*)(&targetdata),sizeof(targetdata))	
#define RCD_loosen(targetdata)	_RCD_loosen((rt_uint8_t*)(&targetdata),sizeof(targetdata))
	
//���ݽ��㺯��
static int remote_data_process(rt_uint8_t *pData, RC_Ctrl_t *RC_CtrlData)
{
	//�ж�����ָ���Ƿ��ǿ�ָ��
	if (pData == NULL)
	{
		return 0;
	}

	//��ȡһ��������
	rt_uint8_t s1 = ((pData[5] >> 4) & 0x000c) >> 2;
	rt_uint8_t s2 = ((pData[5] >> 4) & 0x0003);
	rt_uint16_t ch0 = ((rt_int16_t)pData[0] | ((rt_int16_t)pData[1] << 8)) & 0x07FF;
	rt_uint16_t ch1 = (((rt_int16_t)pData[1] >> 3) | ((rt_int16_t)pData[2] << 5)) & 0x07FF;
	rt_uint16_t ch2 = (((rt_int16_t)pData[2] >> 6) | ((rt_int16_t)pData[3] << 2) | ((rt_int16_t)pData[4] << 10)) & 0x07FF;
	rt_uint16_t ch3 = (((rt_int16_t)pData[4] >> 1) | ((rt_int16_t)pData[5] << 7)) & 0x07FF;

	//�жϲ���������Ч��
	if (
		((s1 == 1) || (s1 == 2) || (s1 == 3)) &&
		((s2 == 1) || (s2 == 2) || (s2 == 3)) &&
		((pData[12] == 0) || (pData[12] == 1)) && //�ж�����������
		((pData[13] == 0) || (pData[13] == 1)) && //�ж�����Ҽ�����
		(ch0 < 1684) && (ch0 > 364) &&
		(ch1 < 1684) && (ch1 > 364) &&
		(ch2 < 1684) && (ch2 > 364) &&
		(ch3 < 1684) && (ch3 > 364))
	{
		//������õ����ݸ�ֵ
		RC_CtrlData->Remote_Data.ch0 = ch0;
		RC_CtrlData->Remote_Data.ch1 = ch1;
		RC_CtrlData->Remote_Data.ch2 = ch2;
		RC_CtrlData->Remote_Data.ch3 = ch3;
		RC_CtrlData->Remote_Data.s1 = s1;
		RC_CtrlData->Remote_Data.s2 = s2;

		//�����������
		RC_CtrlData->Mouse_Data.x_speed = ((rt_int16_t)pData[6]) | ((rt_int16_t)pData[7] << 8);
		RC_CtrlData->Mouse_Data.y_speed = ((rt_int16_t)pData[8]) | ((rt_int16_t)pData[9] << 8);
		RC_CtrlData->Mouse_Data.z_speed = ((rt_int16_t)pData[10]) | ((rt_int16_t)pData[11] << 8);
		RC_CtrlData->Mouse_Data.press_l = pData[12];
		RC_CtrlData->Mouse_Data.press_r = pData[13];

		//�����������
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

/* ���ڽ�����Ϣ�ṹ*/
struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};

/* �����豸��� */
static rt_device_t serial;

/* ��Ϣ���п��ƿ� */
static struct rt_messagequeue rx_mq;

/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	struct rx_msg msg;
	rt_err_t result;
	msg.dev = dev;
	msg.size = size;

	result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
	if (result == -RT_EFULL)
	{
		/* ��Ϣ������ */
		rt_kprintf("message queue full��\n");
	}
	return result;
}

static void serial_thread_entry(void *parameter)
{
	struct rx_msg msg;
	rt_err_t result;
	rt_uint32_t rx_length;
	rt_uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
	//��ʼ������ָ��
	RC_data = ctrl_data_container;
	RC_data_last = ctrl_data_container + 1;
	while (1)
	{
		rt_memset(&msg, 0, sizeof(msg));
		/* ����Ϣ�����ж�ȡ��Ϣ*/
		result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
		if (result == RT_EOK)
		{
			/* �Ӵ��ڶ�ȡ����*/
			rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
			rx_buffer[rx_length] = '\0';

			//�л�����ָ��
			RC_Ctrl_t *a = RC_data_last;//�л��õ���ʱ����
			RC_data_last = RC_data;
			RC_data = a;
			//�������ݵ�������������
			remote_data_process(rx_buffer, RC_data);
			
			//�ж�s1 s2�Ƿ��иı䲢����
			remote_sx_data[0] |= RCD_change(RC_data->Remote_Data.s1);
			remote_sx_data[1] |= RCD_change(RC_data->Remote_Data.s2);
			//��ȡ����������Ϣ
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

	/* step1�����Ҵ����豸 */
	serial = rt_device_find("uart1");

	/* ��ʼ�����ò��� */
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 

	/* step2���޸Ĵ������ò��� */
	config.baud_rate = 100000;		//�޸Ĳ�����Ϊ 100000
	config.data_bits = DATA_BITS_9; //����λ 9
	config.stop_bits = STOP_BITS_1; //ֹͣλ 1
	config.bufsz = 128;				//�޸Ļ����� buff size Ϊ 128
	config.parity = PARITY_EVEN;	//żУ��

	/* step3�����ƴ����豸��ͨ�����ƽӿڴ�����������֣�����Ʋ��� */
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

	/* ��ʼ����Ϣ���� */
	rt_mq_init(&rx_mq, "rx_mq",
			   msg_pool,			  /* �����Ϣ�Ļ����� */
			   sizeof(struct rx_msg), /* һ����Ϣ����󳤶� */
			   sizeof(msg_pool),	  /* �����Ϣ�Ļ�������С */
			   RT_IPC_FLAG_FIFO);	 /* ����ж���̵߳ȴ������������ȵõ��ķ���������Ϣ */

	/* �� DMA ���ռ���ѯ���ͷ�ʽ�򿪴����豸 */
	rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
	/* ���ý��ջص����� */
	rt_device_set_rx_indicate(serial, uart_input);

	/* ���� serial �߳� */
	rt_thread_t thread = rt_thread_create(
		"remotr_serial", 
		serial_thread_entry, 
		RT_NULL, 
		1024, 
		2, 
		1);

	/* �����ɹ��������߳� */
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
INIT_APP_EXPORT(remote_uart_init);
static rt_uint8_t _RCD_change(rt_uint8_t *targetdata,rt_uint8_t length)
{
	//�����Ӧ����ʷ���ݵ�ַ
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
	//�����Ӧ����ʷ���ݵ�ַ
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
	//�����Ӧ����ʷ���ݵ�ַ
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
