/** 
 * @file net_thread.c
 *
 * Net thread creation. Function bodies implementation
 */
#include "cmsis_os2.h"

#include "udp_comm_app_thread.h"

/* IPAL */
#include "ipal.h"
#include "ipal_app_cfg.h"

#include "net_thread.h"

#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include <stdio.h>

/* ===========================================================================
 * MACROS
 * =========================================================================== */

#define NET_PERIODIC_MS	(10)

/* ===========================================================================
 * STATIC VARIABLES
 * =========================================================================== */
 
/* ---------------------------------------------------------------------------
 * Net Thread
 * --------------------------------------------------------------------------- */
 
/* Net thread ID */
osThreadId_t net_thread_id;

/* Net thread flags id */
osEventFlagsId_t net_thread_flags_id;

/* Net thread attributes */
static osThreadAttr_t net_thread_attr = 
{
	.name = "Net Thread",
	.stack_size = 1024
};

/* ---------------------------------------------------------------------------
 * Net Periodic Timer
 * --------------------------------------------------------------------------- */

/* The net periodic tick */
static volatile uint32_t net_periodic_tick = 0;

/* The net periodic timer id */
static osTimerId_t net_periodic_timer_id;

/* The net periodi timer attribute */
static osTimerAttr_t net_periodic_timer_attr = 
{
	.name = "TLX net periodic timer"
};

/* ---------------------------------------------------------------------------
 * IPAL
 * --------------------------------------------------------------------------- */

/* IPAL mem size */
static uint32_t ipal_mem_size;

 
/* ===========================================================================
 * PRIVATE FUNCTIONS
 * =========================================================================== */

/* Error handler */
static void error_handler(void)
{
	while(1);
}

/* Net periodic timer function, just increment the tick */
static void net_periodic_timer_func(void *args)
{
	/* Increment IPAL time (+10ms) */
	ipal_sys_timetick_increment();
}

extern volatile bool frame_retrieved_in_irq_handler;
extern ETH_HandleTypeDef *heth;


/**
 * The net thread
 */
static void net_thread(void *argument) 
{
	ipal_result_t res;
	
	/* Avoid compiler warnings */
	res = res;
	
	/* Getsize */
	if (ipal_base_memory_getsize(&ipal_app_cfg, &ipal_mem_size) > 0)
	{
		/* Allocate memory */
		__nop();
	}
	
	/* Initializes IPAL base */
	res = ipal_base_initialise(&ipal_app_cfg, NULL);
	
	/* Start periodic timer */ 
	net_periodic_timer_id = osTimerNew(net_periodic_timer_func, osTimerPeriodic, NULL, &net_periodic_timer_attr);
	
	if (net_periodic_timer_id == NULL)
	{
		error_handler();
	}

	osTimerStart(net_periodic_timer_id, NET_PERIODIC_MS);
	
	/* Starts IPAL */
	res = ipal_sys_start();
	
	/* Link should be up */
	printf("LINK IS UP\n");
	
	/* Initialize UDP communication thread */
	udp_comm_app_thread_init();

	/* Create the event flags */
	net_thread_flags_id = osEventFlagsNew	(NULL);	

	while(1)
	{
		/* Wait for event */
		osEventFlagsWait (net_thread_flags_id, (NET_THREAD_RX_EVENT | NET_THREAD_TX_EVENT), osFlagsWaitAny, 2);
		
		/* Process communication */
		ipal_sys_process_communication();
	}
}

/* ===========================================================================
 * PUBLIC FUNCTIONS
 * =========================================================================== */  

/*
 * net_thread_init
 * See header file for details
 */
int net_thread_init(void) 
{
  net_thread_id = osThreadNew(net_thread, NULL, &net_thread_attr);
  
	if (net_thread_id == NULL) {
    return(-1);
  }
 
  return(0);
}
