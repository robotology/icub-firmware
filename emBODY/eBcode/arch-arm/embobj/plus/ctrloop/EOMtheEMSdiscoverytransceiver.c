/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"

#include "EoBoards.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOaction.h"

#include "osal.h"
#include "eEsharedServices.h" 

#include "eEmemorymap.h" 


#include "EoUpdaterProtocol.h"

#include "EOVtheCallbackManager.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSdiscoverytransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSdiscoverytransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#undef KEEP_OLD_SCAN

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsdiscoverytransceiver_cfg_t eom_emsdiscoverytransceiver_DefaultCfg = 
{
    EO_INIT(.hostipv4addr)              EO_COMMON_IPV4ADDR_LOCALHOST, //EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)              3333,
    EO_INIT(.txpktcapacity)             64,
    EO_INIT(.discoveryprotocol)         eodiscovery_protocol_ethloader_reduced,
    EO_INIT(.dbgshutdowntime)           0
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport);

static void s_callback_shutdown2updater(void *p);

#if defined(KEEP_OLD_SCAN)
static uint8_t s_app_core_manage_cmd(uint8_t *pktin, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
#endif

//static uint8_t s_fill_scan2(uint8_t *pktout, uint16_t *sizeout);
//static uint8_t s_process_SCAN2(uint8_t *pktin, uint8_t *pktout, uint16_t *sizeout);
//static uint8_t s_process_SCAN_legacy(uint8_t *pktin, uint8_t *pktout, uint16_t *sizeout, uint16_t capacityout);


//static uint8_t s_app_core_manage_cmd2(uint8_t *pktin, uint16_t pktinsize, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);

static uint8_t s_appl_parse(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout, eOipv4port_t *port, eObool_t *restart);


static uint16_t s_add_description(char *data, uint16_t capacity, const char *prefix);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSdiscoverytransceiver";

static eObool_t s_restart = eobool_false;

 
static EOMtheEMSdiscoverytransceiver s_emsdiscoverytransceiver_singleton = 
{
    EO_INIT(.replypkt)                  NULL,
    EO_INIT(.transmit)                  eobool_false,
    EO_INIT(.shutdowntmr)               NULL,
    EO_INIT(.dbgshutdowntmr)            NULL,
    EO_INIT(.cfg)                       {0}
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSdiscoverytransceiver * eom_emsdiscoverytransceiver_Initialise(const eOemsdiscoverytransceiver_cfg_t *cfg)
{
    if(NULL != s_emsdiscoverytransceiver_singleton.replypkt)
    {
        return(&s_emsdiscoverytransceiver_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsdiscoverytransceiver_DefaultCfg;
    }
    
    memcpy(&s_emsdiscoverytransceiver_singleton.cfg, cfg, sizeof(eOemsdiscoverytransceiver_cfg_t));
    
    s_emsdiscoverytransceiver_singleton.replypkt = eo_packet_New(s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity);
    s_emsdiscoverytransceiver_singleton.transmit = eobool_false;
    
    s_emsdiscoverytransceiver_singleton.shutdowntmr = eo_timer_New();
    s_emsdiscoverytransceiver_singleton.dbgshutdowntmr = NULL;
    
    if(0 != s_emsdiscoverytransceiver_singleton.cfg.dbgshutdowntime)
    {   // create and start the debugshutdown timer only if .... its time is non-zero. otherwise doent even create it
        s_emsdiscoverytransceiver_singleton.dbgshutdowntmr = eo_timer_New();
        EOaction_strg astg = {0};
        EOaction *action = (EOaction*)&astg;
        EOVtaskDerived *executingtask = NULL; // if NULL the EOtheTimerManager will directly execute the action. it is ok for small sized callbacks
        //EOVtaskDerived *executingtask = eov_callbackman_GetTask(eov_callbackman_GetHandle());
        eo_action_SetCallback(action, s_callback_shutdown2updater, NULL, executingtask);
        eo_timer_Start(s_emsdiscoverytransceiver_singleton.dbgshutdowntmr, eok_abstimeNOW, s_emsdiscoverytransceiver_singleton.cfg.dbgshutdowntime, eo_tmrmode_ONESHOT, action);                             
    }   

    return(&s_emsdiscoverytransceiver_singleton);
}


extern EOMtheEMSdiscoverytransceiver* eom_emsdiscoverytransceiver_GetHandle(void) 
{
    if(NULL != s_emsdiscoverytransceiver_singleton.replypkt)
    {
        return(&s_emsdiscoverytransceiver_singleton);
    }
    else
    {
        return(NULL);
    }
}




extern eOresult_t eom_emsdiscoverytransceiver_Parse(EOMtheEMSdiscoverytransceiver* p, EOpacket* rxpkt)
{
    eOresult_t res;
    uint8_t *data = NULL;
    uint16_t size = 0;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
        
    eo_packet_Payload_Get(rxpkt, &data, &size);
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);
    
    if((eok_ipv4addr_localhost != p->cfg.hostipv4addr) && (p->cfg.hostipv4addr != remaddr))
    {   // p->cfg.hostipv4addr is not eok_ipv4addr_localhost, then we can parse packets coming only from the host 
        p->transmit = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {   // ok can parse the packet    
        p->transmit = s_parse_and_form(data, size, remaddr, remport);        
        res = eores_OK;        
    }
     
    return(res);
}


extern eOresult_t eom_emsdiscoverytransceiver_GetReply(EOMtheEMSdiscoverytransceiver* p, EOpacket** replypkt)
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == replypkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    *replypkt = p->replypkt;
    
    res = (eobool_true == p->transmit) ? eores_OK : eores_NOK_generic;
    
    p->transmit = eobool_false;
    
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_callback_shutdown2updater(void *p)
{
    osal_system_scheduling_suspend();
    ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
    ee_sharserv_sys_restart();
}

// returns eobool_true if a tx is required
static eObool_t s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport)
{
//    eOresult_t res = eores_NOK_generic;
    eObool_t transmit = eobool_false;
    EOaction_strg astg = {0};
    EOaction *action = (EOaction*)&astg;

    uint8_t *txdata;
    uint16_t txsize = 0;
    eo_packet_Payload_Get(s_emsdiscoverytransceiver_singleton.replypkt, &txdata, &txsize);  

    
    if(1)
    {   // it can be a eth-loader protocol
        
        eOipv4port_t port = s_emsdiscoverytransceiver_singleton.cfg.hostipv4port;
       
        //uint8_t r = s_app_core_manage_cmd(data, txdata, s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity, &txsize);
        
        //uint8_t r = s_app_core_manage_cmd2(data, size, txdata, s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity, &txsize);
        
        eObool_t  restart2updater = eobool_true;
        
        uint8_t r = s_appl_parse(data, size, remaddr, txdata, s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity, &txsize, &port, &restart2updater);
        
        
        
        switch(r)
        {
            case 1:
            {
                if(0 != txsize)
                {
                    transmit = eobool_true;
                }
                else
                {
                    transmit = eobool_false;
                }
                
                // timer di shutdown
                if(eobool_true == restart2updater)
                {
                    EOVtaskDerived *executingtask = NULL; // if NULL the EOtheTimerManager will directly execute the action. it is ok for small sized callbacks
                    //EOVtaskDerived *executingtask = eov_callbackman_GetTask(eov_callbackman_GetHandle());
                    eo_action_SetCallback(action, s_callback_shutdown2updater, NULL, executingtask);
                    eo_timer_Start(s_emsdiscoverytransceiver_singleton.shutdowntmr, eok_abstimeNOW, 50*eok_reltime1ms, eo_tmrmode_ONESHOT, action);  
                }
            
            } break;
            
            
            default:
            case 0:
            {
                transmit = eobool_false;
                
            } break;
                       
        }
        
        if(eobool_true == transmit)
        {       
            eOipv4addr_t address = s_emsdiscoverytransceiver_singleton.cfg.hostipv4addr;
            if(eok_ipv4addr_localhost == address)
            {
                address = remaddr;
            }
            
           
            // there is a msg to send back
            eo_packet_Size_Set(s_emsdiscoverytransceiver_singleton.replypkt, txsize); 
            eo_packet_Addressing_Set(s_emsdiscoverytransceiver_singleton.replypkt, 
                                     address, port);   
        }                
               
    }


    return(transmit);
}

// static eOresult_t s_parse_and_form_old(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport)
// {
//     eOresult_t res = eores_NOK_generic;
//     EOaction action;

//     uint8_t *txdata;
//     uint16_t txsize = 0;
//     eo_packet_Payload_Get(s_emsdiscoverytransceiver_singleton.txpkt, &txdata, &txsize);  

//     
//     uint8_t r = s_app_core_manage_cmd(data, txdata, s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity, &txsize);
//     
//     if(2 == r)
//     {
//         // timer di shutdown
//         eo_action_SetCallback(&action, s_callback_shutdown2updater, NULL, NULL);
//         eo_timer_Start(s_emsdiscoverytransceiver_singleton.shutdowntmr, eok_abstimeNOW, 50*eok_reltime1ms, eo_tmrmode_ONESHOT, &action);   
//     }
//     
//     if((0 != r) && (3 != r))
//     {
//         eOipv4addr_t address = s_emsdiscoverytransceiver_singleton.cfg.hostipv4addr;
//         if(EO_COMMON_IPV4ADDR_LOCALHOST == address)
//         {
//             address = remaddr;
//         }
//         
//         // there is a msg to send back
//         //eo_packet_Payload_Set(s_emsdiscoverytransceiver_singleton.txpkt, rxdata, rxsize); 
//         eo_packet_Size_Set(s_emsdiscoverytransceiver_singleton.txpkt, txsize); 
//         eo_packet_Addressing_Set(s_emsdiscoverytransceiver_singleton.txpkt, 
//                                  address, s_emsdiscoverytransceiver_singleton.cfg.hostipv4port);
//         res = eores_OK;
//     }
//     else if(3 == r)
//     {
//         // call the user-def parser
//         uint16_t opctxsize = 0;
//         opcprotman_res_t rr = opcprotman_Parse(s_emsdiscoverytransceiver_singleton.opcprotman, (opcprotman_message_t*)&data[0], (opcprotman_message_t*)&txdata[0], &opctxsize);    

//         if(opcprotman_OK_withreply == rr)
//         {
//             // ok ... a msg to be sent back.
//             // set the size
//             eo_packet_Size_Set(s_emsdiscoverytransceiver_singleton.txpkt, opctxsize); 
//             
//             eOipv4addr_t address = s_emsdiscoverytransceiver_singleton.cfg.hostipv4addr;
//             if(EO_COMMON_IPV4ADDR_LOCALHOST == address)
//             {
//                 address = remaddr;
//             }
//             eo_packet_Addressing_Set(s_emsdiscoverytransceiver_singleton.txpkt, 
//                                  address, s_emsdiscoverytransceiver_singleton.cfg.hostipv4port);
//             
//         } 
//         
//         res = eores_OK;           
//     }


//     return(res);
// }



// content of appl-core.c





#if defined(KEEP_OLD_SCAN)

typedef enum {
    CMD_SCAN    = 0xFF,          // supported
    CMD_SCAN2   = 0x7F,         //  supported
    CMD_START   =0x01,
    CMD_DATA    =0x02,
    CMD_JUMP    =0x03,
    CMD_END     =0x04,
    CMD_BOOT    =0x05,
    CMD_RESET   =0x06,          // supported
    CMD_IPSET   =0x07,
    CMD_MASKSET =0x08,
    CMD_PROCS   =0x09,
    CMD_SHALS   =0x0A,
    CMD_BLINK   =0x0B,
    CMD_UPD_ONCE=0x0C           // supported
} ethloader_opcode_t;

enum {
    UPD_OK        = 0,
    UPD_ERR_PROT  = 1,
    UPD_ERR_FLASH = 2,
    UPD_ERR_LOST  = 3,
    UPD_ERR_UNK   = 4,
    UPD_ERR_OOM   = 5
};

#define BOARD_TYPE_EMS 0x0A

#define MAX0(a) ( ((a)>0) ? (a) : (0) )

#define PROTOCOL3333_VERSION 1


// acemor: 0 error or nothing to do. 1 transmit back. 2 tx back and start countdown
// acemor: bytes: CMD_SCAN 14, 
static uint8_t s_app_core_manage_cmd(uint8_t *pktin, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    uint8_t opcode = pktin[0]; // use 0 .... 1 only for debug
//#warning --> to debug CHANGE IN 1 but then put 0 back !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    *sizeout=0;
    
    uint8_t retval = 0;

    switch(opcode) // opcode
    {
        case CMD_SCAN:
        {
            
            if(capacityout < 14)
            {
                *sizeout = 0;
                retval = 2; 
            }
            else
            {
                *sizeout = 14;
                
                eEmoduleInfo_t* module=(eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);

                pktout[0] = CMD_SCAN;
                pktout[1] = module->info.entity.version.major;
                pktout[2] = module->info.entity.version.minor;
                pktout[3] = BOARD_TYPE_EMS;
                
                const eEipnetwork_t *ipnetworkstrg;
                ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);

                pktout[4] = (ipnetworkstrg->ipnetmask>>24) & 0xFF;
                pktout[5] = (ipnetworkstrg->ipnetmask>>16) & 0xFF;
                pktout[6] = (ipnetworkstrg->ipnetmask>>8)  & 0xFF;
                pktout[7] =  ipnetworkstrg->ipnetmask      & 0xFF;

                pktout[ 8] = (ipnetworkstrg->macaddress>>40) & 0xFF;
                pktout[ 9] = (ipnetworkstrg->macaddress>>32) & 0xFF;
                pktout[10] = (ipnetworkstrg->macaddress>>24) & 0xFF;
                pktout[11] = (ipnetworkstrg->macaddress>>16) & 0xFF;
                pktout[12] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
                pktout[13] = (ipnetworkstrg->macaddress)     & 0xFF;

                retval = 2;
            }
        } break;


        case CMD_RESET:
        {
            *sizeout = 0;

            ee_sharserv_sys_restart();
            
            retval = 0;
   
        } break;

        case CMD_PROCS:
        {
            uint8_t num_procs = 0;
            const eEprocess_t *s_proctable = NULL;
            const eEmoduleInfo_t *s_modinfo = NULL;
            eEprocess_t defproc;
			eEprocess_t startup;
			eEprocess_t running = ee_procApplication;


            ee_sharserv_part_proc_def2run_get(&defproc);
			ee_sharserv_part_proc_startup_get(&startup);

            if (ee_res_OK == ee_sharserv_part_proc_allavailable_get(&s_proctable, &num_procs))
            {
                pktout[0] = CMD_PROCS;
                pktout[1] = num_procs; 
                volatile eEmoduleExtendedInfo_t* extinfo = NULL;

                char *data = (char*)pktout;
                uint16_t size = 2;

                for (uint8_t i=0; i<num_procs; ++i)
                {
                    ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);

                    size+=snprintf(data+size, MAX0(capacityout-size), "*** e-process #%d \r\n", i);
                    size+=snprintf(data+size, MAX0(capacityout-size), "props %s%s%s \r\n", defproc==i?"DEF ":"", startup==i?"START ":"", running==i?"RUNNING ":"" ) ;

                    size+=snprintf(data+size, MAX0(capacityout-size), "name  %s\r\n", s_modinfo->info.name);
                    size+=snprintf(data+size, MAX0(capacityout-size), "vers  %d.%d\r\n", 
                        s_modinfo->info.entity.version.major, 
                        s_modinfo->info.entity.version.minor
                    );
                    size+=snprintf(data+size, MAX0(capacityout-size), "date  %s %.2d %d %d:%.2d\r\n", 
                        ee_common_get_month_string(s_modinfo->info.entity.builddate),
                        s_modinfo->info.entity.builddate.day,                    
                        s_modinfo->info.entity.builddate.year,
                        s_modinfo->info.entity.builddate.hour,
                        s_modinfo->info.entity.builddate.min
                    );
                    
                    extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);
                    
                    //if(0 == strcmp((const char*)extinfo->moduleinfo.extra, extendstr))
                    if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
                    {
                        size+=snprintf(data+size, MAX0(capacityout-size), "built %s\r\n", 
                            extinfo->compilationdatetime
                        );                        
                    }
                    else
                    {
                        size+=snprintf(data+size, MAX0(capacityout-size), "built unknown date\r\n"
                        );    
                    }

                    size+=snprintf(data+size, MAX0(capacityout-size), "rom   @+%dKB, s=%dKB\r\n", (s_modinfo->info.rom.addr-EENV_ROMSTART+1023)/1024, (s_modinfo->info.rom.size+1023)/1024);
                    size+=snprintf(data+size, MAX0(capacityout-size), "\n");
            
                }

                *sizeout = size + 1;
            }

            retval = 2;
        }// break

        case CMD_UPD_ONCE:
        {
            *sizeout = 0;
     
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
            ee_sharserv_sys_restart();
            
            retval = 0;
            
        } break;
        

        default:
        { 
            *sizeout = 0;
            retval = 0; 
            
        } break;
    }

    return retval;
}

