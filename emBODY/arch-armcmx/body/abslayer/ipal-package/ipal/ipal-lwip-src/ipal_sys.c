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
//#include "EOVtheSystem.h"
//#include "hal_trace.h"

#ifdef IPAL_USE_SYS


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"

//#include "ipiit.h"
#include "lwip_interface.h"


#include "ipal_base_hid.h"

#include "ipal_ftp_hid.h"
#include "ipal_telnet_hid.h"
#include "ipal_tftp_hid.h"
#include "ipal_eth_hid.h"
#include "ipal_fifo_hid.h"

// - storage for ipal
#include "ipal_ipiit_storage.h"
#include "ipal_ipiit_storage_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_sys_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern uint8_t  ipal_sys_hid_started                                    = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
    uint32_t value;
    uint32_t numofticks;
} ipal_sys_timerinfo_t;

typedef struct
{
    ipal_sys_timerinfo_t tcp_tmr;
    ipal_sys_timerinfo_t arp_tmr;
}ipal_sys_timerstamps_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_sys_internal_ram_reset(void);
static void s_ipal_sys_internal_ram_init(void);
static void s_ipal_sys_timers_init(void);
static void s_ipal_sys_timers_tick(void);
static void s_ipal_sys_rxpkt_process(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
volatile uint32_t ipal_sys_localtime = 0; /* this variable is used to create a time reference incremented by 10ms */

static ipal_sys_timerstamps_t s_ipal_sys_timerstamps = {0};

static uint32_t s_ipal_sys_timetick_ms; //timetick of cfg expressed in timetick

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern ipal_result_t ipal_sys_start(void)
{
    if(0 == ipal_base_hid_cfg.sys_timetick)
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_sys_start(): missing configuration");
        return(ipal_res_NOK_generic);
    }

    if(1 == ipal_sys_hid_started)
    {                                                 
        ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_sys_start(): cannot start again");
        return(ipal_res_NOK_generic);
    }

    s_ipal_sys_timetick_ms = ipal_base_hid_cfg.sys_timetick/1000;
    
    // init lwip stack
    lwip_init();
    
    //initi timestamp of sys timers
    s_ipal_sys_timers_init();
    
    ipal_eth_hid_run();
    
    ipal_sys_hid_started = 1;
    
    return(ipal_res_OK);
}


extern void ipal_sys_process_communication(void)
{                           
    ipal_base_hid_threadsafety_lock();
    
    s_ipal_sys_timers_tick();
    
    s_ipal_sys_rxpkt_process();
    
    ipal_base_hid_threadsafety_unlock();
}


extern void ipal_sys_timetick_increment(void)
{
    ipal_base_hid_threadsafety_lock();
    
    ipal_sys_localtime += s_ipal_sys_timetick_ms; //(ipal_base_hid_cfg.sys_timetick/1000); //e' espresso in microsecondi
    
    ipal_base_hid_threadsafety_unlock();
}


extern uint32_t ipal_sys_timetick_get(void)
{
    if(0 == ipal_base_hid_cfg.sys_timetick)
    {
        return(0);
    }

    return(ipal_base_hid_cfg.sys_timetick);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_sys_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_sys_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_sys_hid_init(ipal_cfg_t *cfg)
{
    // nothing is required
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_sys_hid_vars_init(const ipal_cfg_t *cfg)
{

    // we reset the internally allocated memory and we init internal variables
    s_ipal_sys_internal_ram_reset();
    s_ipal_sys_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_sys_init(cfg);

    
    return(ipal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_ipal_sys_internal_ram_reset(void)
{
    ipal_sys_hid_started = 0;
}


static void s_ipal_sys_internal_ram_init(void)
{
    // nothing to do
}


static void s_ipal_sys_timers_init(void)
{
#ifdef IPAL_USE_TCP    
    s_ipal_sys_timerstamps.tcp_tmr.value = 0;
    s_ipal_sys_timerstamps.tcp_tmr.numofticks = TCP_TMR_INTERVAL/s_ipal_sys_timetick_ms;
#endif
    s_ipal_sys_timerstamps.arp_tmr.value = 0;
    s_ipal_sys_timerstamps.arp_tmr.numofticks = ARP_TMR_INTERVAL/s_ipal_sys_timetick_ms;
}

static void s_ipal_sys_timers_tick(void)
{
#ifdef IPAL_USE_TCP
  /* TCP periodic process every 250 ms */
    if (ipal_sys_localtime - s_ipal_sys_timerstamps.tcp_tmr.value >= s_ipal_sys_timerstamps.tcp_tmr.numofticks) //TCP_TMR_INTERVAL is expressed in millisec
    {
        s_ipal_sys_timerstamps.tcp_tmr.value =  ipal_sys_localtime;
        tcp_tmr();
    }
#endif
  
    /* ARP periodic process every 5s */
    if ((ipal_sys_localtime - s_ipal_sys_timerstamps.arp_tmr.value) >= s_ipal_sys_timerstamps.arp_tmr.numofticks) // ARP_TMR_INTERVAL is expressed in millisec
    {
        s_ipal_sys_timerstamps.arp_tmr.value =  ipal_sys_localtime;
        etharp_tmr();
    }
}

static void s_ipal_sys_rxpkt_process(void)
{
    struct pbuf         *pbuf_ptr;
    err_t               err;
    
    ipal_eth_hid_netif *netif_ptr = ipal_eth_hid_getnetif();
 
    pbuf_ptr = low_level_input(&(netif_ptr->netif));  

    if(NULL == pbuf_ptr)
    {
        return; //ok, no problem. no pkt received!!!
    }
  
    
//     uint64_t timecurr = 0; //eov_sys_LifeTimeGet(eov_sys_GetHandle());
//     char str[60];
//     sprintf(str, "ipal_sys_rxpkt_process: time=%llu", (timecurr));
//     hal_trace_puts(str);
    
    /* entry point to the LwIP stack */
    err = netif_ptr->netif.input(pbuf_ptr, &(netif_ptr->netif));
    
    pbuf_free(pbuf_ptr); //se esco con errore riprovo a spedire il pkt??e quindi non faccio qui la free??
    
    if(ERR_OK!=err)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "error in process rx pkt by lwip");
        return;
    }
}
#endif//IPAL_USE_SYS

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




