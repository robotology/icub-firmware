/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
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

#include "eupdater_parser.h"


#define USERAM_FOR_LOADER

#include "updater-core.h"
#include "eupdater_cangtw.h"

#include "stdlib.h"
#include "string.h"

#include "hal.h"
#include "osal.h"
#include "ipal.h"

#include "eEsharedServices.h"
#include "emBODYrobot.h"

#include "eEmemorymap.h"

const char extendstr[24] = "extend";

static const eEmoduleInfo_t erasedappl = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = 0, 
                .minor = 0
            },  
            .builddate  = 
            {   // sept 13 1999: the moon was ejected out of earth orbit and into deep space, thus stranding the 311 personnel stationed on moonbase alpha
                .year  = 1999,
                .month = 9,
                .day   = 13,
                .hour  = 9,
                .min   = 9
            }
        },
        .rom        = 
        {   
            .addr   = EENV_MEMMAP_EAPPLICATION_ROMADDR,
            .size   = 0
        },
        .ram        = 
        {   
            .addr   = 0,
            .size   = 0
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_none,  
        .name           = "erasedApp"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 1},
        .can1protversion = { .major = 0, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}           
};
 

static const eEmoduleInfo_t erasedupdt = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procUpdater,
            .version    = 
            { 
                .major = 0, 
                .minor = 0
            },  
            .builddate  = 
            {   // sept 13 1999: the moon was ejected out of earth orbit and into deep space, thus stranding the 311 personnel stationed on moonbase alpha
                .year  = 1999,
                .month = 9,
                .day   = 13,
                .hour  = 9,
                .min   = 9
            }
        },
        .rom        = 
        {   
            .addr   = EENV_MEMMAP_EUPDATER_ROMADDR,
            .size   = 0
        },
        .ram        = 
        {   
            .addr   = 0,
            .size   = 0
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_none,  
        .name           = "erasedUpd"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 1},
        .can1protversion = { .major = 0, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}           
};



#if     defined(USERAM_FOR_LOADER) 
static uint32_t s_ramforloader_maxoffset = 0;
static uint8_t s_ramforloader_data[EENV_MEMMAP_ELOADER_ROMSIZE] = {0};
#endif

typedef enum {
    CMD_SCAN            = 0xFF,
    CMD_START           = 0x01,
    CMD_DATA            = 0x02,
    CMD_JUMP            = 0x03,
    CMD_END             = 0x04,
    CMD_BOOT            = 0x05,
    CMD_RESET           = 0x06,
    CMD_IPSET           = 0x07,
    CMD_MASKSET         = 0x08,
    CMD_PROCS           = 0x09,
    CMD_SHALS           = 0x0A,
    CMD_BLINK           = 0x0B,
    CMD_UPD_ONCE        = 0x0C,
    CMD_MACGET          = 0x10,
    CMD_MACSET          = 0x11,
    CMD_SYSEEPROMERASE  = 0x12,
    CMD_CANGTW_START    = 0x20,
    CMD_CANGTW_STOP     = 0x21,
} canloader_opcode_t;

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


// static functions
#if     !defined(_MAINTAINER_APPL_)
static eEresult_t s_sys_eeprom_erase(void);
#endif

static uint8_t s_overlapping_with_code_space(uint32_t addr, uint32_t size);

void upd_core_init(void)
{
#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)
    const  hal_gpio_t gpio = 
    {
        .port   = hal_gpio_portE,
        .pin    = hal_gpio_pin13   
    };
    
    const hal_gpio_cfg_t gpiocfg =
    {
        .dir = hal_gpio_dirOUT,
        .speed = hal_gpio_speed_low,
        .altcfg = NULL
    };
    hal_gpio_init(gpio, &gpiocfg);
#else
    hal_gpio_init(hal_gpio_portE, hal_gpio_pin13, hal_gpio_dirOUT, hal_gpio_speed_low);
#endif
    
}

#define PROGRAM_LOADER   0x55
#define PROGRAM_UPDATER  0xAA
#define PROGRAM_APP      0x5A
#define PROGRAM_SHARSERV 0x5B

