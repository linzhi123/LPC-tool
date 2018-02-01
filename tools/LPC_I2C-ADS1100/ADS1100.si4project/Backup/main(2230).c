#include "lpc11uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"

extern void I2C_delay(uint32_t ms);
extern int init_i2c(void);
extern void PCA9555_write(uint8_t addr,uint8_t *pbuf,uint32_t len);
extern void PCA9555_read(uint8_t addr,uint8_t *pbuf,uint32_t len);


void delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}

int main(void)
{
	uint32_t i;
	uint8_t w_buf[256] = {0xcf};
	uint8_t r_buf[256]={0xdf};
	uint8_t c[1]={0x00};
	
	SystemInit();  //Turn on system clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //Enable GPIO and IOCON clock
	
	LPC_IOCON->PIO0_17 &= ~0x7;   //??PIO0_17?GPIO??
	LPC_IOCON->PIO0_17 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[0] |= (1<<17);   //set output mod
	LPC_GPIO->CLR[0] |= 1<<17;     //Low level
	
	DebugInit();   //打开串口调试
	
	init_i2c();
	
	ADS1100_write(0x6,c,1);
	delay(200);
	
	while(1)
	{		
		delay(1000); 
	}


	return 0;
}
