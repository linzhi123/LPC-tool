/**********************************************************************
* Name:    		DEMO.C
* Description: 	USB Composite Device(DFU+MSC+HID) Demo
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
#include "type.h"

/* HID Demo Variables */
extern uint8_t InReport, PrevInReport;
extern uint8_t OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetOutReport (void);

/* MSC Disk Image Definitions */
#define MSC_ImageSize   0x00001000
