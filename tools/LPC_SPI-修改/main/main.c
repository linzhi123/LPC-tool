#include "LPC11Uxx.h"
#include "sbl.h"
#include "lpc11xx_uart.h"
#include "print.h"
#include "fpm.h"
#include "sbl_master.h"

#define PIO0_PORT        		0           
#define PIO0_PORT_17            17               
#define PIO0_PORT_MASK       (1 << 17)   

extern void init_spi(void);
extern  void delay(uint32_t ms);
extern int ml8160_testSPI(uint32_t pollingTimes);

uint8_t buf_recv[10] ;

int main(void)
{
	uint32_t i;
	
	SystemInit();  //����ϵͳʱ��	
	DebugInit();   //�򿪴��ڵ���	

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //��GPIO��IOCONʱ��
	
	LPC_IOCON->PIO0_17 &= ~0x7;   //����PIO0_17ΪGPIO���
	LPC_IOCON->PIO0_17 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[PIO0_PORT] |= (PIO0_PORT_MASK);   //set output mod
	LPC_GPIO->CLR[PIO0_PORT] |= PIO0_PORT_MASK;     //Low level

	init_spi();    //spi1 ��ʼ��
	
    for(i = 0; i < 10000; i++);  // �ȴ�һ��
	
	while(1)
	{		
		i = ml8160_testSPI(50);		//read 012
		_PRINT_H(i);				//UART put data
		_PRINT("\r\n>");			//Wrap
		delay(1000);                //delay 1s
	}
	
	return 0;

}

void UART_IRQHandler(void)   //�����ж�
{
	UART_Receive(buf_recv, 1); //���մ�������
	
	//UART_Send(buf_recv,1);
	_PRINT(buf_recv);
}

