; <<< Use Configuration Wizard in Context Menu >>>
;******************************************************************************
;
; startup_LM3S.s - Startup code for use with Keil's uVision.
;
; Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
; Software License Agreement
; 
; Luminary Micro, Inc. (LMI) is supplying this software for use solely and
; exclusively on LMI's microcontroller products.
; 
; The software is owned by LMI and/or its suppliers, and is protected under
; applicable copyright laws.  All rights are reserved.  You may not combine
; this software with "viral" open-source software in order to form a larger
; program.  Any use in violation of the foregoing restrictions may subject
; the user to criminal sanctions under applicable laws, as well as to civil
; liability for the breach of the terms and conditions of this license.
; 
; THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; 
; This is part of revision 32 of the Stellaris CMSIS Package.
;
;******************************************************************************

;******************************************************************************
;
; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Stack   EQU     0x00000200

;******************************************************************************
;
; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Heap    EQU     0x00000000

;******************************************************************************
;
; Allocate space for the stack.
;
;******************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

;******************************************************************************
;
; Allocate space for the heap.
;
;******************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

;******************************************************************************
;
; Indicate that the code in this file preserves 8-byte alignment of the stack.
;
;******************************************************************************
        PRESERVE8

;******************************************************************************
;
; Place code into the reset code section.
;
;******************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;******************************************************************************
;
; The vector table.
;
;******************************************************************************
        EXPORT  __Vectors
__Vectors
        DCD     StackMem + Stack            ; Top of Stack
        DCD     Reset_Handler               ; Reset Handler
        DCD     NMI_Handler                 ; NMI Handler
        DCD     HardFault_Handler           ; Hard Fault Handler
        DCD     MemManage_Handler           ; MPU Fault Handler
        DCD     BusFault_Handler            ; Bus Fault Handler
        DCD     UsageFault_Handler          ; Usage Fault Handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     SVC_Handler                 ; SVCall Handler
        DCD     DebugMon_Handler            ; Debug Monitor Handler
        DCD     0                           ; Reserved
        DCD     PendSV_Handler              ; PendSV Handler
        DCD     SysTick_Handler             ; SysTick Handler

        ; External Interrupts
        DCD     GPIOPortA_IRQHandler        ;  0: GPIO Port A
        DCD     GPIOPortB_IRQHandler        ;  1: GPIO Port B
        DCD     GPIOPortC_IRQHandler        ;  2: GPIO Port C
        DCD     GPIOPortD_IRQHandler        ;  3: GPIO Port D
        DCD     GPIOPortE_IRQHandler        ;  4: GPIO Port E
        DCD     UART0_IRQHandler            ;  5: UART0
        DCD     UART1_IRQHandler            ;  6: UART1
        DCD     SSI0_IRQHandler             ;  7: SSI0
        DCD     I2C0_IRQHandler             ;  8: I2C0
        DCD     PWMFault_IRQHandler         ;  9: PWM Fault
        DCD     PWMGen0_IRQHandler          ; 10: PWM Generator 0
        DCD     PWMGen1_IRQHandler          ; 11: PWM Generator 1
        DCD     PWMGen2_IRQHandler          ; 12: PWM Generator 2
        DCD     QEI0_IRQHandler             ; 13: Quadrature Encoder
        DCD     ADCSeq0_IRQHandler          ; 14: ADC Sequence 0
        DCD     ADCSeq1_IRQHandler          ; 15: ADC Sequence 1
        DCD     ADCSeq2_IRQHandler          ; 16: ADC Sequence 2
        DCD     ADCSeq3_IRQHandler          ; 17: ADC Sequence 3
        DCD     Watchdog_IRQHandler         ; 18: Watchdog
        DCD     Timer0A_IRQHandler          ; 19: Timer 0A
        DCD     Timer0B_IRQHandler          ; 20: Timer 0B
        DCD     Timer1A_IRQHandler          ; 21: Timer 1A
        DCD     Timer1B_IRQHandler          ; 22: Timer 1B
        DCD     Timer2A_IRQHandler          ; 23: Timer 2A
        DCD     Timer2B_IRQHandler          ; 24: Timer 2B
        DCD     Comp0_IRQHandler            ; 25: Comp 0
        DCD     Comp1_IRQHandler            ; 26: Comp 1
        DCD     Comp2_IRQHandler            ; 27: Comp 2
        DCD     SysCtrl_IRQHandler          ; 28: System Control
        DCD     FlashCtrl_IRQHandler        ; 29: Flash Control
        DCD     GPIOPortF_IRQHandler        ; 30: GPIO Port F
        DCD     GPIOPortG_IRQHandler        ; 31: GPIO Port G
        DCD     GPIOPortH_IRQHandler        ; 32: GPIO Port H
        DCD     USART2_IRQHandler           ; 33: UART2 Rx and Tx
        DCD     SSI1_IRQHandler             ; 34: SSI1 Rx and Tx
        DCD     Timer3A_IRQHandler          ; 35: Timer 3 subtimer A
        DCD     Timer3B_IRQHandler          ; 36: Timer 3 subtimer B
        DCD     I2C1_IRQHandler             ; 37: I2C1 Master and Slave
        DCD     QEI1_IRQHandler             ; 38: Quadrature Encoder 1
        DCD     CAN0_IRQHandler             ; 39: CAN0
        DCD     CAN1_IRQHandler             ; 40: CAN1
        DCD     CAN2_IRQHandler             ; 41: CAN2
        DCD     Ethernet_IRQHandler         ; 42: Ethernet
        DCD     Hibernate_IRQHandler        ; 43:Hibernate
        DCD     USB0_IRQHandler             ; 44: USB0
        DCD     PWMGen3_IRQHandler          ; 45: PWM Generator 3
        DCD     uDMA_IRQHandler             ; 46: uDMA Software Transfer
        DCD     uDMAErr_IRQHandler          ; 47: uDMA Error