#endif

//// acemor: 0 error or nothing to do. 1 transmit back. 2 tx back and start countdown
//// acemor: bytes: CMD_SCAN 14, 
//static uint8_t s_app_core_manage_cmd2(uint8_t *pktin, uint16_t pktinsize, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
//{
//    uint8_t opcode = pktin[0]; // use 0 .... 1 only for debug
////#warning --> to debug CHANGE IN 1 but then put 0 back !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    *sizeout=0;
//    
//    uint8_t retval = 0;

//    switch(opcode) // opcode
//    {
//        case CMD_SCAN2:
//        {
//            retval = s_process_SCAN2(pktin, pktout, sizeout);            
//        } break;
//        
//        case CMD_SCAN:
//        {
//            if(1 == pktinsize)
//            {
//                retval = s_process_SCAN_legacy(pktin, pktout, sizeout, capacityout);                
//            }
//            else
//            {   // because packet of size 2 with: {CMD_SCAN, CMD_SCAN2} is equivalent to {CMD_SCAN2} of size 1 
//                retval = s_process_SCAN2(pktin, pktout, sizeout);
//            }
//            
//        } break;


//        case CMD_RESET:
//        {
//            *sizeout = 0;

//            ee_sharserv_sys_restart();
//            
//            retval = 0;
//   
//        } break;