uint8_t upd_core_manage_cmd(uint8_t *pktin, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    static uint32_t s_prog_mem_start = 0xFFFFFFFF;
    static uint32_t s_prog_mem_size  = 0;
    static uint16_t s_download_packets = 0;
    static uint8_t  s_download_state = 0;
    static uint8_t  s_erased_eeprom = 0;
    
    hal_result_t halres = hal_res_NOK_generic;
    

    *sizeout=0;

        
#if defined(_DEBUG_MODE_FULL_)
    #warning --> we are in _DEBUG_MODE_FULL_ and use: POS_OPC = 1
    #define POS_OPC 1
#else
    #define POS_OPC 0
#endif
    
    switch(pktin[POS_OPC]) // opcode
    {
        case CMD_CANGTW_START:
        {
#if !defined(_MAINTAINER_APPL_ )        
            // if updater we also start the can gateway
            eupdater_cangtw_start(remaddr);
#endif               
            return 0;
        }// break;
        
        case CMD_SCAN:
        {
            //hal_trace_puts("CMD_SCAN");
#if !defined(_MAINTAINER_APPL_ )
            eEmoduleInfo_t* module = (eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
#else
            eEmoduleInfo_t* module = (eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
#endif
            
            *sizeout = 14;

            pktout[ 0] = CMD_SCAN;
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

            return 1;
        }// break;
        
//         case CMD_ERASE:
//         {
//             uint8_t erase_state = pktin[1];
//             
//             *sizeout = 2;
//             pktout[0] = CMD_ERASE;

//             switch (erase_state)
//             {
//             case PROGRAM_LOADER:
//                 s_prog_mem_start = EENV_MEMMAP_ELOADER_ROMADDR;
//                 s_prog_mem_size  = EENV_MEMMAP_ELOADER_ROMSIZE;
//                 break;

//             case PROGRAM_UPDATER:
//                 s_prog_mem_start = EENV_MEMMAP_EUPDATER_ROMADDR;
//                 s_prog_mem_size  = EENV_MEMMAP_EUPDATER_ROMSIZE;
//                 break;
//             case PROGRAM_APP:
//                 s_prog_mem_start = EENV_MEMMAP_EAPPLICATION_ROMADDR;
//                 s_prog_mem_size  = EENV_MEMMAP_EAPPLICATION_ROMSIZE;
//                 break;
//             
// #if defined(_MAINTAINER_APPL_)
//             case PROGRAM_SHARSERV: // only maintainer can do that.
//                 s_prog_mem_start = EENV_MEMMAP_SHARSERV_ROMADDR;
//                 s_prog_mem_size  = EENV_MEMMAP_SHARSERV_ROMSIZE;
//                 break;            
// #endif
//             
//             default:
//                 erase_state = 0;
//                 pktout[1] = UPD_ERR_UNK;
//                 return 1;    
//             }
//             
//             osal_system_scheduling_suspend();
//                 
//             if (hal_res_OK != hal_flash_erase(s_prog_mem_start, s_prog_mem_size))
//             {
//                 erase_state = 0;
//                 //hal_trace_puts("ERASE FAILED");
//                 pktout[1] = UPD_ERR_FLASH;    
//             }
//             else
//             {
//                 //hal_trace_puts("ERASE DONE");
//                 pktout[1] = UPD_OK;
//             }
//             
//             osal_system_scheduling_restart();
//                         
//             return 1;
//         }// break;        

        case CMD_START:
        {
            //hal_trace_puts("CMD_START");

            s_download_packets = 1;
            s_download_state = pktin[1];
            s_erased_eeprom = 0;
            
            *sizeout = 2;
            pktout[0] = CMD_START;

            switch (s_download_state)
            {
                case PROGRAM_LOADER:
                {
                    s_prog_mem_start = EENV_MEMMAP_ELOADER_ROMADDR;
                    s_prog_mem_size  = EENV_MEMMAP_ELOADER_ROMSIZE;
                } break;

                case PROGRAM_UPDATER:
                {
                    s_prog_mem_start = EENV_MEMMAP_EUPDATER_ROMADDR;
                    s_prog_mem_size  = EENV_MEMMAP_EUPDATER_ROMSIZE;
                } break;

                case PROGRAM_APP:
                {
                    s_prog_mem_start = EENV_MEMMAP_EAPPLICATION_ROMADDR;
                    s_prog_mem_size  = EENV_MEMMAP_EAPPLICATION_ROMSIZE;
                } break;
            
            
                default:
                {
                    s_download_state = 0;
                    s_prog_mem_start = s_prog_mem_size = 0;
                    pktout[1] = UPD_ERR_UNK;
                } break;                
            }
            
            // cannot accept a download of such a hex file
            if(0 == s_download_state)
            {
                return 1;   
            }
            


            if(1 == s_overlapping_with_code_space(s_prog_mem_start, s_prog_mem_size))
            {
                //hal_trace_puts("overlapping");
                s_download_state = 0;
                s_prog_mem_start = s_prog_mem_size = 0;
                pktout[1] = UPD_ERR_UNK;               
            }
            else
            {
                //hal_trace_puts("not overlapping");   
                pktout[1] = UPD_OK;
            }
            
            // cannot accept a download of such a hex file
            if(0 == s_download_state)
            {
                return 1;   
            }            
            
            // if in here, it means that i am attempting to program something in a space memory different than
            // the one of tthe running process, either updater or maintainer. 
            // the important thing in case something fails in between the flash erase and the end of programming is that
            // the START process is set at the one running now ... if so, anything that goes wrong is always recovered.
            // HOWEVER: YOU MUST NOT ERASE EEPROM
            
#if     defined(_MAINTAINER_APPL_)
            ee_sharserv_part_proc_startup_set(ee_procApplication);
#else
            ee_sharserv_part_proc_startup_set(ee_procUpdater);
#endif            
 
#if defined(ERASE_EARLY)
 
            if(0 == s_erased_eeprom)
            {
                hal_sys_irq_disable();  //osal_system_scheduling_suspend();                       
                halres = hal_flash_erase(s_prog_mem_start, s_prog_mem_size);
                hal_sys_irq_enable();   //osal_system_scheduling_restart();
                
                    
                if(hal_res_OK != halres)
                {
                    s_download_state = 0;
                    //hal_trace_puts("ERASE FAILED");
                    pktout[1] = UPD_ERR_FLASH;    
                }
                else
                {
                    s_erased_eeprom = s_download_state;
                    //hal_trace_puts("ERASE DONE");
                    pktout[1] = UPD_OK;
                } 
            }            

#else
            pktout[1] = UPD_OK;
            
#if defined(USERAM_FOR_LOADER) 
            if(PROGRAM_LOADER == s_download_state)
            {
                s_ramforloader_maxoffset = 0;
                memset(s_ramforloader_data, 0xff, s_prog_mem_size);
            }
#endif
            
#endif
            
            if(0 != s_download_state)
            {
                // download is to begin, thus start blinking led in a different way
                eupdater_parser_download_blinkled_start();
            }
            
            return 1;
        }// break;
            
        case CMD_DATA:
        {
            if (0 == s_download_state)
            {
                *sizeout = 2;
                pktout[0] = CMD_DATA;
                pktout[1] = UPD_ERR_UNK;
                return 1;
            }

            //hal_trace_puts("CMD_DATA");

            *sizeout = 2;
            pktout[0] = CMD_DATA;

            uint32_t address = pktin[4]<<24 |
                               pktin[3]<<16 |
                               pktin[2]<<8  |
                               pktin[1];

            uint16_t size = pktin[6]<<8 | pktin[5];

            void *data = pktin + 7;

                  
            if ((address >= s_prog_mem_start) && (address+size < s_prog_mem_start+s_prog_mem_size))
            {
                
#if !defined(ERASE_EARLY)
                
#if defined(USERAM_FOR_LOADER) 
                
                // erase only if not loader
                
                if((0 == s_erased_eeprom) && (PROGRAM_LOADER != s_download_state))
                {
                    hal_sys_irq_disable();  //osal_system_scheduling_suspend();                       
                    halres = hal_flash_erase(s_prog_mem_start, s_prog_mem_size);
                    hal_sys_irq_enable();   //osal_system_scheduling_restart();
                    
                        
                    if(hal_res_OK != halres)
                    {
                        s_download_state = 0;
                        //hal_trace_puts("ERASE FAILED");
                        pktout[1] = UPD_ERR_FLASH;    
                    }
                    else
                    {
                        s_erased_eeprom = s_download_state;
                        //hal_trace_puts("ERASE DONE");
                        //pktout[1] = UPD_OK;
                    }  
                }
#else
                // if not erased .... erase flash now
                if(0 == s_erased_eeprom)
                {
                    hal_sys_irq_disable();  //osal_system_scheduling_suspend();                       
                    halres = hal_flash_erase(s_prog_mem_start, s_prog_mem_size);
                    hal_sys_irq_enable();   //osal_system_scheduling_restart();
                    
                        
                    if(hal_res_OK != halres)
                    {
                        s_download_state = 0;
                        //hal_trace_puts("ERASE FAILED");
                        pktout[1] = UPD_ERR_FLASH;    
                    }
                    else
                    {
                        s_erased_eeprom = s_download_state;
                        //hal_trace_puts("ERASE DONE");
                        //pktout[1] = UPD_OK;
                    }  
                }
#endif
                
#endif                


#if defined(USERAM_FOR_LOADER) 
                if(PROGRAM_LOADER == s_download_state)
                {
                    uint32_t position = address - s_prog_mem_start;
                    if(position < s_prog_mem_size)
                    {
                        memcpy(&s_ramforloader_data[position], data, size);
                        if((position+size)> s_ramforloader_maxoffset)
                        {
                            s_ramforloader_maxoffset = position+size;
                        }
                        pktout[1] = UPD_OK;
                    }
                    else
                    {
                        pktout[1] = UPD_ERR_PROT;
                    }
                    ++s_download_packets;
                }
                else
                {
                    // write the chunk of received data only if it is inside the expected memory space and it is already erased
                    if(0 != s_erased_eeprom)
                    {
                        
                        hal_sys_irq_disable();
                        halres = hal_flash_write(address, size, data);
                        hal_sys_irq_enable();
                        
                        ++s_download_packets;
                        
                        if(hal_res_NOK_generic == halres)
                        {
                            pktout[1] = UPD_ERR_PROT;
                        }
                        else
                        {    
                            pktout[1] = UPD_OK;
                        }
                    }
                }
#else                
                // write the chunk of received data only if it is inside the expected memory space and it is already erased
                if(0 != s_erased_eeprom)
                {
                    
                    hal_sys_irq_disable();
                    halres = hal_flash_write(address, size, data);
                    hal_sys_irq_enable();
                    
                    ++s_download_packets;
                    
                    if(hal_res_NOK_generic == halres)
                    {
                        pktout[1] = UPD_ERR_PROT;
                    }
                    else
                    {    
                        pktout[1] = UPD_OK;
                    }
                }
                
#endif
                
            }
            else
            {
                pktout[1] = UPD_ERR_PROT;
            }
            
            
            eupdater_parser_download_toggleled();   
               
            return 1;
        }// break;
            
        case CMD_END:
        {
            if (0 == s_download_state)
            {
                //hal_trace_puts("CMD_END received and s_download_state is 0"); 
                if(0 !=  s_erased_eeprom)
                {
                    const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                    const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                    ee_sharserv_part_proc_synchronise(ee_procUpdater, (const eEmoduleInfo_t*)updt);
                    ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                    //hal_trace_puts("CMD_END received and s_download_state is 0 and s_erased_eeprom is not 0"); 
                } 
                
                 
                *sizeout = 2;
                pktout[0] = CMD_DATA;
                pktout[1] = UPD_ERR_UNK;
                return 1;
            }

            //hal_trace_puts("CMD_END");

            *sizeout = 2;
            pktout[0] = CMD_END;
            
            //char str[64] = {0};            
            uint16_t sentpkts = (pktin[2]<<8)|pktin[1];
            //snprintf(str, sizeof(str), "sent = %d, downloaded = %d", sentpkts, s_download_packets);
            //hal_trace_puts(str);
            
            if (sentpkts == s_download_packets)
            {
                pktout[1] = UPD_OK;

                switch (s_download_state)
                {
                    case PROGRAM_LOADER:
                    {
#if defined(USERAM_FOR_LOADER) 
                        // 1. erase flash and write
                        hal_sys_irq_disable();                   
                        halres = hal_flash_erase(s_prog_mem_start, s_prog_mem_size);
                        halres = hal_flash_write(s_prog_mem_start, s_prog_mem_size, s_ramforloader_data);
                        hal_sys_irq_enable();  
                        s_erased_eeprom = PROGRAM_LOADER;                        
#endif                        
                        const volatile eEmoduleInfo_t* lder = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_ELOADER_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procLoader,(const eEmoduleInfo_t*)lder);
                    } break;    
                    case PROGRAM_UPDATER:
                    {
                        const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)updt);
                        //ee_sharserv_part_proc_startup_set(ee_procApplication);
                        //ee_sharserv_part_proc_def2run_set(ee_procApplication);
                    } break;
                    case PROGRAM_APP:
                    {
                        const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                        //ee_sharserv_part_proc_startup_set(ee_procUpdater);
                    }  break;
                }
            }
            else
            {
                pktout[1] = UPD_ERR_LOST;
                
#if defined(ERASE_EARLY)
#else
                if(0 !=  s_erased_eeprom)
                {
#endif

                // must manage failure. we surely have erased eeprom in here and maybe we have written some sectors.
                // thus we had better to erase eeprom fully, set the def2run and start to a safe process, and ... put in partition table message about erased process
                if (s_download_state != PROGRAM_LOADER)
                {
                    osal_system_scheduling_suspend();
                    //hal_sys_irq_disable();
                    if(PROGRAM_APP == s_download_state)
                    {
                        hal_flash_erase(EENV_MEMMAP_EAPPLICATION_ROMADDR, EENV_MEMMAP_EAPPLICATION_ROMSIZE);
                        //hal_trace_puts("erased app and removed it from partition table"); 
                        ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)&erasedappl);
                        const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)updt);
                        ee_sharserv_part_proc_startup_set(ee_procUpdater);
                        ee_sharserv_part_proc_def2run_set(ee_procUpdater);
                    }
                    else if(PROGRAM_UPDATER == s_download_state)
                    {
                        hal_flash_erase(EENV_MEMMAP_EUPDATER_ROMADDR, EENV_MEMMAP_EUPDATER_ROMSIZE);
                        // cannot remove updater so far .... ee_sharserv_part_proc_rem(ee_procUpdater);
                        //hal_trace_puts("erased upt and removed it from partition table"); 
                        //const volatile eEmoduleInfo_t* updt = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procUpdater,(const eEmoduleInfo_t*)&erasedupdt);
                        const volatile eEmoduleInfo_t* appl = (const volatile eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);
                        ee_sharserv_part_proc_synchronise(ee_procApplication,(const eEmoduleInfo_t*)appl);
                        ee_sharserv_part_proc_startup_set(ee_procApplication);
                        ee_sharserv_part_proc_def2run_set(ee_procApplication);
                    }
                    
                    osal_system_scheduling_restart();
                    //hal_sys_irq_disable();
                }
                
