/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// Startup of the amcmj1 board (STM32H747, used by both the CM7 and the CM4), written in C.
//
// It replaces stm32hal.h7.startup.amcmj1.s (armasm syntax) with the same behaviour:
// - vector table __Vectors in section RESET (166 entries: 16 of the core + 150 of the STM32H747 NVIC), the list and the
//   names are those of startup_stm32h747xx.s of ST (github.com/STMicroelectronics/cmsis-device-h7)
// - Reset_Handler: SystemInit() and then __main of the Arm C library (it copies RW data, zeroes ZI data, calls the
//   constructors and main())
// - stack of 4 KB (MSP: used by main() before the RTOS starts, then by the ISRs) and heap of 192 KB, as two separate
//   regions of the Arm C library (__use_two_region_memory + __user_initial_stackheap())
// - all handlers are weak: the application overrides them by defining a function with the same name. The faults have
//   their own infinite loop (so the debugger shows which one), all the IRQs share Default_Handler.
//
// Differences from the .s:
// - stack and heap are in sections .bss.STACK and .bss.HEAP (zero initialised data for armclang) instead of STACK and
//   HEAP. If a scatter file selects them by name, its selectors must be changed. With .ANY nothing changes.
// - __Vectors_End and __Vectors_Size are not defined (nobody uses them in this project)
//
// Sizes can be changed in the project with STARTUP_AMCMJ1_STACK_SIZE and STARTUP_AMCMJ1_HEAP_SIZE (multiples of 8).
//
// Toolchains:
// - armclang (Arm Compiler 6) with armlink and the Arm C library: the scatter file amcmj1.2cm4-lwip-0offset.sct.
//   Reset_Handler calls __main, the library gets stack and heap from __user_initial_stackheap().
// - Arm Toolchain for Embedded (ATfE: clang, ld.lld and picolibc): the linker script amcmj1.2cm4-lwip-0offset.ld.
//   There is no __main, so Reset_Handler does what it does: copy of .data, zero of .bss (and of the RAM of the ETH
//   DMA, which armlink zeroes as ZI data), thread local storage of picolibc (errno), constructors, main().
//   The heap is given to malloc() by the linker script (__heap_start, __heap_end around .heap.amcmj1).
// The vector table, the handlers and the sizes of stack and heap are the same for both.
// --------------------------------------------------------------------------------------------------------------------

#include <stdint.h>

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000)
    #define STARTUP_AMCMJ1_ARMCLANG
#elif defined(__clang__)
    #define STARTUP_AMCMJ1_ATFE
#else
    #error this startup supports armclang (Arm Compiler 6) and ATfE (Arm Toolchain for Embedded) only
#endif

// -- configuration

#ifndef STARTUP_AMCMJ1_STACK_SIZE
#define STARTUP_AMCMJ1_STACK_SIZE   (0x00001000)
#endif

#ifndef STARTUP_AMCMJ1_HEAP_SIZE
#define STARTUP_AMCMJ1_HEAP_SIZE    (0x00030000)
#endif

#if ((STARTUP_AMCMJ1_STACK_SIZE % 8) != 0) || ((STARTUP_AMCMJ1_HEAP_SIZE % 8) != 0)
    #error STARTUP_AMCMJ1_STACK_SIZE and STARTUP_AMCMJ1_HEAP_SIZE must be multiples of 8
#endif

// -- stack and heap. uint64_t and aligned(8): the AAPCS wants the stack 8-byte aligned

// armclang: zero initialised data (the .bss prefix), placed by .ANY of the scatter file.
// ATfE: own sections, placed by the linker script after .bss and not zeroed.
#if defined(STARTUP_AMCMJ1_ARMCLANG)
    #define STARTUP_AMCMJ1_SECTION_STACK    ".bss.STACK"
    #define STARTUP_AMCMJ1_SECTION_HEAP     ".bss.HEAP"
#else
    #define STARTUP_AMCMJ1_SECTION_STACK    ".stack.amcmj1"
    #define STARTUP_AMCMJ1_SECTION_HEAP     ".heap.amcmj1"
#endif

static uint64_t s_stack[STARTUP_AMCMJ1_STACK_SIZE / 8] __attribute__((section(STARTUP_AMCMJ1_SECTION_STACK), aligned(8), used));
static uint64_t s_heap[STARTUP_AMCMJ1_HEAP_SIZE / 8] __attribute__((section(STARTUP_AMCMJ1_SECTION_HEAP), aligned(8), used));

