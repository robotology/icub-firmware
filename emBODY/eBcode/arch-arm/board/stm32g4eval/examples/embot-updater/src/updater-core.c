
/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

// marco.accame@iit.it: revised on sept 2020.
// - removed osal
// - substituted osal with embot::os::rtos 



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "eupdater_parser.h"

#include "eupdater_cangtw.h"

#include "hal.h"
#include "ipal.h"
#if defined(EMBOBJ_USE_EMBOT)
#include "embot_os_rtos.h"
#else
#include "osal.h"
#endif

#include "eEsharedServices.h"



#include "eEmemorymap.h"

#include "eupdater-info.h"

#include "EoUpdaterProtocol.h"

#include "EoBoards.h"

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "updater-core.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef uint8_t (*uprot_fp_process_t) (eOuprot_opcodes_t, uint8_t *, uint16_t, eOipv4addr_t, uint8_t *, uint16_t, uint16_t *);


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_uprot_proc_NONE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_UNSUPP(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_BLINK(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_CANGATEWAY(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_PROGRAM(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_DISCOVER(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_MOREINFO(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_DEF2RUN(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_RESTART(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_IP_ADDR_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_EEPROM_ERASE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_EEPROM_READ(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_JUMP2UPDATER(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_PAGE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_JUMP2ADDRESS(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_LEGACY_MAC_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
static uint8_t s_uprot_proc_LEGACY_IP_MASK_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);

static void s_sys_reset(void);
static uint16_t s_discover_fill(eOuprot_cmd_DISCOVER_REPLY_t *reply, eOuprot_opcodes_t opcode2use, uint16_t sizeofreply2use);
static uint16_t s_add_description(char *data, uint16_t capacity, const char *prefix);
//#if     !defined(_MAINTAINER_APPL_)
static eEresult_t s_sys_eeprom_erase(void);
//#endif

static uint8_t s_overlapping_with_code_space(uint32_t addr, uint32_t size);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static uint32_t s_process_capabilities  = uprot_canDO_nothing;

#if defined(_MAINTAINER_APPL_)
static const eOuprot_process_t s_running_process = eApplPROGupdater;
#else
static const eOuprot_process_t s_running_process = eUpdater;
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void updater_core_init(void)
{   
    s_process_capabilities = eouprot_get_capabilities(s_running_process, EOUPROT_PROTOCOL_VERSION);
}


extern void updater_core_trace(const char *caller, char *format, ...)
{
#if defined(UPDATER_USE_TRACE)
    
#if defined(UPDATER_USE_FULL_TRACE)    
    char str[96];
//    char at[24];
    uint32_t ms = osal_system_ticks_abstime_get() / 1000;
    va_list args;
    va_start(args, format);
    vsnprintf(str, sizeof(str), format, args);
    va_end(args);    
    
//    snprintf(at, sizeof(at), " @ ms %d", ms)    strcat(str, at);
//    hal_trace_puts(str);      
      
    eo_errman_Trace(eo_errman_GetHandle(), str, caller);

#else    
    hal_trace_puts(caller);
#endif
 
#endif    
}



extern uint8_t updater_core_uprot_parse(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    eOuprot_opcodes_t opc = uprot_OPC_NONE;    
    uprot_fp_process_t process = s_uprot_proc_NONE;
    
    switch(pktin[0])
    {        
        case uprot_OPC_LEGACY_SCAN:
        case uprot_OPC_DISCOVER:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_SCAN_t) == pktinsize) ? uprot_OPC_LEGACY_SCAN : uprot_OPC_DISCOVER;
            process = s_uprot_proc_DISCOVER;           
        } break;
        
        case uprot_OPC_LEGACY_PROCS: 
        case uprot_OPC_MOREINFO:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_PROCS_t) == pktinsize) ? uprot_OPC_LEGACY_PROCS : uprot_OPC_MOREINFO;
            process = s_uprot_proc_MOREINFO;
        } break;       
        
        case uprot_OPC_LEGACY_CANGATEWAY:
        case uprot_OPC_CANGATEWAY:  
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_CANGATEWAY_t) == pktinsize) ? uprot_OPC_LEGACY_CANGATEWAY : uprot_OPC_CANGATEWAY;
            process = s_uprot_proc_CANGATEWAY;
        } break;   
               
        case uprot_OPC_PROG_START:
        {
            opc = uprot_OPC_PROG_START;  
            process = s_uprot_proc_PROGRAM;            
        } break;
        
        case uprot_OPC_PROG_DATA:  
        {            
            opc = uprot_OPC_PROG_DATA; 
            process = s_uprot_proc_PROGRAM;
        } break;
        
        case uprot_OPC_PROG_END:
        {            
            opc = uprot_OPC_PROG_END;    
            process = s_uprot_proc_PROGRAM;             
        } break;
        
        case uprot_OPC_DEF2RUN:
        {           
            opc = uprot_OPC_DEF2RUN;    
            process = s_uprot_proc_DEF2RUN;               
        } break;
        
        case uprot_OPC_RESTART:
        {            
            opc = uprot_OPC_RESTART; 
            process = s_uprot_proc_RESTART;
        } break;
        
        case uprot_OPC_LEGACY_IP_ADDR_SET:
        case uprot_OPC_IP_ADDR_SET:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_IP_ADDR_SET_t) == pktinsize) ? uprot_OPC_LEGACY_IP_ADDR_SET : uprot_OPC_IP_ADDR_SET;     
            process = s_uprot_proc_IP_ADDR_SET;            
        } break;
        
        case uprot_OPC_LEGACY_EEPROM_ERASE:
        case uprot_OPC_EEPROM_ERASE:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_EEPROM_ERASE_t) == pktinsize) ? uprot_OPC_LEGACY_EEPROM_ERASE : uprot_OPC_EEPROM_ERASE;         
            process = s_uprot_proc_EEPROM_ERASE;            
        } break;
                
        case uprot_OPC_EEPROM_READ: 
        {
            opc = uprot_OPC_EEPROM_READ;  
            process = s_uprot_proc_EEPROM_READ;
        } break;
        
        case uprot_OPC_BLINK:
        {
            opc = uprot_OPC_BLINK;
            process = s_uprot_proc_BLINK;
        } break;
        
        case uprot_OPC_JUMP2UPDATER:        
        {
            opc = uprot_OPC_JUMP2UPDATER;
            process = s_uprot_proc_JUMP2UPDATER;            
        } break;
        
        case uprot_OPC_PAGE_CLR:
        {
            opc = uprot_OPC_PAGE_CLR;
            process = s_uprot_proc_PAGE;              
        } break;
        
        case uprot_OPC_PAGE_SET:
        {
            opc = uprot_OPC_PAGE_SET;
            process = s_uprot_proc_PAGE;              
        } break;
        
        case uprot_OPC_PAGE_GET:   
        {
            opc = uprot_OPC_PAGE_GET;
            process = s_uprot_proc_PAGE;  
        } break;
        
        case uprot_OPC_JUMP2ADDRESS:
        {
            opc = uprot_OPC_JUMP2ADDRESS;
            process = s_uprot_proc_JUMP2ADDRESS;              
        } break;
                 
        // kept but ... maybe useless
        case uprot_OPC_LEGACY_MAC_SET:
        {
            opc = uprot_OPC_LEGACY_MAC_SET;
            process = s_uprot_proc_LEGACY_MAC_SET;              
        } break;
        
        case uprot_OPC_LEGACY_IP_MASK_SET:
        {
            opc = uprot_OPC_LEGACY_IP_MASK_SET;
            process = s_uprot_proc_LEGACY_IP_MASK_SET;   
        } break;
        
        default:
        {
            opc = uprot_OPC_NONE;
            process = s_uprot_proc_NONE;
        } break;
        
    }
    
    
    return(process(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#define _EEPROM_ERASE_KEEP_INFO32_

#define _EEPROM_ERASE_INIT_AFTERWARDS_

//#if     !defined(_MAINTAINER_APPL_)
static eEresult_t s_sys_eeprom_erase(void)
{
    eEresult_t r = ee_res_OK;
    
#if defined(_EEPROM_ERASE_KEEP_INFO32_)    
    const void *page = NULL;
    uint8_t thepage32[32] = {0};    
    ee_sharserv_info_deviceinfo_item_get(sharserv_info_page32, &page);
    memcpy(thepage32, page, 32); 
#endif
    
    eEstorage_t strg = { .type = ee_strg_eeprom, .size = EENV_MEMMAP_SHARSERV_STGSIZE, .addr = EENV_MEMMAP_SHARSERV_STGADDR};
    r = ee_sharserv_sys_storage_clr(&strg, EENV_MEMMAP_SHARSERV_STGSIZE);
    
#if defined(_EEPROM_ERASE_INIT_AFTERWARDS_)
    sharserv_mode_t sharservmode = 
    {
        .onerror    = NULL,
        .initmode   = sharserv_base_initmode_forcestorageinit
    };    
    r = ee_sharserv_init(&sharservmode);    
//    const eEmoduleInfo_t * loader_info = (const eEmoduleInfo_t*)(EENV_ROMSTART+EENV_MODULEINFO_OFFSET);      
//    ee_sharserv_part_proc_synchronise(ee_procLoader, loader_info);
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());        
    ee_sharserv_part_proc_def2run_set(ee_procUpdater);
    ee_sharserv_part_proc_startup_set(ee_procUpdater);
#endif
    

#if defined(_EEPROM_ERASE_KEEP_INFO32_)    
    // ... impose thepage32 again ...    
    ee_sharserv_info_deviceinfo_item_set(sharserv_info_page32, thepage32);
#endif

    
    return r;    
}
//#endif

static uint8_t s_overlapping_with_code_space(uint32_t addr, uint32_t size)
{
    uint32_t end = addr+size;
    
#if defined(_MAINTAINER_APPL_)
    //  maintainer cannnot program the application space
    #define BEG     EENV_MEMMAP_EAPPLICATION_ROMADDR 
    #define END     (EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MEMMAP_EAPPLICATION_ROMSIZE)
#else
    //  updater cannnot program the updater space
    #define BEG     EENV_MEMMAP_EUPDATER_ROMADDR 
    #define END     (EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MEMMAP_EUPDATER_ROMSIZE)
#endif    
 
    // updater_core_trace(CORE, "addr = %x size = %d vs beg = %x siz = %x", addr, size, BEG, END-BEG);
            
    if((addr == BEG) || (end == END))
    {   // addr or end are equal ...
        return(1);
    }
    
    if((addr > BEG) && (addr < END))
    {   // addr is within
        return(1);
    }
    
    if((end > BEG) && (end < END))
    {   // end is within
        return(1);
    }

    if((addr < BEG) && (end > END))
    {   // addr and end are across
        return(1);
    }
    
    
    return(0);
}


static void s_sys_reset(void)
{
    hal_sys_irq_disable();
    hal_sys_systemreset();  
}



static uint8_t s_uprot_proc_NONE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    // no reply
    *sizeout = 0;    
    return(0);   
}