;******************************************************************************
;
; This is the code that gets called when the processor first starts execution
; following a reset event.
;
;******************************************************************************
        EXPORT  Reset_Handler [WEAK]
Reset_Handler
        ;
        ; Call the C library enty point that handles startup.  This will copy
        ; the .data section initializers from flash to SRAM and zero fill the
        ; .bss section.
        ;
        IMPORT  SystemInit
        IMPORT  __main
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__main
        BX      R0
;        B       __main

;******************************************************************************
;
; This is the code that gets called when the processor receives a NMI.  This
; simply enters an infinite loop, preserving the system state for examination
; by a debugger.
;
;******************************************************************************
        EXPORT NMI_Handler [WEAK]
NMI_Handler
        B       NMI_Handler

;******************************************************************************
;
; This is the code that gets called when the processor receives a fault
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
        EXPORT HardFault_Handler [WEAK]
HardFault_Handler
        B       HardFault_Handler

;******************************************************************************
;
; The rest of the handlers are weakly defined to use the Default_Handler.
; If a handler of the same name is defined in the user's application, then
; the linker will use that one instead.  If the user does not define a handler
; for an interrupt source, and that interrupt occurs, then this is the code
; that gets called.
; This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
        EXPORT  MemManage_Handler       [WEAK]
        EXPORT  BusFault_Handler        [WEAK]
        EXPORT  UsageFault_Handler      [WEAK]
        EXPORT  SVC_Handler             [WEAK]
        EXPORT  DebugMon_Handler        [WEAK]
        EXPORT  PendSV_Handler          [WEAK]
        EXPORT  SysTick_Handler         [WEAK]
        EXPORT  GPIOPortA_IRQHandler    [WEAK]
        EXPORT  GPIOPortB_IRQHandler    [WEAK]
        EXPORT  GPIOPortC_IRQHandler    [WEAK]
        EXPORT  GPIOPortD_IRQHandler    [WEAK]
        EXPORT  GPIOPortE_IRQHandler    [WEAK]
        EXPORT  UART0_IRQHandler        [WEAK]
        EXPORT  UART1_IRQHandler        [WEAK]
        EXPORT  SSI0_IRQHandler         [WEAK]
        EXPORT  I2C0_IRQHandler         [WEAK]
        EXPORT  PWMFault_IRQHandler     [WEAK]
        EXPORT  PWMGen0_IRQHandler      [WEAK]
        EXPORT  PWMGen1_IRQHandler      [WEAK]
        EXPORT  PWMGen2_IRQHandler      [WEAK]
        EXPORT  QEI0_IRQHandler         [WEAK]
        EXPORT  ADCSeq0_IRQHandler      [WEAK]
        EXPORT  ADCSeq1_IRQHandler      [WEAK]
        EXPORT  ADCSeq2_IRQHandler      [WEAK]
        EXPORT  ADCSeq3_IRQHandler      [WEAK]
        EXPORT  Watchdog_IRQHandler     [WEAK]
        EXPORT  Timer0A_IRQHandler      [WEAK]
        EXPORT  Timer0B_IRQHandler      [WEAK]
        EXPORT  Timer1A_IRQHandler      [WEAK]
        EXPORT  Timer1B_IRQHandler      [WEAK]
        EXPORT  Timer2A_IRQHandler      [WEAK]
        EXPORT  Timer2B_IRQHandler      [WEAK]
        EXPORT  Comp0_IRQHandler        [WEAK]
        EXPORT  Comp1_IRQHandler        [WEAK]
        EXPORT  Comp2_IRQHandler        [WEAK]
        EXPORT  SysCtrl_IRQHandler      [WEAK]
        EXPORT  FlashCtrl_IRQHandler    [WEAK]
        EXPORT  GPIOPortF_IRQHandler    [WEAK]
        EXPORT  GPIOPortG_IRQHandler    [WEAK]
        EXPORT  GPIOPortH_IRQHandler    [WEAK]
        EXPORT  USART2_IRQHandler       [WEAK]
        EXPORT  SSI1_IRQHandler         [WEAK]
        EXPORT  Timer3A_IRQHandler      [WEAK]
        EXPORT  Timer3B_IRQHandler      [WEAK]
        EXPORT  I2C1_IRQHandler         [WEAK]
        EXPORT  QEI1_IRQHandler         [WEAK]
        EXPORT  CAN0_IRQHandler         [WEAK]
        EXPORT  CAN1_IRQHandler         [WEAK]
        EXPORT  CAN2_IRQHandler         [WEAK]
        EXPORT  Ethernet_IRQHandler     [WEAK]
        EXPORT  Hibernate_IRQHandler    [WEAK]
        EXPORT  USB0_IRQHandler         [WEAK]
        EXPORT  PWMGen3_IRQHandler      [WEAK]
        EXPORT  uDMA_IRQHandler         [WEAK]
        EXPORT  uDMAErr_IRQHandler      [WEAK]

MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler
GPIOPortA_IRQHandler
GPIOPortB_IRQHandler
GPIOPortC_IRQHandler
GPIOPortD_IRQHandler
GPIOPortE_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
SSI0_IRQHandler
I2C0_IRQHandler
PWMFault_IRQHandler
PWMGen0_IRQHandler
PWMGen1_IRQHandler
PWMGen2_IRQHandler
QEI0_IRQHandler
ADCSeq0_IRQHandler
ADCSeq1_IRQHandler
ADCSeq2_IRQHandler
ADCSeq3_IRQHandler
Watchdog_IRQHandler
Timer0A_IRQHandler
Timer0B_IRQHandler
Timer1A_IRQHandler
Timer1B_IRQHandler
Timer2A_IRQHandler
Timer2B_IRQHandler
Comp0_IRQHandler
Comp1_IRQHandler
Comp2_IRQHandler
SysCtrl_IRQHandler
FlashCtrl_IRQHandler
GPIOPortF_IRQHandler
GPIOPortG_IRQHandler
GPIOPortH_IRQHandler
USART2_IRQHandler
SSI1_IRQHandler
Timer3A_IRQHandler
Timer3B_IRQHandler
I2C1_IRQHandler
QEI1_IRQHandler
CAN0_IRQHandler
CAN1_IRQHandler
CAN2_IRQHandler
Ethernet_IRQHandler
Hibernate_IRQHandler
USB0_IRQHandler
PWMGen3_IRQHandler
uDMA_IRQHandler
uDMAErr_IRQHandler
Default_Handler
        B       Default_Handler

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Some code in the normal code section for initializing the heap and stack.
;
;******************************************************************************
        AREA    |.text|, CODE, READONLY

;******************************************************************************
;
; The function expected of the C library startup code for defining the stack
; and heap memory locations.  For the C library version of the startup code,
; provide this function so that the C library initialization code can find out
; the location of the stack and heap.
;
;******************************************************************************
    IF :DEF: __MICROLIB
        EXPORT  __initial_sp
        EXPORT  __heap_base
        EXPORT  __heap_limit
    ELSE
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR
    ENDIF

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
