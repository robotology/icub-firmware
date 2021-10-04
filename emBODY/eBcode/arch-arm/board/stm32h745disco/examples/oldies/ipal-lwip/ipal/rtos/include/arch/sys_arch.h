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
#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "ipal_rtos_cfg.h"
#include <stdint.h>

#if (!NO_SYS)

/* ============================================================================================
 * TYPE DEFINITIONS
 * ============================================================================================ */
 
typedef void * ipal_rtos_mutexId_t;
typedef void * ipal_rtos_semId_t;
typedef void * ipal_rtos_mboxId_t;
typedef void * ipal_rtos_threadId_t;	

/* ------------------------------------------------------------------------------
 * MUTEXES
 * ------------------------------------------------------------------------------ */
typedef ipal_rtos_mutexId_t 						sys_mutex_t;
#define sys_mutex_valid(mutex) 					(((mutex) != NULL) && (*(mutex) != NULL))
#define sys_mutex_set_invalid(mutex) 		if ((mutex) != NULL) { *(mutex) = NULL; }

/* ------------------------------------------------------------------------------
 * SEMAPHORES
 * ------------------------------------------------------------------------------ */
typedef ipal_rtos_semId_t 							sys_sem_t;
#define sys_sem_valid(sem) 							(((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_set_invalid(sem) 				if ((sem) != NULL) { *(sem) = NULL; }

/* ------------------------------------------------------------------------------
 * MESSAGE QUEUES
 * ------------------------------------------------------------------------------ */
typedef ipal_rtos_mboxId_t							sys_mbox_t;
#define sys_mbox_valid(mbox) 						(((mbox) != NULL) && (*(mbox) != NULL))
#define sys_mbox_set_invalid(mbox) 			if ((mbox) != NULL) { *(mbox) = NULL; }
#define sys_mbox_trypost_fromisr 				sys_mbox_trypost

/* ------------------------------------------------------------------------------
 * DELAY FUNCTION
 * ------------------------------------------------------------------------------ */
#define sys_msleep(ms) 									rtos_delay(ms)

/* ------------------------------------------------------------------------------
 * THREADS
 * ------------------------------------------------------------------------------ */
typedef ipal_rtos_threadId_t* 					sys_thread_t;

#endif

#ifdef SYS_LIGHTWEIGHT_PROT

/* ------------------------------------------------------------------------------
 * SYSTEM PROTECTION OBJECT
 * ------------------------------------------------------------------------------ */
typedef int32_t 												sys_prot_t;

#endif
		
#endif /* LWIP_ARCH_SYS_ARCH_H */
