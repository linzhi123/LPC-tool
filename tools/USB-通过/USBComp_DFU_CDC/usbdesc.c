/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing 
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Changed string descriptor handling
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/
#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbdesc.h"

#include "dfuuser.h"
#include "cdc.h"
#include "cdcuser.h"


/* USB Standard Device Descriptor */
__align(4) const uint8_t USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
#if LPM_SUPPORT
  WBVAL(0x0201), /* 2.01 */          /* bcdUSB */
#else
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
#endif
  USB_DEVICE_CLASS_MISCELLANEOUS,    /* bDeviceClass */
  0x02,                              /* bDeviceSubClass */
  0x01,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0x1FC9),                     /* idVendor */
  WBVAL(0x500B),                     /* idProduct */
  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations: one possible configuration*/
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
const uint8_t USB_ConfigDescriptor[] = { 
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    USB_DFU_DESCRIPTOR_SIZE      +  
	1*USB_INTERFACE_ASSOCIATION_DESC_SIZE +  /* interface association */
    1*USB_INTERFACE_DESC_SIZE     +  /* communication interface */
    0x0013                        +  /* CDC functions */
    1*USB_ENDPOINT_DESC_SIZE      +  /* interrupt endpoint */
    1*USB_INTERFACE_DESC_SIZE     +  /* data interface */
    2*USB_ENDPOINT_DESC_SIZE         /* bulk endpoints */	
	
  ),
  0x03,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
#if REMOTE_WAKEUP_ENABLE			 /* bmAttributes */
  USB_CONFIG_SELF_POWERED | USB_CONFIG_REMOTE_WAKEUP, 
#else
  USB_CONFIG_SELF_POWERED,
#endif
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
  
/* Interface 0, Alternate Setting 0, DFU Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_DFU_IF_NUM /*0x00*/,           /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x00,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_APP,              /* bInterfaceClass */
  USB_DFU_SUBCLASS,                  /* bInterfaceSubClass */
  0x01,                               /* bInterfaceProtocol */
  0x04,                              /* iInterface */
/* DFU RunTime/DFU Mode Functional Descriptor */
  USB_DFU_DESCRIPTOR_SIZE,                /* bLength */
  USB_DFU_DESCRIPTOR_TYPE,           /* bDescriptorType */
  USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL | USB_DFU_WILL_DETACH, /* bmAttributes */
  WBVAL(0xFF00),                     /* wDetachTimeout */
  WBVAL(USB_DFU_XFER_SIZE),          /* wTransferSize */
  WBVAL(0x100),                      /* bcdDFUVersion */
	/* IAD to associate the two CDC interfaces */
	USB_INTERFACE_ASSOCIATION_DESC_SIZE,       /* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_CIF_NUM,                   /* bFirstInterface */
	2,                                 /* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS, /* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL,        /* bFunctionSubClass */
	0,                                 /* bFunctionProtocol */
	0x05,                              /* iFunction (Index of string descriptor describing this function) */
	
 	
/* Interface 1, Alternate Setting 0, Communication class interface descriptor */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_CDC_CIF_NUM,                   /* bInterfaceNumber: Number of Interface */
  0x00,                              /* bAlternateSetting: Alternate setting */
  0x01,                              /* bNumEndpoints: One endpoint used */
  CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass: Communication Interface Class */
  CDC_ABSTRACT_CONTROL_MODEL,        /* bInterfaceSubClass: Abstract Control Model */
  0x00,                              /* bInterfaceProtocol: no protocol used */
  0x05,                              /* iInterface: */
/* Header Functional Descriptor */
  0x05,                              /* bLength: Endpoint Descriptor size */
  CDC_CS_INTERFACE,                  /* bDescriptorType: CS_INTERFACE */
  CDC_HEADER,                        /* bDescriptorSubtype: Header Func Desc */
  WBVAL(CDC_V1_10), /* 1.10 */       /* bcdCDC */
/*Call Management Functional Descriptor*/
  0x05,                              /* bFunctionLength */
  CDC_CS_INTERFACE,                  /* bDescriptorType: CS_INTERFACE */
  CDC_CALL_MANAGEMENT,               /* bDescriptorSubtype: Call Management Func Desc */
  0x01,                              /* bmCapabilities: device handles call management */
  USB_CDC_DIF_NUM,                   /* bDataInterface: CDC data IF ID */
