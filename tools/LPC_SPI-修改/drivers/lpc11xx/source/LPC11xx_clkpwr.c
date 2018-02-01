/**
 * @file     : lpc11xx_clkpwr.c
 * @brief    : Contains all functions support for system clock and power control
 *                 firmware library on LPC11xx.
 * @version  : 1.0
 * @date     : 05. Nov. 2009
 * @author   : HieuNguyen
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
 **********************************************************************/
/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup CLKPWR
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc_types.h"
#include "lpc11xx_clkpwr.h"



/* Public Functions ----------------------------------------------------------- */
/** @addtogroup CLKPWR_Public_Functions
 * @{
 */


/***************************************************************************//**
 * @brief        Configures System AHB clock control
 * @param[in]    AHBClk_Type    AHB clock type
 * @param[in]    CmdState       Command State, should be:
 *                              - ENABLE: Enable clock for this AHB clock type
 *                              - DISABLE: Disable clock for this AHB clock type
 * @return        None
 *****************************************************************************/
void CLKPWR_ConfigAHBClock(uint32_t AHBClk_Type, FunctionalState CmdState)
{
    if (CmdState == ENABLE){
        LPC_SYSCON->SYSAHBCLKCTRL |= (AHBClk_Type);
    } else {
        LPC_SYSCON->SYSAHBCLKCTRL &= ~(AHBClk_Type);
    }
}


/***************************************************************************//**
 * @brief        Enter Power Down for a specified peripheral immediately
 * @param[in]    PDtype      Specified peripheral for power down configuration
 * @param[in]    CmdState    Command State, should be:
 *                             - ENABLE: Enable power down for this peripheral
 *                             - DISABLE: Disable power down for this peripheral
 * @return        None
 *****************************************************************************/
void CLKPWR_ConfigPowerDown(uint32_t PDtype, FunctionalState CmdState)
{
    if (CmdState == ENABLE){
        LPC_SYSCON->PDRUNCFG |= (PDtype);
    } else {
        LPC_SYSCON->PDRUNCFG &= ~(PDtype);
    }
}


/***************************************************************************//**
 * @brief        Configures wake up for specified peripheral to indicate
 *               the state the chip must enter when it is waking up from
 *               Deep-sleep mode.
 * @param[in]    WUtype        Specified peripheral for wake up configuration
 * @param[in]    CmdState      Command State, should be:
 *                             - ENABLE: Enable power down for this peripheral
 *                             after waking up
 *                             - DISABLE: Disable power down for this peripheral
 *                             after waking up
 * @return       None
 *****************************************************************************/
void CLKPWR_ConfigWakeUp(uint32_t WUtype, FunctionalState CmdState)
{
    if (CmdState == ENABLE){
        LPC_SYSCON->PDRUNCFG |= (WUtype);
    } else {
        LPC_SYSCON->PDRUNCFG &= ~(WUtype);
    }
}


/***************************************************************************//**
 * @brief        Configures Deep sleep for specified peripheral to indicate
 *                 the state the chip must enter when Deep-sleep mode is
 *                 asserted by the ARM.
 * @param[in]    DStype      Specified peripheral for Deep sleep configuration
 * @param[in]    CmdState    Command State, should be:
 *                           - ENABLE: Enable power down for this peripheral
 *                           when the Deep sleep mode is entered
 *                           - DISABLE: Disable power down for this peripheral
 *                           when the Deep sleep mode is entered
 * @return        None
 *****************************************************************************/
void CLKPWR_ConfigDeepSleep(uint32_t DStype, FunctionalState CmdState)
{
    if (CmdState == ENABLE){
        LPC_SYSCON->PDRUNCFG |= (DStype);
    } else {
        LPC_SYSCON->PDRUNCFG &= ~(DStype);
    }
}


/***************************************************************************//**
 * @brief        Executes a reset command to a specified peripheral.
 * @param[in]    PReset      Peripheral type.
 * @param[in]    CmdState    Command State, should be:
 *                           - ENABLE.
 *                           - DISABLE.
 * @return       None
 *****************************************************************************/
void CLKPWR_ResetPeripheralCmd(uint32_t PReset, FunctionalState CmdState)
{
    if (CmdState == ENABLE){
        LPC_SYSCON->PRESETCTRL &= ~(PReset);
    } else {
        LPC_SYSCON->PRESETCTRL |= (PReset);
    }
}


/***************************************************************************//**
 * @brief        Set clock divider value for SSP peripheral.
 * @param[in]    SSPx SSP peripheral, should be LPC_SSP0 or LPC_SSP1
 * @param[in]    div    Divider value, from 1 to 255, value '0' is used
 *                      to disable clock source for SSP
 * @return       None
 *****************************************************************************/
void CLKPWR_SetSSPClockDiv(uint8_t SSPx, uint32_t div)
{
    if(SSPx == 0)
        LPC_SYSCON->SSP0CLKDIV = div;
    else
        LPC_SYSCON->SSP1CLKDIV = div;
}


/***************************************************************************//**
 * @brief        Set clock divider value for UART peripheral.
 * @param[in]    div    Divider value, from 1 to 255, value '0' is used
 *                      to disable clock source for UART
 * @return       None
 *****************************************************************************/
void CLKPWR_SetUARTClockDiv(uint32_t div)
{
    LPC_SYSCON->UARTCLKDIV = div;
}



// TODO: Start logic, POR, SYSTICK divider, System reset status.
// TODO: the following functions not yet tested...



/***************************************************************************//**
 * @brief        Enter Sleep mode with co-operated instruction by the Cortex-M3.
 * @param[in]    None
 * @return       None
 *****************************************************************************/
void PMU_Sleep(void)
{
    LPC_PMU->PCON = 0x00;
    /* Sleep Mode*/
    __WFI();
}

/***************************************************************************//**
 * @brief         Enter Deep Power Down mode with co-operated instruction by the Cortex-M3.
 * @param[in]     None
 * @return        None
 *****************************************************************************/
void PMU_DeepPowerDown(void)
{
    /* Deep-Sleep Mode, set SLEEPDEEP bit */
    // SCB->SCR = 0x4;
    LPC_PMU->PCON = PMU_PCON_DPDEN;
    /* Sleep Mode*/
    __WFI();
}
/**
 * @}
 */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
