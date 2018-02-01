/**********************************************************************
* Name:    		dfuuser.c
* Description: 	Device Firmware Upgrade Class Custom User Module
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
#include <string.h>
#include "type.h"

#include "usb.h"
#include "dfu.h"
#include "usbcfg.h"
#include "usbuser.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "dfuuser.h"
#include "sbl_iap.h"
#include "demo.h"

uint8_t DFUDetachReceived = 0;
uint32_t DFUDownloadDone = 0;
uint8_t  DFUState = DFU_STATE_appIDLE;            /* DFU State */
uint8_t  DFUStatus = DFU_STATUS_OK;               /* DFU Status */
uint8_t  DFUBuffer[USB_DFU_XFER_SIZE];
uint8_t *pDfuBuffer = DFUBuffer;
DFU_STATUS_T DFUGetStatusReq;
USB_DFU_FUNC_DESCRIPTOR DFU_FunctionDescriptor;


/*
 * Returns the DFU Protocol Status
 */
/*
 *  DFU get status Callback
 *   DFU get status
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_GetStatus (void) {

  if (DFUState == DFU_STATE_dfuMANIFEST_SYNC) {
  	/* If we're MainfestationTolerant */
    if (DFU_FunctionDescriptor.bmAttributes & USB_DFU_MANIFEST_TOL)
      DFUState = DFU_STATE_dfuIDLE;
    else
      DFUState = DFU_STATE_dfuMANIFEST_WAIT_RST;
    DFUDownloadDone = 1;
  }

  switch (DFUState) {
  	case DFU_STATE_dfuDNLOAD_SYNC:
      /* ***TBD *** block in progress case ???***/
  	case DFU_STATE_dfuDNBUSY:
      DFUState = DFU_STATE_dfuDNLOAD_IDLE;
  		break;
  	default:
  		break;
  }

  /* send status response */
  DFUGetStatusReq.bStatus = DFUStatus;
  DFUGetStatusReq.bState = DFUState;
  DFUGetStatusReq.iString = 0;
  EP0Buf[0] = DFUGetStatusReq.bStatus;
  EP0Buf[1] = DFUGetStatusReq.bwPollTimeout[0];
  EP0Buf[2] = DFUGetStatusReq.bwPollTimeout[1];
  EP0Buf[3] = DFUGetStatusReq.bwPollTimeout[2];
  EP0Buf[4] = DFUGetStatusReq.bState;
  EP0Buf[5] = DFUGetStatusReq.iString;
  return (TRUE);	

}

/*
 *  DFU clear status Callback
 *   DFU clear status
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_ClearStatus (void) {

  if (DFUState == DFU_STATE_dfuERROR) {
    DFUState = DFU_STATE_dfuIDLE;
  	DFUStatus = DFU_STATUS_OK;
  	return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/* return 
* DFU_STATUS_ : values defined in mw_usbd_dfu.h in case of errors
* 0 : If end of memory reached
* length : Amount of data copied to destination buffer
*/
uint32_t DFU_Read( uint32_t block_num, uint8_t *pBuff, uint32_t length)
{
  uint32_t src_addr = DFU_DEST_BASE;

  if ( length != 0 )
  {
    if (block_num == DFU_MAX_BLOCKS)
      return 0;
  
    if (block_num > DFU_MAX_BLOCKS)
      return DFU_STATUS_errADDRESS;
    
    src_addr += (block_num * USB_DFU_XFER_SIZE);
    
    memcpy((void *)(pBuff), (void *)src_addr, length);
  }
  return length;
}


/*
 * Returns the DFU State
 */
/*
 *  DFU get state Callback
 *   DFU get state
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_GetState (void) {

  if (DFUState == DFU_STATE_dfuDNBUSY) {
	DFUState = DFU_STATE_dfuERROR;
    return (FALSE);
  }
  else {
	EP0Buf[0] = DFUState;
    return (TRUE);
  }
}

void DFU_Done(void)
{
  return;  
}

/* return DFU_STATUS_ values defined in mw_usbd_dfu.h */
uint8_t DFU_Write( uint32_t block_num, uint8_t *pBuff, uint32_t length, uint8_t *bwPollTimeout)
{
  uint32_t dest_addr = DFU_DEST_BASE;
	
  bwPollTimeout[0] = 255;
//  bwPollTimeout[1] = 255;
//  bwPollTimeout[2] = 255;
  if ( length != 0 )
  {
    if (block_num >= DFU_MAX_BLOCKS)
      return ( DFU_STATUS_errADDRESS );
    
    dest_addr += (block_num * USB_DFU_XFER_SIZE);
	write_flash((unsigned *)dest_addr, (char *)pBuff, length);
	
  }
  return ( DFU_STATUS_OK );
}

uint32_t DFU_Download(uint32_t block_num, uint32_t len)
{
	/* Store Received Data into Flash or External Memory */
	DFUStatus = DFU_Write( (uint32_t)SetupPacket.wValue.W, &DFUBuffer[0], 
						                SetupPacket.wLength, &DFUGetStatusReq.bwPollTimeout[0]);
	
	if(DFUStatus != DFU_STATUS_OK){	
		DFUState = DFU_STATE_dfuERROR;
		return FALSE; //RET_STALL;
  }
	return TRUE; //RET_ZLP;
}

/*
 *  DFU Detach Callback
 *   DFU Detach
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_Detach(void)
{
	/* Handles USB Cable unplug event */
	if (DFUState == DFU_STATE_appIDLE) {
		DFUState = DFU_STATE_appDETACH;
	}	
	pConfigDescriptor = (uint8_t *)USB_dfuConfigDescriptor;
	DFUDetachReceived = 1;
	return TRUE;
}