//        case CMD_UPD_ONCE:
//        {
//            *sizeout = 0;
//     
//            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
//            ee_sharserv_sys_restart();
//            
//            retval = 0;
//            
//        } break;
//        

//        default:
//        { 
//            *sizeout = 0;
//            retval = 0; 
//            
//        } break;
//    }

//    return retval;
//}


//static uint8_t s_process_SCAN_legacy(uint8_t *pktin, uint8_t *pktout, uint16_t *sizeout, uint16_t capacityout)
//{
//    uint8_t retval = 0;
//    
//    if(capacityout < 14)
//    {
//        *sizeout = 0;
//        retval = 2; 
//    }
//    else
//    {
//        *sizeout = 14;
//        
//        eEmoduleInfo_t* module=(eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);

//        pktout[0] = CMD_SCAN;
//        pktout[1] = module->info.entity.version.major;
//        pktout[2] = module->info.entity.version.minor;
//        pktout[3] = BOARD_TYPE_EMS;
//        
//        const eEipnetwork_t *ipnetworkstrg;
//        ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);

//        pktout[4] = (ipnetworkstrg->ipnetmask>>24) & 0xFF;
//        pktout[5] = (ipnetworkstrg->ipnetmask>>16) & 0xFF;
//        pktout[6] = (ipnetworkstrg->ipnetmask>>8)  & 0xFF;
//        pktout[7] =  ipnetworkstrg->ipnetmask      & 0xFF;

