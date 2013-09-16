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

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"

#include "EOaction_hid.h"

#include "osal.h"
#include "eEsharedServices.h" 


//#include "appl-core.h"



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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsdiscoverytransceiver_cfg_t eom_emsdiscoverytransceiver_DefaultCfg = 
{
    EO_INIT(.hostipv4addr)              EO_COMMON_IPV4ADDR_LOCALHOST, //EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)              3333,
    EO_INIT(.txpktcapacity)             64,
    EO_INIT(.discoveryprotocol)         eodiscovery_protocol_ethloader_reduced
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport);


static void s_callback_shutdown2updater(void *p);



//static void s_app_core_init(void);

static uint8_t s_app_core_manage_cmd(uint8_t *pktin, uint8_t *pktout, uint16_t pktoutcapacity, uint16_t *sizeout);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSdiscoverytransceiver";

 
static EOMtheEMSdiscoverytransceiver s_emsdiscoverytransceiver_singleton = 
{
    EO_INIT(.replypkt)                  NULL,
    EO_INIT(.transmit)                  eobool_false,
    EO_INIT(.shutdowntmr)               NULL,
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
    
    if((EO_COMMON_IPV4ADDR_LOCALHOST != p->cfg.hostipv4addr) && (p->cfg.hostipv4addr != remaddr))
    {   // p->cfg.hostipv4addr is not EO_COMMON_IPV4ADDR_LOCALHOST, then we can parse packets coming only from the host 
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
    EOaction action;

    uint8_t *txdata;
    uint16_t txsize = 0;
    eo_packet_Payload_Get(s_emsdiscoverytransceiver_singleton.replypkt, &txdata, &txsize);  

    
    if(1)
    {   // it can be a eth-loader protocol
       
        uint8_t r = s_app_core_manage_cmd(data, txdata, s_emsdiscoverytransceiver_singleton.cfg.txpktcapacity, &txsize);
        
        switch(r)
        {
            case 2:
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
                eo_action_SetCallback(&action, s_callback_shutdown2updater, NULL, NULL);
                eo_timer_Start(s_emsdiscoverytransceiver_singleton.shutdowntmr, eok_abstimeNOW, 50*eok_reltime1ms, eo_tmrmode_ONESHOT, &action);                             
            
            } break;
            
            case 1:
            {
                transmit = eobool_true;               
                
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
            if(EO_COMMON_IPV4ADDR_LOCALHOST == address)
            {
                address = remaddr;
            }
            
           
            // there is a msg to send back
            eo_packet_Size_Set(s_emsdiscoverytransceiver_singleton.replypkt, txsize); 
            eo_packet_Addressing_Set(s_emsdiscoverytransceiver_singleton.replypkt, 
                                     address, s_emsdiscoverytransceiver_singleton.cfg.hostipv4port);   
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

typedef enum {
    CMD_SCAN    =0xFF,          // supported
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

// static void s_app_core_init(void)
// {
//     // acemor: removed
// //    hal_gpio_init(hal_gpio_portE, hal_gpio_pin13, hal_gpio_dirOUT, hal_gpio_speed_low); 
// }


// acemor: 0 error or nothing to do. 1 transmit back. 2 tx back and start countdown
// acemor: bytes: CMD_SCAN 14, 
static uint8_t s_app_core_manage_cmd(uint8_t *pktin, uint8_t *pktout, uint16_t pktoutcapacity, uint16_t *sizeout)
{
    uint8_t opcode = pktin[0]; // use 0 .... 1 only for debug
//#warning --> to debug CHANGE IN 1 but then put 0 back !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    *sizeout=0;
    
    uint8_t retval = 0;

    switch(opcode) // opcode
    {
        case CMD_SCAN:
        {
            
            if(pktoutcapacity < 14)
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

                char *data = (char*)pktout;
                uint16_t size = 2;

                for (uint8_t i=0; i<num_procs; ++i)
                {
                    ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);

                    size+=sprintf(data+size,"*** e-proc #%d %s %s %s ***\r\n", i, defproc==i?"(def2run)":"", startup==i?"(startup)":"", running==i?"(RUNNING)":"" ) ;

                    size+=sprintf(data+size, "name\t%s\r\n", s_modinfo->info.name);
                    size+=sprintf(data+size, "version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
                        s_modinfo->info.entity.version.major, 
                        s_modinfo->info.entity.version.minor,
                        s_modinfo->info.entity.builddate.day,
                        s_modinfo->info.entity.builddate.month,
                        s_modinfo->info.entity.builddate.year,
                        s_modinfo->info.entity.builddate.hour,
                        s_modinfo->info.entity.builddate.min
                    );
                    size+=sprintf(data+size, "rom.addr\t0x%0.8X\r\n", s_modinfo->info.rom.addr);
                    size+=sprintf(data+size, "rom.size\t0x%0.8X\r\n", s_modinfo->info.rom.size);
                    size+=sprintf(data+size, "ram.addr\t0x%0.8X\r\n", s_modinfo->info.ram.addr);
                    size+=sprintf(data+size, "ram.size\t0x%0.8X\r\n", s_modinfo->info.ram.size);

                    size+=sprintf(data+size, "stg.type\t%s\r\n", (ee_strg_none == s_modinfo->info.storage.type) ? ("none") 
                                                                : ((ee_strg_eflash==s_modinfo->info.storage.type) ? ("flash") : ("eeprom")));
                    size+=sprintf(data+size, "stg.addr\t0x%0.8X\r\n", s_modinfo->info.storage.addr);
                    size+=sprintf(data+size, "stg.size\t0x%0.8X\r\n", s_modinfo->info.storage.size);
                    size+=sprintf(data+size, "com.msk\t0x%0.8X\r\n\r\n", s_modinfo->info.communication);
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




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





