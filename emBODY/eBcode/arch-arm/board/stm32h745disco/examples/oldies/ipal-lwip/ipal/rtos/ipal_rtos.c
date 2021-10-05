/** 
 * @file ipal_rtos.h
 *
 * IPAL-LWIP RTOS interface
 */

#include "ipal_rtos.h"

/* =========================================================================================
 * PUBLIC FUNCTIONS
 * ========================================================================================= */
 
#if (!NO_SYS)
 
/* Time to kernel ticks */
uint32_t ms2tick (uint32_t ms)
{
	return IPAL_LWIP_EXT_ms2tick (ms);
}

/* Kernel ticks to time */
uint32_t tick2ms (uint32_t tick)
{
	return IPAL_LWIP_EXT_tick2ms (tick);
}

/* RTOS delay */
void rtos_delay(uint32_t dly)
{
	IPAL_LWIP_EXT_rtos_delay(dly);
}

/* -------------------------------------------------------------------------------------------
 * MUTEXES 
 * ------------------------------------------------------------------------------------------- */
int8_t sys_mutex_new(sys_mutex_t *mutex)
{
	return IPAL_LWIP_EXT_sys_mutex_new(mutex);
}

void sys_mutex_free(sys_mutex_t *mutex)
{
	IPAL_LWIP_EXT_sys_mutex_free(mutex);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
	IPAL_LWIP_EXT_sys_mutex_lock(mutex);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
	IPAL_LWIP_EXT_sys_mutex_unlock(mutex);
}

/* -------------------------------------------------------------------------------------------
 * SEMAPHORES
 * ------------------------------------------------------------------------------------------- */
int8_t sys_sem_new(sys_sem_t *sem, uint8_t count)
{
	return IPAL_LWIP_EXT_sys_sem_new(sem, count);
}

void sys_sem_free(sys_sem_t *sem)
{
	IPAL_LWIP_EXT_sys_sem_free(sem);
}

uint32_t sys_arch_sem_wait(sys_sem_t *sem, uint32_t timeout)
{
	return IPAL_LWIP_EXT_sys_arch_sem_wait(sem, timeout);
}

void sys_sem_signal(sys_sem_t *sem)
{
	IPAL_LWIP_EXT_sys_sem_signal(sem);
}

/* -------------------------------------------------------------------------------------------
 * MAILBOXES
 * ------------------------------------------------------------------------------------------- */
int8_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	return IPAL_LWIP_EXT_sys_mbox_new(mbox, size);
}

void sys_mbox_free(sys_mbox_t *mbox)
{
	IPAL_LWIP_EXT_sys_mbox_free(mbox);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	IPAL_LWIP_EXT_sys_mbox_post(mbox, msg);
}

int8_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	return IPAL_LWIP_EXT_sys_mbox_trypost(mbox, msg);
}

uint32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, uint32_t timeout)
{
	return IPAL_LWIP_EXT_sys_arch_mbox_fetch(mbox, msg, timeout);
}

uint32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return IPAL_LWIP_EXT_sys_arch_mbox_tryfetch(mbox, msg);
}

/* -------------------------------------------------------------------------------------------
 * THREADS 
 * ------------------------------------------------------------------------------------------- */
sys_thread_t sys_thread_new(const char *name, void (* function)(void *), void *arg, int stacksize, int prio)
{
	return IPAL_LWIP_EXT_sys_thread_new(name, function, arg, stacksize, prio);
}

/* Time */
uint32_t sys_now(void)
{
	return IPAL_LWIP_EXT_sys_now();
}

uint32_t sys_jiffies(void)
{
	return IPAL_LWIP_EXT_sys_jiffies();
}

void sys_init(void)
{
	IPAL_LWIP_EXT_sys_init();
}

#endif

/* Critical sections */
#if SYS_LIGHTWEIGHT_PROT

	sys_prot_t sys_arch_protect(void)
	{
		return IPAL_LWIP_EXT_sys_arch_protect();
	}

	void sys_arch_unprotect(sys_prot_t pval)
	{
		IPAL_LWIP_EXT_sys_arch_unprotect(pval);
	}

#endif /* SYS_LIGHTWEIGHT_PROT */