static uint8_t s_uprot_proc_UNSUPP(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{ 
#if 1
    // no reply
    *sizeout = 0;    
    return(0); 
#else    
    eOuprot_cmdREPLY_t *reply =  (eOuprot_cmdREPLY_t*)pktout;        
    reply->opc = opc;
    reply->res = uprot_RES_ERR_UNSUPPORTED;
    reply->protversion = EOUPROT_PROTOCOL_VERSION;
    reply->sizeofextra = 0;
    *sizeout = sizeof(eOuprot_cmdREPLY_t);  
    
    return 1;
#endif    
}


static uint8_t s_uprot_proc_BLINK(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    // we simply toggle a LED. we could use the object EOtheLEDpulser, but we do it much more simply
    for(uint8_t n=0; n<16; ++n)
    { 
        hal_led_toggle(hal_led2);
#if defined(EMBOBJ_USE_EMBOT)
        embot::core::wait(250*embot::core::time1millisec);
#else
        osal_task_wait(250*1000);
#endif        
        
    }
       
    // no reply
    *sizeout = 0; 
    return(0);   
}

// both uprot_OPC_CANGATEWAY and uprot_OPC_LEGACY_CANGATEWAY. the only difference is that the former has params hence is of length 8 instad of 1
static uint8_t s_uprot_proc_CANGATEWAY(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
#if !defined(_MAINTAINER_APPL_)  

    cangtw_parameters_t params = {0};
    cangtw_parameters_t *pp = NULL;
    
    if(uprot_OPC_LEGACY_CANGATEWAY == opc)
    {
        // legacy mode: ... we use default params
        pp = NULL;
    }
    else
    {
        eOuprot_cmd_CANGATEWAY_t *cg = (eOuprot_cmd_CANGATEWAY_t*) pktin;
        params.t_can_stabilisation = cg->time4canstable;
        params.send_ff = cg->sendcanbroadcast;
        params.t_wait_ff_reply = cg->time2waitcanreply;
        params.clear_can_onentry_gtw = cg->rxcanbufferclear;
        params.send_ack = cg->ackrequired;
        
        pp = &params;
    }
    
    eupdater_cangtw_start(remaddr, pp);    
    
    // no reply for now ...
    *sizeout = 0;   
    return(0);    
#else
    // no reply at all ...
    *sizeout = 0;   
    return(0);       
#endif
}

static uint8_t s_uprot_proc_PROGRAM(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    // i use a single funtion for start, data, end, so that i embed in here 
    
    static uint32_t s_ramforloader_maxoffset = 0;
    static uint8_t s_ramforloader_data[EENV_MEMMAP_ELOADER_ROMSIZE] = {0};

    
    static uint32_t s_proc_PROG_mem_start = 0xFFFFFFFF;
    static uint32_t s_proc_PROG_mem_size  = 0;
    static uint16_t s_proc_PROG_rxpackets = 0;
    static uint8_t  s_proc_PROG_downloading_partition = 0;
    static uint8_t  s_proc_PROG_flash_erased = 0;
    
    // prepare the ok reply.
    eOuprot_cmdREPLY_t *reply =  (eOuprot_cmdREPLY_t*)pktout;        
    reply->opc = opc;
    reply->res = uprot_RES_OK;
    reply->protversion = EOUPROT_PROTOCOL_VERSION;
    reply->sizeofextra = 0;
    *sizeout = sizeof(eOuprot_cmdREPLY_t);
    const uint8_t ret = 1;
      
    
    switch(opc)
    {
        case uprot_OPC_PROG_START:
        {
            // we init everything for programming the FLASH ...       
            s_proc_PROG_rxpackets = 1;
            s_proc_PROG_flash_erased = 0;    
            s_proc_PROG_downloading_partition = 0;   
            s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;        
                        
            // here is the command. i get what i need to program into flash
            eOuprot_cmd_PROG_START_t *cmd = (eOuprot_cmd_PROG_START_t*)pktin;
                      
            
            switch(cmd->partition)
            {
                case uprot_partitionLOADER:
                {
                    s_proc_PROG_downloading_partition = uprot_partitionLOADER;
                    s_proc_PROG_mem_start = EENV_MEMMAP_ELOADER_ROMADDR;
                    s_proc_PROG_mem_size  = EENV_MEMMAP_ELOADER_ROMSIZE;
                } break;

                case uprot_partitionUPDATER:
                {
                    s_proc_PROG_downloading_partition = uprot_partitionUPDATER;
                    s_proc_PROG_mem_start = EENV_MEMMAP_EUPDATER_ROMADDR;
                    s_proc_PROG_mem_size  = EENV_MEMMAP_EUPDATER_ROMSIZE;
                } break;

                case uprot_partitionAPPLICATION:
                {
                    s_proc_PROG_downloading_partition = uprot_partitionAPPLICATION;
                    s_proc_PROG_mem_start = EENV_MEMMAP_EAPPLICATION_ROMADDR;
                    s_proc_PROG_mem_size  = EENV_MEMMAP_EAPPLICATION_ROMSIZE;
                } break;
                        
                default:
                {
                    s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;
                } break;                
            }
                
            // cannot accept a download of such a hex file
            if(0 == s_proc_PROG_downloading_partition)
            {
                s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;
                
                reply->res = uprot_RES_ERR_UNK;
                return ret;                
            } 
            
            // the running process does not support programming the partition s_proc_PROG_downloading_partition
            if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, s_proc_PROG_downloading_partition))
            {
                s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;
                
                //return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
                reply->res = uprot_RES_ERR_UNSUPPORTED;
                return ret;    
            } 

            
            // now we check about overlapping between the running process and the space reserved to the declared process to program in flash.
            // if we declare wrong (we tell it is an application but we send the hex of an updater) ... however, in here we cannot do anything about it.
            // but there are protections in the following. 
            if(1 == s_overlapping_with_code_space(s_proc_PROG_mem_start, s_proc_PROG_mem_size))
            {
                // updater_core_trace("CORE", "overlapping");
                s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;
                
                reply->res = uprot_RES_ERR_UNK; 
                return ret;              
            }
            
            
            // we make sure to be able to recover in case something goes wrong, thus:
            // 1.   if this process is eupdater and:
            //      a.  we program the loader: we set startup = eupdater. we buffer in ram the whole loader and we erase the flash only when we 
            //          are sure we have received everything (at reception of the uprot_OPC_PROG_END). that is the safest way to proceed.        
            //      b.  we program the application: we set startup = eupdater. we dont have enough RAM to store the whole binary, hence we must erase the flash 
            //          at latest at reception of the first uprot_OPC_PROG_DATA.
            // 2.   if this process is the maintainer (which uses the space of application) and:
            //      a.  we program the loader: ... hmmm better to simplify and to avoid that. 
            //          however ... we set startup = eapplication. we buffer in ram the whole loader.
            //      b.  we program the updater: we set startup = eapplication. we dont have enough RAM to store the whole binary, hence we must erase the flash 
            //          at latest at reception of the first uprot_OPC_PROG_DATA.

            
            eEprocess_t startup = (eApplPROGupdater == s_running_process) ? (ee_procApplication) : (ee_procUpdater);
            ee_sharserv_part_proc_startup_set(startup);
        
                     
            if(uprot_partitionLOADER == s_proc_PROG_downloading_partition)
            {
                s_ramforloader_maxoffset = 0;
                memset(s_ramforloader_data, 0xff, s_proc_PROG_mem_size);
            }
                
            // download is about to begin, thus start blinking led in a different way
            eupdater_parser_download_blinkled_start();
       
        } break;
        
        
        case uprot_OPC_PROG_DATA:
        {
            // signal download activity
            eupdater_parser_download_toggleled();
            

            // here is the command. i get what i need to program into flash
            eOuprot_cmd_PROG_DATA_t *cmd = (eOuprot_cmd_PROG_DATA_t*)pktin;
            uint32_t address = (cmd->address[0]) | (cmd->address[1] << 8) | (cmd->address[2] << 16) | (cmd->address[3] << 24);
            uint16_t size = (cmd->size[0]) | (cmd->size[1] << 8);
            uint8_t *data = &cmd->data[0];
            
            // if i am not in programming mode, i quit
            if(0 == s_proc_PROG_downloading_partition)
            {
                reply->res = uprot_RES_ERR_UNK; 
                return ret;   
            }  
#if 0
            // before it was like that .... but it does not allow to write applications with bigger code space ....
            if((address < s_proc_PROG_mem_start) || ((address+size) >= (s_proc_PROG_mem_start+s_proc_PROG_mem_size)))
            {   
                // i cannot write below the space of the process AND i cannot write above the space of the process.
                reply->res = uprot_RES_ERR_PROT;
                return ret;            
            } 
#else
            // now we allow to program an application which goes out of upper boundary.
            if(uprot_partitionAPPLICATION == s_proc_PROG_downloading_partition)
            {   // prevent writing only is address is lower ...
                if(address < s_proc_PROG_mem_start)
                {   
                    // i cannot write below the space of the process
                    reply->res = uprot_RES_ERR_PROT;
                    return ret;            
                }               
            }
            else 
            {   // prevent writing lower and higher 
                if((address < s_proc_PROG_mem_start) || ((address+size) >= (s_proc_PROG_mem_start+s_proc_PROG_mem_size)))
                {   
                    // i cannot write below the space of the process AND i cannot write above the space of the process.
                    reply->res = uprot_RES_ERR_PROT;
                    return ret;            
                }              
            }   
#endif
            
    // it could be in this way, but ... check vs deletion of flash beyond the limit ... 
    //        if(address < s_proc_PROG_mem_start)
    //        {   
    //            // i cannot write below the space of the process
    //            reply->res = uprot_RES_ERR_PROT;
    //            return ret;            
    //        } 
    //
    //        if((uprot_partitionAPPLICATION != s_proc_PROG_downloading_partition) && ((address+size) >= (s_proc_PROG_mem_start+s_proc_PROG_mem_size)))
    //        {   
    //            // i cannot write beyond the space of the process unless i want to write into an application. 
    //            #warning TODO: i should put a check vs limit of 1024k ... but nevemind for now
    //            reply->res = uprot_RES_ERR_PROT;
    //            return ret;            
    //        } 

            // ok, i can go on
            

            if(uprot_partitionLOADER == s_proc_PROG_downloading_partition)
            {   // if loader: i write into ram. no need to erase the flash at this stage
                
                uint32_t position = address - s_proc_PROG_mem_start;
                if(position < s_proc_PROG_mem_size)
                {
                    memcpy(&s_ramforloader_data[position], data, size);
                    if((position+size)> s_ramforloader_maxoffset)
                    {
                        s_ramforloader_maxoffset = position+size;
                    }
                }
                else
                {
                    reply->res = uprot_RES_ERR_PROT;
                    return ret;
                }
                ++s_proc_PROG_rxpackets;            
                
            }
            else // if updater or application
            {
                // if never done: erase the flash
                if(0 == s_proc_PROG_flash_erased)
                {
                    hal_sys_irq_disable();  //osal_system_scheduling_suspend();                       
                    hal_result_t halres = hal_flash_erase(s_proc_PROG_mem_start, s_proc_PROG_mem_size);
                    hal_sys_irq_enable();   //osal_system_scheduling_restart();
                    
                        
                    if(hal_res_OK != halres)
                    {
                        s_proc_PROG_downloading_partition = 0;
                        // updater_core_trace("CORE", "ERASE FAILED");
                        reply->res = uprot_RES_ERR_FLASH;
                        return ret;    
                    }
                    else
                    {
                        s_proc_PROG_flash_erased = s_proc_PROG_downloading_partition;
                        // updater_core_trace("CORE", "ERASE DONE");
                    }  
                }
                
                // write the chunk of received data only if it is inside the expected memory space and it is already erased
                if(0 != s_proc_PROG_flash_erased)
                {                
                    hal_sys_irq_disable();
                    hal_result_t halres = hal_flash_write(address, size, data);
                    hal_sys_irq_enable();
                    
                    ++s_proc_PROG_rxpackets;
                    
                    if(hal_res_NOK_generic == halres)
                    {
                        reply->res = uprot_RES_ERR_PROT;
                        return ret;
                    }
                }
             
            }
            
        } break;
        
        
        case uprot_OPC_PROG_END:
        {
            // download is over, thus resume the normal blinking of led
            eupdater_parser_download_blinkled_stop();
            
            
            // here is the command. i get what i need to program into flash
            eOuprot_cmd_PROG_END_t *cmd = (eOuprot_cmd_PROG_END_t*)pktin;
            uint16_t sentpkts = (cmd->numberofpkts[0]) | (cmd->numberofpkts[1] << 8);
            
            // some error cases. first: i have an unexpectedd s_proc_PROG_downloading_partition equal to 0 ... why? i keep previous code
            if(0 == s_proc_PROG_downloading_partition)
            {
                if(0 != s_proc_PROG_flash_erased)
                {   // if i have previously erased the flash ... well at least i refresh the partition table of updater and application
                    const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                    const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                    ee_sharserv_part_proc_synchronise(ee_procUpdater, (const eEmoduleInfo_t*)updt);
                    ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                    // updater_core_trace("CORE", "CMD_END received and s_proc_PROG_downloading_partition is 0 and s_erased_eeprom is not 0"); 
                } 
                 
                s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;            
                reply->res = uprot_RES_ERR_UNK;
                return ret;            
            }
            
            // some error cases. second: number of sent and of received packets are mismathing
            if(sentpkts != s_proc_PROG_rxpackets)
            {

                // must manage the erased flash ... it is only for updater or application because the eloader flash is not erased yeet
                if(0 != s_proc_PROG_flash_erased)
                {
                    // must manage failure. we surely have erased flash of eapplication / eupdater in here and maybe we have written some sectors.
                    // thus we had better to erase flash fully, set the def2run and start to a safe process, and ... put in partition table message about erased process
                    if(s_proc_PROG_downloading_partition != uprot_partitionLOADER)
                    {   // application or updater
                        
                        eEmoduleInfo_t erasedproc = {0};
                        erasedproc.info.entity.type = ee_entity_process;                      
                        erasedproc.info.entity.builddate.year = 1999;
                        erasedproc.info.entity.builddate.month = 9;
                        erasedproc.info.entity.builddate.day = 13;

#if defined(EMBOBJ_USE_EMBOT)
                        embot::os::rtos::scheduler_lock();
#else
                        osal_system_scheduling_suspend();
#endif
                        
                        if(uprot_partitionAPPLICATION == s_proc_PROG_downloading_partition)
                        {
                            hal_flash_erase(EENV_MEMMAP_EAPPLICATION_ROMADDR, EENV_MEMMAP_EAPPLICATION_ROMSIZE);
                            // updater_core_trace("CORE", "erased app and removed it from partition table"); 

                            erasedproc.info.entity.signature = ee_procApplication;
                            erasedproc.info.rom.addr = EENV_MEMMAP_EAPPLICATION_ROMADDR;
                            memcpy(erasedproc.info.name, "erasedUpd", sizeof("erasedApp"));
                            
                            ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)&erasedproc);
                            const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                            ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)updt);
                            ee_sharserv_part_proc_startup_set(ee_procUpdater);
                            ee_sharserv_part_proc_def2run_set(ee_procUpdater);
                        }
                        else if(uprot_partitionUPDATER == s_proc_PROG_downloading_partition)
                        {
                            hal_flash_erase(EENV_MEMMAP_EUPDATER_ROMADDR, EENV_MEMMAP_EUPDATER_ROMSIZE);
                            // cannot remove updater so far .... ee_sharserv_part_proc_rem(ee_procUpdater);
                            // updater_core_trace("CORE", "erased upt and removed it from partition table"); 
                            
                            erasedproc.info.entity.signature = ee_procUpdater;
                            erasedproc.info.rom.addr = EENV_MEMMAP_EUPDATER_ROMADDR;
                            memcpy(erasedproc.info.name, "erasedUpd", sizeof("erasedUpd"));
                            
                            ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)&erasedproc);
                            const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                            ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                            ee_sharserv_part_proc_startup_set(ee_procApplication);
                            ee_sharserv_part_proc_def2run_set(ee_procApplication);
                        }
