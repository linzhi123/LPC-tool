/**********************************************************************
* Name:    		DEMO.C
* Description: 	USB Composite Device(DFU+MSC) Demo
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
#include "mscuser.h"
#include "demo.h"


// MSC			
extern const unsigned char DiskImage[MSC_ImageSize];  	/* Disk Image */
extern uint8_t Memory[MSC_PhysicalMemorySize];      	/* MSC Memory in RAM */

volatile uint32_t u32Milliseconds = 0;
extern volatile uint8_t DFUDetachReceived;


void SysTick_Handler (void) 					
{           
	if(u32Milliseconds > 0)
	{
		u32Milliseconds--; 
	}
}

/* Main Program */
int main (void) 
{
  uint32_t n;
	
  SystemCoreClockUpdate();
  SysTick_Config( SystemCoreClock/1000 - 1);


	for (n = 0; n < MSC_PhysicalMemorySize; n++) {     /* Copy Initial Disk Image */
    Memory[n] = DiskImage[n];               /*   from Flash to RAM     */
  }

  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1)
  {

	u32Milliseconds = 100;
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

