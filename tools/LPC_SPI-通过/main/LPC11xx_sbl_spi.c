/**********************************************************************
* $Id$		lpc11xx_sbl_spi.c			2012-05-04
*//**
* @file		lpc11xx_sbl_spi.c	
* @brief        Contain API for SBL transmission using SPI (Master Role).
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
#include "sbl.h"
#include "LPC11xx_ssp.h"
#include "LPC11xx_gpio.h"
#include "LPC11xx_pinsel.h"

/*********************************************************************//**
 * @brief        SBL Initialize for master/slave
 * @param[in]    None.
 * @return       SBL_OK/SBL_INVALID_PARAM
 **********************************************************************/
//SBL_RET_CODE SBL_Init(void)
//{                                     
//    uint32_t i;

//    // Initialize SSP peripheral with parameter given in structure above
//    SSP_Init();

//#if !USE_CS
//    // CS Init
//    LPC_GPIO0->DIR |= (1<<2);
//    LPC_GPIO0->DATA |= (1<<2);
//#endif

//    // delay for a while
//    for(i = 0; i < 10000; i++);
//    
//    return SBL_OK;
//}

/*********************************************************************//**
 * @brief        De-Initialize a SBL device.
 * @param[in]    None
 * @return       SBL_OK
 **********************************************************************/
SBL_RET_CODE SBL_DeInit(void)
{
    SSP_DeInit();
    return SBL_OK;
}

/*********************************************************************//**
 * @brief        Read/Write data.
 * @param[in]    pBuffer  Store information about buffers for sending/receiving.
 * @return       SBL_OK/SBL_INVALID_PARAM/SBL_COMM_ERR
 **********************************************************************/
SBL_RET_CODE SBL_ReadWrite(PSBL_TRANSER_Type pBuffer)
{
    SSP_DATA_SETUP_Type xferConfig;
    uint32_t tmp;

    if((pBuffer->TxLen == 0 && pBuffer->RxLen == 0))
    {
        return SBL_OK;
    }

    xferConfig.tx_data = pBuffer->TxBuf;
    xferConfig.rx_data = pBuffer->RxBuf;
    xferConfig.length = (pBuffer->TxLen > pBuffer->RxLen) ? pBuffer->TxLen: pBuffer->RxLen;

#if !USE_CS
    // CS LOW
	LPC_GPIO->CLR[1] |= (1<<19);
    // delay for a while
    for (tmp = 100; tmp; tmp--);
#endif

    if(SSP_ReadWrite(&xferConfig) != xferConfig.length)
    {
        return SBL_COMM_ERR;
    }

#if !USE_CS    
    // delay for a while
    for (tmp = 100; tmp; tmp--);

    // CS HIGH
    LPC_GPIO->SET[1] |= (1<<19);
#endif
    

    return SBL_OK;
}



