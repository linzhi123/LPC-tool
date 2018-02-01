#include "LPC11Uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"
#include "fpm.h"


#define PIO0_PORT        		0           
#define PIO0_PORT_17            17               
#define PIO0_PORT_MASK       (1 << 17)   


extern void init_spi(void);
extern int ml8160_testSPI(uint32_t pollingTimes);
extern int init_i2c(void);
extern void PCA9555_read(uint8_t addr,uint8_t *pbuf,uint32_t len);
extern void PCA9555_write(uint8_t addr,uint8_t *pbuf,uint32_t len);


uint8_t buf_recv[128] ;


void delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}


int main(void)
{	
	uint32_t i,j=0;
	uint8_t w_buf[1] = {0xcf};
	uint8_t r_buf[1]={0xdf};
	uint8_t c[1]={0x00};
	
	SystemInit();  //开启系统时钟	
	DebugInit();   //打开串口调试	

	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //打开GPIO和IOCON时钟
	
	LPC_IOCON->PIO0_17 &= ~0x7;   //配置PIO0_17为GPIO输出
	LPC_IOCON->PIO0_17 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[PIO0_PORT] |= (PIO0_PORT_MASK);   //set output mod
	LPC_GPIO->CLR[PIO0_PORT] |= PIO0_PORT_MASK;     //Low level
	
	LPC_IOCON->PIO0_16 &= ~0x7;   //配置PIO0_16为GPIO输出
	LPC_IOCON->PIO0_16 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[0] |= (1<<16);   //set output mod
	LPC_GPIO->SET[0] |= 1<<16;     //HIGH level

	
	init_spi();    //spi1 初始化	
    for(i = 0; i < 10000; i++);  // 等待一会	
	init_i2c();
	
	PCA9555_write(0x6,c,1);
	delay(200);
	PCA9555_write(0x7,c,1);
	delay(200);
	
	PCA9555_write(0x2,w_buf,1);
	delay(200);
	
	PCA9555_write(0x3,r_buf,1);
	delay(200);
	
	while(1)
	{	
		if(j>=5)
			j=0;
		i = ml8160_testSPI(50);		//read 012
		_PRINT_H(i);				//UART put data
		_PRINT("\r\n>");			//Wrap
		delay(1000);                //delay 1s
		w_buf[0] &= ~(1<<j);
		PCA9555_write(0x2,w_buf,1);	
		j++;
		w_buf[0] |= (1<<(j-1));
	}
	
	return 0;

}


void UART_IRQHandler(void)   //串口中断
{
	UART_Receive(buf_recv, 1); //接收串口数据
	
	UART_Send(buf_recv,1);
	//_PRINT(buf_recv);
}

