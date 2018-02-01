/**********************************************************************
* $Id$        sbl_master.c            2012-05-04
*//**
* @file       sbl_master.c    
* @brief      SBL Master.
* @version    1.0
* @date       04. May. 2012
* @author     NXP MCU SW Application Team
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

#include "sbl_master.h"

/** Send Function ID to Slave */
SBL_RET_CODE SBL_MasterSendFnID(SBL_FUNC_ID FnID);
/** Send Firmware Data to Slave */
SBL_RET_CODE SBL_MasterSendFwData(Bool UpgradeSector0, uint32_t BlockNum);
/** Inform to Slave about there is no more firmware data */
SBL_RET_CODE SBL_MasterSendFwDataDone(Bool UpgradeSector0);
extern uint32_t SystemCoreClock;
/*********************************************************************//**
 * @brief        SBL Initialize for master
 * @param[in]    None
 * @return       SBL_OK/SBL_INVALID_PARAM
 **********************************************************************/
//SBL_RET_CODE SBL_MasterInit(void)
//{
//    return SBL_Init();
//}

/*********************************************************************//**
 * @brief        Delay for a while
 * @param[in]    ms  miliseconds
 * @return       None
 **********************************************************************/
 void delay(uint32_t ms)
{
    volatile uint32_t i =0;
    uint32_t loop_1ms = SystemCoreClock/(1000*6);
    for(;ms>0;ms--)
        for(i = 0; i < loop_1ms; i++);
}

/*********************************************************************//**
 * @brief        Send a byte to slave
 * @param[in]    byte        data will be sent
 * @return       SBL_OK/SBL_COMM_ERR
 **********************************************************************/
SBL_RET_CODE write_byte(uint8_t byte)
{
    SBL_TRANSER_Type transfer;
    
    /* Send Function ID*/
    transfer.TxBuf = &byte;
    transfer.TxLen = 1;
    transfer.RxBuf = NULL;
    transfer.RxLen = 0;
    return SBL_ReadWrite(&transfer);
}

///*********************************************************************//**
// * @brief        Send function ID to slave
// * @param[in]    FnID        Function ID
// * @return       SBL_OK/SBL_COMM_ERR
// **********************************************************************/
//SBL_RET_CODE SBL_MasterSendFnID(SBL_FUNC_ID FnID)
//{
//    uint8_t ready = 0;
//    int32_t timeout = SBL_TIMEOUT;
//    SBL_RET_CODE ret;
//    SBL_TRANSER_Type transfer;
//    
//    transfer.TxBuf = NULL;
//    transfer.TxLen = 0;
//    transfer.RxBuf = &ready;
//    transfer.RxLen = 1;

//    /* Wait for SBL Slave ready */
//    while((ready != SBL_READY_BYTE)&&(timeout>=0))
//    {
//        ret = SBL_ReadWrite(&transfer);
//        if(ret != SBL_OK)
//            return ret;
//        timeout-= 10;
//        delay(SBL_MST_SLAVE_RDY_DELAY);
//    }

//    if(timeout < 0)
//        return SBL_TIMEOUT_ERR;

//    delay(SBL_MST_SLAVE_RDY_DELAY);

//    ret = write_byte(FnID);
//    return ret;
//}

///*********************************************************************//**
// * @brief        Read Firmware Version ID from Slave
// * @param[out]   pFirmVers        pointer to  buffer storing firmware version.
// * @return       SBL_OK/SBL_INVALID_PARAM/SBL_COMM_ERR
// **********************************************************************/
//SBL_RET_CODE SBL_MasterReadFwVersionID(PSBL_FirmVerion_Type pFirmVers)
//{
//    SBL_RET_CODE ret;
//    SBL_TRANSER_Type transfer;
//    
//    if(pFirmVers == NULL)
//        return SBL_INVALID_PARAM;

//    {
//        /* Send Function ID*/
//        ret = SBL_MasterSendFnID(SBL_ReadFirmVerionID_FnID);
//        if(ret != SBL_OK)
//            return ret;
//        
//        delay(SBL_MST_SLAVE_RDY_DELAY);
//        transfer.TxBuf = NULL;
//        transfer.TxLen = 0;
//        transfer.RxBuf = (uint8_t*)pFirmVers;
//        transfer.RxLen = SBL_FwVersionID_ParamSize;
//        ret = SBL_ReadWrite(&transfer);
//    }
//    return ret;
//}

/*********************************************************************//**
 * @brief        Upgrade firmware for slave
 * @param[in]    pCallbacks    callback functions.
 * @return       SBL_OK/SBL_COMM_ERR/SBL_FLASH_WRITE_ERR
 **********************************************************************/