#if defined(ERASE_EARLY)
#else
            }
#endif
              
            }
   
            s_download_state = 0;
            s_prog_mem_start = s_prog_mem_size = 0;

            // download is over, thus resume the normal blinking of led
            eupdater_parser_download_blinkled_stop();

            return 1;
        }// break;

        case CMD_BOOT:
        {
            //hal_trace_puts("CMD_BOOT");

            eEprocess_t active_part = (eEprocess_t)pktin[1];

            if ((active_part >= ee_procUpdater) && (active_part <= ee_procOther05))
            {
                ee_sharserv_part_proc_startup_set(ee_procUpdater);
                ee_sharserv_part_proc_def2run_set(active_part);
            }

            return 0;
        }// break;

        case CMD_RESET:
        {
            //hal_trace_puts("CMD_RESET");
            //ee_sharserv_sys_restart(); better avoiding the shared lib call
            hal_sys_irq_disable();
            hal_sys_systemreset();
            
            return 0;
        }// break;

        case CMD_IPSET:
        {
            //hal_trace_puts("CMD_IPSET");

            uint8_t *ip = pktin;
            
            eEipnetwork_t ipnetwork;
            const eEipnetwork_t *ipnetworkstrg;

            ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
            memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
            ipnetwork.ipaddress = EECOMMON_ipaddr_from(ip[1], ip[2], ip[3], ip[4]);
            
            ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork);

            return 0;
        }// break;

        case CMD_MASKSET:
        {
            //hal_trace_puts("CMD_MASKSET");
            
            uint8_t *ip = pktin;
            
            eEipnetwork_t ipnetwork;
            const eEipnetwork_t *ipnetworkstrg;

            ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
            memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
            ipnetwork.ipnetmask = EECOMMON_ipaddr_from(ip[1], ip[2], ip[3], ip[4]);
            ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork);

            return 0;
        }// break;
        
        case CMD_MACGET:
        {                     
            const eEipnetwork_t *ipnetworkstrg;
            
            ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
            pktout[0] = CMD_MACGET;
            
            pktout[1] = (ipnetworkstrg->macaddress>>40) & 0xFF;
            pktout[2] = (ipnetworkstrg->macaddress>>32) & 0xFF;
            pktout[3] = (ipnetworkstrg->macaddress>>24) & 0xFF;
            pktout[4] = (ipnetworkstrg->macaddress>>16) & 0xFF;
            pktout[5] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
            pktout[6] = (ipnetworkstrg->macaddress)     & 0xFF;

            *sizeout = 7;
            return 1;
        }// break;        
        
        case CMD_MACSET:
        {
                     
            eEipnetwork_t ipnetwork;
            const eEipnetwork_t *ipnetworkstrg;
            uint64_t mac = 0;
            //memcpy(&mac, &pktin[1], 6);
            uint64_t tmp = 0;
            tmp = pktin[1];     mac |= (tmp<<40);
            tmp = pktin[2];     mac |= (tmp<<32);
            tmp = pktin[3];     mac |= (tmp<<24);
            tmp = pktin[4];     mac |= (tmp<<16);
            tmp = pktin[5];     mac |= (tmp<<8);
            tmp = pktin[6];     mac |= (tmp);

            ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
            memcpy(&ipnetwork, ipnetworkstrg, sizeof(eEipnetwork_t));
            ipnetwork.macaddress = mac;
            ee_sharserv_info_deviceinfo_item_set(sharserv_info_ipnet, (const void*)&ipnetwork);
            
            //char str[128];
            //snprintf(str, sizeof(str), "mac is h = %llx, l = %llx", ipnetwork.macaddress >> 32, ipnetwork.macaddress & 0xffffffff);
            //hal_trace_puts(str);    
            
            
            return 0;
        }// break;
                
        case CMD_SYSEEPROMERASE:
        {   
            eEresult_t ret = ee_res_NOK_generic;
            
#if     defined(_MAINTAINER_APPL_)
            // the maintainer cannot erase the eeprom
            ret = ee_res_NOK_generic;       
#else
            // only the updater can do it. 
            // because if eeprom is erased, the loader will set the START equal to updater.
            // if we are not sure the updater is able to run, it is better not to risk a communication failure
            ret = s_sys_eeprom_erase();      
#endif
            
            pktout[0] = CMD_SYSEEPROMERASE;
            pktout[1] = (ee_res_OK == ret) ? UPD_OK : UPD_ERR_UNK;
            
            *sizeout = 2;
            
            return 1;
        }// break;   
        
        case CMD_PROCS:
        {
            uint8_t num_procs = 0;
            const eEprocess_t *s_proctable = NULL;
            const eEmoduleInfo_t *s_modinfo = NULL;

            pktout[0] = CMD_PROCS;
            pktout[1] = 0;   
            
            char *data = (char*)pktout;
            uint16_t size = 2;            
            
            const eEmoduleInfo_t *sharserv = ee_sharserv_moduleinfo_get();
//            
//            size+=snprintf(data+size, MAX0(capacityout-size), "*** running e-proc uses sharserv\r\n");
//            size+=snprintf(data+size, MAX0(capacityout-size), "version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
//                    sharserv->info.entity.version.major, 
//                    sharserv->info.entity.version.minor,
//                    sharserv->info.entity.builddate.month,
//                    sharserv->info.entity.builddate.day,
//                    sharserv->info.entity.builddate.year-2000,
//                    sharserv->info.entity.builddate.hour,
//                    sharserv->info.entity.builddate.min
//                );
//            size+=snprintf(data+size, MAX0(capacityout-size), "rom.addr\t0x%0.8X\r\n", sharserv->info.rom.addr);
//            size+=snprintf(data+size, MAX0(capacityout-size), "rom.size\t0x%0.8X\r\n", sharserv->info.rom.size);
//            size+=snprintf(data+size, MAX0(capacityout-size), "ram.addr\t0x%0.8X\r\n", sharserv->info.ram.addr);
//            size+=snprintf(data+size, MAX0(capacityout-size), "ram.size\t0x%0.8X\r\n", sharserv->info.ram.size);

//            size+=snprintf(data+size, MAX0(capacityout-size), "stg.type\t%s\r\n", (ee_strg_none == sharserv->info.storage.type) ? ("none") 
//                                                        : ((ee_strg_eflash==sharserv->info.storage.type) ? ("flash") : ("eeprom")));
//            size+=snprintf(data+size, MAX0(capacityout-size), "stg.addr\t0x%0.8X\r\n", sharserv->info.storage.addr);
//            size+=snprintf(data+size, MAX0(capacityout-size), "stg.size\t0x%0.8X\r\n", sharserv->info.storage.size);                
//            size+=snprintf(data+size, MAX0(capacityout-size), "\n");
//            
            const eEmoduleInfo_t *strsharserv = ee_sharserv_storage_moduleinfo_get();
            
            if((strsharserv->info.entity.version.major != sharserv->info.entity.version.major) || (strsharserv->info.entity.version.minor != sharserv->info.entity.version.minor))
            {
                size+=snprintf(data+size, MAX0(capacityout-size), "EEPROM contains sharserv version different from that of this process. it has:\r\n");
                size+=snprintf(data+size, MAX0(capacityout-size), "eeprom version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
                        strsharserv->info.entity.version.major, 
                        strsharserv->info.entity.version.minor,
                        strsharserv->info.entity.builddate.month,
                        strsharserv->info.entity.builddate.day,
                        strsharserv->info.entity.builddate.year,
                        strsharserv->info.entity.builddate.hour,
                        strsharserv->info.entity.builddate.min
                    );  
                size+=snprintf(data+size, MAX0(capacityout-size), "process version\t%d.%d %d/%d/%d %d:%.2d\r\n", 
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
                size+=snprintf(data+size, MAX0(capacityout-size), "*** AND IT IS NOT VALID\r\n");
                size+=snprintf(data+size, MAX0(capacityout-size), "*** CANNOT GIVE PARTITION INFO\r\n");              
                size+=snprintf(data+size, MAX0(capacityout-size), "*** BYE BYE\r\n"); 
                size+=snprintf(data+size, MAX0(capacityout-size), "\n");                    
            }                        
            else if (ee_res_OK == ee_sharserv_part_proc_allavailable_get(&s_proctable, &num_procs))
            {
                pktout[0] = CMD_PROCS;
                pktout[1] = num_procs; 
                

                volatile eEmoduleExtendedInfo_t* extinfo = NULL;    
                eEprocess_t defproc;
                eEprocess_t startup;
                eEprocess_t running =
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

                    size+=snprintf(data+size, MAX0(capacityout-size), "*** e-process #%d \r\n", i);
                    size+=snprintf(data+size, MAX0(capacityout-size), "props\t%s%s%s \r\n", defproc==i?"DEF ":"", startup==i?"START ":"", running==i?"RUNNING ":"" ) ;

                    size+=snprintf(data+size, MAX0(capacityout-size), "name\t%s\r\n", s_modinfo->info.name);
                    size+=snprintf(data+size, MAX0(capacityout-size), "vers\t%d.%d\r\n", 
                        s_modinfo->info.entity.version.major, 
                        s_modinfo->info.entity.version.minor
                    );
                    size+=snprintf(data+size, MAX0(capacityout-size), "date\t%s %.2d %d %d:%.2d\r\n", 
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
                        size+=snprintf(data+size, MAX0(capacityout-size), "built\t%s\r\n", 
                            extinfo->compilationdatetime
                        );                        
                    }
                    else
                    {
                        size+=snprintf(data+size, MAX0(capacityout-size), "built\tin unknown date\r\n"
                        );    
                    }

                    size+=snprintf(data+size, MAX0(capacityout-size), "rom\t@+%dKB, s=%dKB\r\n", (s_modinfo->info.rom.addr-EENV_ROMSTART+1023)/1024, (s_modinfo->info.rom.size+1023)/1024);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "rom\t[0x%0.8X, 0x%0.8X)\r\n", s_modinfo->info.rom.addr, s_modinfo->info.rom.addr+s_modinfo->info.rom.size);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "ram\t[0x%0.8X, 0x%0.8X)\r\n", s_modinfo->info.ram.addr, s_modinfo->info.ram.addr+s_modinfo->info.ram.size);

                    
                    //                    size+=snprintf(data+size, MAX0(capacityout-size), "rom.addr\t0x%0.8X\r\n", s_modinfo->info.rom.addr);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "rom.size\t0x%0.8X\r\n", s_modinfo->info.rom.size);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "ram.addr\t0x%0.8X\r\n", s_modinfo->info.ram.addr);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "ram.size\t0x%0.8X\r\n", s_modinfo->info.ram.size);

//                    size+=snprintf(data+size, MAX0(capacityout-size), "stg.type\t%s\r\n", (ee_strg_none == s_modinfo->info.storage.type) ? ("none") 
//                                                                : ((ee_strg_eflash==s_modinfo->info.storage.type) ? ("flash") : ("eeprom")));
//                    size+=snprintf(data+size, MAX0(capacityout-size), "stg.addr\t0x%0.8X\r\n", s_modinfo->info.storage.addr);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "stg.size\t0x%0.8X\r\n", s_modinfo->info.storage.size);
//                    size+=snprintf(data+size, MAX0(capacityout-size), "com.msk\t0x%0.8X\r\n\r", s_modinfo->info.communication);
                    size+=snprintf(data+size, MAX0(capacityout-size), "\n");
                }

            }

            *sizeout = size + 1;
            return 1;
        }// break

        case CMD_BLINK:
        {
            for (uint8_t n=0; n<16; ++n)
            { 
                hal_led_toggle(hal_led2);
                osal_task_wait(250*1000);
            }

            return 0;
        }// break;

        case CMD_UPD_ONCE:
        {
#if defined(_MAINTAINER_APPL_) 
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
            ee_sharserv_sys_restart();
#endif            
            return 0;
        }// break;

        default:
        {
            hal_trace_puts("DEFAULT");
        } break;
    }

    return 0;
}

