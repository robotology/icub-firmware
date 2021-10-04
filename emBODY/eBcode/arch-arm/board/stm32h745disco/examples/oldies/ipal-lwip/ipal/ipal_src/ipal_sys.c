/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


/* @file       ipal_sys.c
	@brief      This file implements internal implementation of the ipal xxx module
	@author     valentina.gaggero@iit.it
    @date       01/07/2013

**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_SYS

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "lwip_interface.h"
#include "ethernetif.h"

/* Friends functions */
#include "ipal_hid.h"

/* API interface */
#include "ipal.h"

#include "ipal_debug.h"

#include <stdbool.h>

/* ====================================================================================================================
 * MACROS
 * ==================================================================================================================== */

/* Link-Up checking time */
#define IPAL_LWIP_LINKUP_CHECKING_TIME	(100)

/* ====================================================================================================================
 * FRIEND VARIABLES
 * ==================================================================================================================== */

/* Is IPAL SYS started? */
uint8_t ipal_sys_hid_started	= 0;

/* ====================================================================================================================
 * TYPE DEFINITIONS
 * ==================================================================================================================== */

/*
 * Keep trance of a timer value and its granularity
 */
typedef struct
{
	/* Timer value */
	uint32_t value;
	
	/* Number of ticks */
	uint32_t numofticks;
} ipal_sys_timerinfo_t;

/*
 * Timestamps used for TCP and ARP
 */
typedef struct
{
	/* TCP Timer */
	ipal_sys_timerinfo_t tcp_tmr;
	
	/* ARP Timer - ARM timer is used to periodically refresh ARP table entries */
	ipal_sys_timerinfo_t arp_tmr;
}ipal_sys_timerstamps_t;

/* ====================================================================================================================
 * PRIVATE FUNCTIONS DECLARATION
 * They are defined at the end of this file 
 * ==================================================================================================================== */

/*
 * Resets internally allocated RAM
 */
static void s_ipal_sys_internal_ram_reset(void);

/*
 * Initfializes internally allocated variables
 */
static void s_ipal_sys_internal_ram_init(void);

/*
 * Initializes timers
 */
static void s_ipal_sys_timers_init(void);

/*
 * Increment internal timers
 */
static void s_ipal_sys_timers_tick(void);

/*
 * Process incoming packets
 */
static void s_ipal_sys_rxpkt_process(void);

/* ====================================================================================================================
 * STATIC VARIABLES
 * ==================================================================================================================== */

/* this variable is used to create a time reference incremented by 10ms */
static volatile uint32_t s_ipal_sys_localtime = 0; 

/* IPAL time granularity expressed in ms */
static uint32_t s_ipal_sys_timetick_ms; 

#if defined(IPAL_USE_TCP) || defined(IPAL_USE_ARP)
		
	/* Timestamps used for TCP and ARP modules */
	static ipal_sys_timerstamps_t s_ipal_sys_timerstamps = 
	{
		.tcp_tmr = {0,0},
		.arp_tmr = {0,0}
	};
	
#endif

/* ====================================================================================================================
 * PUBLIC FUNCTIONS (API)
 * ==================================================================================================================== */

/*
 * ipal_sys_start
 * See header file for details
 */
ipal_result_t ipal_sys_start(void)
{
	/* MARTINO Begin.
     So far commented. It will be added back in the new version (diagnostics features)	
	 */
	#if (0)
		bool link_up = false;
	#endif
	/* MARTINO End */
	
	/* Configuration coherency check */
	if(ipal_base_hid_cfg.sys_timetick == 0)
	{
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_sys_start(): missing configuration");
		return(ipal_res_NOK_generic);
	}

	/* Checks if already started */
	if(ipal_sys_hid_started == 1)
	{                                                 
		ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_sys_start(): cannot start again");
		return(ipal_res_NOK_generic);
	}

	/* 
	 * Set the timetick as an integre number of milliseconds, multiple of 1000.
	 * For instance, 1000 means 1ms, 10000 means 10ms, and so on.
	 * This value represent the granularity of internal IPAL timer, which is used for 
	 * some purposes, for instance to keep updated the ARP tables
	 */ 
	s_ipal_sys_timetick_ms = ipal_base_hid_cfg.sys_timetick/1000;
	
	/* Init lwip stack */
	lwip_init();
	
	/* Initializes timers */
	s_ipal_sys_timers_init();
	
	/* Link  UP */
	ipal_eth_hid_run();
	
	/* MARTINO: so far commented / in the future it will part of diagnostics features */
	#if (0)
		/* Wait for LINK UP (it is blocking) */
		while(!link_up)
		{
			/* Check link - Handle link up / link down */
			if ((s_ipal_sys_localtime % IPAL_LWIP_LINKUP_CHECKING_TIME) == 0)
			{		
				ethernetif_check_link (&(ipal_eth_hid_getnetif()->netif));
			}
			
			if (ipal_eth_hid_getnetif()->netif.flags & NETIF_FLAG_LINK_UP)
			{
				link_up = true;
			}
		}
	#endif
	
	/* Mark it as started */
	ipal_sys_hid_started = 1;
	
	return(ipal_res_OK);
}

/*
 * ipal_sys_start
 * See header file for details
 */
