

#include "ipal_rtos.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#warning marco.accame: see my comment about RTOS protection
#if 0
    marco.accame: i have added the RTOS protection with this mechanism.
    however, if we use the api of the ipal in a single thread, as we do
    in EOtheIPnet or in the udpdemo-basic, then we dont need RTOS protection
    i keep it nevertheless.
    moreover: the variable eth_irq is pretty useless. it is never changed in the tlx project or
    in the udpdemo-basic.
    i cannot in here use the embot::os::rtos namespace because ... API of IPAL_LWIP_EXT_sys_arch_protect
    cannot be used w/ C++ (they miss the proper code)
    //#ifdef __cplusplus
    //extern "C" {
    //#endif
#endif



#if defined(EMBOT_USE_rtos_cmsisos2)
#include "cmsisos2.h"
// cannot be called from inside an isr
int32_t kernel_lock()   { return osKernelLock(); }
int32_t kernel_restorelock(int32_t l) { return osKernelRestoreLock(l); }
#elif defined(EMBOT_USE_rtos_osal)
#include "osal.h"
int32_t kernel_lock()   { osal_system_scheduling_suspend(); return 0; }
int32_t kernel_restorelock(int32_t l) { osal_system_scheduling_restart(); return 0; }
#else
int32_t kernel_lock()   { return 0; }
int32_t kernel_restorelock(int32_t l) { return 0; }
#endif



bool eth_irq = 0;

volatile uint32_t flag = 0;
/*
 * IPAL_LWIP_EXT_sys_arch_protect
 * See ipal_rtos.h for details
 */
sys_prot_t IPAL_LWIP_EXT_sys_arch_protect(void)
{
	sys_prot_t pval = 0;

	if (!eth_irq)
	{
		pval = kernel_lock();
	}
    else
    {
        flag++; 
        // marco.accame: we never execute in here. it means that eth_irq is never changes.
        // it is defined 
    }
  return pval;
}

/*
 * IPAL_LWIP_EXT_sys_arch_unprotect
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_arch_unprotect(sys_prot_t pval)
{
	if (!eth_irq)
	{
		kernel_restorelock (pval);
	}
}

