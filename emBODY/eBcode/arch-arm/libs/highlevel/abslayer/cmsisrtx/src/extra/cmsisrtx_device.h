// cmsisrtx_device.h
// Minimal CMSIS device header used ONLY to build the cmsisrtx library (RTX kernel + OS tick) and its config files.
// It replaces the "CMSIS_device_header" that the CMSIS-Pack RTE would generate: this project does not use the RTE.
// It describes the two cores of the STM32H7 dual-core family: the M7 (target cm7) and the M4 (target cm4).
// The values that influence the binary code of the library are __NVIC_PRIO_BITS and the FPU / core type:
// they must match the ones of the application device header (stm32h7xx.h): both have 4 priority bits.
//
// Select the core with one of: CMSISRTX_CM7, CMSISRTX_CM4 (defined by the targets of the uvprojx).

#pragma once

#include <stdint.h>

#if defined(CMSISRTX_CM7)

    #define __CM7_REV               0x0100U     // r1p0
    #define __MPU_PRESENT           1U
    #define __NVIC_PRIO_BITS        4U
    #define __Vendor_SysTickConfig  0U
    #define __FPU_PRESENT           1U
    #define __ICACHE_PRESENT        1U
    #define __DCACHE_PRESENT        1U
    #define __DTCM_PRESENT          1U

#elif defined(CMSISRTX_CM4)

    #define __CM4_REV               0x0001U     // r0p1
    #define __MPU_PRESENT           1U
    #define __NVIC_PRIO_BITS        4U
    #define __Vendor_SysTickConfig  0U
    #define __FPU_PRESENT           1U

#else
    #error "define CMSISRTX_CM7 or CMSISRTX_CM4"
#endif

// only the core exceptions are needed by the kernel and by the OS tick: the peripheral IRQs belong to the application
typedef enum IRQn
{
    NonMaskableInt_IRQn         = -14,
    HardFault_IRQn              = -13,
    MemoryManagement_IRQn       = -12,
    BusFault_IRQn               = -11,
    UsageFault_IRQn             = -10,
    SVCall_IRQn                 =  -5,
    DebugMonitor_IRQn           =  -4,
    PendSV_IRQn                 =  -2,
    SysTick_IRQn                =  -1
} IRQn_Type;

#if defined(CMSISRTX_CM7)
    #include "core_cm7.h"
#else
    #include "core_cm4.h"
#endif

// the OS tick (os_systick.c) derives the SysTick reload value from it: the application must provide it and keep it
// updated (as the STM32 system_stm32h7xx.c / HAL do) with the frequency of the core clock
extern uint32_t SystemCoreClock;
