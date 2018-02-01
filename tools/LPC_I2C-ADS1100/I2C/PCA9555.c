#include "lpc11uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"


#define I2C_SDA_H    LPC_GPIO->SET[0] |= (1<<12)
#define I2C_SDA_L    LPC_GPIO->CLR[0] |= (1<<12)

#define I2C_SCL_H    LPC_GPIO->SET[0] |= (1<<14)
#define I2C_SCL_L    LPC_GPIO->CLR[0] |= (1<<14)

#define I2C_SDA_OUT  LPC_GPIO->DIR[0] |= (1<<12)
#define I2C_SDA_IN  LPC_GPIO->DIR[0] &= ~(1<<12)

#define I2C_SCL_OUT_IN(n)  LPC_GPIO->DIR[0] |= (n<<14)

//#define I2C_SDA            LPC_GPIO->PIN[0] |= (1<<12)


void I2C_delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}



void i2c_start(void)
{
	I2C_SDA_OUT;
	
	I2C_SDA_H;	
	I2C_SCL_H;
		
	I2C_delay(10);
	I2C_SDA_L;
	I2C_delay(10);
	I2C_SCL_L;
}


void i2c_stop(void)
{
	I2C_SDA_OUT;
		
	I2C_SCL_L;
	I2C_SDA_L;	
	I2C_delay(10);
	
	I2C_SCL_H;
	I2C_delay(100);	
	I2C_SDA_H;
}

void I2C_ACK(uint8_t ack)
{
	I2C_SDA_OUT;
	
	if(ack)
		I2C_SDA_H;
	else
		I2C_SDA_L;
	
	I2C_delay(20);
	
	I2C_SCL_H;
	I2C_delay(20);
	
	I2C_SCL_L;
	I2C_delay(20);

}


void i2c_sendbyte(unsigned char sendData)
{
    unsigned char serialNum = 0;  
  
	I2C_SDA_OUT;
	
	I2C_SCL_L;
	I2C_delay(20); 
  
    for(serialNum=0;serialNum<8;serialNum++)   //MSB
    {  
		if(sendData&(1<<(7-serialNum)))
			I2C_SDA_H;
		else
			I2C_SDA_L;
		I2C_delay(20);   
        
		I2C_SCL_H;  
        I2C_delay(20);   
        
		I2C_SCL_L;  
        I2C_delay(20);   
    } 
}

unsigned char i2c_recvbyte(void)
{
    unsigned char serialNum = 0;  
    unsigned char dataValue=0;  
  
    I2C_SDA_IN;  

	I2C_SCL_L;  
    I2C_delay(100); 
  
    for(serialNum=0;serialNum<8;serialNum++)//MSB 
    {   
        if(((LPC_GPIO->PIN[0])&(1<<12)))
			dataValue|=1<<(7-serialNum);
		I2C_delay(20);
        
		I2C_SCL_H;  
        I2C_delay(20);
		
		I2C_SCL_L;  
        I2C_delay(20);
    }  
  
    return dataValue;
}

uint8_t I2C_wait_ack(void)
{
	uint8_t ack=0;
	uint32_t timeout=0;
	
    I2C_SDA_IN;  
	
	I2C_SDA_H;
	I2C_SCL_H;
	I2C_delay(20);
	
	while((LPC_GPIO->PIN[0])&(0x1<<12))
	{
		timeout++;
		if(timeout>250)
		{
			ack=1;
			i2c_stop();
			return ack;
		}
	
	}
	ack = 0;
	
	I2C_SCL_L;
	I2C_delay(20);
	
	return ack;

}

void PCA9555_write(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack = 0;
	
	i2c_start();
	
	i2c_sendbyte(0x40);
	
	ack = I2C_wait_ack();
	
	if(ack)
	{
		_PRINT("PCA9555 ack device address failed\r\n");
		
		return;	
	}
	
	_PRINT("PCA9555 is online\r\n");
	
	i2c_sendbyte(addr);
	
	ack = I2C_wait_ack();
	if(ack)
	{
		_PRINT("PCA9555 ack word address failed\r\n");
		return;
	}
	
	_PRINT("PCA9555 word address is ok\r\n");
	
	while(len--)
	{
		i2c_sendbyte(*pbuf++);
		
		ack = I2C_wait_ack();
		if(ack)
		{
			_PRINT("PCA9555 ack send data is failed\r\n");
			return;
		}	
	}
	
	i2c_stop();

	_PRINT("PCA9555 write ok\r\n");
}

void PCA9555_read(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack=0;
	
	i2c_start();
	
	i2c_sendbyte(0x40);
	
	ack = I2C_wait_ack();
	
	if(ack)
	{
		_PRINT("PCA9555 ack read word address is failed\r\n");
		return;
	}
	
	_PRINT("PCA9555 read  is online\r\n");
	
	i2c_sendbyte(addr);
	
	ack = I2C_wait_ack();
	
	if(ack)
	{
		_PRINT("PCA9555 ack read  word address failed\r\n");
		return;
	}
	
	_PRINT("PCA9555 word read address ok\r\n");
	
	i2c_start();
	
	i2c_sendbyte(0x41);
	
	ack = I2C_wait_ack();
	
	if(ack)
	{
		_PRINT("PCA9555 ack read word address fail\r\n");
		return;
	}
	
	len--;
	while(len--)
	{
		*pbuf++ = i2c_recvbyte();
		I2C_ACK(0);
	}
	
	*pbuf++ = i2c_recvbyte();
	I2C_ACK(1);
	i2c_stop();
	
	_PRINT("PCA9555 read ok\r\n");

}


int init_i2c(void)
{
	LPC_IOCON->TMS_PIO0_12 |= 0X1;    //set pio12 GPIO mod
	LPC_IOCON->TRST_PIO0_14 |= 0X1;   //set pio14 GPIO mod
	
	I2C_SCL_OUT_IN(1);
	I2C_SDA_OUT;
		
	return 0;
}









