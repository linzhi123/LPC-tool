/**********************************************************************
* $Id$		sbl_slave.h			2012-05-04
*//**
* @file		sbl_slave.h	
* @brief		SBL Slave.
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

#ifndef __SBL_SLAVE_H__
#define __SBL_SLAVE_H__
#include "sbl.h"

/** The type of SBL Entry function */
typedef void (*sbl_entry)(void);

/****Memory Map********/
/** The start address to place new Firmware Data */
#ifdef __MCU_LPC17xx
#define SBL_SECTOR0_START			(0x0000)
#define SBL_SLV_FIRMWARE_START		(0x1000) 
#define SBL_SLV_FIRMWARE_ID_ADDR    (0x7FFFC)
#define SBL_RAM_START				(0x10000000)
#define SBL_RAM_BUFFER_ADDR         (0x10007C00)
#define SBL_SLV_LAST_SECTOR_NUM     (0x1D)
#elif defined (__MCU_LPC11xx)
#define SBL_SECTOR0_START			(0x0000)
#define SBL_SLV_FIRMWARE_START		(0x1000) 
#define SBL_SLV_FIRMWARE_ID_ADDR    (0x7FFC)
#define SBL_RAM_START				(0x10000000)
#define SBL_RAM_BUFFER_ADDR         (0x10001C00)
#define SBL_SLV_LAST_SECTOR_NUM     (0x07)
#endif

/* SBL API Id */
#define SBL_SLAVE_INIT_FUNC         0x01
#define SBL_SLAVE_DEINIT_FUNC       0x02
#define SBL_CMD_HANDLER_FUNC        0x03

typedef void (*USER_ENTRY_PFN)();

/****Public Functions********/
/** Initialize SBL Slave Device */
SBL_RET_CODE SBL_SlaveInit(void);
/** Notify that SBL Slave is ready */
SBL_RET_CODE SBL_SlaveReady(void);
/** Send Firmware Version ID to master */
SBL_RET_CODE SBL_SlaveSendFwVersionID(void);
/** Upgrade Firmware with data sent by Master */
SBL_RET_CODE SBL_SlaveUpgradeFw(void);
/** Send Status to Master */
SBL_RET_CODE SBL_SlaveSendStatus(SBL_STATUS status);
/** Run new firmware */
SBL_RET_CODE SBL_SlaveRunUserCode(void);
/** SBL Entry */
void SBL_SlaveEntry(void);
/** Call SBL Function */
void SBL_APICall(uint32_t API, uint8_t* pData)  __attribute__((section("sbl_call")));
/** SBL Function Handler */
SBL_RET_CODE SBL_FnHandler(SBL_FUNC_ID fnID);



#endif /*__SBL_SLAVE_H__*/\

