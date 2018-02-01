#include "stdio.h"
#include "LPC11Uxx.h"
#include "string.h"
#include "uart.h"
#include "lpc11xx_uart.h"

#include "serial.h"


#define LED_PORT        0           
#define LED             17               
#define LED_MASK       (1 << 17)   

//UART_IRQHandler();

uint8_t buf_recv[10]={0} ;

int main(void)
{
	volatile unsigned long i=0;
	
	uint8_t buf_send[10] = "0x55";
	
	SystemInit();     //��ʼ��ϵͳʱ��
		
	UART_Init();      //���ڳ�ʼ��
	//UARTInit(115200);
	
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //����gpioʱ�Ӻ�Iconʱ��
	
	LPC_IOCON->PIO0_17 &= ~0x7;  //����gpio0_17Ϊ��ͨio�����
	LPC_IOCON->PIO0_17 |= 0x0;
	
	LPC_GPIO->DIR[LED_PORT] |= (LED_MASK);  //����Ϊ���ģʽ
	LPC_GPIO->CLR[LED_PORT] |= LED_MASK;    //����͵�ƽ

	while(1)
	{
		while(i<0x10000000)
		{
			//UART_Send(buf_send,sizeof(buf_send));	  //�򴮿ڷ�������	
			//UART_Send(buf_recv,1);
			i++;
		}
		
		while(i>0)
		{						
			i--;
		}	
	}
					
	return 0;

}

void UART_IRQHandler(void)   //�����ж�
{
	UART_Receive(buf_recv, 1); //���մ�������
	
	UART_Send(buf_recv,1);
}