//        pktout[ 8] = (ipnetworkstrg->macaddress>>40) & 0xFF;
//        pktout[ 9] = (ipnetworkstrg->macaddress>>32) & 0xFF;
//        pktout[10] = (ipnetworkstrg->macaddress>>24) & 0xFF;
//        pktout[11] = (ipnetworkstrg->macaddress>>16) & 0xFF;
//        pktout[12] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
//        pktout[13] = (ipnetworkstrg->macaddress)     & 0xFF;

//        retval = 2;
//    } 

//    return retval;
//}


//static uint8_t s_process_SCAN2(uint8_t *pktin, uint8_t *pktout, uint16_t *sizeout)
//{
//    // eupdater_info_trace("CORE", "CMD_SCAN2");
//    
//    pktout[ 0] = CMD_SCAN2;
//    
//    return s_fill_scan2(pktout, sizeout);        
//}

//static uint8_t s_fill_scan2(uint8_t *pktout, uint16_t *sizeout)
//{
//    pktout[ 1] = PROTOCOL3333_VERSION;
//    
//    *sizeout = 40+32;

//    const eEipnetwork_t *ipnetworkstrg = NULL;
//    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
//    
//    const eEboardInfo_t* boardinfo = NULL;
//    ee_sharserv_info_boardinfo_get(&boardinfo);
//    // now get the board type.
//    uint8_t boardtype = 255; // use eobrd_ethtype_unknown later on
//    if(0x11 == boardinfo->info.entity.signature)
//    {   // old eLoader upto version 2.11 of build date 2015 May 26 11:11. it have info about board type in a string
//        
//        if(0 == strcmp((const char*)boardinfo->info.name, "ems4rd"))
//        {
//            boardtype = 32; // use eobrd_ethtype_ems4 later on        
//        }
//        else if(0 == strcmp((const char*)boardinfo->info.name, "mc4plus"))
//        {
//             boardtype = 33; // use eobrd_ethtype_mc4plus later on 
//        }        
//    }
//    else
//    {   // since eLoader version 2.12 the fiels boardinfo->info.entity.signature contains the board type. 
//        boardtype = boardinfo->info.entity.signature;
//    }
//    
//    eEprocess_t startup = ee_procNone;
//    eEprocess_t def2run = ee_procNone;
//    ee_sharserv_part_proc_startup_get(&startup);
//    ee_sharserv_part_proc_def2run_get(&def2run);
//    
//    pktout[ 2] = (ipnetworkstrg->macaddress>>40) & 0xFF;
//    pktout[ 3] = (ipnetworkstrg->macaddress>>32) & 0xFF;
//    pktout[ 4] = (ipnetworkstrg->macaddress>>24) & 0xFF;
//    pktout[ 5] = (ipnetworkstrg->macaddress>>16) & 0xFF;
//    pktout[ 6] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
//    pktout[ 7] = (ipnetworkstrg->macaddress)     & 0xFF;
//    
//    pktout[ 8] = boardtype;
//    pktout[ 9] = startup;
//    pktout[10] = def2run;
//    
//    uint8_t nprocs = 0;
//    const eEprocess_t *s_proctable = NULL;
//    ee_sharserv_part_proc_allavailable_get(&s_proctable, &nprocs);
//    
//    pktout[11] = nprocs;
//    
//    for(uint8_t i=0; i<nprocs; i++)
//    {
//        const eEmoduleInfo_t *s_modinfo = NULL;
//        ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);
//        pktout[12+8*i] = s_proctable[i];
//        pktout[13+8*i] = s_modinfo->info.entity.version.major;
//        pktout[14+8*i] = s_modinfo->info.entity.version.minor;
//        uint8_t* builddate = (uint8_t*) &s_modinfo->info.entity.builddate;
//        pktout[15+8*i] = builddate[0];
//        pktout[16+8*i] = builddate[1];
//        pktout[17+8*i] = builddate[2];
//        pktout[18+8*i] = builddate[3];      
//    }
//    
//    pktout[36] = ee_procApplication;
//    pktout[37] = 0;
//    pktout[38] = 0;
//    pktout[39] = 0;
//    const void *page = NULL;
//    ee_sharserv_info_deviceinfo_item_get(sharserv_info_page32, &page);
//    memcpy(&pktout[40], page, 32);
//    

