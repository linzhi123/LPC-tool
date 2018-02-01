/**********************************************************************
* $Id$		lpc11xx_ssp.c				2012-05-09
*//**
* @file		lpc11xx_ssp.c
* @brief	Contains all functions support for SPI firmware library on LPC17xx
* @version	2.0
* @date		21. May. 2012
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

/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup SSP
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc11xx_ssp.h"
#include "lpc11xx_pinsel.h"
#include "lpc11xx_gpio.h"

#define SBL_CLKPWR_PCONP_PCSSP                (1<<8)
#define SSP_CPHA_USED                         (SSP_CPHA_SECOND)
#define SSP_CPOL_USED                         (SSP_CPOL_LO)
#define SSP_CLK_RATE_USED                     (1000000)
#define SSP_DATA_BIT_USED                     (SSP_DATABIT_8)
#define SSP_DATA_WORD                         (0)
#define SSP_MODE_USED                         (SSP_MASTER_MODE)

/* Public Functions ------------------------------------------------------------------- */
/** @defgroup SSP_Public_Functions
 * @{
 */

/*********************************************************************//**
 * @brief        De-initializes the LPC_SSP0 peripheral registers to their
*                  default reset values.
 * @param[in]    LPC_SSP0	SSP peripheral selected, should be SSP0 or SSP1
 * @return       None
 **********************************************************************/
void SSP_DeInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<11);
}

/********************************************************************//**
 * @brief        Initializes the LPC_SSP0 peripheral according to the specified
*                 parameters in the SSP_ConfigStruct.
 * @param[in]    None
 * @return       None
 *********************************************************************/
void SSP_Init(void)
{
    uint32_t tmp;

    // initialize pinsel for SSP0
    /* MISO0: P0.8 */
    LPC_IOCON->PIO0_8 = PINSEL_PIO_FUNC(1) | PINSEL_PIO_MODE(2);
    /* MOSI0: P0.9 */
    LPC_IOCON->PIO0_9 = PINSEL_PIO_FUNC(1) | PINSEL_PIO_MODE(2);

    /* P0.6 function 2 is SSP clock, need to combined with IOCONSCKLOC register setting */
    LPC_IOCON->SCK_LOC = 0x02;
    LPC_IOCON->PIO0_6 = PINSEL_PIO_FUNC(2) | PINSEL_PIO_MODE(2);

#if USE_CS
    LPC_IOCON->PIO0_2 &= ~0x1F;
    LPC_IOCON->PIO0_2 |= PINSEL_PIO_FUNC(1) | PINSEL_PIO_MODE(2);        /* SSP SSEL */
#else
    /* Enable AHB clock to the GPIO domain. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);  
#endif
    /* Reset SSP0 peripheral */
    LPC_SYSCON->PRESETCTRL |= (0x1<<0);
    /* Enable clock for SSP0 */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11);
    LPC_SYSCON->SSP0CLKDIV = 0x02;
    

    /* Configure SSP, interrupt is disable, LoopBack mode is disable,
     * SSP is disable, Slave output is disable as default
     */
    tmp = ((SSP_CPHA_USED) | (SSP_CPOL_USED) \
            | (SSP_FRAME_SPI) | (SSP_DATA_BIT_USED))
            & SSP_CR0_BITMASK;
    // write back to SSP control register
    LPC_SSP0->CR0 = tmp;
    
    // Write back to CR1
    LPC_SSP0->CR1 = SSP_MASTER_MODE;

    // Set clock rate for SSP peripheral
    LPC_SSP0->CR0 &= (~ SSP_CR0_SCR (0xFF)) & SSP_CR0_BITMASK;
    LPC_SSP0->CR0 |= (SSP_CR0_SCR(0x05)) & SSP_CR0_BITMASK;
    LPC_SSP0->CPSR = 0x02 & SSP_CPSR_BITMASK;

    // SSP Enable
    LPC_SSP0->CR1 |= SSP_CR1_SSP_EN;
}

