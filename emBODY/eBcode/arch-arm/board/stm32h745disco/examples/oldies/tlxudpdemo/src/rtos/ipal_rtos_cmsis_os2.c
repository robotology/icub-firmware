/**
 * CMSIS_OS2 implementation of IPAL functions
 */
#include "cmsis_os2.h"
#include "ipal_rtos.h"
#include <stdint.h>
#include <string.h>

/* ==========================================================================================
 * STATIC VARIABLES
 * ========================================================================================== */

/* Tick frequency */
static uint32_t tick_freq;

/* ==========================================================================================
 * PUBLIC FUNCTIONS
 * ========================================================================================== */
 
/*
 * IPAL_LWIP_EXT_ms2tick
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_ms2tick(uint32_t ms)
{
	if (ms == 0U) 
	{
    return (osWaitForever);
  }
  if (tick_freq != 0U) 
	{
    ms = (ms * tick_freq) / 1000U;
  }
  return (ms);
}

/*
 * IPAL_LWIP_EXT_tick2ms
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_tick2ms(uint32_t tick)
{
	if (tick_freq != 0U) 
	{
    tick = (tick * 1000U) / tick_freq;
  }
  return (tick);
}

/*
 * IPAL_LWIP_EXT_rtos_delay
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_rtos_delay(uint32_t dly)
{
	osDelay(dly);
}

/* -------------------------------------------------------------------------------------------
 * MUTEXES 
 * ------------------------------------------------------------------------------------------- */

/*
 * IPAL_LWIP_EXT_sys_mutex_new
 * See ipal_rtos.h for details
 */
int8_t IPAL_LWIP_EXT_sys_mutex_new(sys_mutex_t *mutex)
{
	static const osMutexAttr_t attr = 
	{
    NULL, osMutexPrioInherit | osMutexRecursive, NULL, NULL
  };
  osMutexId_t id;

  id = osMutexNew(&attr);
  if (id == NULL) 
	{
    return LWIP_ERR_MEM;
  }
	
  *mutex = id;
  return LWIP_ERR_OK;
}

/*
 * IPAL_LWIP_EXT_sys_mutex_free
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_mutex_free(sys_mutex_t *mutex)
{
	osMutexDelete(*mutex);
}

/*
 * IPAL_LWIP_EXT_sys_mutex_lock
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_mutex_lock(sys_mutex_t *mutex)
{
	osMutexAcquire(*mutex, osWaitForever);
}

/*
 * IPAL_LWIP_EXT_sys_mutex_unlock
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_mutex_unlock(sys_mutex_t *mutex)
{
	 osMutexRelease(*mutex);
}

/* -------------------------------------------------------------------------------------------
 * SEMAPHORES
 * ------------------------------------------------------------------------------------------- */

/*
 * IPAL_LWIP_EXT_sys_sem_new
 * See ipal_rtos.h for details
 */
int8_t IPAL_LWIP_EXT_sys_sem_new(sys_sem_t *sem, uint8_t count)
{
	osSemaphoreId_t id;

  id = osSemaphoreNew(1U, count, NULL);
	
  if (id == NULL) 
	{
    return LWIP_ERR_MEM;
  }
  
	*sem = id;
  return LWIP_ERR_OK;
}

/*
 * IPAL_LWIP_EXT_sys_sem_free
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_sem_free(sys_sem_t *sem)
{
	osSemaphoreDelete(*sem);
}

/*
 * IPAL_LWIP_EXT_sys_arch_sem_wait
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_sys_arch_sem_wait(sys_sem_t *sem, uint32_t timeout)
{
	osStatus_t status;

  status = osSemaphoreAcquire(*sem, IPAL_LWIP_EXT_ms2tick(timeout));
  
	if (status != osOK) 
	{
    return LWIP_SYS_ARCH_TIMEOUT;
  }
  return 0U;
}

/*
 * IPAL_LWIP_EXT_sys_sem_signal
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_sem_signal(sys_sem_t *sem)
{
	osSemaphoreRelease(*sem);
}

/* -------------------------------------------------------------------------------------------
 * MAILBOXES
 * ------------------------------------------------------------------------------------------- */

/*
 * IPAL_LWIP_EXT_sys_mbox_new
 * See ipal_rtos.h for details
 */
int8_t IPAL_LWIP_EXT_sys_mbox_new(sys_mbox_t *mbox, int size)
{
	osMessageQueueId_t id;

  id = osMessageQueueNew((uint32_t)size, sizeof(void *), NULL);
  
	if (id == NULL) 
	{
		return LWIP_ERR_MEM;
  }
	
  *mbox = id;
  return LWIP_ERR_OK;
}

/*
 * IPAL_LWIP_EXT_sys_mbox_free
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_mbox_free(sys_mbox_t *mbox)
{
	osMessageQueueDelete(*mbox);
}

/*
 * IPAL_LWIP_EXT_sys_mbox_post
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	osMessageQueuePut(*mbox, &msg, 0, osWaitForever);
}

/*
 * IPAL_LWIP_EXT_sys_mbox_trypost
 * See ipal_rtos.h for details
 */
int8_t IPAL_LWIP_EXT_sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	osStatus_t status;

  status = osMessageQueuePut(*mbox, &msg, 0U, 0U);
  
	if (status != osOK) 
	{
    return LWIP_ERR_MEM;
  }
  
	return LWIP_ERR_OK;
}

/*
 * IPAL_LWIP_EXT_sys_arch_mbox_fetch
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, uint32_t timeout)
{
	osStatus_t status;

  status = osMessageQueueGet (*mbox, msg, NULL, IPAL_LWIP_EXT_ms2tick(timeout));
  
	if (status != osOK) 
	{
    return LWIP_SYS_ARCH_TIMEOUT;
  }
  
	return 0U;
}

/*
 * IPAL_LWIP_EXT_sys_arch_mbox_tryfetch
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	osStatus_t status;

  status = osMessageQueueGet (*mbox, msg, NULL, 0U);
  
	if (status != osOK) 
	{
		return LWIP_SYS_MBOX_EMPTY;
  }
  
	return 0U;
}

/* -------------------------------------------------------------------------------------------
 * THREADS 
 * ------------------------------------------------------------------------------------------- */

/*
 * IPAL_LWIP_EXT_sys_thread_new
 * See ipal_rtos.h for details
 */
sys_thread_t IPAL_LWIP_EXT_sys_thread_new(const char *name, void (*function)(void *arg), void *arg, int stacksize, int prio)
{
	osThreadAttr_t attr;
  osThreadId_t   id;

  memset (&attr, 0, sizeof(attr));
  attr.name       = name;
  attr.stack_size = (uint32_t)stacksize;
  attr.priority   = (osPriority_t)prio;

  id = osThreadNew(function, arg, &attr);
  return id;
}

/*
 * IPAL_LWIP_EXT_sys_now
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_sys_now(void)
{
	return (IPAL_LWIP_EXT_tick2ms(osKernelGetTickCount()));
}

/*
 * IPAL_LWIP_EXT_sys_jiffies
 * See ipal_rtos.h for details
 */
uint32_t IPAL_LWIP_EXT_sys_jiffies(void)
{
	return (osKernelGetTickCount());
}

/*
 * IPAL_LWIP_EXT_sys_init
 * See ipal_rtos.h for details
 */
void IPAL_LWIP_EXT_sys_init(void)
{
	tick_freq = osKernelGetTickFreq ();
	
  if (tick_freq > 990U && tick_freq < 1010U) 
	{
    // No scaling for 1ms ticks
    tick_freq = 0U;
  }
}