#if defined(EMBOBJ_USE_EMBOT)
                        embot::os::rtos::scheduler_unlock();
#else
                        osal_system_scheduling_restart();
#endif                       
                        
                    }
                    

                }
                
                s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;
                reply->res = uprot_RES_ERR_LOST;
                return ret;
            }

            // everything seems ok: we finish. actions depens on what i am managing
            switch(s_proc_PROG_downloading_partition)
            {
                case uprot_partitionLOADER:
                {
                    // 1. erase flash and write what in ram. do it as quickly and as safely as possible
                    hal_sys_irq_disable();                   
                    hal_flash_erase(s_proc_PROG_mem_start, s_proc_PROG_mem_size);
                    hal_flash_write(s_proc_PROG_mem_start, s_proc_PROG_mem_size, s_ramforloader_data);
                    hal_sys_irq_enable();  
                    s_proc_PROG_flash_erased = uprot_partitionLOADER;                                              
                    const volatile eEmoduleInfo_t* lder = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_ELOADER_ROMADDR+EENV_MODULEINFO_OFFSET);
                    ee_sharserv_part_proc_synchronise(ee_procLoader,(const eEmoduleInfo_t*)lder);
                } break; 
                
                case uprot_partitionUPDATER:
                {
                    const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                    ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)updt);
                    //ee_sharserv_part_proc_startup_set(ee_procApplication);
                    //ee_sharserv_part_proc_def2run_set(ee_procApplication);
                } break;
                
                case uprot_partitionAPPLICATION:
                {
                    const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                    ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                    //ee_sharserv_part_proc_startup_set(ee_procUpdater);
                }  break;
                
                default:
                {   // it is not possible from the code i have ...
                    reply->res = uprot_RES_ERR_UNK;
                } break;
            }
       
            s_proc_PROG_downloading_partition = s_proc_PROG_mem_start = s_proc_PROG_mem_size = 0;   
               
        } break;
        
        
        default:
        {
            reply->res = uprot_RES_ERR_UNK;
        } break;
        
    }
     
       
    return ret;
}




