/**********************************************************************
* Name:    		DEMO.C
* Description: 	USB Composite Device(DFU+CDC) Demo
*
* Copyright(C) 2012, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include "lpc11Uxx.h"                        /* LPC11xx definitions */
#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "dfuuser.h"
#include "cdc.h"
#include "cdcuser.h"
#include "serial.h"
#include "demo.h"


volatile uint32_t u32Milliseconds = 0;
extern volatile uint8_t DFUDetachReceived;

extern int init_i2c(void);
extern void PCA9555_write(uint8_t addr,uint8_t *pbuf,uint32_t len);


void SysTick_Handler (void) 					
{           
	if(u32Milliseconds > 0)
	{
		u32Milliseconds--; 
	}
}

/*----------------------------------------------------------------------------
 Initialises the VCOM port.
 Call this function before using VCOM_putchar or VCOM_getchar
 *---------------------------------------------------------------------------*/
void VCOM_Init(void) {

  CDC_Init ();
}


/*----------------------------------------------------------------------------
  Reads character from serial port buffer and writes to USB buffer
 *---------------------------------------------------------------------------*/
void VCOM_Serial2Usb(void) {
  static char serBuf [USB_CDC_BUFSIZE];
         int  numBytesRead, numAvailByte;
	
  ser_AvailChar (&numAvailByte);
  if (numAvailByte > 0) {
    if (CDC_DepInEmpty) {
      numBytesRead = ser_Read (&serBuf[0], &numAvailByte);

      CDC_DepInEmpty = 0;
	  USB_WriteEP (CDC_DEP_IN, (unsigned char *)&serBuf[0], numBytesRead);
    }
  }

}

/*----------------------------------------------------------------------------
  Reads character from USB buffer and writes to serial port buffer
 *---------------------------------------------------------------------------*/
void VCOM_Usb2Serial(void) {
  static char serBuf [32];
         int  numBytesToRead, numBytesRead, numAvailByte;

  CDC_OutBufAvailChar (&numAvailByte);
  if (numAvailByte > 0) {
      numBytesToRead = numAvailByte > 32 ? 32 : numAvailByte; 
      numBytesRead = CDC_RdOutBuf (&serBuf[0], &numBytesToRead);
      ser_Write (&serBuf[0], &numBytesRead);    
  }

}


/*----------------------------------------------------------------------------
  checks the serial state and initiates notification
 *---------------------------------------------------------------------------*/
void VCOM_CheckSerialState (void) {
         unsigned short temp;
  static unsigned short serialState;

  temp = CDC_GetSerialState();
  if (serialState != temp) {
     serialState = temp;
     CDC_NotificationIn();                  // send SERIAL_STATE notification
  }
}


/* Main Program */
int main (void) 
{
	
	uint8_t w_buf[1] = {0xdc};
	//uint8_t r_buf[1]={0xdf};
	uint8_t c[1]={0x00};
	
	uint32_t j=0;
	
	SystemCoreClockUpdate();
	SysTick_Config( SystemCoreClock/1000 - 1);

	VCOM_Init(); 

	USB_Init();                               /* USB Initialization */
	USB_Connect(TRUE);                        /* USB Connect */
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6) | (1 << 16);   //打开GPIO和IOCON时钟
	
	LPC_IOCON->PIO0_17 &= ~0x7;   //配置PIO0_17为GPIO输出
	LPC_IOCON->PIO0_17 |= 0x0;    //GPIO mod
	LPC_GPIO->DIR[0] |= (1<<17);   //set output mod
	LPC_GPIO->CLR[0] |= 1<<17;     //Low level

	init_i2c();

	PCA9555_write(0x6,c,1);	
	PCA9555_write(0x2,w_buf,1);

	while (1)
	{
		if(j>=5)
			j=0;
		w_buf[0] &= ~(1<<j);
		PCA9555_write(0x2,w_buf,1);	
		j++;
		w_buf[0] |= (1<<(j-1));
		
		// CDC
		VCOM_Serial2Usb();                      // read serial port and initiate USB event
		VCOM_CheckSerialState();
		VCOM_Usb2Serial();  
		  
		// DFU 
		u32Milliseconds = 1000;
		if (DFUDetachReceived)
		{
			USB_Connect(FALSE);
			/* for current test wrap-around condition is overlooked */
			while (u32Milliseconds < 98);
			USB_Connect(TRUE);
			DFUDetachReceived = 0;
		}
		  /* Wait... */
		  while(u32Milliseconds);	
	}
	

}	 

