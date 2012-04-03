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
#include "hal_arch.h"
#include "hal_switch.h"



#include "string.h"
#include "stdlib.h"

#include "eEcommon.h"
#include "eEmemorymap_stm32f1.h"

#include "shalBASE.h" 
#include "shalPART.h"
#include "shalINFO.h"

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
#define LOADER_ADR_DEF_UPD         EENV_MEMMAP_EUPDATER_ROMADDR 
#define LOADER_ADR_DEF_APP         EENV_MEMMAP_EAPPLICATION_ROMADDR

#define LOADER_LED                 0x100




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_loader_hardware_init(void);

static void s_loader_shalibs_init(void);

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

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
static void s_loader_eval_user_request(void);
static void s_loader_HW_INP_Config(void);
static uint32_t s_loader_HW_INP_IsPushed(void);
#endif

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
static void s_eval_eeprom_erase(void);
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eEmoduleInfo_t s_loader_info __attribute__((at(EENV_MEMMAP_ELOADER_ROMADDR+EENV_MODULEINFO_OFFSET))) = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procLoader,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2012,
                .month = 3,
                .day   = 30,
                .hour  = 12,
                .min   = 0
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
    .extra      = {0}
};

#if defined(BOARD_EMS001)

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

#elif defined(BOARD_MCBSTM32C)

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
#endif


static volatile uint32_t s_loader_msTicks;  

static uint8_t hw_initted = 0; 

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
static const hal_gpio_cfg_t s_loader_input_button =
{   // userBUTTON on mcbstm32c
    .port       = hal_gpio_portB,
    .pin        = hal_gpio_pin7,
    .speed      = hal_gpio_speed_low,
    .dir        = hal_gpio_dirINP
};
#endif

                        

// --------------------------------------------------------------------------------------------------------------------
// - definition of main 
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    static volatile hal_result_t reshal = hal_res_NOK_generic;

    s_loader_hardware_init();
    s_loader_shalibs_init();
    


#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
    // if any user request, it either jumps or manage error. else, it returns.
    //#warning --> pensa a mettere qualcosa ???
    //s_loader_eval_user_request();
#endif

    // if any, it either jumps or manage errors. else, it returns.
    s_loader_eval_jump_request_from_an_eproc();
    
    // now we attempt to jump to updater. 
    s_loader_attempt_jump(ee_procUpdater, EENV_MEMMAP_EUPDATER_ROMADDR);

    // nothing to do ...
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


static void s_loader_shalibs_init(void)
{
    static volatile hal_result_t reshal = hal_res_NOK_generic;
    eEprocess_t defproc = ee_procNone;

    if(ee_res_OK != shalbase_isvalid())
    {
        s_loader_manage_error(20, 80);
    }
    else
    {
        // init shalbase
        if(ee_res_OK != shalbase_init(1))
        {
            s_loader_manage_error(20, 80);
        }
    }


    if(ee_res_OK != shalpart_isvalid())
    {
        s_loader_manage_error(20, 80);
    }
    else
    {
        // init shalpart
        if(ee_res_OK != shalpart_init())
        {
            s_loader_manage_error(20, 80);
        }
        // put signature in partition table using shalpart
        if(ee_res_OK != shalpart_proc_synchronise(ee_procLoader, &s_loader_info))
        {
            s_loader_manage_error(20, 80);
        }
    }

    if(ee_res_OK != shalinfo_isvalid())
    {
        s_loader_manage_error(20, 80);
    }
    else
    {
        if(ee_res_OK != shalinfo_init())
        {
            s_loader_manage_error(20, 80);
        }
        
        s_loader_boardinfo.uniqueid = hal_arch_arm_uniqueid64_get();
        
        if(ee_res_OK != shalinfo_boardinfo_synchronise(&s_loader_boardinfo))
        {
            s_loader_manage_error(20, 80);
        }
    }
    
    
    if(ee_res_OK != shalpart_proc_def2run_get(&defproc))
    {
        // we impose that the default process is teh application
        shalpart_proc_def2run_set(ee_procApplication);
    }    

}

