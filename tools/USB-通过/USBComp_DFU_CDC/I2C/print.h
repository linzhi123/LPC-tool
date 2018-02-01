/**********************************************************************
* $Id$		print.h		2010-05-21
*//**
* @file		print.h
* @brief	Contains some utilities that used for debugging through UART
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
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
#ifndef _UART_H_
#define _UART_H_

#define USED_UART_DEBUG_PORT	0

#define _PRINT(x)	 	DEBUGPuts(x)
#define _PRINT_(x)	    DEBUGPuts_(x)
#define _PRINT_C(x)	 	DEBUGPutChar(x)
#define _PRINT_H(x)	 	DEBUGPutHex(x)
#define _GET_C			DEBUGGetChar()
#define _DATA_READY    DEBUGDataReady()
//void  _printf (const  char *format, ...);

void DebugInit(void);
int  DEBUGDataReady(void);
void DEBUGPutHex ( uint8_t hexnum);
void DEBUGPuts_( const uint8_t *str);
void DEBUGPuts( const uint8_t *str);
uint8_t DEBUGGetChar (void);
void DEBUGPutChar (uint8_t ch);
#endif /* _UART_H_ */
