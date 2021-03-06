/**********************************************************************
* Name:    		DEMO.C
* Description: 	USB Composite Device(MSC+HID) Demo
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
#include "usbhw.h"
#include "usbcore.h"

#include "hiduser.h"
#include "mscuser.h"

#include "usbcfg.h"
#include "usbdesc.h"
#include "demo.h"

extern uint8_t  USB_Configuration;

// HID
uint8_t InReport;                           /* HID Input Report    */
uint8_t PrevInReport;
uint8_t OutReport;                          /* HID Out Report      */

// MSC			
extern const unsigned char DiskImage[MSC_ImageSize];  	/* Disk Image */
extern uint8_t Memory[MSC_PhysicalMemorySize];      	/* MSC Memory in RAM */


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
  /* Control 8 LEDs */
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


/* Main Program */
int main (void) 
{
  uint32_t n;
	
  SystemCoreClockUpdate();

  /* Keil MCB1U00 board(LPC11U14 Rev.A): Status of LED[0..7] as OutReport, Status of S3&S4 as InReport */
  LPC_GPIO->DIR[1] |= 0xFF<<19;	/* output */
  LPC_GPIO->CLR[1] |=  0xFF<<19; /* all LEDs off */
  LPC_GPIO->DIR[0] &= ~((0x1<<16) | (0X1<<17)); /* input */
  
  // USB_MSC
  for (n = 0; n < MSC_PhysicalMemorySize; n++) {     /* Copy Initial Disk Image */
    Memory[n] = DiskImage[n];               /*   from Flash to RAM     */
  }
	
  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1)
  {
	// USB_HID	  
 	HID_Device_USBTask();
  }
}	 

