/**********************************************************************
* $Id$		Sector0_program.c			2012-05-04
*//**
* @file		Sector0_program.c	
* @brief	Slave for programming sector 0.
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
#include "sector0_program.h"
#include "sbl_slave.h"
#include "iap.h"

/** Read Sector0 data from Master */
SBL_RET_CODE SECTOR0_RecvBlock(uint32_t* pBlockNum);

/*********************************************************************//**
 * @brief      Upgrade Sector0 with data sent by master
 * @param[in]  None.
 * @return     SBL_OK/SBL_FLASH_WRITE_ERR
 **********************************************************************/
SBL_RET_CODE SECTOR0_UpgradeFw(void)
{
    SBL_RET_CODE ret = SBL_OK;
    IAP_STATUS_CODE iap_ret = CMD_SUCCESS;
    uint32_t flash_memory = SBL_SECTOR0_START;
    uint32_t sec_num = 0;
    uint32_t block_num = 0;
    uint8_t* Sector0_Buffer = (uint8_t*)SBL_RAM_BUFFER_ADDR;
    
    __disable_irq();
    /* Loop to receive Sector0 data */
    while(1)
    {
        /* Read a block of data */
        ret = SECTOR0_RecvBlock(&block_num);

        /* Check result */
        if(ret == SBL_CHECKSUM_ERR)
        {
            iap_ret = (IAP_STATUS_CODE)-1;
            /* Checksum is incorrect */
            goto send_status;
        }

        /* End of receiving Sector0 data */
        if(ret == SBL_SECTOR0_UPGRADE_END)
        {
            return SBL_OK;
        }
        
        if(ret != SBL_OK)
        {
            return ret;
        }
    
        /* Write flash */
        flash_memory = SBL_SECTOR0_START + (block_num<<10);

        sec_num = GetSecNum(flash_memory);
        iap_ret = CMD_SUCCESS;
        
        /* Erase sectos ( if any) */
        if(block_num%4 == 0) // first block of a sector
        {
            iap_ret = EraseSector(sec_num,sec_num);
        }
         
        /* Write Sector0 data */
        if( iap_ret == CMD_SUCCESS)
        {
            iap_ret = CopyRAM2Flash((uint8_t*)flash_memory,Sector0_Buffer,(IAP_WRITE_SIZE)SBL_FW_DATA_BLOCK_SIZE);
        }

     send_status:
        // Send status
        if( iap_ret == CMD_SUCCESS)
        {
            SECTOR0_SendStatus(SBL_Status_OK);
        }
        else
        {
            SECTOR0_SendStatus(SBL_Status_ERR);
            ret = SBL_FLASH_WRITE_ERR;
        }
        
    }

}

/*********************************************************************//**
 * @brief      Receive Sector0 data from master
 * @param[out] pBlockNum pointer to the buffer which is used to store the block number of the received data.
 * @return     SBL_OK/SBL_INVALID_PARAM/SBL_CHECKSUM_ERR/SBL_SECTOR0_UPGRADE_END
 **********************************************************************/
SBL_RET_CODE SECTOR0_RecvBlock(uint32_t* pBlockNum)
{
    uint8_t checksum = 0;
    uint8_t FnID, MSB, LSB;
    SBL_RET_CODE ret;
    uint8_t tmp;
    uint32_t j = 0;
    SBL_TRANSER_Type transfer;
    uint8_t* Sector0_Buffer = (uint8_t*)SBL_RAM_BUFFER_ADDR;

    if(pBlockNum == NULL)
        return SBL_INVALID_PARAM;

    transfer.TxBuf = NULL;
    transfer.TxLen = 0;
    transfer.RxBuf = Sector0_Buffer;
    transfer.RxLen = SBL_Sector0Upgrade_Data_Idx;
    transfer.Type = START_PACKET;
    ret = SBL_ReadWrite(&transfer);
    if(ret != SBL_OK)
        return ret;
    
    /* FnID */
    FnID = Sector0_Buffer[SBL_Sector0Upgrade_FnID_Idx];
    if(FnID != SBL_Sector0Upgrade_FnID)
        return SBL_FN_ID_ERR;

    checksum -= FnID;

    /* MSB */
    MSB = Sector0_Buffer[SBL_Sector0Upgrade_MSB_Idx];
    checksum -= MSB;

    /* LSB */
    LSB = Sector0_Buffer[SBL_Sector0Upgrade_LSB_Idx];
    checksum -= LSB;    

    if(MSB == 0 && LSB == 0) // End of data
    {
         /* checksum */
        transfer.RxBuf = &tmp;
        transfer.RxLen = 1;
        transfer.Type = STOP_PACKET;
        ret = SBL_ReadWrite(&transfer);
        if(ret != SBL_OK)
            return ret;
        
        if(checksum != tmp)
            return SBL_CHECKSUM_ERR;

        return SBL_SECTOR0_UPGRADE_END;
    }

    /* Fw Data */
    transfer.RxBuf = Sector0_Buffer;
    transfer.RxLen = SBL_FW_DATA_BLOCK_SIZE;
    transfer.Type = MID_PACKET;
    ret = SBL_ReadWrite(&transfer);
    if(ret != SBL_OK)
        return ret;

    /* Check sum receive */
    transfer.RxBuf = &tmp;
    transfer.RxLen = 1;
    transfer.Type = STOP_PACKET;
    ret = SBL_ReadWrite(&transfer);
    if(ret != SBL_OK)
       return ret;

    *pBlockNum = ((MSB<<8)|LSB)-1;

    for(j = 0; j < SBL_FW_DATA_BLOCK_SIZE;j++)
    {
        checksum -= Sector0_Buffer[j];
    }

    /* checksum */
    if(checksum != tmp)
        return SBL_CHECKSUM_ERR;

    return SBL_OK;
}

/*********************************************************************//**
 * @brief      Send status to master
 * @param[in]  status     The status which will be sent.
 * @return     SBL_OK/SBL_COMM_ERR
 **********************************************************************/
SBL_RET_CODE SECTOR0_SendStatus(SBL_STATUS status)
{
    uint8_t arrStatus[SBL_Status_ParamSize];
    uint32_t tmp = 0;
    SBL_RET_CODE ret;

    SBL_TRANSER_Type transfer;

    /* Prepare buffer to send */
    for(tmp = 0; tmp < SBL_Status_ParamSize; tmp++)
        arrStatus[tmp] = status;

    /* Send to master */
    transfer.TxBuf = arrStatus;
    transfer.TxLen = SBL_Status_ParamSize;
    transfer.RxBuf = NULL;
    transfer.RxLen = 0;
    transfer.Type = STOP_PACKET;
    ret = SBL_ReadWrite(&transfer);
    return ret;
}