static uint16_t s_discover_fill(eOuprot_cmd_DISCOVER_REPLY_t *reply, eOuprot_opcodes_t opcode2use, uint16_t sizeofreply2use)
{
    uint16_t size = sizeof(eOuprot_cmd_DISCOVER_REPLY_t);  
    

    reply->reply.opc = opcode2use;
    reply->reply.res = uprot_RES_OK;
    reply->reply.protversion = EOUPROT_PROTOCOL_VERSION;
    reply->reply.sizeofextra = sizeofreply2use - sizeof(eOuprot_cmdREPLY_t);   

    // ok, now i fill the values
    
    const eEipnetwork_t *ipnetworkstrg = NULL;
    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
    
    const eEboardInfo_t* boardinfo = NULL;
    ee_sharserv_info_boardinfo_get(&boardinfo);
    // now get the board type.
    uint8_t boardtype = 255; // use eobrd_ethtype_unknown later on
    if(0x11 == boardinfo->info.entity.signature)
    {   // old eLoader upto version 2.11 of build date 2015 May 26 11:11. it have info about board type in a string
        
        if(0 == strcmp((const char*)boardinfo->info.name, "ems4rd"))
        {
            boardtype = eobrd_ethtype_ems4; // use eobrd_ethtype_ems4 later on        
        }
        else if(0 == strcmp((const char*)boardinfo->info.name, "mc4plus"))
        {
             boardtype = eobrd_ethtype_mc4plus; // use eobrd_ethtype_mc4plus later on 
        }        
    }
    else
    {   // after eLoader version 2.12 the fields boardinfo->info.entity.signature contains the board type. 
        boardtype = boardinfo->info.entity.signature;
    }
    
    eEprocess_t startup = ee_procNone;
    eEprocess_t def2run = ee_procNone;
    ee_sharserv_part_proc_startup_get(&startup);
    ee_sharserv_part_proc_def2run_get(&def2run);
    
    uint8_t nprocs = 0;
    const eEprocess_t *s_proctable = NULL;
    ee_sharserv_part_proc_allavailable_get(&s_proctable, &nprocs);
    
    reply->boardtype = boardtype;
    reply->unused[0] = EOUPROT_VALUE_OF_UNUSED_BYTE;
    reply->capabilities = s_process_capabilities;
    reply->mac48[0] = (ipnetworkstrg->macaddress>>40) & 0xFF;
    reply->mac48[1] = (ipnetworkstrg->macaddress>>32) & 0xFF;
    reply->mac48[2] = (ipnetworkstrg->macaddress>>24) & 0xFF;
    reply->mac48[3] = (ipnetworkstrg->macaddress>>16) & 0xFF;
    reply->mac48[4] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
    reply->mac48[5] = (ipnetworkstrg->macaddress)     & 0xFF;
    reply->processes.numberofthem = nprocs;
    reply->processes.startup = startup;
    reply->processes.def2run = def2run;
    reply->processes.runningnow = s_running_process;
    
    for(uint8_t i=0; i<3; i++)
    {
        const eEmoduleInfo_t *s_modinfo = NULL;
        ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);
        reply->processes.info[i].type = s_proctable[i];
        reply->processes.info[i].filler[0] = EOUPROT_VALUE_OF_UNUSED_BYTE;
        reply->processes.info[i].version.major = s_modinfo->info.entity.version.major;
        reply->processes.info[i].version.minor = s_modinfo->info.entity.version.minor;
        memcpy(&reply->processes.info[i].date, &s_modinfo->info.entity.builddate, sizeof(eOdate_t));
        
        reply->processes.info[i].compilationdate.year = 1999;
        reply->processes.info[i].compilationdate.month = 9;
        reply->processes.info[i].compilationdate.day = 9;
        reply->processes.info[i].compilationdate.hour = 9;
        reply->processes.info[i].compilationdate.min = 9;
        
        volatile eEmoduleExtendedInfo_t * extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);            
        if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
        {
            eo_common_compiler_string_to_date((const char*)extinfo->compilationdatetime, &reply->processes.info[i].compilationdate);
        }
       
        reply->processes.info[i].rom_addr_kb = s_modinfo->info.rom.addr / 1024;
        reply->processes.info[i].rom_size_kb = s_modinfo->info.rom.size / 1024;  
      
    }

    // now boardinfo32
    const void *page = NULL;
    ee_sharserv_info_deviceinfo_item_get(sharserv_info_page32, &page);
    memcpy(reply->boardinfo32, page, 32);   
    
    return size;
}


