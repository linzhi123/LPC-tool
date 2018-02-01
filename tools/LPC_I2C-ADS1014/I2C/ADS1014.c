#include "lpc11uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"


#define ADS1014_I2C_SDA_H    LPC_GPIO->SET[0] |= (1<<12)
#define ADS1014_I2C_SDA_L    LPC_GPIO->CLR[0] |= (1<<12)

#define ADS1014_I2C_SCL_H    LPC_GPIO->SET[0] |= (1<<14)
#define ADS1014_I2C_SCL_L    LPC_GPIO->CLR[0] |= (1<<14)

#define ADS1014_I2C_SDA_OUT  LPC_GPIO->DIR[0] |= (1<<12)
#define ADS1014_I2C_SDA_IN  LPC_GPIO->DIR[0] &= ~(1<<12)

#define ADS1014_I2C_SCL_OUT_IN(n)  LPC_GPIO->DIR[0] |= (n<<14)

void ADS1014_I2C_delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}



void ADS1014_i2c_start(void)
{
	ADS1014_I2C_SDA_OUT;
	
	ADS1014_I2C_SDA_H;	
	ADS1014_I2C_SCL_H;
		
	ADS1014_I2C_delay(2);
	ADS1014_I2C_SDA_L;
	ADS1014_I2C_delay(2);
	ADS1014_I2C_SCL_L;
}


void ADS1014_i2c_stop(void)
{
	ADS1014_I2C_SDA_OUT;
		
	ADS1014_I2C_SCL_L;
	ADS1014_I2C_SDA_L;	
	ADS1014_I2C_delay(2);
	
	ADS1014_I2C_SCL_H;
	ADS1014_I2C_delay(2);	
	ADS1014_I2C_SDA_H;
}

void ADS1014_I2C_ACK(uint8_t ack)
{
	ADS1014_I2C_SDA_OUT;
	
	if(ack)
		ADS1014_I2C_SDA_H;
	else
		ADS1014_I2C_SDA_L;
	
	ADS1014_I2C_delay(1);
	
	ADS1014_I2C_SCL_H;
	ADS1014_I2C_delay(1);
	
	ADS1014_I2C_SCL_L;
	ADS1014_I2C_delay(2);

}


void ADS1014_i2c_sendbyte(unsigned char sendData)
{
    unsigned char serialNum = 0;  
  
	ADS1014_I2C_SDA_OUT;
	
	ADS1014_I2C_SCL_L;
	ADS1014_I2C_delay(2); 
  
    for(serialNum=0;serialNum<8;serialNum++)   //MSB
    {  
		if(sendData&(1<<(7-serialNum)))
			ADS1014_I2C_SDA_H;
		else
			ADS1014_I2C_SDA_L;
		ADS1014_I2C_delay(2);   
        
		ADS1014_I2C_SCL_H;  
        ADS1014_I2C_delay(1);   
        
		ADS1014_I2C_SCL_L;  
        ADS1014_I2C_delay(2);   
    } 
}

unsigned char ADS1014_i2c_recvbyte(void)
{
    unsigned char serialNum = 0;  
    unsigned char dataValue=0;  
  
    ADS1014_I2C_SDA_IN;  

	ADS1014_I2C_SCL_L;  
    ADS1014_I2C_delay(100); 
  
    for(serialNum=0;serialNum<8;serialNum++)//MSB 
    {   
        if(((LPC_GPIO->PIN[0])&(1<<12)))
			dataValue|=1<<(7-serialNum);
		ADS1014_I2C_delay(2);
        
		ADS1014_I2C_SCL_H;  
        ADS1014_I2C_delay(2);
		
		ADS1014_I2C_SCL_L;  
        ADS1014_I2C_delay(2);
    }  
  
    return dataValue;
}

uint8_t ADS1014_I2C_wait_ack(void)
{
	uint8_t ack=0;
	uint32_t timeout=0;
	
    ADS1014_I2C_SDA_IN;  
	
	ADS1014_I2C_SDA_H;
	ADS1014_I2C_SCL_H;
	ADS1014_I2C_delay(2);
	
	while((LPC_GPIO->PIN[0])&(0x1<<12))
	{
		timeout++;
		if(timeout>250)
		{
			ack=1;
			ADS1014_i2c_stop();
			return ack;
		}
	
	}
	ack = 0;
	
	ADS1014_I2C_SCL_L;
	ADS1014_I2C_delay(2);
	
	return ack;

}

void ADS1014_write(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack = 0;
	
	ADS1014_i2c_start();
	
	ADS1014_i2c_sendbyte(0x90);
	
	ack = ADS1014_I2C_wait_ack();	
	if(ack)
	{
		_PRINT("ADS1014 write ack device address failed\r\n");
		
		return;	
	}
	
	_PRINT("ADS1014 is online\r\n");
	
	ADS1014_i2c_sendbyte(addr);	
	ack =ADS1014_I2C_wait_ack();
	if(ack)
	{
		_PRINT("ADS1014 ack word address failed\r\n");
		return;
	}
	
	_PRINT("ADS1014 word address is ok\r\n");
	
	while(len--)
	{
		ADS1014_i2c_sendbyte(*pbuf++);
		
		ack = ADS1014_I2C_wait_ack();
		if(ack)
		{
			_PRINT("ADS1014 ack send data is failed\r\n");
			return;
		}	
	}
	
	ADS1014_i2c_stop();

	_PRINT("ADS1014 write ok\r\n");

}

void ADS1014_read(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack=0;
	
	ADS1014_i2c_start();
	
	ADS1014_i2c_sendbyte(0x90);
	
	ack = ADS1014_I2C_wait_ack();
	
	if(ack)
	{
		_PRINT("ADS1014 ack read word address is failed\r\n");
		return;
	}
	
	_PRINT("ADS1014 read  is online\r\n");
	
	ADS1014_i2c_sendbyte(addr);
	
	ack = ADS1014_I2C_wait_ack();	
	if(ack)
	{
		_PRINT("ADS1014 ack read  word address failed\r\n");
		return;
	}
	
	_PRINT("ADS1014 word read address ok\r\n");
	
	ADS1014_i2c_start();
	
	ADS1014_i2c_sendbyte(0x91);
	
	ack = ADS1014_I2C_wait_ack();
	if(ack)
	{
		_PRINT("ADS1014 ack read word address fail\r\n");
		return;
	}
	
	while(len--)
	{
		*pbuf++ = ADS1014_i2c_recvbyte();
		ADS1014_I2C_ACK(0);
	}
	
	ADS1014_i2c_stop();
	
	_PRINT("ADS1014 read ok\r\n");

}










