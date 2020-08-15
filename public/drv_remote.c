#include "drv_remote.h"

RC_Ctrl_t RC_data;
RC_Ctrl_t RC_data_last;
rt_int16_t remote_s1_data = 0;
rt_int16_t remote_s2_data = 0;
rt_uint16_t temp_s_data = 0;

void RCReadKeyBoard_Data(RC_Ctrl_t *RC_CtrlData)
{
		RC_CtrlData->KeyBoard_Data.Key_Data.W = (RC_CtrlData->KeyBoard_Data.v&0x0001)==0x0001;
		RC_CtrlData->KeyBoard_Data.Key_Data.S = (RC_CtrlData->KeyBoard_Data.v&0x0002)==0x0002;
		RC_CtrlData->KeyBoard_Data.Key_Data.A = (RC_CtrlData->KeyBoard_Data.v&0x0004)==0x0004;
		RC_CtrlData->KeyBoard_Data.Key_Data.D = (RC_CtrlData->KeyBoard_Data.v&0x0008)==0x0008;
		RC_CtrlData->KeyBoard_Data.Key_Data.shift = (RC_CtrlData->KeyBoard_Data.v&0x0010)==0x0010;
		RC_CtrlData->KeyBoard_Data.Key_Data.ctrl = (RC_CtrlData->KeyBoard_Data.v&0x0020)==0x0020;
		RC_CtrlData->KeyBoard_Data.Key_Data.Q = (RC_CtrlData->KeyBoard_Data.v&0x0040)==0x0040;
		RC_CtrlData->KeyBoard_Data.Key_Data.E = (RC_CtrlData->KeyBoard_Data.v&0x0080)==0x0080;
		RC_CtrlData->KeyBoard_Data.Key_Data.R = (RC_CtrlData->KeyBoard_Data.v&0x0100)==0x0100;
		RC_CtrlData->KeyBoard_Data.Key_Data.F = (RC_CtrlData->KeyBoard_Data.v&0x0200)==0x0200;
		RC_CtrlData->KeyBoard_Data.Key_Data.G = (RC_CtrlData->KeyBoard_Data.v&0x0400)==0x0400;
		RC_CtrlData->KeyBoard_Data.Key_Data.Z = (RC_CtrlData->KeyBoard_Data.v&0x0800)==0x0800;
		RC_CtrlData->KeyBoard_Data.Key_Data.X = (RC_CtrlData->KeyBoard_Data.v&0x1000)==0x1000;
		RC_CtrlData->KeyBoard_Data.Key_Data.C = (RC_CtrlData->KeyBoard_Data.v&0x2000)==0x2000;
		RC_CtrlData->KeyBoard_Data.Key_Data.V = (RC_CtrlData->KeyBoard_Data.v&0x4000)==0x4000;
		RC_CtrlData->KeyBoard_Data.Key_Data.B = (RC_CtrlData->KeyBoard_Data.v&0x8000)==0x8000;
}
void RemoteDataProcess(uint8_t *pData, RC_Ctrl_t *RC_CtrlData)
{
	int i;
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
					RC_CtrlData->KeyBoard_Data.v=((int16_t)pData[14])|((int16_t)pData[15]<<8);
				} 
}
#define SAMPLE_UART_NAME       "uart1"      /* �����豸���� */

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
    if ( result == -RT_EFULL)
    {
        /* ��Ϣ������ */
        rt_kprintf("message queue full��\n");
    }
    return result;
}
uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;

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
			RemoteDataProcess(rx_buffer,&RC_data);
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

    /* ���Ҵ����豸 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* ��ʼ�����ò��� */

		/* step1�����Ҵ����豸 */
		serial = rt_device_find(SAMPLE_UART_NAME);

		/* step2���޸Ĵ������ò��� */
		config.baud_rate = 100000;        //�޸Ĳ�����Ϊ 9600
		config.data_bits = DATA_BITS_9;           //����λ 8
		config.stop_bits = STOP_BITS_1;           //ֹͣλ 1
		config.bufsz     = 128;                   //�޸Ļ����� buff size Ϊ 128
		config.parity    = PARITY_EVEN;           //

		/* step3�����ƴ����豸��ͨ�����ƽӿڴ�����������֣�����Ʋ��� */
		rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* ��ʼ����Ϣ���� */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* �����Ϣ�Ļ����� */
               sizeof(struct rx_msg),    /* һ����Ϣ����󳤶� */
               sizeof(msg_pool),         /* �����Ϣ�Ļ�������С */
               RT_IPC_FLAG_FIFO);        /* ����ж���̵߳ȴ������������ȵõ��ķ���������Ϣ */

    /* �� DMA ���ռ���ѯ���ͷ�ʽ�򿪴����豸 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    /* ���ý��ջص����� */
    rt_device_set_rx_indicate(serial, uart_input);
    /* �����ַ��� */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* ���� serial �߳� */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 1, 1);
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
	return no_action;
}
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
	return no_action;
}
rt_uint8_t Keys_state_read(rt_uint8_t *targetdata)
{
	rt_uint8_t length = (rt_uint8_t *)&RC_data - targetdata;
	//����������
	return 0 ;
}