// both uprot_OPC_DISCOVER and uprot_OPC_LEGACY_SCAN. the only difference is that the reply to the former is more structured
static uint8_t s_uprot_proc_DISCOVER(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{    
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
        
    const eEmoduleInfo_t* module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
    
    if(eApplPROGupdater == s_running_process)
    {
        module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
    }
          
    
    if(uprot_OPC_LEGACY_SCAN == opc)
    {    
        const uint8_t BOARD_TYPE_EMS = 0x0A;
        // updater_core_trace("CORE", "CMD_SCAN");
        *sizeout = 14;
        
        pktout[ 0] = uprot_OPC_LEGACY_SCAN;
        pktout[ 1] = module->info.entity.version.major;
        pktout[ 2] = module->info.entity.version.minor;
        pktout[ 3] = BOARD_TYPE_EMS;
        
        const eEipnetwork_t *ipnetworkstrg;
        ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);

        pktout[ 4] = (ipnetworkstrg->ipnetmask>>24) & 0xFF;
        pktout[ 5] = (ipnetworkstrg->ipnetmask>>16) & 0xFF;
        pktout[ 6] = (ipnetworkstrg->ipnetmask>>8)  & 0xFF;
        pktout[ 7] =  ipnetworkstrg->ipnetmask      & 0xFF;

        pktout[ 8] = (ipnetworkstrg->macaddress>>40) & 0xFF;
        pktout[ 9] = (ipnetworkstrg->macaddress>>32) & 0xFF;
        pktout[10] = (ipnetworkstrg->macaddress>>24) & 0xFF;
        pktout[11] = (ipnetworkstrg->macaddress>>16) & 0xFF;
        pktout[12] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
        pktout[13] = (ipnetworkstrg->macaddress)     & 0xFF;        
    }
    else
    {
#if defined(_MAINTAINER_APPL_)     
        eOuprot_cmd_DISCOVER_t *cmd = (eOuprot_cmd_DISCOVER_t*) pktin;        
        if(1 == cmd->jump2updater)
        {            
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
            ee_sharserv_sys_restart();            
        }        
#endif        
        // prepare the reply.
        eOuprot_cmd_DISCOVER_REPLY_t *reply = (eOuprot_cmd_DISCOVER_REPLY_t*) pktout;        
        *sizeout = s_discover_fill(reply, uprot_OPC_DISCOVER, sizeof(eOuprot_cmd_DISCOVER_REPLY_t));
    }
    
    return 1;    
}