//    return 2; // tx back and start countdown
//}

typedef uint8_t (*uprot_fp_process_t) (eOuprot_opcodes_t, uint8_t *, uint16_t, eOipv4addr_t, uint8_t *, uint16_t, uint16_t *);


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
    reply->capabilities = eouprot_get_capabilities(eApplication, EOUPROT_PROTOCOL_VERSION);;
    reply->mac48[0] = (ipnetworkstrg->macaddress>>40) & 0xFF;
    reply->mac48[1] = (ipnetworkstrg->macaddress>>32) & 0xFF;
    reply->mac48[2] = (ipnetworkstrg->macaddress>>24) & 0xFF;
    reply->mac48[3] = (ipnetworkstrg->macaddress>>16) & 0xFF;
    reply->mac48[4] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
    reply->mac48[5] = (ipnetworkstrg->macaddress)     & 0xFF;
    reply->processes.numberofthem = nprocs;
    reply->processes.startup = startup;
    reply->processes.def2run = def2run;
    reply->processes.runningnow = eApplication;
    
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
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
        
    const eEmoduleInfo_t* module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);

    if(opc == uprot_OPC_DISCOVER)
    {
        eOuprot_cmd_DISCOVER_t * cmd = (eOuprot_cmd_DISCOVER_t*)pktin;
        // shall i restart? let's read it
        s_restart = (1 == cmd->jump2updater) ? (eobool_true) : (eobool_false);
        //s_restart = eobool_true;
        
        // prepare the reply.
        eOuprot_cmd_DISCOVER_REPLY_t *reply = (eOuprot_cmd_DISCOVER_REPLY_t*) pktout;    
        if(capacityout > sizeof(eOuprot_cmd_DISCOVER_REPLY_t))
        {   // yes, we have enough capacity to reply           
            *sizeout = s_discover_fill(reply, uprot_OPC_DISCOVER, sizeof(eOuprot_cmd_DISCOVER_REPLY_t));
            return 1; 
        }
        else
        {   // i must reply with old protocol because i dont have enough capacity
            opc = uprot_OPC_LEGACY_SCAN;
        }
    }
    else
    {
        s_restart = eobool_true;
    }
    
                 
    if(uprot_OPC_LEGACY_SCAN == opc)
    {    
        const uint8_t BOARD_TYPE_EMS = 0x0A;
        // eupdater_info_trace("CORE", "CMD_SCAN");
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
    
    return 1;    
}



