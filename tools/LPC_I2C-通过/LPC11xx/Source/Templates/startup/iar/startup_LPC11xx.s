;/******************************************************************************
; * @file:    startup_LPC11xx.s
; * @purpose: CMSIS ARM Cortex-M0 Core Device Startup File
;
; * @version: V1.02
; * @date:    31. July 2009
; *---------------------------------------------------------------------------
; *
; * Copyright (C) 2009 ARM Limited. All rights reserved.
; *
; * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/


;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size
        EXTWEAK I2C_IRQHandler

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     0
        DCD     0
        DCD     0
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     0
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.0 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.1 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.2 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.3 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.4 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.5 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.6 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.7 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.8 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.9 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.10 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO0.11 Interrupt
        DCD     WAKEUP_IRQHandler           ; WAKEUP PIO1.0 Interrupt
        DCD     0                           ; Reserved
        DCD     SSP1_IRQHandler             ; SSP1 Interrupt
        DCD     I2C_IRQHandler              ; I2C Interrupt
        DCD     TIMER16_0_IRQHandler        ; 16-bit Counter Timer0 Interrupt
        DCD     TIMER16_1_IRQHandler        ; 16-bit Counter Timer1 Interrupt
        DCD     TIMER32_0_IRQHandler        ; 32-bit Counter Timer0 Interrupt
        DCD     TIMER32_1_IRQHandler        ; 32-bit Counter Timer1 Interrupt
        DCD     SSP0_IRQHandler             ; SSP0 Interrupt
        DCD     UART_IRQHandler             ; UART Interrupt
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     ADC_IRQHandler              ; ADC Interrupt
        DCD     WDT_IRQHandler              ; WDT Interrupt
        DCD     BOD_IRQHandler              ; BOD Interrupt
        DCD     0                           ; Reserved
        DCD     PIOINT3_IRQHandler          ; PIO INT3 Interrupt
        DCD     PIOINT2_IRQHandler          ; PIO INT2 Interrupt
        DCD     PIOINT1_IRQHandler          ; PIO INT1 Interrupt
        DCD     PIOINT0_IRQHandler          ; PIO INT0 Interrupt





__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size 	EQU 	__Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER(1)
HardFault_Handler
        B HardFault_Handler
        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER(1)
SVC_Handler
        B SVC_Handler
        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER(1)
SysTick_Handler
        B SysTick_Handler
        PUBWEAK WAKEUP_IRQHandler
        SECTION .text:CODE:REORDER(1)
WAKEUP_IRQHandler
        B WAKEUP_IRQHandler

        PUBWEAK SSP1_IRQHandler
        SECTION .text:CODE:REORDER(1)
SSP1_IRQHandler
        B SSP1_IRQHandler

;        PUBWEAK I2C_IRQHandler
;        SECTION .text:CODE:REORDER(1)
;I2C_IRQHandler
;        B I2C_IRQHandler

        PUBWEAK TIMER16_0_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER16_0_IRQHandler
        B TIMER16_0_IRQHandler

        PUBWEAK TIMER16_1_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER16_1_IRQHandler
        B TIMER16_1_IRQHandler

        PUBWEAK TIMER32_0_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER32_0_IRQHandler
        B TIMER32_0_IRQHandler

        PUBWEAK TIMER32_1_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER32_1_IRQHandler
        B TIMER32_1_IRQHandler

        PUBWEAK SSP0_IRQHandler
        SECTION .text:CODE:REORDER(1)
SSP0_IRQHandler
        B SSP0_IRQHandler

        PUBWEAK UART_IRQHandler
        SECTION .text:CODE:REORDER(1)
UART_IRQHandler
        B UART_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK WDT_IRQHandler
        SECTION .text:CODE:REORDER(1)
WDT_IRQHandler
        B WDT_IRQHandler

        PUBWEAK BOD_IRQHandler
        SECTION .text:CODE:REORDER(1)
BOD_IRQHandler
        B BOD_IRQHandler

        PUBWEAK PIOINT3_IRQHandler
        SECTION .text:CODE:REORDER(1)
PIOINT3_IRQHandler
        B PIOINT3_IRQHandler

        PUBWEAK PIOINT2_IRQHandler
        SECTION .text:CODE:REORDER(1)
PIOINT2_IRQHandler
        B PIOINT2_IRQHandler

        PUBWEAK PIOINT1_IRQHandler
        SECTION .text:CODE:REORDER(1)
PIOINT1_IRQHandler
        B PIOINT1_IRQHandler

        PUBWEAK PIOINT0_IRQHandler
        SECTION .text:CODE:REORDER(1)
PIOINT0_IRQHandler
        B PIOINT0_IRQHandler
        END