/*********************************************************************//**
 * @brief        SSP Read write data function
 * @param[in]    dataCfg	Pointer to a SSP_DATA_SETUP_Type structure that
 * 				    contains specified information about transmit
 * 				    data configuration.
 * @return       Actual Data length has been transferred in polling mode.
 * 		    In interrupt mode, always return (0)
 * 		    Return (-1) if error.
 * Note: This function can be used in both master and slave mode.
 ***********************************************************************/
int32_t SSP_ReadWrite (SSP_DATA_SETUP_Type *dataCfg)
{
    uint8_t *rdata8;
    uint8_t *wdata8;
    uint16_t *rdata16;
    uint16_t *wdata16;
    uint32_t stat;
    uint32_t tmp;
//    int32_t sspnum;
    int32_t dataword;

    dataCfg->rx_cnt = 0;
    dataCfg->tx_cnt = 0;
    dataCfg->status = 0;


    /* Clear all remaining data in RX FIFO */
    while (LPC_SSP0->SR & SSP_SR_RNE){
        tmp = (SSP_DR_BITMASK(LPC_SSP0->DR));
    }

    // Clear status
    LPC_SSP0->ICR = SSP_ICR_BITMASK;

    dataword = SSP_DATA_WORD;

    // Polling mode ----------------------------------------------------------------------
    if (dataword == 0){
        rdata8 = (uint8_t *)dataCfg->rx_data;
        wdata8 = (uint8_t *)dataCfg->tx_data;
    } else {
        rdata16 = (uint16_t *)dataCfg->rx_data;
        wdata16 = (uint16_t *)dataCfg->tx_data;
    }
    while ((dataCfg->tx_cnt != dataCfg->length) || (dataCfg->rx_cnt != dataCfg->length)){
        if ((LPC_SSP0->SR & SSP_SR_TNF) && (dataCfg->tx_cnt != dataCfg->length)){
            // Write data to buffer
            if(dataCfg->tx_data == NULL){
                if (dataword == 0){
                    LPC_SSP0->DR = SSP_DR_BITMASK(0xFF);
                    dataCfg->tx_cnt++;
                } else {
                    LPC_SSP0->DR = SSP_DR_BITMASK(0xFFFF);
                    dataCfg->tx_cnt += 2;
                }
            } else {
                if (dataword == 0){
                    LPC_SSP0->DR = SSP_DR_BITMASK(*wdata8);
                    wdata8++;
                    dataCfg->tx_cnt++;
                } else {
                    LPC_SSP0->DR = SSP_DR_BITMASK(*wdata16);
                    wdata16++;
                    dataCfg->tx_cnt += 2;
                }
            }
        }

        // Check overrun error
        if ((stat = LPC_SSP0->RIS) & SSP_RIS_RORRIS){
            // save status and return
            dataCfg->status = stat | SSP_STAT_ERROR;
            return (-1);
        }

        // Check for any data available in RX FIFO
        while ((LPC_SSP0->SR & SSP_SR_RNE) && (dataCfg->rx_cnt != dataCfg->length)){
            // Read data from SSP data
            tmp = (SSP_DR_BITMASK(LPC_SSP0->DR));

            // Store data to destination
            if (dataCfg->rx_data != NULL)
            {
                if (dataword == 0){
                    *(rdata8) = (uint8_t) tmp;
                    rdata8++;
                } else {
                    *(rdata16) = (uint16_t) tmp;
                    rdata16++;
                }
            }
            // Increase counter
            if (dataword == 0){
                dataCfg->rx_cnt++;
            } else {
                dataCfg->rx_cnt += 2;
            }
        }
    }

    // save status
    dataCfg->status = SSP_STAT_DONE;

    if (dataCfg->tx_data != NULL){
        return dataCfg->tx_cnt;
    } else if (dataCfg->rx_data != NULL){
        return dataCfg->rx_cnt;
    } else {
        return (0);
    }
}

/**
 * @}
 */


/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