static uint8_t s_uprot_proc_MOREINFO(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{       
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    eOuprot_cmd_MOREINFO_t * cmd = (eOuprot_cmd_MOREINFO_t*)pktin;
    // shall i restart? let's read it asap
    s_restart = (1 == cmd->jump2updater) ? (eobool_true) : (eobool_false);
    s_restart = eobool_false;
    
    // prepare the reply.
    eOuprot_cmd_MOREINFO_REPLY_t *reply = (eOuprot_cmd_MOREINFO_REPLY_t*) pktout;
    
    // surely there is the discover part.
    if(capacityout > sizeof(eOuprot_cmd_MOREINFO_REPLY_t))
    {   // yes, we have enough capacity to reply    
        *sizeout = s_discover_fill(&reply->discover, uprot_OPC_MOREINFO, sizeof(eOuprot_cmd_MOREINFO_REPLY_t));
    }
    else
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }    
    

    // but there may be also the description
    reply->hasdescription = cmd->plusdescription;
       
    if(1 == reply->hasdescription)
    {
        // 1. add the description in reply->description
        //const char *prefix = (eobool_false == s_restart) ? "MOREINFO-APP\r\n" : "MOREINFO-UPD\r\n";
        const char *prefix = "MOREINFO-APP\r\n";

        uint16_t sizeofdescription = s_add_description((char*)reply->description, capacityout - sizeof(eOuprot_cmd_MOREINFO_REPLY_t), prefix);
        
        // 2. increase some sizes  
        reply->discover.reply.sizeofextra += sizeofdescription;
        *sizeout += sizeofdescription;                
    }   

    return 1;     
    
//    // surely there is the discover part.
//    const eEmoduleInfo_t* module = NULL;
//    
//    // shall i restart? let's read it
//    s_restart = (1 == cmd->jump2updater) ? (eobool_true) : (eobool_false);
//    
//    if(eobool_true == s_restart)
//    {
//        module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
//    }
//    else
//    {
//        module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
//    }
//    

//    
//   

//    // now i prepare the reply. i must see if capacity is big enough to fill .... 
//    // 1. reply->discover (of type eOuprot_cmd_DISCOVER_REPLY_t) 
//    // 2. reply->description[] if cmd->plusdescription is 1.  
//    
//    if(capacityout > sizeof(eOuprot_cmd_MOREINFO_REPLY_t))
//    {   // yes, we have enough capacity to reply    
//        *sizeout = s_discover_fill(&reply->discover, uprot_OPC_MOREINFO, sizeof(eOuprot_cmd_MOREINFO_REPLY_t));
//    }
//    else
//    {
//        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
//    }
        
    
//    return 1;    
}