void ipal_sys_process_communication(void)
{ 
	/* IPAL Lock */
	ipal_base_hid_threadsafety_lock();
	
	/* Do periodic operations (TCP, ARM, ?Diagnostics?) */
	s_ipal_sys_timers_tick();
	
	/* Process packets */
	s_ipal_sys_rxpkt_process();
	
	/* Check link - Handle link up / link down */
	if ((s_ipal_sys_localtime % IPAL_LWIP_LINKUP_CHECKING_TIME) == 0)
	{		
		ethernetif_check_link (&(ipal_eth_hid_getnetif()->netif));
	}
	
	/* IPAL Unlock */
	ipal_base_hid_threadsafety_unlock();
}


/*
 * ipal_sys_timetick_increment
 * See header file for details
 */
void ipal_sys_timetick_increment(void)
{
	dbg_ipal_timetick = true;
	
	ipal_base_hid_threadsafety_lock();
	
	/* Local time is incremented by one "unity".  Granularity is specified by s_ipal_sys_timetick_ms.
	   One "unity" is equal to a number of ms specified by s_ipal_sys_timetick_ms */
	s_ipal_sys_localtime += 1; 
	
	ipal_base_hid_threadsafety_unlock();
	
	dbg_ipal_timetick = false;
}

/*
 * ipal_sys_timetick_increment
 * See header file for details
 */
uint32_t ipal_sys_timetick_get(void)
{
	if(0 == ipal_base_hid_cfg.sys_timetick)
	{
		return(0);
	}

	return(ipal_base_hid_cfg.sys_timetick);
}


/* ====================================================================================================================
 * FRIEND FUNCTIONS
 * ==================================================================================================================== */

/*  
 * ipal_sys_hid_getsize
 * See header file for details
 */
uint32_t ipal_sys_hid_getsize(const ipal_cfg_t *cfg)
{
	return(0);
}

/* 
 * ipal_sys_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_sys_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
	return(ipal_res_OK); 
}

/* 
 * ipal_sys_hid_init
 * See header file for details
 */
ipal_result_t ipal_sys_hid_init(ipal_cfg_t *cfg)
{
	return(ipal_res_OK); 
}

/* 
 * ipal_sys_hid_vars_init
 * See header file for details
 */
ipal_result_t ipal_sys_hid_vars_init(const ipal_cfg_t *cfg)
{
	/*  Reset the internally allocated memory */ 
	s_ipal_sys_internal_ram_reset();
	
	/* Initialize internal RAM */
	s_ipal_sys_internal_ram_init();
	
	return(ipal_res_OK);
}


/* ====================================================================================================================
 * PRIVATE FUNCTIONS DEFINITIONS
 * ==================================================================================================================== */

/* 
 * s_ipal_sys_internal_ram_reset
 * See header file for details
 */
static void s_ipal_sys_internal_ram_reset(void)
{
	ipal_sys_hid_started = 0;
}

/* 
 * s_ipal_sys_internal_ram_init
 * See header file for details
 */
static void s_ipal_sys_internal_ram_init(void)
{
	/* Nothing to do */
}

/* 
 * s_ipal_sys_timers_init
 * See header file for details
 */
static void s_ipal_sys_timers_init(void)
{
	#ifdef IPAL_USE_TCP    
		s_ipal_sys_timerstamps.tcp_tmr.value = 0;
		s_ipal_sys_timerstamps.tcp_tmr.numofticks = TCP_TMR_INTERVAL/s_ipal_sys_timetick_ms;
	#endif
	
	#ifdef IPAL_USE_ARP
		s_ipal_sys_timerstamps.arp_tmr.value = 0;
		s_ipal_sys_timerstamps.arp_tmr.numofticks = ARP_TMR_INTERVAL/s_ipal_sys_timetick_ms;
	#endif
}

/* 
 * s_ipal_sys_timers_tick
 * See header file for details
 */
static void s_ipal_sys_timers_tick(void)
{
	/* TCP timer */
	#ifdef IPAL_USE_TCP
		/* TCP periodic process every 250 ms */
		if (ipal_sys_localtime - s_ipal_sys_timerstamps.tcp_tmr.value >= s_ipal_sys_timerstamps.tcp_tmr.numofticks) //TCP_TMR_INTERVAL is expressed in millisec
		{
			s_ipal_sys_timerstamps.tcp_tmr.value =  ipal_sys_localtime;
			tcp_tmr();
		}
	#endif

	/* ARP timer */
	#ifdef IPAL_USE_ARP  		
		/* ARP periodic process every 5s - Check ARP_TMO_INTERVAL */
		if ((s_ipal_sys_localtime - s_ipal_sys_timerstamps.arp_tmr.value) >= s_ipal_sys_timerstamps.arp_tmr.numofticks) 
		{
			dbg_etharp_tmr = true;
			
			s_ipal_sys_timerstamps.arp_tmr.value =  s_ipal_sys_localtime;
			
			/* Clean cache */
			etharp_tmr();
			
			dbg_etharp_tmr = false;
		}
	#endif
}

/*
 * s_ipal_sys_rxpkt_process
 * See declaration for details
 */
static void s_ipal_sys_rxpkt_process(void)
{
	/* Poll incoming packets */
	ethernetif_poll(&(ipal_eth_hid_getnetif()->netif));
}
#endif//IPAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