/*Abstract Control Management Functional Descriptor*/
  0x04,                              /* bFunctionLength */
  CDC_CS_INTERFACE,                  /* bDescriptorType: CS_INTERFACE */
  CDC_ABSTRACT_CONTROL_MANAGEMENT,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,                              /* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */
/*Union Functional Descriptor*/
  0x05,                              /* bFunctionLength */
  CDC_CS_INTERFACE,                  /* bDescriptorType: CS_INTERFACE */
  CDC_UNION,                         /* bDescriptorSubtype: Union func desc */
  USB_CDC_CIF_NUM,                   /* bMasterInterface: Communication class interface is master */
  USB_CDC_DIF_NUM,                   /* bSlaveInterface0: Data class interface is slave 0 */
/*Endpoint 1 Descriptor*/            /* event notification (optional) */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  CDC_CEP_IN,                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0010),                     /* wMaxPacketSize */
  0x10,          /* 16ms */           /* bInterval */
/* Interface 2, Alternate Setting 0, Data class interface descriptor*/
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_CDC_DIF_NUM,                   /* bInterfaceNumber: Number of Interface */
  0x00,                              /* bAlternateSetting: no alternate setting */
  0x02,                              /* bNumEndpoints: two endpoints used */
  CDC_DATA_INTERFACE_CLASS,          /* bInterfaceClass: Data Interface Class */
  0x00,                              /* bInterfaceSubClass: no subclass available */
  0x00,                              /* bInterfaceProtocol: no protocol used */
  0x06,                              /* iInterface */
/* Endpoint, EP5 Bulk OUT */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  CDC_DEP_OUT,               		/* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(USB_CDC_BUFSIZE),            /* wMaxPacketSize */
  0x00,                              /* bInterval: ignore for Bulk transfer */
/* Endpoint, EP5 Bulk In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  CDC_DEP_IN,		                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(USB_CDC_BUFSIZE),            /* wMaxPacketSize */
  0x00,                              /* bInterval: ignore for Bulk transfer */
    
/* Terminator */
  0                                  /* bLength */
};


__align(4) const uint8_t USB_dfuConfigDescriptor[] = {
  /* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    USB_DFU_DESCRIPTOR_SIZE            
  ),
  0x01,                              /* bNumInterfaces */
  0x02,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_SELF_POWERED /*|*/       /* bmAttributes */
  /*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
/* Interface 0, Alternate Setting 0, DFU Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x00,                              /* bNumEndpoints */
  0xFE/*USB_DEVICE_CLASS_APP*/,              /* bInterfaceClass */
  0x01/*USB_DFU_SUBCLASS*/,                  /* bInterfaceSubClass */
  0x02,                              /* bInterfaceProtocol */  /* 02: DFU mode */
  0x04,                              /* iInterface */
/* DFU RunTime/DFU Mode Functional Descriptor */ 
  USB_DFU_DESCRIPTOR_SIZE,                /* bLength */
  USB_DFU_DESCRIPTOR_TYPE,           /* bDescriptorType */ 
  USB_DFU_CAN_DOWNLOAD | USB_DFU_CAN_UPLOAD | USB_DFU_MANIFEST_TOL,
  WBVAL(0xFF00),                     /* wDetachTimeout */
  WBVAL(USB_DFU_XFER_SIZE),          /* wTransferSize */
  WBVAL(0x100),                      /* bcdDFUVersion */
  /* Terminator */
  0                                  /* bLength */
};

