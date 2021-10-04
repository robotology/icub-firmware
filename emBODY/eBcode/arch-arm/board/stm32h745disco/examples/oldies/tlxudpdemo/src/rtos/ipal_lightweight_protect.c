/**
 * CMSIS_OS2 implementation of IPAL functions
 */
#include "cmsis_os2.h"
#include "ipal_rtos.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

extern bool eth_irq;

/*
 * IPAL_LWIP_EXT_sys_arch_protect
 * See ipal_rtos.h for details
 */
sys_prot_t IPAL_LWIP_EXT_sys_arch_protect(void)
{
	sys_prot_t pval = 0;

	if (!eth_irq)
	{
		pval = osKernelLock();
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
		osKernelRestoreLock (pval);
	}
}
