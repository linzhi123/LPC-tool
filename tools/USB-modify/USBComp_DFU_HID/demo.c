/**********************************************************************
* Name:    		DEMO.C
* Description: 	USB Composite Device(DFU+HID) Demo
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
#include "hiduser.h"
#include "dfuuser.h"
#include "demo.h"

extern uint8_t  USB_Configuration;

// HID
uint8_t InReport;                           /* HID Input Report    */
uint8_t PrevInReport;
uint8_t OutReport;                          /* HID Out Report      */

volatile uint32_t u32Milliseconds = 0;
extern volatile uint8_t DFUDetachReceived;

/*
 *  Get HID Input Report -> InReport
 */
void GetInReport (void) {
  InReport = 0;
  if (( LPC_GPIO->PIN[0] & (1<<16) ) == 0)		/* Check if S3 is pressed */
    InReport |= 0x01;
  if (( LPC_GPIO->PIN[0] & (1<<17) ) == 0)		/* Check if S4 is pressed */
    InReport |= 0x02;
  return;
}


/*
 *  Set HID Output Report <- OutReport
 */
void SetOutReport (void) {

  LPC_GPIO->PIN[1] &= ~(0xFF<<19);
  LPC_GPIO->PIN[1] |= (OutReport<<19);

  return; 
}

void HID_Device_USBTask()
{
	if (USB_Configuration == 0) return;

	if (Endpoint_IsINReady(HID_EP_IN)) {
		GetInReport();
		if (PrevInReport != InReport) {
			PrevInReport = InReport;
			USB_WriteEP(HID_EP_IN, (uint8_t *)&InReport, sizeof(InReport));
		}
	}
}

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
  SystemCoreClockUpdate();
  SysTick_Config( SystemCoreClock/1000 - 1);

  /* Keil MCB1U00 board(LPC11U14 Rev.A): Status of LED[0..7] as OutReport, Status of S3&S4 as InReport */
  LPC_GPIO->DIR[1] |= 0xFF<<19;	/* output */
  LPC_GPIO->CLR[1] |=  0xFF<<19; /* all LEDs off */
  LPC_GPIO->DIR[0] &= ~((0x1<<16) | (0X1<<17)); /* input */
	
  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */
	
  while (1)
  { 
 	HID_Device_USBTask();

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

