/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// embot_os_rtos_cpu.h: the only code of embot::os::rtos that depends on the cpu.
// - it is private: only the backends (embot_os_rtos_cmsisrtx.cpp, embot_os_rtos_freertos.cpp) and embot_os_rtos.cpp
//   include it
// - Cortex-M: PRIMASK and IPSR, written w/out the CMSIS core headers, which would clash w/ the ones of the stm32 hal
// - RISC-V: the port layer of the chip gives three functions (extern "C"), because the way to know whether the cpu is
//   serving an interrupt depends on the port of the RTOS (e.g. the FreeRTOS RISC-V port runs the ISRs on their own
//   stack) and on the interrupt controller
// - a port to another cpu adds a branch here, or gives the same three functions
// --------------------------------------------------------------------------------------------------------------------

#ifndef __EMBOT_OS_RTOS_CPU_H_
#define __EMBOT_OS_RTOS_CPU_H_

#include <cstdint>

#if defined(__riscv)

// given by the port layer of the chip
extern "C" uint32_t embot_os_rtos_cpu_irqsave(void);         // interrupts off, it returns the previous state
extern "C" void embot_os_rtos_cpu_irqrestore(uint32_t state);   // the state returned by embot_os_rtos_cpu_irqsave()
extern "C" uint16_t embot_os_rtos_cpu_interruptid(void);     // 0 in a thread, else the id of the interrupt being served

#endif

namespace embot { namespace os { namespace rtos { namespace cpu {

#if defined(__riscv)

    inline uint32_t irqsave()
    {
        return embot_os_rtos_cpu_irqsave();
    }

    inline void irqrestore(uint32_t state)
    {
        embot_os_rtos_cpu_irqrestore(state);
    }

    inline uint16_t interruptid()
    {
        return embot_os_rtos_cpu_interruptid();
    }

#else

    // Cortex-M: interrupts off, it returns the previous PRIMASK
    inline uint32_t irqsave()
    {
        uint32_t primask {0};
        __asm volatile ("mrs %0, primask\n\tcpsid i" : "=r" (primask) :: "memory");
        return primask;
    }

    inline void irqrestore(uint32_t primask)
    {
        __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
    }

    // IPSR: 0 in thread mode, else the exception number (16 + the number of the IRQ)
    inline uint16_t interruptid()
    {
        uint32_t ipsr {0};
        __asm volatile ("mrs %0, ipsr" : "=r" (ipsr));
        return static_cast<uint16_t>(ipsr & 0x1ffU);
    }

#endif

    inline bool isinisr()
    {
        return 0 != interruptid();
    }

}}}} // namespace embot { namespace os { namespace rtos { namespace cpu {

#endif // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
