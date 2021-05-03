/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include <string.h>
#include "cmsis_os2.h"

#include "lwip/opt.h"
#include "lwip/sys.h"

#if !NO_SYS

static uint32_t tick_freq;

/* Time to kernel ticks */
static uint32_t ms2tick (uint32_t ms) {
  if (ms == 0U) {
    return (osWaitForever);
  }
  if (tick_freq != 0U) {
    ms = (ms * tick_freq) / 1000U;
  }
  return (ms);
}

/* Kernel ticks to time */
static uint32_t tick2ms (uint32_t tick) {
  if (tick_freq != 0U) {
    tick = (tick * 1000U) / tick_freq;
  }
  return (tick);
}

/* Mutexes */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  static const osMutexAttr_t attr = {
    NULL, osMutexPrioInherit | osMutexRecursive, NULL, NULL
  };
  osMutexId_t id;

  id = osMutexNew(&attr);
  if (id == NULL) {
    return ERR_MEM;
  }
  *mutex = id;
  return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
  osMutexDelete(*mutex);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
  osMutexAcquire(*mutex, osWaitForever);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
  osMutexRelease(*mutex);
}

/* Semaphores */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  osSemaphoreId_t id;

  id = osSemaphoreNew(1U, count, NULL);
  if (id == NULL) {
    return ERR_MEM;
  }
  *sem = id;
  return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
  osSemaphoreDelete(*sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  osStatus_t status;

  status = osSemaphoreAcquire(*sem, ms2tick(timeout));
  if (status != osOK) {
    return SYS_ARCH_TIMEOUT;
  }
  return 0U;
}

void sys_sem_signal(sys_sem_t *sem)
{
  osSemaphoreRelease(*sem);
}

/* Mailboxes */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  osMessageQueueId_t id;

  id = osMessageQueueNew((uint32_t)size, sizeof(void *), NULL);
  if (id == NULL) {
    return ERR_MEM;
  }
  *mbox = id;
  return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
  osMessageQueueDelete(*mbox);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  osMessageQueuePut(*mbox, &msg, 0, osWaitForever);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  osStatus_t status;

  status = osMessageQueuePut(*mbox, &msg, 0U, 0U);
  if (status != osOK) {
    return ERR_MEM;
  }
  return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  osStatus_t status;

  status = osMessageQueueGet (*mbox, msg, NULL, ms2tick(timeout));
  if (status != osOK) {
    return SYS_ARCH_TIMEOUT;
  }
  return 0U;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  osStatus_t status;

  status = osMessageQueueGet (*mbox, msg, NULL, 0U);
  if (status != osOK) {
    return SYS_MBOX_EMPTY;
  }
  return 0U;
}

/* Threads */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
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

/* Time */
u32_t sys_now()
{
  return (tick2ms(osKernelGetTickCount()));
}

u32_t sys_jiffies()
{
  return (osKernelGetTickCount());
}

/* Critical sections */
#if SYS_LIGHTWEIGHT_PROT

sys_prot_t sys_arch_protect(void)
{
  sys_prot_t pval;

  pval = osKernelLock();
  return pval;
}

void sys_arch_unprotect(sys_prot_t pval)
{
  osKernelRestoreLock (pval);
}

#endif /* SYS_LIGHTWEIGHT_PROT */

void sys_init(void)
{
  tick_freq = osKernelGetTickFreq ();
  if (tick_freq > 990U && tick_freq < 1010U) {
    // No scaling for 1ms ticks
    tick_freq = 0U;
  }
}

#endif /* !NO_SYS */