#define STARTUP_AMCMJ1_INITIAL_SP   ((uint32_t)&s_stack[STARTUP_AMCMJ1_STACK_SIZE / 8])

#if defined(STARTUP_AMCMJ1_ARMCLANG)

// -- the Arm C library: memory model with separate stack and heap regions, and the function that gives them

__asm(".global __use_two_region_memory");

struct startup_amcmj1_stackheap
{
    uint32_t heap_base;
    uint32_t stack_base;    // the top of the stack: it grows downwards
    uint32_t heap_limit;
    uint32_t stack_limit;
};

// called by __user_setup_stackheap() inside __main, before RW / ZI data are initialised: it must not use them.
// the four values are returned in r0-r3 (value_in_regs), as the assembly version did.
__attribute__((value_in_regs)) struct startup_amcmj1_stackheap __user_initial_stackheap(uint32_t r0, uint32_t sp, uint32_t r2, uint32_t sl);

__attribute__((value_in_regs)) struct startup_amcmj1_stackheap __user_initial_stackheap(uint32_t r0, uint32_t sp, uint32_t r2, uint32_t sl)
{
    (void)r0; (void)sp; (void)r2; (void)sl;
    struct startup_amcmj1_stackheap r;
    r.heap_base   = (uint32_t)&s_heap[0];
    r.stack_base  = STARTUP_AMCMJ1_INITIAL_SP;
    r.heap_limit  = (uint32_t)&s_heap[STARTUP_AMCMJ1_HEAP_SIZE / 8];
    r.stack_limit = (uint32_t)&s_stack[0];
    return r;
}

#endif // STARTUP_AMCMJ1_ARMCLANG

// -- external functions

extern void SystemInit(void);

#if defined(STARTUP_AMCMJ1_ARMCLANG)

extern __attribute__((noreturn)) void __main(void);

#else

extern int main(void);
extern void __libc_init_array(void);    // picolibc: calls the functions in .preinit_array and .init_array
extern void _set_tls(void *tls);        // picolibc: sets the thread pointer used for errno

// symbols of the linker script amcmj1.2cm4-lwip-0offset.ld. only their address matters
extern uint8_t __data_start[];
extern uint8_t __data_source[];
extern uint8_t __data_size[];
extern uint8_t __bss_start[];
extern uint8_t __bss_size[];
extern uint8_t __tls_base[];
extern uint8_t __eth_dma_start[];
extern uint8_t __eth_dma_end[];

#endif

// -- the handlers

typedef void (*startup_amcmj1_vector_t)(void);

__attribute__((noreturn)) void Reset_Handler(void);
void Default_Handler(void);

// core exceptions: weak, each with its own loop
void NMI_Handler(void) __attribute__((weak));
void HardFault_Handler(void) __attribute__((weak));
void MemManage_Handler(void) __attribute__((weak));
void BusFault_Handler(void) __attribute__((weak));
void UsageFault_Handler(void) __attribute__((weak));
void DebugMon_Handler(void) __attribute__((weak));

// RTOS exceptions: weak (CMSIS-RTX defines them). If they are not defined, they go to Default_Handler
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

// IRQs of the STM32H747: weak, they all go to Default_Handler
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_AVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FDCAN1_IT0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FDCAN2_IT0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FDCAN1_IT1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FDCAN2_IT1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FMC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SDMMC1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ETH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ETH_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FDCAN_CAL_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CM7_SEV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CM4_SEV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_OUT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_IN_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DCMI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RNG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART8_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SAI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LTDC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LTDC_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2D_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SAI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void QUADSPI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CEC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C4_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C4_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPDIF_RX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_EP1_OUT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_EP1_IN_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMAMUX1_OVR_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_Master_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_TIMA_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_TIMB_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_TIMC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_TIMD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_TIME_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HRTIM1_FLT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SAI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWPMI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM15_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM16_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM17_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void MDIOS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void MDIOS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void JPEG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void MDMA_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DSI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SDMMC2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HSEM1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HSEM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMAMUX2_OVR_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void BDMA_Channel7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void COMP1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPTIM5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void LPUART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void WWDG_RST_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CRS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ECC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SAI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HOLD_CORE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void WAKEUP_PIN_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

// -- the vector table. armlink places section RESET first in the load region of the scatter file (RESET, +First)