static uint16_t s_add_description(char *data, uint16_t capacity, const char *prefix)
{
    uint16_t size = 0;
        
    size+=snprintf(data+size, capacity, "%s", prefix);

    uint8_t num_procs = 0;
    const eEprocess_t *s_proctable = NULL;
    const eEmoduleInfo_t *s_modinfo = NULL;
    
    const eEmoduleInfo_t *sharserv = ee_sharserv_moduleinfo_get();
    const eEmoduleInfo_t *strsharserv = ee_sharserv_storage_moduleinfo_get();
    
    if((strsharserv->info.entity.version.major != sharserv->info.entity.version.major) || (strsharserv->info.entity.version.minor != sharserv->info.entity.version.minor))
    {
        size+=snprintf(data+size, capacity, "EEPROM contains sharserv version different from that of this process. it has:\r\n");
        size+=snprintf(data+size, capacity, "eeprom version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
                strsharserv->info.entity.version.major, 
                strsharserv->info.entity.version.minor,
                strsharserv->info.entity.builddate.month,
                strsharserv->info.entity.builddate.day,
                strsharserv->info.entity.builddate.year,
                strsharserv->info.entity.builddate.hour,
                strsharserv->info.entity.builddate.min
            );  
        size+=snprintf(data+size, capacity, "process version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
                sharserv->info.entity.version.major, 
                sharserv->info.entity.version.minor,
                sharserv->info.entity.builddate.month,
                sharserv->info.entity.builddate.day,
                sharserv->info.entity.builddate.year,
                sharserv->info.entity.builddate.hour,
                sharserv->info.entity.builddate.min
            );                       
    }
    
    if(ee_res_NOK_generic == ee_sharserv_storage_isvalid())
    {
        size+=snprintf(data+size, capacity, "*** EEPROM FORMAT IS NOT VALID\r\n");
        size+=snprintf(data+size, capacity, "*** CANNOT GIVE PARTITION INFO\r\n");              
        size+=snprintf(data+size, capacity, "*** BYE BYE\r\n"); 
        size+=snprintf(data+size, capacity, "\n");                    
    }                        
    else if (ee_res_OK == ee_sharserv_part_proc_allavailable_get(&s_proctable, &num_procs))
    {        
        volatile eEmoduleExtendedInfo_t* extinfo = NULL;    
        eEprocess_t defproc;
        eEprocess_t startup;
        eEprocess_t indexofrunning =
#if defined(_MAINTAINER_APPL_)  
                                ee_procApplication;
#else
                                ee_procUpdater;
#endif
            
        ee_sharserv_part_proc_def2run_get(&defproc);
        ee_sharserv_part_proc_startup_get(&startup);
                    
        // processes
        for (uint8_t i=0; i<num_procs; ++i)
        {
            ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);

            size+=snprintf(data+size, capacity, "*** e-process #%d \r\n", i);
            size+=snprintf(data+size, capacity, "props %s%s%s \r\n", defproc==i?"DEF ":"", startup==i?"START ":"", indexofrunning==i?"RUNNING ":"" ) ;

            size+=snprintf(data+size, capacity, "name  %s\r\n", s_modinfo->info.name);
            size+=snprintf(data+size, capacity, "vers  %d.%d\r\n", 
                s_modinfo->info.entity.version.major, 
                s_modinfo->info.entity.version.minor
            );
            size+=snprintf(data+size, capacity, "date  %s %.2d %d %d:%.2d\r\n", 
                ee_common_get_month_string(s_modinfo->info.entity.builddate),
                s_modinfo->info.entity.builddate.day,                    
                s_modinfo->info.entity.builddate.year,
                s_modinfo->info.entity.builddate.hour,
                s_modinfo->info.entity.builddate.min
            );
            
            extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);
            
            if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
            {
                size+=snprintf(data+size, capacity, "built %s\r\n", 
                    extinfo->compilationdatetime
                );                        
            }
            else
            {
                size+=snprintf(data+size, capacity, "built unknown date\r\n"
                );    
            }

            size+=snprintf(data+size, capacity, "rom   @+%dKB, s=%dKB\r\n", (s_modinfo->info.rom.addr-EENV_ROMSTART+1023)/1024, (s_modinfo->info.rom.size+1023)/1024);
        }

    }
    
    return size;
}