static uint8_t s_uprot_proc_RESTART(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    ee_sharserv_sys_restart();

    // no reply
    *sizeout = 0;   
    return(0);      
}


static uint8_t s_uprot_proc_CANGATEWAY(eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }

    eOuprot_cmdREPLY_t * reply = (eOuprot_cmdREPLY_t*) pktout;
    
    reply->opc = uprot_OPC_CANGATEWAY;
    reply->protversion = EOUPROT_PROTOCOL_VERSION;
    reply->res = uprot_RES_ERR_TRYAGAIN;
    reply->sizeofextra = 0;

    // we send a ko message.
    *sizeout = sizeof(eOuprot_cmdREPLY_t);   
    
    // we force a restart to eUpdater
    return(1);      
}


static uint8_t s_appl_parse(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout, eOipv4port_t *port, eObool_t *restart)
{  
    eOuprot_opcodes_t opc = uprot_OPC_NONE;    
    uprot_fp_process_t process = s_uprot_proc_NONE;
    
    
    *restart = s_restart = eobool_false;
    
    switch(pktin[0])
    {        
        case uprot_OPC_LEGACY_SCAN:
        case uprot_OPC_DISCOVER:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_SCAN_t) == pktinsize) ? uprot_OPC_LEGACY_SCAN : uprot_OPC_DISCOVER;
            process = s_uprot_proc_DISCOVER;    
            *port = s_emsdiscoverytransceiver_singleton.cfg.hostipv4port;            
        } break;

        case uprot_OPC_LEGACY_PROCS:
        case uprot_OPC_MOREINFO:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_PROCS_t) == pktinsize) ? uprot_OPC_LEGACY_PROCS : uprot_OPC_MOREINFO;
            process = s_uprot_proc_MOREINFO;    
            *port = s_emsdiscoverytransceiver_singleton.cfg.hostipv4port;            
        } break;

        
        case uprot_OPC_LEGACY_CANGATEWAY:
        case uprot_OPC_CANGATEWAY:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_CANGATEWAY_t) == pktinsize) ? uprot_OPC_LEGACY_CANGATEWAY : uprot_OPC_CANGATEWAY;
            process = s_uprot_proc_CANGATEWAY; 
            *port = 3334;            
        } break;
        
        case uprot_OPC_RESTART:
        {            
            opc = uprot_OPC_RESTART; 
            process = s_uprot_proc_RESTART;
            *port = s_emsdiscoverytransceiver_singleton.cfg.hostipv4port;
        } break;
 
        default:
        {
            opc = uprot_OPC_NONE;
            process = s_uprot_proc_NONE;
            *port = s_emsdiscoverytransceiver_singleton.cfg.hostipv4port;
        } break;
        
    }
    
    
    uint8_t ret = process(opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout);
    
    // s_restart is changed inside process()
    *restart = s_restart;
    
    return ret;
}

