/**********************************************************************
* Name:    		sbl_coinfig.h
* Description: 	Configuration files for sector erase and program
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

#ifndef  _SBL_CONFIG_H
#define  _SBL_CONFIG_H

#define CCLK 48000 					/* 48,000 KHz for IAP call */
#define FLASH_BUF_SIZE 256

#define SECTOR_0_START_ADDR 0
#define SECTOR_SIZE 4096
#define MAX_USER_SECTOR 8
//#define USER_FLASH_START (sector_start_map[USER_START_SECTOR])
//#define USER_FLASH_END	 (sector_end_map[MAX_USER_SECTOR])
//#define USER_FLASH_SIZE  ((USER_FLASH_END - USER_FLASH_START) + 1)

                          
#endif  /* __SBL_CONFIG_H__ */
