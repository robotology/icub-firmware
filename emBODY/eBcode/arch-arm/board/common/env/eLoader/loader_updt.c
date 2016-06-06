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

/* @file       loader_updt.c
    @brief      This  file implements the eLoader process for a stm32f107 in a way which favours a jump to eUpdater unless 
                otherwise ordered.
    @author     marco.accame@iit.it
    @date       03/30/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal.h"
#include "hal_switch.h"
#include "hal_eeprom.h"
#include "hal_led.h"

//#if		defined(HAL_IS_VERSION_2)

#include "hal_uniqueid.h"

//#else	// must be version 1	
//#include "hal_arch.h"
//#define hal_uniqueid_id64bit_get hal_arch_arm_uniqueid64_get
//#endif

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "eEcommon.h"
#include "eEmemorymap.h"

#include "eEsharedServices.h" 

#include "emBODYrobot.h"

#include "EoBoards.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define LOADER_ADR_INVALID         0xffffffff
#define LOADER_LED                 0x100


// #define FORCE_EEPROM_ERASE // it must be undefined unless you wanto to erase the eeprom
// #define DEBUG_MODE

#if defined(FORCE_EEPROM_ERASE)
	#warning -> ATTENTION: FORCE_EEPROM_ERASE is set ... undef it after you have erased the eeprom
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_loader_hardware_init(void);

static void s_loader_shared_services_init(void);

static void s_loader_Delay(uint32_t dlyTicks);
static void s_loader_manage_error(uint32_t onmilli, uint32_t offmilli);
static void s_loader_exec_loader(void);
static void s_loader_eval_jump_request_from_an_eproc(void);
//static void s_loader_eval_def_from_partition_table(void);
//static void s_loader_eval_default(void);
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails);

static void s_loader_HW_init(void);
static void s_loader_HW_SYSTICK_Config(void);
static void s_loader_HW_LED_Config(void);
static void s_loader_HW_LED_On(uint32_t led);
static void s_loader_HW_LED_Off(uint32_t led);

//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
////static void s_loader_eval_user_request(void);
//static void s_loader_HW_INP_Config(void);
//static uint32_t s_loader_HW_INP_IsPushed(void);
//#endif



//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
//static void s_eval_eeprom_erase(void);
//#endif

#if defined(FORCE_EEPROM_ERASE) || defined(ENABLE_ERASE_EEPROM)
static void s_eeprom_erase(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



static const eEmoduleExtendedInfo_t s_loader_info_extended __attribute__((at(EENV_MEMMAP_ELOADER_ROMADDR+EENV_MODULEINFO_OFFSET))) = 
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procLoader,
                .version    = 
                { 
                    .major = 2, 
                    .minor = 12
                },  
                .builddate  = 
                {
                    .year  = 2016,
                    .month = 6,
                    .day   = 6,
                    .hour  = 14,
                    .min   = 22
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_ROMADDR,
                .size   = EENV_MEMMAP_ELOADER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_RAMADDR,
                .size   = EENV_MEMMAP_ELOADER_RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_none,
            .name           = "eLoader"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 0, .minor = 0},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 0},
            .gtwprotversion  = { .major = 0, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};


#if (emBODYrobot_BOARD_NAME == boardEMS001)


static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = 0x11,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2011,
                .month = 11,
                .day   = 3,
                .hour  = 18,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "ems001"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#elif (emBODYrobot_BOARD_NAME == boardMCBSTM32C)


static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = 0x11,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2010,
                .month = 1,
                .day   = 1,
                .hour  = 0,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "mcbstm32c"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#elif (emBODYrobot_BOARD_NAME == boardEMS4RD)


static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_ems4,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2011,
                .month = 11,
                .day   = 3,
                .hour  = 18,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "ems4rd"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#elif (emBODYrobot_BOARD_NAME == boardMCBSTM32F400)


#error --> MISSING THE CORRECT HAL for boardMCBSTM32F400


static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = 0x11,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2010,
                .month = 1,
                .day   = 1,
                .hour  = 0,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "mcbstm32f400"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#elif   defined(boardMC4PLUS)

static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_mc4plus,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2016,
                .month = 1,
                .day   = 1,
                .hour  = 12,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "mc4plus"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#elif   defined(boardMC2PLUS)

static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_mc2plus,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2016,
                .month = 6,
                .day   = 6,
                .hour  = 12,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "mc2plus"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#else
    #error --> specify emBODYrobot_BOARD_NAME
#endif


static volatile uint32_t s_loader_msTicks;  

static uint8_t hw_initted = 0; 

////#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
//#if (emBODYrobot_BOARD_NAME == boardMCBSTM32C) && defined(ENABLE_USER_INPUT)
//static const hal_gpio_cfg_t s_loader_input_button =
//{   // userBUTTON on mcbstm32c
//    .port       = hal_gpio_portB,
//    .pin        = hal_gpio_pin7,
//    .speed      = hal_gpio_speed_low,
//    .dir        = hal_gpio_dirINP
//};
//#endif

                        

// --------------------------------------------------------------------------------------------------------------------
// - definition of main 
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    s_loader_hardware_init();
    s_loader_shared_services_init();
    

//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
//    // if any user request, it either jumps or manage error. else, it returns.
//    //s_loader_eval_user_request();
//#endif

    // if any jump request from another eprocess, and it can satisfy that the function does not return:
    // it either jumps or stays in eloader with a forever loop. 
    // else, the function just returns.
    s_loader_eval_jump_request_from_an_eproc();
    
    // now we attempt to jump to startup process. reading cannot fail ... but we put control anyway and 
    // in such a case we force to updater
    eEprocess_t startup = ee_procNone;
    eEresult_t eeres = ee_sharserv_part_proc_startup_get(&startup);
    if(ee_res_NOK_generic == eeres)
    {
        startup = ee_procUpdater;
    }
    s_loader_attempt_jump(startup, LOADER_ADR_INVALID);
    
    // if it fails a jump to startup ... do a last attempt to jump to eUpdater
    s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);

    // if we are in here we cannot jump to the startup and not even to the updater.
    s_loader_manage_error(80, 20);

    // just for ...
    for(;;);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_loader_hardware_init(void)
{
    // init the hw ...
    s_loader_HW_init();
}

static void s_on_sharserv_error(void)
{
    s_loader_manage_error(20, 80);
}

static void s_loader_shared_services_init(void)
{
    eEprocess_t defproc = ee_procNone;
    eEprocess_t startup = ee_procNone;
    
    sharserv_mode_t sharservmode = 
    {
        .onerror    = s_on_sharserv_error,
        .initmode   = sharserv_base_initmode_forcestorageinit
    };
    
    if(ee_res_OK != ee_sharserv_isvalid())
    {
        s_loader_manage_error(20, 80);
    }
    else
    {
        // init sharserv
        if(ee_res_OK != ee_sharserv_init(&sharservmode))
        {
            s_loader_manage_error(20, 80);
        }        
    }
    
    // now all are initted. then ...

    // put signature in partition table
    if(ee_res_OK != ee_sharserv_part_proc_synchronise(ee_procLoader, (const eEmoduleInfo_t *)&s_loader_info_extended))
    {
        s_loader_manage_error(20, 80);
    }    
    
    // impose boardinfo
    s_loader_boardinfo.uniqueid = hal_uniqueid_id64bit_get();
    
    if(ee_res_OK != ee_sharserv_info_boardinfo_synchronise(&s_loader_boardinfo))
    {
        s_loader_manage_error(20, 80);
    }    
       
    // impose startup process
    if(ee_res_OK != ee_sharserv_part_proc_startup_get(&startup))
    {
        // we impose that the startup process is the updater
        ee_sharserv_part_proc_startup_set(ee_procUpdater);
    }  
    
    // impose def2run process
    if(ee_res_OK != ee_sharserv_part_proc_def2run_get(&defproc))
    {
        // we impose that the default process is the updater
#if !defined(DEBUG_MODE)        
        ee_sharserv_part_proc_def2run_set(ee_procUpdater);
#else
        ee_sharserv_part_proc_def2run_set(ee_procApplication);  
#endif        

    }  
    
    
      
}

// used to eval the jump request coming from another process
static void s_loader_eval_jump_request_from_an_eproc(void)
{
    eEprocess_t pr = ee_procNone;

    if(ee_res_OK == ee_sharserv_ipc_gotoproc_get(&pr))
    {
        ee_sharserv_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // we communicate to the updater to stay forever and not to jump to default after the 5 (or what) seconds
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
        }
        
        // attempt only to the requested process.
        s_loader_attempt_jump(pr, LOADER_ADR_INVALID);
        
        // if in here ... the jump failed, thus ... behave as if no order at all
        ee_sharserv_ipc_gotoproc_clr();
        
//        // if in here ... the jump failed, thus ... it is better to go to the updater and stay there forever
//        ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
//        s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);
        
    }
}


static void s_loader_Delay(uint32_t dlyTicks) 
{
    uint32_t curTicks;
    curTicks = s_loader_msTicks;
    while((s_loader_msTicks - curTicks) < dlyTicks);
}



static void s_loader_manage_error(uint32_t onmilli, uint32_t offmilli)
{
    s_loader_HW_init();
    s_loader_HW_LED_Config();
    s_loader_HW_SYSTICK_Config(); 

    for(;;) 
    {
        s_loader_HW_LED_On(LOADER_LED);                            
        s_loader_Delay(onmilli);                                
        s_loader_HW_LED_Off(LOADER_LED);                            
        s_loader_Delay(offmilli);                               
    }

}


// the default application in case a process ask to execute the loader
void s_loader_exec_loader(void)
{
    uint32_t cnt = 10;
    uint8_t data[8];
    uint8_t size = 0;
    eEresult_t res;

    uint8_t factor_on  = 1;
    uint8_t factor_off = 9;

    s_loader_HW_SYSTICK_Config(); 
    s_loader_HW_LED_Config(); 


    // i could see if anybody has given me any message on shared data.

    res = ee_sharserv_ipc_userdefdata_get(data, &size, 8);

    if(ee_res_OK == res)
    {
        factor_on  = data[0];
        factor_off = data[1]; 
        
        ee_sharserv_ipc_userdefdata_clr();  
    }

    for(;;) 
    {
        if(0 == cnt)
        {
//            // try to go back to default applications
//            s_loader_eval_default();
//            // nothing to do ...
//            s_loader_manage_error(200, 50);
            
            cnt = 10;
        }

        cnt --;

        s_loader_HW_LED_On (LOADER_LED);                            
        s_loader_Delay (100*factor_on);                                
        s_loader_HW_LED_Off (LOADER_LED);                            
        s_loader_Delay (100*factor_off);                               
    }

}

// used to attempt a jump to a process 
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails)
{
    uint32_t address = LOADER_ADR_INVALID;

    if(ee_procNone != proc)
    {

        if(ee_procLoader == proc)
        {
            // avoid recursive bootstraps
            s_loader_exec_loader();
        }
     
        // attempt to get the address of the proc from partition table.
        // it works only if the process already run before and register itself in the partition table.
        // if it fails ... use brute force mode
        if(ee_res_NOK_generic == ee_sharserv_part_proc_runaddress_get(proc, &address))
        {
            switch(proc)
            {
                case ee_procUpdater:        address = EENV_MEMMAP_EUPDATER_ROMADDR;         break;
                case ee_procApplication:    address = EENV_MEMMAP_EAPPLICATION_ROMADDR;     break;
                default:                    address = LOADER_ADR_INVALID;                   break;
            }
        }
        
        // if we retrieve the address ... attempt to jump
        if(LOADER_ADR_INVALID != address)
        {
            if(ee_res_OK == ee_sharserv_sys_canjump(address))
            {
                ee_sharserv_sys_jumpnow(address);
            }                
        }        

    }
    
    // if in here it means the we could not jump to the address of proc, thus we attempt an alternative.
    if(LOADER_ADR_INVALID != adr_in_case_proc_fails)
    {
        // attempt with adr_in_case_proc_fails 
        if(ee_res_OK == ee_sharserv_sys_canjump(adr_in_case_proc_fails))
        {
            ee_sharserv_sys_jumpnow(adr_in_case_proc_fails);
        }
    }

    // if i am in here we cannot jump ...
    return;
}

// #if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
// static void s_loader_eval_user_request(void)
// {
//     // we see if the user wants to go to the updater by pushing the button
//     if(1 == s_loader_HW_INP_IsPushed())
//     {
//         s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_DEF_UPD);
//         // if in here ... the jump failed, thus i go to error.
//         s_loader_manage_error(200, 800); 
//     }

//     // if hw can tell none, updater, one, another etc. put a switch-case and if the chosen fails ... manage error.
// }
// #endif//BOARD_MVBSTM32C


// static void s_loader_eval_def_from_partition_table(void)
// {
//     eEprocess_t pr = ee_procNone;

//     if(ee_res_OK == shalpart_proc_def2run_get(&pr))
//     {
//         s_loader_attempt_jump(pr, LOADER_ADR_INVALID);
//         // but if jump fails, go on ...    
//     }
// }

// static void s_loader_eval_default(void)
// {
//     s_loader_attempt_jump(ee_procNone, LOADER_ADR_DEF_APP);
//     s_loader_attempt_jump(ee_procNone, LOADER_ADR_DEF_UPD);
// }


// - hw dependant functions -------------------------------------------------------------------------------------------


void systickhandler(void) 
{
    s_loader_msTicks++;                      
}

static void s_loader_HW_init(void)
{     
    if(1 == hw_initted)
    {
        return;
    }
    else
    {
        hw_initted = 1;
    }

//#if		defined(HAL_IS_VERSION_2)

    extern const hal_core_cfg_t* hal_coreCFGptr;

    hal_core_init(hal_coreCFGptr);
    
    hal_core_start();

	hal_uniqueid_init(NULL);

//#else	// it must be hal version 1
//
//    extern const hal_cfg_t *hal_cfgMINE;
//    uint32_t size04aligned;
//    uint32_t *data32aligned = NULL;
//
//    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 
//
//    if(0 != size04aligned)
//    {
//        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
//    }
//
//    hal_base_initialise(hal_cfgMINE, data32aligned);
//
//    hal_sys_systeminit();
//
//#endif

    hal_eeprom_init(hal_eeprom_i2c_01, NULL); 

//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
//    s_loader_HW_INP_Config();
//#endif
    
//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
//    s_eval_eeprom_erase();
//#endif   

#if defined(FORCE_EEPROM_ERASE)
    s_eeprom_erase();
#endif    
    

#if (emBODYrobot_BOARD_NAME == boardEMS001)
    if(hal_false == hal_switch_initted_is())
    {
        hal_switch_init(NULL);
    } 
#endif

}


static void s_loader_HW_SYSTICK_Config(void)
{
    hal_sys_systick_sethandler(systickhandler, 1000, hal_int_priority15); // 1 milli
}

#if defined(FORCE_EEPROM_ERASE) || defined(ENABLE_ERASE_EEPROM)
static void s_eeprom_erase(void)
{
    uint32_t i=0;
    static uint8_t data[EENV_STGSIZE-EENV_STGSTART] = {0};
    
    // erase-eeprom
    hal_eeprom_erase(hal_eeprom_i2c_01, EENV_STGSTART, EENV_STGSIZE);  
    
    hal_eeprom_read(hal_eeprom_i2c_01, EENV_STGSTART, EENV_STGSIZE, data);

    for(i=0; i<EENV_STGSIZE-EENV_STGSTART; i++)
    {
        if(0xff != data[i])
        {
            s_loader_manage_error(100, 100);
        }
    }              
}
#endif

//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
//static void s_loader_HW_INP_Config(void)
//{
//    hal_gpio_init(s_loader_input_button.port, s_loader_input_button.pin, s_loader_input_button.dir, s_loader_input_button.speed);
//}
//
//static uint32_t s_loader_HW_INP_IsPushed(void)
//{
//    return((hal_gpio_valLOW == hal_gpio_getval(s_loader_input_button.port, s_loader_input_button.pin)) ? (1) : (0));
//}
//#endif//BOARD_MCBSTM32C

//#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
//static void s_eval_eeprom_erase(void)
//{
//  
//    if(1 == s_loader_HW_INP_IsPushed())
//    {    
//        s_eeprom_erase();
//    }
//}
//#endif   

static void s_loader_HW_LED_Config(void) 
{
     hal_led_init(hal_led0, NULL);
     hal_led_init(hal_led1, NULL);
     hal_led_init(hal_led2, NULL);
     hal_led_init(hal_led3, NULL);
     hal_led_init(hal_led4, NULL);
     hal_led_init(hal_led5, NULL);    
}


static void s_loader_HW_LED_On(uint32_t led) 
{
    hal_led_on(hal_led0);
    hal_led_on(hal_led1);
    hal_led_on(hal_led2);
    hal_led_on(hal_led3);
    hal_led_on(hal_led4);
    hal_led_on(hal_led5);
}


static void s_loader_HW_LED_Off(uint32_t led) 
{
    hal_led_off(hal_led0);
    hal_led_off(hal_led1);
    hal_led_off(hal_led2);
    hal_led_off(hal_led3);
    hal_led_off(hal_led4);
    hal_led_off(hal_led5);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


 