SBL_RET_CODE SBL_MasterUpgradeFirmware(Bool UpgradeSector0,PSBL_CB_Type pCallbacks)
{
    SBL_RET_CODE ret;
    uint32_t i;
    uint32_t blockNum = 0;
    uint8_t status_cnt, fnID;
    uint32_t retries = 0;
    uint32_t timeout;
    SBL_TRANSER_Type transfer;
    uint8_t status[SBL_Status_ParamSize];
    uint8_t* Fw_Data, *Master_Buffer;

    transfer.TxBuf = NULL;
    transfer.TxLen = 0;
    transfer.RxBuf = status;
    transfer.RxLen = SBL_Status_ParamSize;


    // Send Function ID
    fnID = (UpgradeSector0) ? SBL_Sector0Upgrade_FnID:SBL_FwUpgrade_FnID;
    ret = SBL_MasterSendFnID(fnID);
    if(ret != SBL_OK)
        return ret;   

    // Wait for slave erase last sector
    delay(SBL_MST_ERASE_FLASH_DELAY);

    /* Send Firmware data */
    while(1)  
    {
        if((UpgradeSector0 && (blockNum >= SBL_SECTOR0_BLOCK_NUM))||
            (!UpgradeSector0 && (blockNum >= SBL_FW_DATA_BLOCK_NUM)))
        {
            break;
        }
        Fw_Data = (uint8_t*)SBL_FIRMWARE_ADDR + (blockNum<<10);
        Master_Buffer = (uint8_t*) SBL_RAM_BUFFER_ADDR;
        for(i = 0; i < SBL_FW_DATA_BLOCK_SIZE; i++)
        {
            Master_Buffer[i]  = Fw_Data[i];
        }
    
        while(retries < SBL_RETRIES_MAX_NUM)
        {
            /* Wait for a while */
            delay(SBL_MST_SLAVE_RDY_DELAY);
            
            if(pCallbacks && pCallbacks->UpgradeStartPFN)
            {
                (pCallbacks->UpgradeStartPFN)(blockNum);
            }
            
            ret = SBL_MasterSendFwData(UpgradeSector0, blockNum+1);
            if(ret != SBL_OK)
                goto retry;
            
            /* Read status */
            status_cnt = 0;
             /* Wait for a while */
            delay(SBL_MST_PROG_FLASH_DELAY);

            for(timeout = 0;timeout < (SBL_MST_PROG_FLASH_TIMEOUT);timeout+=SBL_MST_SLAVE_RDY_DELAY)
            {
                for(i = 0; i < SBL_Status_ParamSize; i++)
                    status[i] = 0xFF;

                /* Check Slave status */
                ret = SBL_ReadWrite(&transfer);
                for(i = 0; i < SBL_Status_ParamSize; i++)
                {
                    if(status[i] == SBL_Status_OK)
                    {
                        if(++status_cnt >= SBL_Status_ParamSize)
                            goto send_next_block;
                    }
                }
                /* Wait for a while */                            
                delay(SBL_MST_SLAVE_RDY_DELAY);
            }
            ret = SBL_FLASH_WRITE_ERR;
retry:
            retries++;
        }
        return ret;
send_next_block:        
        retries = 0;
        blockNum++;
    }

    /* Wait for a while */
    delay(SBL_MST_SLAVE_RDY_DELAY);
    /* No more data to send */
    ret = SBL_MasterSendFwData(UpgradeSector0,0);

    return ret;
}

/*********************************************************************//**
 * @brief        Send Firmware data to Slave
 * @param[in]    BlockNum    
 *                  0: No more data to send
 *                  >0: Block number
 * @return       SBL_OK/SBL_COMM_ERR
 **********************************************************************/
SBL_RET_CODE SBL_MasterSendFwData(Bool UpgradeSector0, uint32_t BlockNum)
{
    uint8_t checksum = 0;
    SBL_RET_CODE ret;
    uint32_t j = 0;
    SBL_TRANSER_Type transfer;
    uint8_t header[4];
    uint8_t* Fw_Data;

    
    if(UpgradeSector0)
    {
         /* FnID */
        checksum -= SBL_Sector0Upgrade_FnID; // decrement MSB from the checksum
        header[j++] = SBL_Sector0Upgrade_FnID;
    }

    /* MSB */
    checksum -= ((BlockNum) >> 8); // decrement MSB from the checksum
    header[j++] = ((BlockNum) >> 8);

    /* LSB */
    checksum -= ((BlockNum) & 0xFF);; // decrement LSB from the checksum
    header[j++] = ((BlockNum) & 0xFF);;

    transfer.TxBuf = header;
    transfer.TxLen = j;
    transfer.RxBuf = NULL;
    transfer.RxLen = 0;    
    ret = SBL_ReadWrite(&transfer);
    if(ret != SBL_OK)
        goto send_block_end;

    // No more data
    if(BlockNum == 0)
    {
        goto send_checksum;
    }

    if(BlockNum==0)
        goto send_block_end;

#if (SBL_CONNECTION == SBL_SPI)
    delay(SBL_MST_SLAVE_RDY_DELAY);
#endif

    /* Firmware data */
    Fw_Data = (uint8_t*) SBL_RAM_BUFFER_ADDR;
    for(j = 0; j < SBL_FW_DATA_BLOCK_SIZE;j++)
    {
        checksum -= Fw_Data[j];
    }

    if(UpgradeSector0 &&(BlockNum == 1))
    {
        uint32_t user_code_checksum = 0;
        uint32_t *pWordPtr =  (uint32_t*) Fw_Data;
        for(j = 0; j < 7; j++)
        {
            user_code_checksum -= pWordPtr[j];
        }
        for(j = 28; j < 32; j++)
        {
            checksum += Fw_Data[j];
        }
        pWordPtr[7]  = user_code_checksum;
        for(j = 28; j < 32; j++)
        {
            checksum -= Fw_Data[j];
        }
    }

    transfer.TxBuf =  Fw_Data;
    transfer.TxLen = SBL_FW_DATA_BLOCK_SIZE;
    ret = SBL_ReadWrite(&transfer);
    if(ret != SBL_OK)
        goto send_block_end;

send_checksum:

#if (SBL_CONNECTION == SBL_SPI)
    delay(SBL_MST_SLAVE_RDY_DELAY);
#endif

    ret = write_byte(checksum);
send_block_end:
    return ret;
}

