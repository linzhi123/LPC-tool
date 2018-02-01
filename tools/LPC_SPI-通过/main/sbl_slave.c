/**********************************************************************
* $Id$		sbl_slave.c			2012-05-04
*//**
* @file		sbl_slave.c	
* @brief	SBL Slave.
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
#include "sbl_slave.h"
#include "sector0_program.h"
#include "includes.h"


/** Data size is used to transfer data */
typedef void (*sbl_api)(uint32_t API, uint8_t* pData);

/** Receive Command */
__IO SBL_FUNC_ID             gCmdRecv;
__IO Bool                    gCmdRecvFlag = FALSE;
__IO AppStatus_Type        gAppStatus = APP_INIT;

/*********************************************************************//**
 * @brief      SBL Initialize for master/slave
 * @param[in]  None.
 * @return     SBL_OK/SBL_INVALID_PARAM
 **********************************************************************/
SBL_RET_CODE SBL_SlaveInit(void)
{
	uint32_t i;
    sbl_api handler;
    handler = (sbl_api) (SBL_API_CALL_LOCATION);
    (handler)(SBL_SLAVE_INIT_FUNC,NULL);
    gAppStatus = APP_INIT;
	// delay for a while
	for(i = 0; i < 10000; i++);

    return SBL_OK;
}

/*********************************************************************//**
 * @brief       De-Initialize a SBL device.
 * @param[in]   None
 * @return      SBL_OK
 **********************************************************************/
SBL_RET_CODE SBL_SlaveDeInit(void)
{
    sbl_api handler;
    handler = (sbl_api) (SBL_API_CALL_LOCATION);
    (handler)(SBL_SLAVE_DEINIT_FUNC,NULL);
    return SBL_OK;
}

/*********************************************************************//**
 * @brief       Check if a command was received.
 * @param[in]   None
 * @return      SBL_OK
 **********************************************************************/
Bool SBL_SlaveCmdRecv(void)
{
    SBL_FUNC_ID tmp;

    while(SBL_TxReady() || SBL_RxReady())
    {
        switch(gAppStatus)
        {
            case APP_INIT:      /* Idle status */
                if(SBL_TxReady())
                {
                    SBL_SendByte(SBL_READY_BYTE );
                    gAppStatus = APP_READY;
                }
                else
                {
                    return FALSE;
                }
                break;
            case APP_READY:      /* Already sent READY byte, start to receive FnID  */
                if( SBL_RxReady())
                {
                    SBL_StartReceive(); 
                    gAppStatus = APP_WAIT_CMD;
                }
                else
                {
                    return FALSE;
                }
                break;
            case APP_WAIT_CMD:
                if(SBL_RxReady())
                {
                    tmp = (SBL_FUNC_ID) SBL_ReceiveByte();
                    gCmdRecv = tmp;
                    gCmdRecvFlag = TRUE;
                    gAppStatus = APP_CMD_RECEIVED;
                }
                else
                {
                    return FALSE;
                }
                break;
            case APP_CMD_RECEIVED:   /* Already received a command, handling the command */
            default:
                return gCmdRecvFlag;
        }
    }      
    return   gCmdRecvFlag;
}
/*********************************************************************//**
 * @brief       Get received command.
 * @param[in]   None
 * @return      SBL_OK
 **********************************************************************/
SBL_FUNC_ID SBL_SlaveGetRecvCmd(void)
{
    return  gCmdRecv;
}
/*********************************************************************//**
 * @brief      SBL Command Handler.
 * @param[in]  cmd     received command
 * @return     None
 **********************************************************************/
void SBL_SlaveCmdHandler(SBL_FUNC_ID cmd)
{
   sbl_api handler;

   switch(cmd)
   {
        case SBL_ReadFirmVerionID_FnID:
        case SBL_FwUpgrade_FnID:
            handler = (sbl_api) (SBL_API_CALL_LOCATION);
            (handler)(SBL_CMD_HANDLER_FUNC,&cmd);
            break;
        case SBL_Sector0Upgrade_FnID:
            // Sector 0 Upgrade
            SECTOR0_UpgradeFw();
            // Reset 
            NVIC_SystemReset();
            break;
        default:
            break;
   }
   gCmdRecvFlag = FALSE;
   gAppStatus = APP_INIT;
}






