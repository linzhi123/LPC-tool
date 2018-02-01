/**********************************************************************
* $Id$		lpc11xx_debug.c				2012-05-09
*//**
* @file		lpc11xx_debug.c
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

#include "includes.h"
#include "lpc11xx_uart.h"

/*********************************************************************//**
 * @brief        Puts a character to UART port
 * @param[in]    ch		Character to put
 * @return       None
 **********************************************************************/
void DEBUGPutChar (uint8_t ch)
{
    UART_Send( &ch, 1);
}


/*********************************************************************//**
 * @brief        Get a character to UART port
 * @param[in]    None
 * @return       character value that returned
 **********************************************************************/
uint8_t DEBUGGetChar (void)
{
    uint8_t tmp = 0;
    UART_Receive( &tmp, 1);
    return(tmp);
}


/*********************************************************************//**
 * @brief        Puts a string to UART port
 * @param[in]    str 	string to put
 * @return       None
 **********************************************************************/
void DEBUGPuts(const uint8_t *str)
{
    while (*str)
    {
        DEBUGPutChar( *str++);
    }
}


/*********************************************************************//**
 * @brief        Puts a string to UART port and print new line
 * @param[in]    str		String to put
 * @return       None
 **********************************************************************/
void DEBUGPuts_(const uint8_t *str)
{
    DEBUGPuts ( str);
    DEBUGPuts ( "\n\r");
}

/*********************************************************************//**
 * @brief        Puts a hex number to UART port
 * @param[in]    hexnum	Hex number (8-bit long)
 * @return       None
 **********************************************************************/
void DEBUGPutHex (uint8_t hexnum)
{
    uint8_t nibble;

    DEBUGPuts( "0x");
    
    nibble = (hexnum >> 4) & 0x0F;
    DEBUGPutChar( (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));

    nibble = hexnum & 0x0F;
    DEBUGPutChar( (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
}
/*********************************************************************//**
 * @brief        Check if UART is ready
 * @param[in]    None
 * @return       TRUE/FALSE
 **********************************************************************/
Bool  DEBUGDataReady(void)
{
    return (UART_GetLineStatus()&UART_LSR_RDR)? TRUE:FALSE;
}

/*********************************************************************//**
 * @brief        Initialize Debug frame work through initializing UART port
 * @param[in]    None
 * @return       None
 **********************************************************************/
void DebugInit(void)
{
    UART_Init();
}



/* --------------------------------- End Of File ------------------------------ */
