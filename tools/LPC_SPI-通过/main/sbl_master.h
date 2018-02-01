/**********************************************************************
* $Id$		sbl_master.h			2012-05-04
*//**
* @file		sbl_master.h	
* @brief		SBL Master.
* @version	1.0
* @date		04. May. 2012
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
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

#ifndef __SBL_MASTER_H__
#define __SBL_MASTER_H__
#include "sbl.h"

#define  _FW_DATA_IN_FLASH      1           // 0: get fw data from Flash, 1: get from user.

#define SBL_FIRMWARE_ADDR       		(0x1000) 

#define SBL_RAM_BUFFER_ADDR             (0x10001C00)
   

/* The callback after sending 1 block of firmware data */
typedef void (*UPGRADE_FW_CB)(uint32_t BlockNum);
/* The callback to get a firwmware data */
typedef uint32_t (*GET_FW_DATA)(uint8_t *buffer,uint32_t size);
/* Delay, Timeout definition*/
#define SBL_MST_SLAVE_RDY_DELAY				(10)
#define SBL_MST_ERASE_FLASH_DELAY           (1000)
#define SBL_MST_PROG_FLASH_DELAY            (10)
#define SBL_MST_PROG_FLASH_TIMEOUT          (1000)

typedef struct 
{
    UPGRADE_FW_CB UpgradeStartPFN;   // Called when a block is sent to slave */
#if !_FW_DATA_IN_FLASH
    GET_FW_DATA ReadFwDataPFN;		// Called to get firmware data
#endif

}SBL_CB_Type, *PSBL_CB_Type;
/*********Public Functions **********/
/** Initialize SBL Master Device */
SBL_RET_CODE SBL_MasterInit(void);
/** Read Firmware Version ID from Slave */
SBL_RET_CODE SBL_MasterReadFwVersionID(PSBL_FirmVerion_Type pFirmVers);
/** Upgrade Firmware to Slave */
SBL_RET_CODE SBL_MasterUpgradeFirmware(Bool UpgradeSector0, PSBL_CB_Type pCallbacks);



#endif /*__SBL_MASTER_H__*/\

