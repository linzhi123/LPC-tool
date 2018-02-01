/**********************************************************************
* $Id$		SECTOR0_.h			2012-05-04
*//**
* @file		SECTOR0_.h	
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

#ifndef __SECTOR0__H__
#define __SECTOR0__H__
#include "sbl.h"
#include "sbl_slave.h"

/****Public Functions********/
/** Notify that Slave is ready */
SBL_RET_CODE SECTOR0_Ready(void);
/** Upgrade Firmware with data sent by Master */
SBL_RET_CODE SECTOR0_UpgradeFw(void);
/** Send Status to Master */
SBL_RET_CODE SECTOR0_SendStatus(SBL_STATUS status);

#endif /*__SECTOR0__H__*/