// used to eval the jump request coming from another process
static void s_loader_eval_jump_request_from_an_eproc(void)
{
    eEprocess_t pr = ee_procNone;
    uint32_t addr = LOADER_ADR_INVALID;

    if(ee_res_OK == shalbase_ipc_gotoproc_get(&pr))
    {
        shalbase_ipc_gotoproc_clr();
        switch(pr)
        {
            case ee_procLoader:         addr = EENV_MEMMAP_ELOADER_ROMADDR;         break;
            case ee_procUpdater:        addr = EENV_MEMMAP_EUPDATER_ROMADDR;        break;
            case ee_procApplication:    addr = EENV_MEMMAP_EAPPLICATION_ROMADDR;    break;
        }
        
        if(ee_procUpdater == pr)
        {   // we communicate to the updater to stay forever and not to jump to default after the 5 (or what) seconds
            shalbase_ipc_gotoproc_set(ee_procUpdater);
        }
        
        // attempt only to the requested process.
        s_loader_attempt_jump(pr, addr);
        // if in here ... the jump failed, thus ... i dont go to error i just go on with other choices ... 
        
        if(ee_procApplication == pr)
        {   // if in here .... we have failed to jump to application thus ... we shall stay in updater forever
            shalbase_ipc_gotoproc_set(ee_procUpdater);
            s_loader_attempt_jump(ee_procUpdater, EENV_MEMMAP_EUPDATER_ROMADDR);            
        }
        
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

    res = shalbase_ipc_volatiledata_get(data, &size, 8);

    if(ee_res_OK == res)
    {
        factor_on  = data[0];
        factor_off = data[1]; 
        
        shalbase_ipc_volatiledata_clr();  
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
     
        // attempt with the proc
        if(ee_res_OK == shalpart_proc_runaddress_get(proc, &address))
        {
            if(ee_res_OK == shalbase_system_canjump(address))
            {
                shalbase_system_jumpnow(address);
            }
        }

    }
    
    // if in here it means the we could not jump to the address of proc, thus we attempt an alternative.
 
    if(LOADER_ADR_INVALID != adr_in_case_proc_fails)
    {
        // attempt with adr_in_case_proc_fails 
        if(ee_res_OK == shalbase_system_canjump(adr_in_case_proc_fails))
        {
            shalbase_system_jumpnow(adr_in_case_proc_fails);
        }
    }

    // if i am in here we cannot jump ...
    return;
}

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
static void s_loader_eval_user_request(void)
{
    // we see if the user wants to go to the updater by pushing the button
    if(1 == s_loader_HW_INP_IsPushed())
    {
        s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_DEF_UPD);
        // if in here ... the jump failed, thus i go to error.
        s_loader_manage_error(200, 800); 
    }

    // if hw can tell none, updater, one, another etc. put a switch-case and if the chosen fails ... manage error.
}
#endif//BOARD_MVBSTM32C


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
    extern const hal_cfg_t *hal_cfgMINE;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;

    if(1 == hw_initted)
    {
        return;
    }
    else
    {
        hw_initted = 1;
    }

    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();

    hal_eeprom_init(hal_eeprom_i2c_01, NULL); 

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
    s_loader_HW_INP_Config();
#endif
    
#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
    s_eval_eeprom_erase();
#endif    
    

#if defined(BOARD_EMS001)
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

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT)
static void s_loader_HW_INP_Config(void)
{
    hal_gpio_init(s_loader_input_button.port, s_loader_input_button.pin, s_loader_input_button.dir, s_loader_input_button.speed);
}

static uint32_t s_loader_HW_INP_IsPushed(void)
{
    return((hal_gpio_valLOW == hal_gpio_getval(s_loader_input_button.port, s_loader_input_button.pin)) ? (1) : (0));
}
#endif//BOARD_MCBSTM32C

#if defined(BOARD_MCBSTM32C) && defined(ENABLE_USER_INPUT) && defined(ENABLE_ERASE_EEPROM)
static void s_eval_eeprom_erase(void)
{
    uint32_t i=0;
    static uint8_t data[EENV_STGSIZE-EENV_STGSTART] = {0};
    
    if(1 == s_loader_HW_INP_IsPushed())
    {
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
}
#endif   

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


 




