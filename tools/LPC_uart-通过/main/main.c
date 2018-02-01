#include "stdio.h"
#include "LPC11Uxx.h"
//#include "lpc11xx_uart.h"
#include "string.h"
#include "uart.h"
// 

#include "serial.h"


#if AUTOBAUD_ENABLE
volatile uint32_t UARTAutoBaud = 0, AutoBaudTimeout = 0;
#endif

extern uint32_t  SystemCoreClock; 

#define ms        (SystemCoreClock / 3000)  

#define LED_PORT        0           
#define LED             17               
#define LED_MASK       (1 << 17)   

//UART_IRQHandler();

	char c;
	uint8_t buf_recv[10] ;

int main(void)
{
	volatile unsigned long i=0;
	
	uint8_t buf_send[10] = "0x55";
	
	int len = sizeof(buf_send);
	
	SystemInit();     //��ʼ��ϵͳʱ��
		
	//UART_Init();
	UARTInit(115200);
	
	//UART_SetBaudrate(115200);
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //����gpioʱ�Ӻ�Iconʱ��
	//printf("hello\n\r");
	
	LPC_IOCON->PIO0_17 &= ~0x7;
	LPC_IOCON->PIO0_17 |= 0x0;
	
	LPC_GPIO->DIR[LED_PORT] |= (LED_MASK);  
	LPC_GPIO->CLR[LED_PORT] |= LED_MASK;
	
	//LPC_GPIO->SET[LED_PORT] |= LED_MASK;
	
	//ser_OpenPort (); //�򿪴���
	//ser_InitPort (115200, 8 , 0 , 1);  //��ʼ������ ������115200 ����λ8 ��У��λ  1λֹͣλ
	
//	char bb[1]=0;
//	UARTSendString("0xaa");
//	while(1)
//	{
//			
//		while(i<0x100000)
//		{
//				i++;
//				ser_Write(buf_send, &len); //�򴮿���д����
//				//LPC_GPIO->CLR[LED_PORT] |= LED_MASK;
//				//i=0;
//		}
//		while(i>0)
//		{
//			i--;
//			//LPC_GPIO->SET[LED_PORT] |= LED_MASK;
//		
//		}
//		
////			LPC_USART->IER = IER_THRE | IER_RLS; 
////			bb[0]=UARTRec();                    //
////			UARTSendString((char *) bb);
////			LPC_USART->IER = IER_THRE | IER_RLS | IER_RBR; 
//	}

	
	while(1)
	{
			//i = 0x100000;
		while(i<0x10000000)
		{
			UARTSend(buf_send,sizeof(buf_send));	
					
			i++;
		}
		
		while(i>0)
		{		
					
					i--;
		}	
	}
			
		
			
	return 0;
}

void UART_IRQHandler(void) 
{
	//	c = get_key();
//		print_string(&c);
//		
//		 while((LPC_USART->LSR & 0x60) != 0x60);
	
	UART_Receive(buf_recv, 1);
//	print_string(buf_recv);
//	UARTSend(buf_recv,1);	
	UARTSend(buf_recv,1);	
}
