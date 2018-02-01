#include "lpc11uxx.h"
#include "lpc11xx_uart.h"
#include "print.h"


#define ADS_I2C_SDA_H    LPC_GPIO->SET[0] |= (1<<12)
#define ADS_I2C_SDA_L    LPC_GPIO->CLR[0] |= (1<<12)

#define ADS_I2C_SCL_H    LPC_GPIO->SET[0] |= (1<<14)
#define ADS_I2C_SCL_L    LPC_GPIO->CLR[0] |= (1<<14)

#define ADS_I2C_SDA_OUT_IN(n)  LPC_GPIO->DIR[0] |= (n<<12)
#define ADS_I2C_SCL_OUT_IN(n)  LPC_GPIO->DIR[0] |= (n<<14)


void I2C_delay_ADS(uint32_t ns)
{
    volatile uint32_t i =0;
    uint32_t loop_1ns = SystemCoreClock/(1000000*6);
    for(;ns>0;ns--)
        for(i = 0; i < loop_1ns; i++);
}

void i2c_start_ADS(void)
{
	ADS_I2C_SDA_OUT_IN(1);
	
	ADS_I2C_SDA_H;	
	ADS_I2C_SCL_H;
		
	I2C_delay_ADS(1);
	ADS_I2C_SDA_L;
	I2C_delay_ADS(1);
	ADS_I2C_SCL_L;
}


void i2c_stop_ADS(void)
{
	ADS_I2C_SDA_OUT_IN(1);
		
	ADS_I2C_SCL_L;
	ADS_I2C_SDA_L;	
	I2C_delay_ADS(1);
	
	ADS_I2C_SCL_H;
	I2C_delay_ADS(1);	
	ADS_I2C_SDA_H;
}

void I2C_ACK_ADS(uint8_t ack)
{
	uint32_t i=0;
	ADS_I2C_SDA_OUT_IN(1);
	
	if(ack)
		ADS_I2C_SDA_H;
	else
		ADS_I2C_SDA_L;
	
	I2C_delay_ADS(1);
	
	ADS_I2C_SCL_H;
	I2C_delay_ADS(1);
	
	ADS_I2C_SCL_L;
	I2C_delay_ADS(2);

}

void i2c_sendbyte_ADS(unsigned char sendData)
{
    unsigned char serialNum = 0;  
  
	ADS_I2C_SDA_OUT_IN(1);
	
	ADS_I2C_SCL_L;
	I2C_delay_ADS(2); 
  
    for(serialNum=0;serialNum<8;serialNum++)   //MSB
    {  
		if(sendData&(1<<(7-serialNum)))
			ADS_I2C_SDA_H;
		else
			ADS_I2C_SDA_L;
		I2C_delay_ADS(1);   
        
		ADS_I2C_SCL_H;  
        I2C_delay_ADS(1);   
        
		ADS_I2C_SCL_L;  
        I2C_delay_ADS(2);   
    } 
}

unsigned char i2c_recvbyte_ADS(void)
{
    unsigned char serialNum = 0;  
    unsigned char dataValue=0;  
  
    ADS_I2C_SDA_OUT_IN(0);  

	ADS_I2C_SCL_L;  
    I2C_delay_ADS(2); 
  
    for(serialNum=0;serialNum<8;serialNum++)//MSB 
    {   
        if(((LPC_GPIO->PIN[0])&(1<<12)))
			dataValue|=1<<(7-serialNum);
		I2C_delay_ADS(1);
        
		ADS_I2C_SCL_H;  
        I2C_delay_ADS(1);
		
		ADS_I2C_SCL_L;  
        I2C_delay_ADS(2);
    }  
  
    return dataValue;
}

uint8_t I2C_wait_ack_ADS(void)
{
	uint8_t ack=0,r=0;
    ADS_I2C_SDA_OUT_IN(0);  
	
	ADS_I2C_SDA_H;
	ADS_I2C_SCL_H;
	I2C_delay_ADS(1);
	
	r = ((LPC_GPIO->PIN[0])&(1<<12));
	if(r)
	{
		ack = 1;
		i2c_stop_ADS();
	}
	else
		ack = 0;
	
	ADS_I2C_SCL_L;
	I2C_delay_ADS(2);
	
	return ack;

}

void ADS1100_write(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack = 0;
	
	i2c_start_ADS();
	
	i2c_sendbyte_ADS(0x90);
	
	ack = I2C_wait_ack_ADS();
	
	if(ack)
	{
		_PRINT("ADS1100 ack device address failed\r\n");
		
		return;	
	}
	
	_PRINT("ADS1100 is online\r\n");

/*	i2c_sendbyte_ADS(addr);
	
	ack = I2C_wait_ack_ADS();
	if(ack)
	{
		_PRINT("ADS1100 ack word address failed\r\n");
		return;
	}
	
	_PRINT("ADS1100 word address is ok\r\n");
*/	
	while(len--)
	{
		i2c_sendbyte_ADS(*pbuf++);
		
		ack = I2C_wait_ack_ADS();
		if(ack)
		{
			_PRINT("ADS1100 ack send data is failed\r\n");
			return;
		}	
	}
	
	i2c_stop_ADS();

	_PRINT("ADS1100 write ok\r\n");
}

void ADS1100_read(uint8_t addr,uint8_t *pbuf,uint32_t len)
{
	uint8_t ack=0;
	
/*	i2c_start_ADS();
	
	i2c_sendbyte_ADS(0x90);
	
	ack = I2C_wait_ack_ADS();
	
	if(ack)
	{
		_PRINT("ADS1100 ack read word address is failed\r\n");
		return;
	}
	
	_PRINT("ADS1100 read  is online\r\n");
	
	i2c_sendbyte_ADS(addr);
	
	ack = I2C_wait_ack_ADS();
	
	if(ack)
	{
		_PRINT("ADS1100 ack read  word address failed\r\n");
		return;
	}
	
	_PRINT("ADS1100 word read address ok\r\n");
*/
	i2c_start_ADS();
	
	i2c_sendbyte_ADS(0x91);
	
	ack = I2C_wait_ack_ADS();
	
	if(ack)
	{
		_PRINT("ADS1100 ack read word address fail\r\n");
		return;
	}
	
	len--;
	while(len--)
	{
		*pbuf++ = i2c_recvbyte_ADS();
		I2C_ACK_ADS(0);
	}
	
	*pbuf++ = i2c_recvbyte_ADS();
	I2C_ACK_ADS(1);
	i2c_stop_ADS();
	
	_PRINT("ADS1100 read ok\r\n");

}


/*int init_i2c(void)
{
	LPC_IOCON->TMS_PIO0_12 |= 0x1;    //set pio12 GPIO mod
	LPC_IOCON->TRST_PIO0_14 |= 0x1;   //set pio14 GPIO mod
	
	ADS_I2C_SCL_OUT_IN(1);
		
	return 0;
}*/









