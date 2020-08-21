#define length 10
#include <stdio.h>
double least_square(double err)
{int i;
double k;
double x_mean = 5.5;
double x_mean_pow = 30.25;
double x_pow_mean = 38.5;
static double y[length];
double xy_mean=0;
double y_mean = 0;
int x[length] = {1,2,3,4,5,6,7,8,9,10};
for(i=0;i<length-1;i++)
{
    y[i] = y[i+1];
    xy_mean += x[i]*y[i];
    y_mean += y[i];
}
y[length-1] = err;
xy_mean += x[length-1]*y[length-1];
y_mean += y[length-1];
xy_mean/=10;
y_mean/=10;
k = (xy_mean-x_mean*y_mean)/(x_pow_mean-x_mean_pow);
return k;
};
int main ()
{
	int i;
	double a;
    
    for (i = 0;i<100;i++){
		a = least_square(i);
        
		printf("%f\n",a);

    }
}
void example_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    if(kStatus_LPUART_RxIdle == status)
    {
				static  short i;
        //�����Ѿ���д�뵽�� ֮ǰ���õ�BUFF��
        //������ʹ�õ�BUFFΪ example_rx_buffer
        if('!' == example_rx_buffer) flag_Brake = 1;//������ȡ�� ɲ��
        else if('@' == example_rx_buffer) flag_Brake = 0;//����
			  else if( ('#' == example_rx_buffer)&&(1 == flag_Brake)) flag_Brake = 2;//�յ������Ҵ���ɲ��״̬
				else if( ('$' == example_rx_buffer)&&(2 == flag_Brake)) {flag_Brake = 3;i=0;}
				else if( 2 ==flag_Brake ) {Q.rear->buffer[i]=example_rx_buffer; i++;}
			  
			  else if( 3 == flag_Brake)
					{
						Q.rear->data = (int)example_rx_buffer;
						EnQueue(&Q);
						modify_param(Q);
						 
						flag_Brake = 1;
					}
						
			
			
    }
	    handle->rxDataSize = example_receivexfer.dataSize;  //��ԭ����������
    handle->rxData = example_receivexfer.data;          //��ԭ��������ַ
}
/*****************************************
�������
******************************************/
#include <stdio.h>
#include "headfile.h"


LinkQueue Q;
 
//����һ���ն���
int InitQueue(LinkQueue *Q)
{
    Q->front=Q->rear=(QNode*)calloc(1,sizeof(QNode));
    if(!Q->front)
        return 0;
    Q->front->next=NULL;
    return 1;
}


 
//���ٶ���
int DestoryQueue(LinkQueue *Q)
{
    while(Q->front)
    {
        Q->rear=Q->front->next;
        free(Q->front);
        Q->front=Q->rear;
    }
    return 1;
}

//�����µĽڵ�
int EnQueue(LinkQueue *Q)
{
	QNode *p;
  p=(QNode*)calloc(1,sizeof(QNode));
  p->next = NULL;
	Q->rear->next = p->next;
	Q->front->next = p;
  Q->rear=p;
}

 
//���� ��ȡ����Ԫ�ظ�ֵ��e
int DeQueue(LinkQueue *Q,int *e)
{
    QNode *p;
    if(Q->front == Q->rear)
        return 0;
    p=Q->front->next;
    *e=p->data;
    Q->front->next=p->next;
    if(Q->rear == p)
        Q->rear=Q->front;
    free(p);
    return 1;
}
 
//��ӡ����
int PrintQueue(QNode *Q)
{
    LinkQueue p;
    p.front = Q;
    while(p.front != p.rear)
    {
        printf("%d\n",Q->data);
        Q = Q->next;
    }
 
}
void QueueTraverse(LinkQueue Q)
 {
    QNode *p=Q.front->next;
    while(p)
    {
        printf("%d\n",p->data);
        p=p->next;
    }
    printf("\n");
 } 
int modify_param(LinkQueue Q)
{
	for(int i=0;i<sizeof(param)/sizeof(param[0]);i++)
		{
			char str[sizeof(param[i].buffer)/sizeof(char)];
			sprintf(str,Q.front->buffer);
			if (0 == strcmp(param[i].buffer , str))
				{
					*(double*)(param[i].data) = Q.front->data;
					QNode *p;
					p = Q.front;
					Q.front = Q.front->next;//jjj
					free(p);
					systick_delay_ms(20);
					
						
				}
			if(Q.front->next == Q.rear) 
						{
							uart_putchar(USART_4,'#');
							break;
						}
		}
		return 1;
}
#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct QNode
{
		char buffer[10];
    float data;
    struct QNode *next;
}QNode;//����ڵ�
 
typedef struct LinkQueue
{
    QNode *front;//����ָ��
    QNode *rear;//��βָ��
}LinkQueue; //��βָ��


extern LinkQueue Q;


//����һ���ն���
int InitQueue(LinkQueue *Q);
//���ٶ���
int DestoryQueue(LinkQueue *Q);
//��� 
int EnQueue(LinkQueue *Q);
//���� ��ȡ����Ԫ�ظ�ֵ��e
int DeQueue(LinkQueue *Q,int *e);
//��ӡ����
int PrintQueue(QNode *Q);
void QueueTraverse(LinkQueue Q);
int modify_param(LinkQueue Q);



#endif