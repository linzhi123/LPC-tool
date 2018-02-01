#include "lpc11uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"


extern int init_i2c(void);
extern void PCA9555_write(uint8_t addr,uint8_t *pbuf,uint32_t len);
extern void PCA9555_read(uint8_t addr,uint8_t *pbuf,uint32_t len);

extern void ADS1100_write(uint8_t addr,uint8_t *pbuf,uint32_t len);
extern void ADS1100_read(uint8_t addr,uint8_t *pbuf,uint32_t len);
extern void ADS1100_RESETIIC(void);


void delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}

int main(void)
{
	uint32_t i,value;
	uint8_t w_buf[256] = {0xc2};
	uint8_t r_buf[256]={0xdf};
	uint8_t c[1]={0x00};
	uint8_t ADS1100_w[1] = {0x8c};
	uint8_t ADS1100_r[8] = {0xff,0xff,0xff};
	double advalue;
	
	SystemInit();  //Turn on system clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //Enable GPIO and IOCON clock
	
	LPC_IOCON->PIO0_17 &= ~0x7;   //配置PIO0_17 为GPIO模式
	LPC_IOCON->PIO0_17 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[0] |= (1<<17);   //set output mod
	LPC_GPIO->CLR[0] |= 1<<17;     //Low level

	LPC_IOCON->PIO0_16 &= ~0x7;   //配置PIO0_16为GPIO输出
	LPC_IOCON->PIO0_16 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[0] |= (1<<16);   //set output mod
	LPC_GPIO->SET[0] |= 1<<16;     //HIGH level

	
	DebugInit();   //打开串口调试
	
	init_i2c();

	PCA9555_write(0x6,c,1);
	delay(200);	
	PCA9555_write(0x2,w_buf,1);
	delay(200);			
	
	while(1)
	{	
		ADS1100_RESETIIC();
		
		//ADS1100_write(0x99,ADS1100_w,1);
		ADS1100_write(0x91,ADS1100_w,1);
		delay(200);

//		ADS1100_read(0x91,ADS1100_r,3);
//		delay(200);
//		
//		_PRINT_H(ADS1100_r[0]);
//		_PRINT("\r\n");	
//		
//		_PRINT_H(ADS1100_r[1]);
//		_PRINT("\r\n");	
//		
//		_PRINT_H(ADS1100_r[2]);
//		_PRINT("\r\n");	

	}

	return 0;
}