static uint8_t s_uprot_proc_MOREINFO(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{    
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }    
    
    if(uprot_OPC_LEGACY_PROCS == opc)
    {
        pktout[0] = uprot_OPC_LEGACY_PROCS;
        pktout[1] = 0;         

        *sizeout = 2 + s_add_description((char*)&pktout[2], capacityout - 2, "legacy PROCS\r\n");        
    }
    else
    {        
        eOuprot_cmd_MOREINFO_t *cmd = (eOuprot_cmd_MOREINFO_t*) pktin;
        
        // prepare the reply.        
        eOuprot_cmd_MOREINFO_REPLY_t *reply = (eOuprot_cmd_MOREINFO_REPLY_t*) pktout;
        
        // surely there is the discover part.
        *sizeout = s_discover_fill(&reply->discover, uprot_OPC_MOREINFO, sizeof(eOuprot_cmd_MOREINFO_REPLY_t));

        // but there may be also the description
        reply->hasdescription = cmd->plusdescription;
        

        if(1 == reply->hasdescription)
        {
            // 1. add the description in reply->description
            uint16_t sizeofdescription = s_add_description((char*)reply->description, capacityout - sizeof(eOuprot_cmd_MOREINFO_REPLY_t), "MOREINFO\r\n"); // we dont care about having 3 bytes less ...
            
            // 2. increase some sizes  
            reply->discover.reply.sizeofextra += sizeofdescription;
            *sizeout += sizeofdescription;                
        }
 
    }
    
    return 1;    
}


static uint8_t s_uprot_proc_DEF2RUN(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{    
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    eOuprot_cmd_DEF2RUN_t *cmd = (eOuprot_cmd_DEF2RUN_t*) pktin;
        
    if((eUpdater == cmd->proc) || (eApplication == cmd->proc))
    {
        ee_sharserv_part_proc_startup_set(ee_procUpdater);
        ee_sharserv_part_proc_def2run_set(cmd->proc);
    }                

    // no reply
    *sizeout = 0;   
    return(0);      
}


static uint8_t s_uprot_proc_RESTART(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    s_sys_reset();

    // no reply
    *sizeout = 0;   
    return(0);      
}

static uint8_t s_uprot_proc_IP_ADDR_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{ 
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    // both uprot_OPC_LEGACY_IP_ADDR_SET and uprot_OPC_IP_ADDR_SET have the same initial layout until address. thus, i use eOuprot_cmd_IP_ADDR_SET_t for both  
    eOuprot_cmd_IP_ADDR_SET_t *cmd = (eOuprot_cmd_IP_ADDR_SET_t*) pktin; 
    
    if((0 == cmd->address[3]) || (255 == cmd->address[3]))
    {   // dont allow invalid address
        // no reply
        *sizeout = 0;   
        return(0);    
    }
    
    eEipnetwork_t ipnetwork;
    const eEipnetwork_t *ipnetworkstrg;

    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
    memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
    ipnetwork.ipaddress = EECOMMON_ipaddr_from(cmd->address[0], cmd->address[1], cmd->address[2], cmd->address[3]);
    
    ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork);
    
    
    if(uprot_OPC_IP_ADDR_SET == opc)
    {
        if(1 == cmd->sysrestart)
        {
            s_sys_reset(); 
        }        
    }
    
    // no reply
    *sizeout = 0;   
    return(0);      
}


static uint8_t s_uprot_proc_EEPROM_ERASE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    const uint8_t ret = 1; 
    
// uncomment only to test...    
//    opc = uprot_OPC_EEPROM_ERASE;    
//    eOuprot_cmd_EEPROM_ERASE_t *cc = (eOuprot_cmd_EEPROM_ERASE_t*) pktin; 
//    cc->size = 0;
//    cc->address = 0;
//    cc->sysrestart = 0;
    
    if(uprot_OPC_LEGACY_EEPROM_ERASE == opc)
    {
        *sizeout = 2;
        pktout[0] = uprot_OPC_LEGACY_EEPROM_ERASE;
        pktout[1] = uprot_RES_ERR_UNK; 

//#if     !defined(_MAINTAINER_APPL_)        
//        // only the updater can erase eeprom 
        eEresult_t rr = s_sys_eeprom_erase(); 
        pktout[1] = (ee_res_OK == rr) ? uprot_RES_OK : uprot_RES_ERR_UNK;        
//#endif      
    }
    else
    {
        // prepare the ok reply.
        eOuprot_cmdREPLY_t *reply =  (eOuprot_cmdREPLY_t*)pktout;        
        reply->opc = uprot_OPC_EEPROM_ERASE;
        reply->res = uprot_RES_OK;
        reply->protversion = EOUPROT_PROTOCOL_VERSION;
        reply->sizeofextra = 0;
        *sizeout = sizeof(eOuprot_cmdREPLY_t);
  
//#if     defined(_MAINTAINER_APPL_)
//            // the maintainer cannot erase the eeprom
//        reply->res = uprot_RES_ERR_UNK;  
//        return ret;                    
//#endif  
        
        eOuprot_cmd_EEPROM_ERASE_t *cmd = (eOuprot_cmd_EEPROM_ERASE_t*) pktin; 
        
        if(0 != cmd->size)
        {
            // we only erase the specified eeprom
            hal_result_t re = hal_eeprom_erase(hal_eeprom_i2c_01, cmd->address, cmd->size);
            reply->res = (hal_res_OK == re) ? uprot_RES_OK : uprot_RES_ERR_UNK; 
        }
        else
        {
            // we erase the sys eeprom in [6k, 8k). 
            eEresult_t rr = s_sys_eeprom_erase();
            reply->res = (ee_res_OK == rr) ? uprot_RES_OK : uprot_RES_ERR_UNK;  
        }            
                                      
        if(1 == cmd->sysrestart)
        {
            s_sys_reset();          
        }
                
    }

    return ret;    
}


