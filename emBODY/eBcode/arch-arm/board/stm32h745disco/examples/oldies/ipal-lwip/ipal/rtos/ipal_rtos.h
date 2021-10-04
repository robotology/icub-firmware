/** 
 * @file ipal_rtos.h
 *
 * IPAL-LWIP RTOS interface
 */
#ifndef IPAL_RTOS_H
#define IPAL_RTOS_H

#include <stdint.h>
#include "ipal_rtos_cfg.h"
#include "arch/sys_arch.h"

#if (!NO_SYS)

/* ============================================================================================
 * TYPE DEFINITIONS
 * ============================================================================================ */

/** 
 * Definitions for error constants. 
 * Here just for mnemonic reasons
 */
typedef enum {
	/** No error, everything OK. */
  LWIP_ERR_OK         = 0,
	/** Out of memory error.     */
  LWIP_ERR_MEM        = -1,
	/** Buffer error.            */
  LWIP_ERR_BUF        = -2,
	/** Timeout.                 */
  LWIP_ERR_TIMEOUT    = -3,
	/** Routing problem.         */
  LWIP_ERR_RTE        = -4,
	/** Operation in progress    */
  LWIP_ERR_INPROGRESS = -5,
	/** Illegal value.           */
  LWIP_ERR_VAL        = -6,
	/** Operation would block.   */
  LWIP_ERR_WOULDBLOCK = -7,
	/** Address in use.          */
  LWIP_ERR_USE        = -8,
	/** Already connecting.      */
  LWIP_ERR_ALREADY    = -9,
	/** Conn already established.*/
  LWIP_ERR_ISCONN     = -10,
	/** Not connected.           */
  LWIP_ERR_CONN       = -11,
	/** Low-level netif error    */
  LWIP_ERR_IF         = -12,
	/** Connection aborted.      */
  LWIP_ERR_ABRT       = -13,
	/** Connection reset.        */
  LWIP_ERR_RST        = -14,
/** Connection closed.       */
  LWIP_ERR_CLSD       = -15,
/** Illegal argument.        */
  LWIP_ERR_ARG        = -16
} lwip_err_enum_t;

#define LWIP_SYS_ARCH_TIMEOUT 0xffffffffUL
#define LWIP_SYS_MBOX_EMPTY		0xffffffffUL

/* ============================================================================================
 * SYMBOLS EXTERNALIZATION
 * ============================================================================================ */
 
/* Time to kernel ticks */
extern uint32_t IPAL_LWIP_EXT_ms2tick (uint32_t ms); 

/* Kernel ticks to time */
extern uint32_t IPAL_LWIP_EXT_tick2ms (uint32_t tick); 

/* RTOS delay */
extern void IPAL_LWIP_EXT_rtos_delay(uint32_t dly);

/* -------------------------------------------------------------------------------------------
 * MUTEXES 
 * ------------------------------------------------------------------------------------------- */
extern int8_t IPAL_LWIP_EXT_sys_mutex_new(sys_mutex_t *mutex);

extern void IPAL_LWIP_EXT_sys_mutex_free(sys_mutex_t *mutex);

extern void IPAL_LWIP_EXT_sys_mutex_lock(sys_mutex_t *mutex);

extern void IPAL_LWIP_EXT_sys_mutex_unlock(sys_mutex_t *mutex);

/* -------------------------------------------------------------------------------------------
 * SEMAPHORES
 * ------------------------------------------------------------------------------------------- */
extern int8_t IPAL_LWIP_EXT_sys_sem_new(sys_sem_t *sem, uint8_t count);

extern void IPAL_LWIP_EXT_sys_sem_free(sys_sem_t *sem);

extern uint32_t IPAL_LWIP_EXT_sys_arch_sem_wait(sys_sem_t *sem, uint32_t timeout);

extern void IPAL_LWIP_EXT_sys_sem_signal(sys_sem_t *sem);

/* -------------------------------------------------------------------------------------------
 * MAILBOXES
 * ------------------------------------------------------------------------------------------- */
extern int8_t IPAL_LWIP_EXT_sys_mbox_new(sys_mbox_t *mbox, int size);

extern void IPAL_LWIP_EXT_sys_mbox_free(sys_mbox_t *mbox);

extern void IPAL_LWIP_EXT_sys_mbox_post(sys_mbox_t *mbox, void *msg);

extern int8_t IPAL_LWIP_EXT_sys_mbox_trypost(sys_mbox_t *mbox, void *msg);

extern uint32_t IPAL_LWIP_EXT_sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, uint32_t timeout);

extern uint32_t IPAL_LWIP_EXT_sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg);

/* -------------------------------------------------------------------------------------------
 * THREADS 
 * ------------------------------------------------------------------------------------------- */
extern sys_thread_t IPAL_LWIP_EXT_sys_thread_new(const char *name, void (*function)(void *arg), void *arg, int stacksize, int prio);

/* Time */
extern uint32_t IPAL_LWIP_EXT_sys_now(void);

extern uint32_t IPAL_LWIP_EXT_sys_jiffies(void);

extern void IPAL_LWIP_EXT_sys_init(void);

#endif

/* Critical sections */
#if (SYS_LIGHTWEIGHT_PROT) 

	extern sys_prot_t IPAL_LWIP_EXT_sys_arch_protect(void);

	extern void IPAL_LWIP_EXT_sys_arch_unprotect(sys_prot_t pval);

#endif /* SYS_LIGHTWEIGHT_PROT */

#endif