#if     !defined(_MAINTAINER_APPL_)
static eEresult_t s_sys_eeprom_erase(void)
{
#if 0    
   // ... it does not stay in memory    
   uint32_t i=0;
   static uint8_t data[EENV_STGSIZE-EENV_STGSTART] = {0};
   eEresult_t ret = ee_res_OK;
    
    // erase-eeprom
    hal_eeprom_erase(hal_eeprom_i2c_01, EENV_STGSTART, EENV_STGSIZE);  
    
    hal_eeprom_read(hal_eeprom_i2c_01, EENV_STGSTART, EENV_STGSIZE, data);

    for(i=0; i<EENV_STGSIZE-EENV_STGSTART; i++)
    {
        if(0xff != data[i])
        {
            ret = ee_res_NOK_generic;
        }
    } 

    return(ret);
#else    
    eEstorage_t strg = { .type = ee_strg_eeprom, .size = EENV_MEMMAP_SHARSERV_STGSIZE, .addr = EENV_MEMMAP_SHARSERV_STGADDR};
    return(ee_sharserv_sys_storage_clr(&strg, EENV_MEMMAP_SHARSERV_STGSIZE));
#endif    
}
#endif

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
 
//    char str[128] = {0};
//    snprintf(str, sizeof(str), "addr = %x size = %d vs beg = %x siz = %x", addr, size, BEG, END-BEG); 
//    hal_trace_puts(str);
            
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