static uint8_t s_uprot_proc_EEPROM_READ(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }    
    
    eOuprot_cmd_EEPROM_READ_t *cmd = (eOuprot_cmd_EEPROM_READ_t*) pktin; 
    
    uint16_t size = (cmd->size > 1024) ? (1024) : (cmd->size);

    
    eOuprot_cmd_EEPROM_READ_REPLY_t * reply = (eOuprot_cmd_EEPROM_READ_REPLY_t*) pktout;
    reply->reply.opc = uprot_OPC_EEPROM_READ;
    reply->reply.res = uprot_RES_OK;
    reply->reply.protversion = EOUPROT_PROTOCOL_VERSION;
    reply->reply.sizeofextra = 4 + size;
    *sizeout = sizeof(eOuprot_cmdREPLY_t) + reply->reply.sizeofextra;
    const uint8_t ret = 1;    

    reply->size = size;
    reply->address = cmd->address;
    hal_eeprom_read(hal_eeprom_i2c_01, reply->address, reply->size, reply->eeprom);


    return ret;
}

static uint8_t s_uprot_proc_JUMP2UPDATER(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{       
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
//#if defined(_MAINTAINER_APPL_)   
    ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
    ee_sharserv_sys_restart();
//#endif
    
    // no reply
    return 0;         
}

static uint8_t s_uprot_proc_PAGE(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{   
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    uint8_t ret = 0; 
    *sizeout = 0;    

    // i get the item. all messages have the filed displacedd by the same offset, thus ... i pick any message
    eOuprot_cmd_PAGE_SET_t * cmd = (eOuprot_cmd_PAGE_SET_t*) pktin;
    
    ee_sharserv_info_deviceinfo_item_t item = sharserv_info_ipnet;
    eObool_t itiszero = eobool_false;
    switch(cmd->pagesize)
    {
        case 0:
        {
            itiszero = eobool_true;
        } break;
        
        case 8:
        {
            item = sharserv_info_page08;
        } break; 
        case 32:
        {
            item = sharserv_info_page32;
        } break;  
        case 64:
        {
            item = sharserv_info_page64;
        } break; 
        case 128:
        {
            item = sharserv_info_page128;
        } break;    
        default:
        {
            item = sharserv_info_ipnet;
        } break;        
    }
    
    // now i operate
    switch(opc)
    {
        case uprot_OPC_PAGE_CLR:
        {
            if(eobool_true == itiszero)
            {
                // delete them all
                ee_sharserv_info_deviceinfo_item_clr(sharserv_info_page08);
                ee_sharserv_info_deviceinfo_item_clr(sharserv_info_page32);
                ee_sharserv_info_deviceinfo_item_clr(sharserv_info_page64);
                ee_sharserv_info_deviceinfo_item_clr(sharserv_info_page128);                
            }
            else if(sharserv_info_ipnet != item)
            {
                ee_sharserv_info_deviceinfo_item_clr(item);   
            }
            
            // no reply ...
            
        } break;
        
        case uprot_OPC_PAGE_GET:
        {
            // prepare the reply.
            eOuprot_cmd_PAGE_GET_REPLY_t *reply =  (eOuprot_cmd_PAGE_GET_REPLY_t*)pktout;        
            reply->reply.opc = uprot_OPC_PAGE_GET;
            reply->reply.res = uprot_RES_ERR_UNSUPPORTED;
            reply->reply.protversion = EOUPROT_PROTOCOL_VERSION;
            reply->reply.sizeofextra = sizeof(eOuprot_cmd_PAGE_GET_REPLY_t);
            reply->pagesize = cmd->pagesize;
            reply->page[0] = 0xff; // so that we invalidate it.
            *sizeout = sizeof(eOuprot_cmd_PAGE_GET_REPLY_t);
            ret = 1;
            
            if(sharserv_info_ipnet != item)
            {
                const void *page = NULL;
                ee_sharserv_info_deviceinfo_item_get(item, &page);  
                if(NULL != page)
                {
                    reply->reply.res = uprot_RES_OK;
                    memcpy(reply->page, page, cmd->pagesize); 
                }                
            }
            
        } break;

        case uprot_OPC_PAGE_SET:
        {
            if(sharserv_info_ipnet != item)
            {
                ee_sharserv_info_deviceinfo_item_set(item, cmd->page);
            }
            // no reply ...
            
        } break;
        
        default:
        {
            // no reply ...
        } break;
        
    }
    
    
    return ret;          
}

static uint8_t s_uprot_proc_JUMP2ADDRESS(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{    
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }   

    eOuprot_cmd_JUMP2ADDRESS_t  * cmd = (eOuprot_cmd_JUMP2ADDRESS_t*) pktin;

    // can we jump to address? if so, i request it and i restart.
    
    if(ee_res_OK == ee_sharserv_sys_canjump(EENV_ROMSTART+cmd->address))
    {
        ee_sharserv_ipc_gotoproc_clr();
        ee_sharserv_ipc_jump2addr_set(EENV_ROMSTART+cmd->address);
        ee_sharserv_sys_restart();
    }
    
    return 0;     
}


static uint8_t s_uprot_proc_LEGACY_MAC_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{   
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    eOuprot_cmd_LEGACY_MAC_SET_t * cmd = (eOuprot_cmd_LEGACY_MAC_SET_t*) pktin;
    
    eEipnetwork_t ipnetwork;
    const eEipnetwork_t *ipnetworkstrg;
    uint64_t mac = 0;

    uint64_t tmp = 0;
    tmp = cmd->mac48[0];        mac |= (tmp<<40);
    tmp = cmd->mac48[1];        mac |= (tmp<<32);
    tmp = cmd->mac48[2];        mac |= (tmp<<24);
    tmp = cmd->mac48[3];        mac |= (tmp<<16);
    tmp = cmd->mac48[4];        mac |= (tmp<<8);
    tmp = cmd->mac48[5];        mac |= (tmp);

    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
    memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
    ipnetwork.macaddress = mac;
    ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork);
    
    // updater_core_trace("CORE", "mac is h = %llx, l = %llx", ipnetwork.macaddress >> 32, ipnetwork.macaddress & 0xffffffff);    
     
    return 0;         
}


static uint8_t s_uprot_proc_LEGACY_IP_MASK_SET(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    if(eobool_false == eouprot_can_process_opcode(s_running_process, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    eOuprot_cmd_LEGACY_IP_MASK_SET_t * cmd = (eOuprot_cmd_LEGACY_IP_MASK_SET_t*) pktin;
    
    eEipnetwork_t ipnetwork;
    const eEipnetwork_t *ipnetworkstrg;

    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
    memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
    ipnetwork.ipnetmask = EECOMMON_ipaddr_from(cmd->mask[0], cmd->mask[1], cmd->mask[2], cmd->mask[3]);
    ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork); 
     
    return 0;         
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