extern const startup_amcmj1_vector_t __Vectors[166];
__attribute__((section("RESET"), used)) const startup_amcmj1_vector_t __Vectors[166] =
{
    (startup_amcmj1_vector_t)STARTUP_AMCMJ1_INITIAL_SP, //   0 Top of Stack
    Reset_Handler,                           //   1 Reset Handler
    NMI_Handler,                             //   2 NMI Handler
    HardFault_Handler,                       //   3 Hard Fault Handler
    MemManage_Handler,                       //   4 MPU Fault Handler
    BusFault_Handler,                        //   5 Bus Fault Handler
    UsageFault_Handler,                      //   6 Usage Fault Handler
    0,                                       //   7 Reserved
    0,                                       //   8 Reserved
    0,                                       //   9 Reserved
    0,                                       //  10 Reserved
    SVC_Handler,                             //  11 SVCall Handler
    DebugMon_Handler,                        //  12 Debug Monitor Handler
    0,                                       //  13 Reserved
    PendSV_Handler,                          //  14 PendSV Handler
    SysTick_Handler,                         //  15 SysTick Handler
    WWDG_IRQHandler,                         //  16 IRQ0   Window WatchDog interrupt ( wwdg1_it, wwdg2_it)
    PVD_AVD_IRQHandler,                      //  17 IRQ1   PVD/AVD through EXTI Line detection
    TAMP_STAMP_IRQHandler,                   //  18 IRQ2   Tamper and TimeStamps through the EXTI line
    RTC_WKUP_IRQHandler,                     //  19 IRQ3   RTC Wakeup through the EXTI line
    FLASH_IRQHandler,                        //  20 IRQ4   FLASH
    RCC_IRQHandler,                          //  21 IRQ5   RCC
    EXTI0_IRQHandler,                        //  22 IRQ6   EXTI Line0
    EXTI1_IRQHandler,                        //  23 IRQ7   EXTI Line1
    EXTI2_IRQHandler,                        //  24 IRQ8   EXTI Line2
    EXTI3_IRQHandler,                        //  25 IRQ9   EXTI Line3
    EXTI4_IRQHandler,                        //  26 IRQ10  EXTI Line4
    DMA1_Stream0_IRQHandler,                 //  27 IRQ11  DMA1 Stream 0
    DMA1_Stream1_IRQHandler,                 //  28 IRQ12  DMA1 Stream 1
    DMA1_Stream2_IRQHandler,                 //  29 IRQ13  DMA1 Stream 2
    DMA1_Stream3_IRQHandler,                 //  30 IRQ14  DMA1 Stream 3
    DMA1_Stream4_IRQHandler,                 //  31 IRQ15  DMA1 Stream 4
    DMA1_Stream5_IRQHandler,                 //  32 IRQ16  DMA1 Stream 5
    DMA1_Stream6_IRQHandler,                 //  33 IRQ17  DMA1 Stream 6
    ADC_IRQHandler,                          //  34 IRQ18  ADC1, ADC2
    FDCAN1_IT0_IRQHandler,                   //  35 IRQ19  FDCAN1 interrupt line 0
    FDCAN2_IT0_IRQHandler,                   //  36 IRQ20  FDCAN2 interrupt line 0
    FDCAN1_IT1_IRQHandler,                   //  37 IRQ21  FDCAN1 interrupt line 1
    FDCAN2_IT1_IRQHandler,                   //  38 IRQ22  FDCAN2 interrupt line 1
    EXTI9_5_IRQHandler,                      //  39 IRQ23  External Line[9:5]s
    TIM1_BRK_IRQHandler,                     //  40 IRQ24  TIM1 Break interrupt
    TIM1_UP_IRQHandler,                      //  41 IRQ25  TIM1 Update Interrupt
    TIM1_TRG_COM_IRQHandler,                 //  42 IRQ26  TIM1 Trigger and Commutation Interrupt
    TIM1_CC_IRQHandler,                      //  43 IRQ27  TIM1 Capture Compare
    TIM2_IRQHandler,                         //  44 IRQ28  TIM2
    TIM3_IRQHandler,                         //  45 IRQ29  TIM3
    TIM4_IRQHandler,                         //  46 IRQ30  TIM4
    I2C1_EV_IRQHandler,                      //  47 IRQ31  I2C1 Event
    I2C1_ER_IRQHandler,                      //  48 IRQ32  I2C1 Error
    I2C2_EV_IRQHandler,                      //  49 IRQ33  I2C2 Event
    I2C2_ER_IRQHandler,                      //  50 IRQ34  I2C2 Error
    SPI1_IRQHandler,                         //  51 IRQ35  SPI1
    SPI2_IRQHandler,                         //  52 IRQ36  SPI2
    USART1_IRQHandler,                       //  53 IRQ37  USART1
    USART2_IRQHandler,                       //  54 IRQ38  USART2
    USART3_IRQHandler,                       //  55 IRQ39  USART3
    EXTI15_10_IRQHandler,                    //  56 IRQ40  External Line[15:10]
    RTC_Alarm_IRQHandler,                    //  57 IRQ41  RTC Alarm (A and B) through EXTI Line
    0,                                       //  58 IRQ42  Reserved
    TIM8_BRK_TIM12_IRQHandler,               //  59 IRQ43  TIM8 Break Interrupt and TIM12 global interrupt
    TIM8_UP_TIM13_IRQHandler,                //  60 IRQ44  TIM8 Update Interrupt and TIM13 global interrupt
    TIM8_TRG_COM_TIM14_IRQHandler,           //  61 IRQ45  TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt
    TIM8_CC_IRQHandler,                      //  62 IRQ46  TIM8 Capture Compare Interrupt
    DMA1_Stream7_IRQHandler,                 //  63 IRQ47  DMA1 Stream7
    FMC_IRQHandler,                          //  64 IRQ48  FMC
    SDMMC1_IRQHandler,                       //  65 IRQ49  SDMMC1
    TIM5_IRQHandler,                         //  66 IRQ50  TIM5
    SPI3_IRQHandler,                         //  67 IRQ51  SPI3
    UART4_IRQHandler,                        //  68 IRQ52  UART4
    UART5_IRQHandler,                        //  69 IRQ53  UART5
    TIM6_DAC_IRQHandler,                     //  70 IRQ54  TIM6 and DAC1&2 underrun errors
    TIM7_IRQHandler,                         //  71 IRQ55  TIM7
    DMA2_Stream0_IRQHandler,                 //  72 IRQ56  DMA2 Stream 0
    DMA2_Stream1_IRQHandler,                 //  73 IRQ57  DMA2 Stream 1
    DMA2_Stream2_IRQHandler,                 //  74 IRQ58  DMA2 Stream 2
    DMA2_Stream3_IRQHandler,                 //  75 IRQ59  DMA2 Stream 3
    DMA2_Stream4_IRQHandler,                 //  76 IRQ60  DMA2 Stream 4
    ETH_IRQHandler,                          //  77 IRQ61  Ethernet
    ETH_WKUP_IRQHandler,                     //  78 IRQ62  Ethernet Wakeup through EXTI line
    FDCAN_CAL_IRQHandler,                    //  79 IRQ63  FDCAN calibration unit interrupt
    CM7_SEV_IRQHandler,                      //  80 IRQ64  CM7 Send event interrupt for CM4
    CM4_SEV_IRQHandler,                      //  81 IRQ65  CM4 Send event interrupt for CM7
    0,                                       //  82 IRQ66  Reserved
    0,                                       //  83 IRQ67  Reserved
    DMA2_Stream5_IRQHandler,                 //  84 IRQ68  DMA2 Stream 5
    DMA2_Stream6_IRQHandler,                 //  85 IRQ69  DMA2 Stream 6
    DMA2_Stream7_IRQHandler,                 //  86 IRQ70  DMA2 Stream 7
    USART6_IRQHandler,                       //  87 IRQ71  USART6
    I2C3_EV_IRQHandler,                      //  88 IRQ72  I2C3 event
    I2C3_ER_IRQHandler,                      //  89 IRQ73  I2C3 error
    OTG_HS_EP1_OUT_IRQHandler,               //  90 IRQ74  USB OTG HS End Point 1 Out
    OTG_HS_EP1_IN_IRQHandler,                //  91 IRQ75  USB OTG HS End Point 1 In
    OTG_HS_WKUP_IRQHandler,                  //  92 IRQ76  USB OTG HS Wakeup through EXTI
    OTG_HS_IRQHandler,                       //  93 IRQ77  USB OTG HS
    DCMI_IRQHandler,                         //  94 IRQ78  DCMI
    0,                                       //  95 IRQ79  Reserved
    RNG_IRQHandler,                          //  96 IRQ80  Rng
    FPU_IRQHandler,                          //  97 IRQ81  FPU
    UART7_IRQHandler,                        //  98 IRQ82  UART7
    UART8_IRQHandler,                        //  99 IRQ83  UART8
    SPI4_IRQHandler,                         // 100 IRQ84  SPI4
    SPI5_IRQHandler,                         // 101 IRQ85  SPI5
    SPI6_IRQHandler,                         // 102 IRQ86  SPI6
    SAI1_IRQHandler,                         // 103 IRQ87  SAI1
    LTDC_IRQHandler,                         // 104 IRQ88  LTDC
    LTDC_ER_IRQHandler,                      // 105 IRQ89  LTDC error
    DMA2D_IRQHandler,                        // 106 IRQ90  DMA2D
    SAI2_IRQHandler,                         // 107 IRQ91  SAI2
    QUADSPI_IRQHandler,                      // 108 IRQ92  QUADSPI
    LPTIM1_IRQHandler,                       // 109 IRQ93  LPTIM1
    CEC_IRQHandler,                          // 110 IRQ94  HDMI_CEC
    I2C4_EV_IRQHandler,                      // 111 IRQ95  I2C4 Event
    I2C4_ER_IRQHandler,                      // 112 IRQ96  I2C4 Error
    SPDIF_RX_IRQHandler,                     // 113 IRQ97  SPDIF_RX
    OTG_FS_EP1_OUT_IRQHandler,               // 114 IRQ98  USB OTG FS End Point 1 Out
    OTG_FS_EP1_IN_IRQHandler,                // 115 IRQ99  USB OTG FS End Point 1 In
    OTG_FS_WKUP_IRQHandler,                  // 116 IRQ100 USB OTG FS Wakeup through EXTI
    OTG_FS_IRQHandler,                       // 117 IRQ101 USB OTG FS
    DMAMUX1_OVR_IRQHandler,                  // 118 IRQ102 DMAMUX1 Overrun interrupt
    HRTIM1_Master_IRQHandler,                // 119 IRQ103 HRTIM Master Timer global Interrupts
    HRTIM1_TIMA_IRQHandler,                  // 120 IRQ104 HRTIM Timer A global Interrupt
    HRTIM1_TIMB_IRQHandler,                  // 121 IRQ105 HRTIM Timer B global Interrupt
    HRTIM1_TIMC_IRQHandler,                  // 122 IRQ106 HRTIM Timer C global Interrupt
    HRTIM1_TIMD_IRQHandler,                  // 123 IRQ107 HRTIM Timer D global Interrupt
    HRTIM1_TIME_IRQHandler,                  // 124 IRQ108 HRTIM Timer E global Interrupt
    HRTIM1_FLT_IRQHandler,                   // 125 IRQ109 HRTIM Fault global Interrupt
    DFSDM1_FLT0_IRQHandler,                  // 126 IRQ110 DFSDM Filter0 Interrupt
    DFSDM1_FLT1_IRQHandler,                  // 127 IRQ111 DFSDM Filter1 Interrupt
    DFSDM1_FLT2_IRQHandler,                  // 128 IRQ112 DFSDM Filter2 Interrupt
    DFSDM1_FLT3_IRQHandler,                  // 129 IRQ113 DFSDM Filter3 Interrupt
    SAI3_IRQHandler,                         // 130 IRQ114 SAI3 global Interrupt
    SWPMI1_IRQHandler,                       // 131 IRQ115 Serial Wire Interface 1 global interrupt
    TIM15_IRQHandler,                        // 132 IRQ116 TIM15 global Interrupt
    TIM16_IRQHandler,                        // 133 IRQ117 TIM16 global Interrupt
    TIM17_IRQHandler,                        // 134 IRQ118 TIM17 global Interrupt
    MDIOS_WKUP_IRQHandler,                   // 135 IRQ119 MDIOS Wakeup  Interrupt
    MDIOS_IRQHandler,                        // 136 IRQ120 MDIOS global Interrupt
    JPEG_IRQHandler,                         // 137 IRQ121 JPEG global Interrupt
    MDMA_IRQHandler,                         // 138 IRQ122 MDMA global Interrupt
    DSI_IRQHandler,                          // 139 IRQ123 DSI global Interrupt
    SDMMC2_IRQHandler,                       // 140 IRQ124 SDMMC2 global Interrupt
    HSEM1_IRQHandler,                        // 141 IRQ125 HSEM1 global Interrupt
    HSEM2_IRQHandler,                        // 142 IRQ126 HSEM2 global Interrupt
    ADC3_IRQHandler,                         // 143 IRQ127 ADC3 global Interrupt
    DMAMUX2_OVR_IRQHandler,                  // 144 IRQ128 DMAMUX Overrun interrupt
    BDMA_Channel0_IRQHandler,                // 145 IRQ129 BDMA Channel 0 global Interrupt
    BDMA_Channel1_IRQHandler,                // 146 IRQ130 BDMA Channel 1 global Interrupt
    BDMA_Channel2_IRQHandler,                // 147 IRQ131 BDMA Channel 2 global Interrupt
    BDMA_Channel3_IRQHandler,                // 148 IRQ132 BDMA Channel 3 global Interrupt
    BDMA_Channel4_IRQHandler,                // 149 IRQ133 BDMA Channel 4 global Interrupt
    BDMA_Channel5_IRQHandler,                // 150 IRQ134 BDMA Channel 5 global Interrupt
    BDMA_Channel6_IRQHandler,                // 151 IRQ135 BDMA Channel 6 global Interrupt
    BDMA_Channel7_IRQHandler,                // 152 IRQ136 BDMA Channel 7 global Interrupt
    COMP1_IRQHandler,                        // 153 IRQ137 COMP1 global Interrupt
    LPTIM2_IRQHandler,                       // 154 IRQ138 LP TIM2 global interrupt
    LPTIM3_IRQHandler,                       // 155 IRQ139 LP TIM3 global interrupt
    LPTIM4_IRQHandler,                       // 156 IRQ140 LP TIM4 global interrupt
    LPTIM5_IRQHandler,                       // 157 IRQ141 LP TIM5 global interrupt
    LPUART1_IRQHandler,                      // 158 IRQ142 LP UART1 interrupt
    WWDG_RST_IRQHandler,                     // 159 IRQ143 Window Watchdog reset interrupt (exti_d2_wwdg_it, exti_d1_wwdg_it)
    CRS_IRQHandler,                          // 160 IRQ144 Clock Recovery Global Interrupt
    ECC_IRQHandler,                          // 161 IRQ145 ECC diagnostic Global Interrupt
    SAI4_IRQHandler,                         // 162 IRQ146 SAI4 global interrupt
    0,                                       // 163 IRQ147 Reserved
    HOLD_CORE_IRQHandler,                    // 164 IRQ148 Hold core interrupt
    WAKEUP_PIN_IRQHandler,                   // 165 IRQ149 Interrupt for all 6 wake-up pins
};

