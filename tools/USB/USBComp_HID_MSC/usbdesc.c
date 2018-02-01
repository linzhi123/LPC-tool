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

#include "msc.h"
#include "mscuser.h"
#include "hid.h"
#include "hiduser.h"

const uint8_t HID_ReportDescriptor[] = {
  HID_UsagePageVendor(0x00),
  HID_Usage(0x01),
  HID_Collection(HID_Application),
    HID_UsagePage(HID_USAGE_PAGE_BUTTON),
    HID_UsageMin(1),
    HID_UsageMax(2),
    HID_LogicalMin(0),
    HID_LogicalMax(1),
    HID_ReportCount(2),
    HID_ReportSize(1),
    HID_Input(HID_Data | HID_Variable | HID_Absolute),
    HID_ReportCount(1),	
    HID_ReportSize(6),
    HID_Input(HID_Constant),
    HID_UsagePage(HID_USAGE_PAGE_LED),
    HID_Usage(HID_USAGE_LED_GENERIC_INDICATOR),
    HID_LogicalMin(0),
    HID_LogicalMax(1),
    HID_ReportCount(8),
    HID_ReportSize(1),
    HID_Output(HID_Data | HID_Variable | HID_Absolute),
  HID_EndCollection,
};
const uint16_t HID_ReportDescSize = sizeof(HID_ReportDescriptor);


/* USB Standard Device Descriptor */
__align(4) const uint8_t USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
#if LPM_SUPPORT
  WBVAL(0x0201), /* 2.01 */          /* bcdUSB */
#else
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
#endif
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0x1FC9),                     /* idVendor */
  WBVAL(0x200B),                     /* idProduct */
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
    HID_DESC_SIZE                 +
    2*USB_ENDPOINT_DESC_SIZE	  +	
    1*USB_INTERFACE_DESC_SIZE     +
    2*USB_ENDPOINT_DESC_SIZE		
  ),
  0x02,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
#if REMOTE_WAKEUP_ENABLE			 /* bmAttributes */
  USB_CONFIG_SELF_POWERED | USB_CONFIG_REMOTE_WAKEUP, 
#else
  USB_CONFIG_SELF_POWERED,
#endif
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
  
/* Interface 0, Alternate Setting 0, HID Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_HID_IF_NUM,                    /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_HUMAN_INTERFACE,  /* bInterfaceClass */
  HID_SUBCLASS_NONE,                 /* bInterfaceSubClass */
  HID_PROTOCOL_NONE,                 /* bInterfaceProtocol */
  0x04,                              /* iInterface */
/* HID Class Descriptor */
  HID_DESC_SIZE,                     /* bLength */
  HID_HID_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(0x0100), /* 1.00 */          /* bcdHID */
  0x00,                              /* bCountryCode */
  HID_REPORT_NUM,                    /* bNumDescriptors */
  HID_REPORT_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(HID_REPORT_DESC_SIZE),         /* wDescriptorLength */
/* Endpoint, HID Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  HID_EP_IN,                         /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0004),                     /* wMaxPacketSize */
  0x20,          /* 16ms */          /* bInterval */
/* Endpoint, HID Interrupt Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  HID_EP_OUT,                        /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0004),                     /* wMaxPacketSize */
  0x20,          /* 16ms */          /* bInterval */  
/* Interface 1, Alternate Setting 0, MSC Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_MSC_IF_NUM,                    /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_STORAGE,          /* bInterfaceClass */
  MSC_SUBCLASS_SCSI,                 /* bInterfaceSubClass */
  MSC_PROTOCOL_BULK_ONLY,            /* bInterfaceProtocol */
  0x05,                              /* iInterface */
/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_IN,                         /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(USB_MAX_NON_ISO_SIZE),       /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_OUT,                        /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(USB_MAX_NON_ISO_SIZE),       /* wMaxPacketSize */
  0,                                 /* bInterval */   
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
  /* Index 0x04: Interface 1, Alternate Setting 0 */
  (12*2 + 2),                        /* bLength (3 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,       /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '1',0,
  '1',0,
  'U',0,
  'x',0,
  'x',0,
  ' ',0,  
  'H', 0,
  'I', 0,
  'D', 0,   
  /* Index 0x05: Interface 2, Alternate Setting 0 */
  (15*2 + 2),                        /* bLength (3 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,       /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '1',0,
  '1',0,
  'U',0,
  'x',0,
  'x',0,
  ' ',0,  
  'M', 0,
  'e', 0,
  'm', 0,  
  'o', 0,
  'r', 0,
  'y', 0,
  /* Index 0x06: Interface 1, Alternate Setting 0 */
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
  /* Index 0x07: Interface 2, Alternate Setting 0 */
  (13*2 + 2),                        /* bLength (3 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,       /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '1',0,
  '1',0,
  'U',0,
  'x',0,
  'x',0,
  ' ',0,  
  'V', 0,
  'C', 0,
  'O', 0,  
  'M', 0, 
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
