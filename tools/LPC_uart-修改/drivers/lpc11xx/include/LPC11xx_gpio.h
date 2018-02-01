/***********************************************************************//**
 * @file	: lpc11xx_gpio.h
 * @brief	: Contains all macro definitions and function prototypes
 * 				support for GPIO firmware library on LPC11xx
 * @version	: 1.0
 * @date	: 30. Nov. 2009
 * @author	: ThieuTrinh
 **************************************************************************
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
 **************************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup GPIO
 * @ingroup LPC1100CMSIS_FwLib_Drivers
 * @{
 */

#ifndef LPC11xx_GPIO_H
#define LPC11xx_GPIO_H


/* Includes ------------------------------------------------------------------- */
#include "LPC11xx.h"
#include "lpc_types.h"


/* Public Macros ----------------------------------------------------------- */
/** @defgroup GPIO_Public_Macros
 * @{
 */
#define PORT0		0			/**< GPIO port 0 */
#define PORT1		1			/**< GPIO port 1 */
#define PORT2		2			/**< GPIO port 2 */
#define PORT3		3			/**< GPIO port 3 */

/**
 * @}
 */


/* Public Macros ----------------------------------------------------------- */
/** @defgroup GPIO_Public_Functions
 * @{
 */

/* GPIO style ------------------------------- */
void GPIO_SetDir(uint8_t portNum, uint32_t bitValue, uint8_t dir);
void GPIO_SetValue(uint8_t portNum, uint32_t bitValue);
void GPIO_ClearValue(uint8_t portNum, uint32_t bitValue);
uint32_t GPIO_ReadValue(uint8_t portNum);

/**
 * @}
 */

#endif /* LPC11xx_GPIO_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
