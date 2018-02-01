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
	
	SystemInit();     //初始化系统时钟
		
	UART_Init();      //串口初始化
	//UARTInit(115200);
	
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //是能gpio时钟和Icon时钟
	
	LPC_IOCON->PIO0_17 &= ~0x7;  //配置gpio0_17为普通io口输出
	LPC_IOCON->PIO0_17 |= 0x0;
	
	LPC_GPIO->DIR[LED_PORT] |= (LED_MASK);  //配置为输出模式
	LPC_GPIO->CLR[LED_PORT] |= LED_MASK;    //输出低电平

	while(1)
	{
		while(i<0x10000000)
		{
			//UART_Send(buf_send,sizeof(buf_send));	  //向串口发送数据	
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

void UART_IRQHandler(void)   //串口中断
{
	UART_Receive(buf_recv, 1); //接收串口数据
	
	UART_Send(buf_recv,1);
}