/* USB String Descriptor (optional) */
const uint8_t USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
/* Index 0x01: Manufacturer */
  (18*2 + 2),                        /* bLength (18 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N', 0,
  'X', 0,
  'P', 0,
  ' ', 0,
  'S', 0,
  'e', 0,
  'm', 0,
  'i', 0,
  'c', 0,
  'o', 0,
  'n', 0,
  'd', 0,
  'u', 0,
  'c', 0,
  't', 0,
  'o', 0,
  'r', 0,
  's', 0,
/* Index 0x02: Product */
  (21*2 + 2),                        /* bLength ( 21 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '1',0,
  '1',0,
  'U',0,
  'x',0,
  'x',0,
  ' ',0,
  'U',0,
  'S',0,
  'B',0,
  ' ',0,
  'C',0,
  'o',0,
  'm',0,
  'p',0,
  'o',0,
  's',0,
  'i',0,
  't',0,  
/* Index 0x03: Serial Number */
  (12*2 + 2),                        /* bLength (12 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'D',0,
  'E',0,
  'M',0,
  'O',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
/* Index 0x04: Interface 0, Alternate Setting 0 */
  ( 12*2 + 2),                        /* bLength (6 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '1',0,
  '1',0,
  'U',0,
  'x',0,
  'x',0,
  ' ',0,
  'D',0,
  'F',0,
  'U',0,
	/* Index 0x05: Interface 1, Alternate Setting 0 */
	( 8*2 + 2),                        /* bLength (4 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	'C',0,
	'O',0,
	'M',0,
	'/',0,
	'D',0,
	'A',0,
	'T',0,
	'A',0, 
/* Index 0x06: Interface 1, Alternate Setting 0 */
  ( 4*2 + 2),                        /* bLength (4 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'V',0,
  'C',0,
  'O',0,
  'M',0,  
};

/* USB Device Qualifier */
/* This is a tricky one again. On some version of USBCV compliance test,
if we support LPM, bcdUSB should be 0x0201, the USBCV will ask
for device qualifier info. If the device doesn't reply(return false), 
USBCV will fail. However, if the device replies with bcdUSB 0x0200 
or above, on the WIN XP system, the host would think it's a HS device 
attached. So, the device will be qualified as bcdUSB value 0x0100 or 
USB1.1. */   
const uint8_t USB_DeviceQualifier[] = {
  USB_DEVICE_QUALI_SIZE,             	/* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
#if LPM_SUPPORT
  WBVAL(0x0201), /* 2.01 */          /* bcdUSB */
//  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
#else
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
#endif
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  0x00,                              /* bNumOtherSpeedConfigurations */
  0x00                               /* bReserved */
};

#if LPM_SUPPORT
/* In order to test Link Power Management(LPM), bcdUSB needs to be 0x0201(USB 2.0
Extension) instead of 0x0200. Below BOS descriptor is for LPM test only. However, 
current USBC2.0 CV test requires both Super Speed Device Capability Descriptor
and Device Capability descriptor to be present and wSpeedSupport should be 0x000F 
instead of 0x0003 indicating it's a Super Speed Device or USBCV test will fail. 
For USB2.0 extention and LPM support, the device is not necessary a Super Speed 
Device. To be discussed with USB-IF. */

/* USB BOS Descriptor */
const uint8_t USB_BOSDescriptor[] = {
  USB_BOS_DESC_SIZE,                 /* bLength */
  USB_BOS_TYPE,                      /* bDescriptorType, BOS */
  WBVAL(                             /* wTotalLength */
    USB_BOS_DESC_SIZE +
    USB_SS_DEVICE_CAPABILITY_SIZE +
    USB_DEVICE_CAPABILITY_SIZE
  ),
  0x02,                               /* bNumDeviceCaps, Two Device Capability desc. */
  /* Device Capabilities, USB 2.0 Extension Descriptor */
  USB_SS_DEVICE_CAPABILITY_SIZE,     /* bLength */
  USB_DEVICE_CAPABILITY_TYPE,        /* bDescriptorType, DEVICE CAPABILITY */
  0x03,                              /* bDeviceCapabilityType, USB Super Speed */
  0x00,                              /* bmAttributes */
  WBVAL(0x000F),                     /* wSpeedsSupported, All speeds are supported. */
  0x02,                              /* bFunctionalitySupport, Full Speed */
  0x00,                              /* bU1DevExitLat */
  WBVAL(0x0000),                     /* bU2DevExitLat */
  /* Device Capabilities, USB 2.0 Extension Descriptor */
  USB_DEVICE_CAPABILITY_SIZE,        /* bLength */
  USB_DEVICE_CAPABILITY_TYPE,        /* bDescriptorType, DEVICE CAPABILITY */
  0x02,                              /* bDeviceCapabilityType, USB2.0 EXTENSION */
  (0x1<<1),                          /* bmAttributes, bit1 to 1, LPM supported. */
  0x00,
  0x00,
  0x00
};
#endif

const uint8_t *pConfigDescriptor = (uint8_t *)USB_ConfigDescriptor; 

