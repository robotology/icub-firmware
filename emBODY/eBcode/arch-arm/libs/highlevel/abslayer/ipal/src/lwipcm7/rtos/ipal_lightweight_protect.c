/**
 * NO OS implementation of IPAL functions
 */
#include "ipal_rtos.h"
#include <stdint.h>

#include "ipal_hid.h"


/*
 * IPAL_LWIP_EXT_sys_arch_protect
 * See ipal_rtos.h for details
 */
int32_t IPAL_LWIP_EXT_sys_arch_protect(void)
{
    int32_t pval = 0;
    ipal_base_hid_threadsafety_lock();
    return pval;
}

/*
 * IPAL_LWIP_EXT_sys_arch_unprotect
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_arch_unprotect(int32_t pval)
{
    ipal_base_hid_threadsafety_unlock();    
}