/*
 *  DFU Abort Request Callback
 *   Called automatically on DFU ABort Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_Abort (void) {

  switch (DFUState){
    case DFU_STATE_dfuIDLE:
    case DFU_STATE_dfuDNLOAD_SYNC:
    case DFU_STATE_dfuDNLOAD_IDLE:
    case DFU_STATE_dfuMANIFEST_SYNC:
    case DFU_STATE_dfuUPLOAD_IDLE:
      DFUState = DFU_STATE_dfuIDLE;
	  return (TRUE);
    default:
      return (FALSE);
  }
}

/*
 *  DFU Download Callback
 *   DFU Download
 *    Parameters:      Length (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_SetupForDownload (uint32_t blockNum, uint32_t len, uint32_t *eotflag ) {

  *eotflag = FALSE;
  switch (DFUState) {
    case DFU_STATE_dfuIDLE: 
      if (len == 0) { 
        DFUState = DFU_STATE_dfuERROR;
        return (FALSE ); //DFU_EP0_STALL
	  }
    /* fall through */
	case DFU_STATE_dfuDNLOAD_IDLE:
      if (len > DFU_FunctionDescriptor.wTransferSize) {
        /* Too big. Not that we'd really care, but it's a DFU protocol violation */
        DFUState = DFU_STATE_dfuERROR;
        DFUStatus = DFU_STATUS_errADDRESS;
	    return ( FALSE );	// DFU_EP0_STALL
      }
      else {
	    /* end of transfer indicator */
        if (len == 0) {
          DFUState = DFU_STATE_dfuMANIFEST_SYNC;
          *eotflag = TRUE; 
          return ( TRUE ); // DFU_EP0_ZLP 
        }
        DFUState = DFU_STATE_dfuDNLOAD_SYNC;
        DFUStatus = DFU_Write( blockNum, &DFUBuffer[0], 0, &DFUGetStatusReq.bwPollTimeout[0] );
		if ( DFUStatus != DFU_STATUS_OK )
		{
          DFUState = DFU_STATE_dfuERROR;
	      return ( FALSE );	// DFU_EP0_STALL
		}
        /* setup transfer buffer */
	    EP0Data.pData = &DFUBuffer[0];
	    EP0Data.Count = len;
        return(TRUE); // DFU_EP0_DATA
      }
	default:
	  break;
  }   
  return ( FALSE );	// DFU_EP0_STALL
}

/*
 *  DFU Upload Callback
 *   DFU upload
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_Upload(uint32_t blockNum, uint32_t len) {
  uint32_t copy_len;
  
  switch (DFUState){
    case DFU_STATE_dfuIDLE:
      DFUState = DFU_STATE_dfuUPLOAD_IDLE;
    case DFU_STATE_dfuUPLOAD_IDLE:
      /* state transition if less data then requested */
	  if (len > DFU_FunctionDescriptor.wTransferSize) {
		/* Too big. Not that we'd really care, but it's a
		 * DFU protocol violation */
        DFUState = DFU_STATE_dfuERROR;
		DFUStatus = DFU_STATUS_errADDRESS;
		return (FALSE);	// DFU_EP0_STALL
	  }

	  /* Fetch Data from Flash or External Memory */
	  copy_len = DFU_Read( blockNum, &DFUBuffer[0], len);
	  /* Send EOF file frame as short data length packet
	   * which is less than maximum DFU transfer size
	  */		
	  if(copy_len == 0) {
		EP0Data.Count = 0;
		DFUState = DFU_STATE_dfuIDLE;
		return (TRUE); // DFU_EP0_ZLP
	  } 
      else if( copy_len <= DFU_STATUS_errSTALLEDPKT) {
        DFUState = DFU_STATE_dfuERROR;
		DFUStatus = (uint8_t)(copy_len & 0xFF);
		return (FALSE);
      }
	  EP0Data.pData = &DFUBuffer[0];
	  EP0Data.Count = copy_len;

	  /* Send EOF file frame as short data length packet
	   * which is less than maximum DFU transfer size
	  */	
	  if (EP0Data.Count < DFU_FunctionDescriptor.wTransferSize) {
        DFUState = DFU_STATE_dfuIDLE;
//         return (TRUE); 
	  }
      return(TRUE);
    default:
      return(FALSE);
  }
}

/*
 *  DFU reset Callback
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */
uint32_t DFU_Reset(void)
{
	/* Handles USB Cable unplug event */
	if (DFUState == DFU_STATE_appIDLE) {
		DFUState = DFU_STATE_appIDLE;
	}
	else {
		/* after detach & reset the DFU interface should be 0 per DFU 1.1 spec*/
		DFUState = DFU_STATE_dfuIDLE;
// 		if (DFUDetachReceived)
// 		{
// 			pConfigDescriptor = (uint8_t *)USB_dfuConfigDescriptor;		
// 			//DFUDetachReceived = 0;
// 		}
	}	
	
	/* Save a local copy of DFU function descriptor. */
	memcpy((void *)&DFU_FunctionDescriptor, (void *)(pConfigDescriptor/*USB_ConfigDescriptor*/ + DFU_DESC_OFFSET), USB_DFU_DESCRIPTOR_SIZE);
	DFUStatus = DFU_STATUS_OK;
	DFUDownloadDone = 0;
	return TRUE;
}

