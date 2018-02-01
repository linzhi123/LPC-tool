/**********************************************************************
* $Id$		lpc11xx_uart.c			2011-06-06
*//**
* @file		lpc11xx_uart.c
* @brief	Contains all functions support for UART firmware library
* 			on LPC11xx
* @version	3.2
* @date		25. July. 2011
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
/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup UART
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "LPC11Uxx.h"
#include "lpc11xx_uart.h"


/* Public Functions ----------------------------------------------------------- */
/** @addtogroup UART_Public_Functions
 * @{
 */
/*********************************************************************//**
 * @brief        Set baudrate
 * @param[in]    baudrate: baudrate will be set, should be between:
 *                         600-115200 bps
 * @return       None
 **********************************************************************/
void UART_SetBaudrate(uint32_t baudrate)
{
    uint32_t Fdiv;
    uint32_t regVal;
    regVal = LPC_SYSCON->UARTCLKDIV;
    Fdiv = ((SystemCoreClock/regVal)/16)/baudrate ;    /*baud rate */

    LPC_USART->LCR |=(1<<7); //DLAB=1
    LPC_USART->DLM = Fdiv / 256;
    LPC_USART->DLL = Fdiv % 256;
    LPC_USART->LCR &=~(1<<7); //DLAB=0
    return;
}

/*********************************************************************//**
 * @brief        UART Init. UART will be set
 * @param[in]    None
 * @return       None
 **********************************************************************/

void UART_Init(void)
{
    /* Enable UART clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);  /* Enable the peripheral clock */

    LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

    /* Setup pin select */
    LPC_IOCON->PIO0_18 &= ~0x07;    /*  UART I/O config */
    LPC_IOCON->PIO0_18 |= 0x1;     /* UART RXD */
    LPC_IOCON->PIO0_19 &= ~0x07;
    LPC_IOCON->PIO0_19 |= 0x1;     /* UART TXD */

    /* Default setup:
     * baudrate = 115200 bps
     * 8 bit data
     * 1 stop bit
     * no parity
     */

    LPC_USART->LCR &= ~0x00000003;
    LPC_USART->LCR |= 0x03;
    LPC_USART->LCR &= ~(0x3C); //clear bit LCR[2:5]
    /* setup baurate */
    UART_SetBaudrate(115200);
    LPC_USART->IER = 0;

   // LPC_USART->FCR = 0x07;        /* Enable and reset TX and RX FIFO. */
	 LPC_USART->FCR = 0x07;
}

/*********************************************************************//**
 * @brief        UART DeInit. Disable clock for UART peripheral
 * @param[in]    None
 * @return       None
 **********************************************************************/
void UART_DeInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<11); /* Disable UART clock */
}



/*********************************************************************//**
 * @brief        Send a block of data via UART peripheral
 * @param[in]    txbuf 	Pointer to Transmit buffer
 * @param[in]    buflen 	Length of Transmit buffer
 * @return       Number of bytes sent.
 *
 * Note: when using UART in BLOCKING mode, a time-out condition is used
 * via defined symbol UART_BLOCKING_TIMEOUT.
 **********************************************************************/
uint32_t UART_Send(uint8_t *txbuf, uint32_t buflen)
{
    uint32_t bToSend, bSent, timeOut, fifo_cnt;
    uint8_t *pChar = txbuf;

    bToSend = buflen;

    // blocking mode
    bSent = 0;
    while (bToSend){
        timeOut = UART_BLOCKING_TIMEOUT;
        // Wait for THR empty with timeout
        while (!(LPC_USART->LSR & (1<<5))) {
            if (timeOut == 0) break;
            timeOut--;
        }
        // Time out!
        if(timeOut == 0) break;
        fifo_cnt = UART_TX_FIFO_SIZE;
        while (fifo_cnt && bToSend){
            LPC_USART->THR = (*pChar++);
            fifo_cnt--;
            bToSend--;
            bSent++;
        }
    }
    
    return bSent;
}

/*********************************************************************//**
 * @brief        Receive a block of data via UART peripheral
 * @param[out]	 rxbuf 	Pointer to Received buffer
 * @param[in]    buflen 	Length of Received buffer
 * @return       Number of bytes received
 *
 * Note: when using UART in BLOCKING mode, a time-out condition is used
 * via defined symbol UART_BLOCKING_TIMEOUT.
 **********************************************************************/
uint32_t UART_Receive(uint8_t *rxbuf, uint32_t buflen)
{
    uint32_t bToRecv, bRecv, timeOut;
    uint8_t *pChar = rxbuf;

    bToRecv = buflen;

    bRecv = 0;
    while (bToRecv){
        timeOut = UART_BLOCKING_TIMEOUT;
        while (!(LPC_USART->LSR & 0x01)){ //check bit RDR
            if (timeOut == 0) break;
            timeOut--;
        }
        // Time out!
        if(timeOut == 0) break;
        // Get data from the buffer
        (*pChar++) = LPC_USART->RBR ;
        bToRecv--;
        bRecv++;
    }

    return bRecv;
}

/*********************************************************************//**
 * @brief        Get line status
 * @param[in]    None
 * @return       Line Status register value
 **********************************************************************/
uint8_t UART_GetLineStatus(void)
{
    return LPC_USART->LSR;
}
/**
 * @}
 */


/**
 * @}
 */


/* --------------------------------- End Of File ------------------------------ */
