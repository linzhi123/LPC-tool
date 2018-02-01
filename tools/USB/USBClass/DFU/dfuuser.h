/**********************************************************************
* Name:    		dfuuser.h
* Description: 	Device Firmware Upgrade Class Custom User Module Definitions
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

#ifndef __DFUUSER_H__
#define __DFUUSER_H__

#include "dfu.h"

/**
 * If USB device is only DFU capable, DFU Interface number is always 0.
 * if USB device is (DFU + Other Class (Audio/Mass Storage/HID), DFU 
 * Interface number should also be 0 in this implementation.
 */ 
//#define USB_DFU_IF_NUM	0x0



/* DFU boot definitions */
#define DFU_DEST_BASE         0x4000
#define DFU_MAX_IMAGE_LEN     (8 * 1024)
#define DFU_MAX_BLOCKS        (DFU_MAX_IMAGE_LEN/USB_DFU_XFER_SIZE)


extern uint32_t DFU_Detach(void);
extern uint32_t DFU_Reset(void);
extern uint32_t DFU_GetStatus (void);
extern uint32_t DFU_GetState (void);
extern uint32_t DFU_ClearStatus (void);
extern uint32_t DFU_Abort (void);
extern uint32_t DFU_SetupForDownload (uint32_t blockNum, uint32_t len, uint32_t *eotflag );
extern uint32_t DFU_Upload(uint32_t blockNum, uint32_t len);
extern uint32_t DFU_Download(uint32_t block_num, uint32_t len);


#endif  /* __DFUUSER_H__ */
