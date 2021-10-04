/** 
 * @file ipal_rtos_cfg.h 
 *
 * IPAL rtos related options configuration file
 */
#ifndef IPAL_RTOS_CFG_H
#define IPAL_RTOS_CFG_H

/* 

There are basically three kind of configuration options related to IPAL-LWIP and RTOS.
They are explained here below.

IPAL_RTOS_FULL_SUPPORT:
	Semaphores, mutexes, timetick, etc, leverages on RTOS-related objects (mutex, semaphores, etc).
	If you activate this option, you must provide externally all the functions listed in ipal_rtos.c.
	
	If this option is active you can (in user application):
		- Send messages, accessing to physical resources, from different threads (see #NOTE1 reference below)
		- Allocate buffers (pbuf) from different threads
	
IPAL_RTOS_MINIMAL_LOCK:
	This option just protect pbuf allocation and de-allocation.
	To do this, you must provide externally the functions (defined in ipal_rtos.c)
		IPAL_LWIP_EXT_sys_arch_protect
		IPAL_LWIP_EXT_sys_arch_unprotect
	
	If this option is active you can (in user application):
		- Send messages, accessing to physical resources, from a single thread	(see #NOTE1 reference below)
		- Allocate buffers (pbuf) from different threads
		
IPAL_NO_RTOS:
	No data-race protection is provided.
	
	If this option is active you can (in user application):
		- Send messages, accessing to physical resources, from a single thread	(see #NOTE2 reference below)
		- Allocate buffers (pbuf) from different threads (or use different kind of buffers according to the operation you want to perform)
		
#NOTE1:

#NOTE2:
	
*/

#if (!defined(IPAL_USE_RTOS_FULL) && !defined(IPAL_USE_RTOS_MINIMAL) && !defined(IPAL_USE_NO_RTOS))

	/* Touch these definitions */

	#define IPAL_RTOS_FULL_SUPPORT				(0)
	#define IPAL_RTOS_MINIMAL_LOCK				(1)
	#define IPAL_NO_RTOS									(0)
	
#else 

	#if defined(IPAL_USE_RTOS_FULL)
		
		#define IPAL_RTOS_FULL_SUPPORT				(1)
		#define IPAL_RTOS_MINIMAL_LOCK				(0)
		#define IPAL_NO_RTOS									(0)
		
	#else 

		#if defined(IPAL_USE_RTOS_MINIMAL)

			#define IPAL_RTOS_FULL_SUPPORT			(0)
			#define IPAL_RTOS_MINIMAL_LOCK			(1)
			#define IPAL_NO_RTOS								(0)
	
		#else
		
			#define IPAL_RTOS_FULL_SUPPORT			(0)
			#define IPAL_RTOS_MINIMAL_LOCK			(0)
			#define IPAL_NO_RTOS								(1)
		
		#endif
		
	#endif
	
#endif

/* --------------------------------------------------------------------------------
 * Do not change what defined herebelow 
 * -------------------------------------------------------------------------------- */

#if (IPAL_RTOS_FULL_SUPPORT==1)
	#define NO_SYS										(0)	
	#define SYS_LIGHTWEIGHT_PROT			(1)
#endif

#if (IPAL_RTOS_MINIMAL_LOCK==1)
	#define NO_SYS										(1)	
	#define SYS_LIGHTWEIGHT_PROT			(1)
#endif

#if	(IPAL_NO_RTOS==1)
	#define NO_SYS										(1)	
	#define SYS_LIGHTWEIGHT_PROT			(0)
#endif

/* -------------------------------------------------------------------------------- */

#endif