// -- implementation

#if defined(STARTUP_AMCMJ1_ARMCLANG)

// it runs with the MSP taken from __Vectors[0], i.e. on the stack above: SystemInit() then the C library, which calls main()
void Reset_Handler(void)
{
    SystemInit();
    __main();
}

#else

// copy and fill with plain loops of volatile words: the C library is not ready yet, and the compiler must not turn the
// loops into calls to memcpy() / memset(). all the regions are 8-byte aligned and multiple of 8 (see the linker script)
static void startup_amcmj1_copy(uint32_t *dst, const uint32_t *src, uint32_t size)
{
    volatile uint32_t *d = dst;
    for(uint32_t i = 0; i < size / 4; i++)
    {
        d[i] = src[i];
    }
}

static void startup_amcmj1_zero(uint32_t *dst, uint32_t size)
{
    volatile uint32_t *d = dst;
    for(uint32_t i = 0; i < size / 4; i++)
    {
        d[i] = 0;
    }
}

// it runs with the MSP taken from __Vectors[0], i.e. on the stack above. same order as __main of the Arm C library:
// SystemInit() first (it runs before the data are initialised, so it must not rely on them, as with armclang)
void Reset_Handler(void)
{
    SystemInit();

    startup_amcmj1_copy((uint32_t *)__data_start, (const uint32_t *)__data_source, (uint32_t)__data_size);
    startup_amcmj1_zero((uint32_t *)__bss_start, (uint32_t)__bss_size);
    startup_amcmj1_zero((uint32_t *)__eth_dma_start, (uint32_t)(__eth_dma_end - __eth_dma_start));

    _set_tls(__tls_base);
    __libc_init_array();

    (void)main();

    // main() of an embedded application does not return. if it does, stay here
    for(;;) { }
}

#endif

void NMI_Handler(void)
{
    for(;;) { }
}

void HardFault_Handler(void)
{
    for(;;) { }
}

void MemManage_Handler(void)
{
    for(;;) { }
}

void BusFault_Handler(void)
{
    for(;;) { }
}

void UsageFault_Handler(void)
{
    for(;;) { }
}

void DebugMon_Handler(void)
{
    for(;;) { }
}

void Default_Handler(void)
{
    // the IRQ number is in IPSR: look at the xPSR register in the debugger (exception number = 16 + IRQn)
    for(;;) { }
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