#define MYABS(x) ( ((x)>0) ? (x) : (0) )
static uint16_t s_add_description(char *data, uint16_t capacity, const char *prefix)
{
    uint16_t size = 0;
        
    size+=snprintf(data+size, MYABS(capacity-size), "%s", prefix);
    
    return size;

//    uint8_t num_procs = 0;
//    const eEprocess_t *s_proctable = NULL;
//    const eEmoduleInfo_t *s_modinfo = NULL;
//    
//    const eEmoduleInfo_t *sharserv = ee_sharserv_moduleinfo_get();
//    const eEmoduleInfo_t *strsharserv = ee_sharserv_storage_moduleinfo_get();
//    
//    if((strsharserv->info.entity.version.major != sharserv->info.entity.version.major) || (strsharserv->info.entity.version.minor != sharserv->info.entity.version.minor))
//    {
//        size+=snprintf(data+size, MYABS(capacity-size), "EEPROM contains sharserv version different from that of this process. it has:\r\n");
//        size+=snprintf(data+size, MYABS(capacity-size), "eeprom version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
//                strsharserv->info.entity.version.major, 
//                strsharserv->info.entity.version.minor,
//                strsharserv->info.entity.builddate.month,
//                strsharserv->info.entity.builddate.day,
//                strsharserv->info.entity.builddate.year,
//                strsharserv->info.entity.builddate.hour,
//                strsharserv->info.entity.builddate.min
//            );  
//        size+=snprintf(data+size, MYABS(capacity-size), "process version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
//                sharserv->info.entity.version.major, 
//                sharserv->info.entity.version.minor,
//                sharserv->info.entity.builddate.month,
//                sharserv->info.entity.builddate.day,
//                sharserv->info.entity.builddate.year,
//                sharserv->info.entity.builddate.hour,
//                sharserv->info.entity.builddate.min
//            );                       
//    }
//    
//    if(ee_res_NOK_generic == ee_sharserv_storage_isvalid())
//    {
//        size+=snprintf(data+size, MYABS(capacity-size), "*** EEPROM FORMAT IS NOT VALID\r\n");
//        size+=snprintf(data+size, MYABS(capacity-size), "*** CANNOT GIVE PARTITION INFO\r\n");              
//        size+=snprintf(data+size, MYABS(capacity-size), "*** BYE BYE\r\n"); 
//        size+=snprintf(data+size, MYABS(capacity-size), "\n");                    
//    }                        
//    else if (ee_res_OK == ee_sharserv_part_proc_allavailable_get(&s_proctable, &num_procs))
//    {        
//        volatile eEmoduleExtendedInfo_t* extinfo = NULL;    
//        eEprocess_t defproc;
//        eEprocess_t startup;
//        eEprocess_t indexofrunning = ee_procApplication; //(eobool_false == s_restart) ? ee_procApplication : ee_procUpdater;
//            
//        ee_sharserv_part_proc_def2run_get(&defproc);
//        ee_sharserv_part_proc_startup_get(&startup);
//                    
//        // processes
//        for (uint8_t i=0; i<num_procs; ++i) 
//        {
//            ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);
// 
//            size+=snprintf(data+size, MYABS(capacity-size), "*** e-process #%d \r\n", i);
//            size+=snprintf(data+size, MYABS(capacity-size), "props %s%s%s \r\n", defproc==i?"DEF ":"", startup==i?"START ":"", indexofrunning==i?"RUNNING ":"" ) ;
// 
//            size+=snprintf(data+size, MYABS(capacity-size), "name  %s\r\n", s_modinfo->info.name);
//            size+=snprintf(data+size, MYABS(capacity-size), "vers  %d.%d\r\n", 
//                s_modinfo->info.entity.version.major, 
//                s_modinfo->info.entity.version.minor
//            );
//            size+=snprintf(data+size, MYABS(capacity-size), "date  %s %.2d %d %d:%.2d\r\n", 
//                ee_common_get_month_string(s_modinfo->info.entity.builddate),
//                s_modinfo->info.entity.builddate.day,                    
//                s_modinfo->info.entity.builddate.year,
//                s_modinfo->info.entity.builddate.hour,
//                s_modinfo->info.entity.builddate.min
//            );
//            
//            extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);
//            
//            if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
//            {
//                size+=snprintf(data+size, MYABS(capacity-size), "built %s\r\n", 
//                    extinfo->compilationdatetime
//                );                        
//            }
//            else
//            {
//                size+=snprintf(data+size, MYABS(capacity-size), "built unknown date\r\n"
//                );    
//            }
// 
//            size+=snprintf(data+size, MYABS(capacity-size), "rom   @+%dKB, s=%dKB\r\n", (s_modinfo->info.rom.addr-EENV_ROMSTART+1023)/1024, (s_modinfo->info.rom.size+1023)/1024);
// 
//    }
//    
//    return size;
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





