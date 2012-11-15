/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/*
    @file       bootloader.c
    @brief      this file contains the actions of the bootloader for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/30/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h  "
#include "hal.h"
#include "hal_arch_dspic.h"
#include "eEcommon.h"
#include "eEmemorymap_dspic33.h"

#include "bootloader_sm.h"
#include "bootloader_parser.h"
#include "board_func.h"

#include "can_protocol.h"

//#include "sharinfo_loader.h"
#include "eEsharedStorage.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "bootloader.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define led_yellow              hal_led0
#define led_red                 hal_led1

#if defined(UPDTBL)
#define timer_tick_period       250000ul
#else
#define timer_tick_period       100000ul
#endif

#if defined(UPDTBL)
#define jump_to_appl_after_sec  1000ul
#else
#define jump_to_appl_after_sec  5ul
#endif

#define BOOTLOADER_UPDT_CANADDRESS   10


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
/* Constant board information for shared use the bootloader must ensure it is written in flash
   obviously it depend on board, do it is defined in boars's file. 
*/
extern const eEboardInfo_t *bl_boardinfo_default_ptr;
 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint32_t    waitforseconds;
    uint32_t    tickperiod;
} bl_cfg_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_bl_hal_init(void);
static void s_bl_can_start(void);
static void s_bl_led_flash(hal_led_t led);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint32_t s_bl_updt_canaddress = BOOTLOADER_UPDT_CANADDRESS;

static const bl_cfg_t s_bl_cfg =
{
    .waitforseconds                 = jump_to_appl_after_sec,
    .tickperiod                     = timer_tick_period
};

static uint32_t s_bl_max_ticks      = 0;


// -- application info about the bootloader or updtbootloader
#if defined(UPDTBL)
static const eEmoduleInfo_t s_bl_moduleinfo = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = 100, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2012,
                .month = 3,
                .day   = 16,
                .hour  = 12,
                .min   = 55
            }
        },
        .rom        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_ROM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_RAM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_can1,
        .name           = "updtbl-dspic33"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 1, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};
#else
static const eEmoduleInfo_t s_bl_moduleinfo = 
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
            .addr   = EE_MEMMAP_LOADER_ROM_ADDR,
            .size   = EE_MEMMAP_LOADER_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_LOADER_RAM_ADDR,
            .size   =  EE_MEMMAP_LOADER_ROM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_can1,
        .name           = "bl-dspic33"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 1, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};

#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void bl_init(void)
{
//1)--- hal: so far ... led yellow and red, eeprom on emulated flash, hal_crc0 w/ crc16 ccitt
    s_bl_hal_init();


//2) --- shared info: initialisation
    /* if deviceInfo (containing canAddress) has not been initiliazed before, 
       then in is initialized with default can address (depending on board).  
       So after this function I have can address with a valid value */
#if defined(UPDTBL)
	// dont use shared libs
#else
    //sharinfo_loader_init(bl_boardinfo_default_ptr, &s_bl_moduleinfo, BOARD_CAN_ADDRESS_DEFAULT); 
	ee_sharedstorage_init(ee_procLoader, &s_bl_moduleinfo, bl_boardinfo_default_ptr, BOARD_CAN_ADDRESS_DEFAULT	);   
#endif

//3) --- shared info: processing of command from application
#if defined(UPDTBL)
    // dont use shared libs
#else
    //sharinfo_loader_ipc_process();
	ee_sharedstorage_ipc_loader_process();
#endif


//4) --- start can: 
    s_bl_can_start();   
 
   
//5) --- flash the leds for some time ... 800 ms?
    s_bl_led_flash(led_yellow);    
 
}

extern void bl_ticktimer_start(void)
{   
    const hal_timer_cfg_t cfg =
    {
        .prescaler          = hal_timer_prescaler256,
        .countdown          = s_bl_cfg.tickperiod,
        .priority           = hal_int_priorityNONE, // no isr
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };
    
    if(0 != s_bl_max_ticks)
    {
        return;
    }

    s_bl_max_ticks = s_bl_cfg.waitforseconds * (1000ul*1000ul / s_bl_cfg.tickperiod);
    
    hal_timer_init(hal_timer1, &cfg, NULL);
    
    hal_timer_start(hal_timer1);
}

extern eEresult_t bl_ticktimer_has_just_expired(void)
{
    static hal_time_t last_read_remain_value = timer_tick_period;
    hal_time_t remain_value;

    hal_timer_remainingtime_get(hal_timer1, &remain_value);
    if(remain_value > last_read_remain_value)
    {
        last_read_remain_value = remain_value;
        return(ee_res_OK);
    }
    
    last_read_remain_value = remain_value;
    return(ee_res_NOK_generic);   
}

extern eEresult_t bl_on_ticktimer_expired(uint8_t *lastexpiry)
{
    static uint32_t ticks = 0;
    
    hal_led_toggle(led_yellow);
    
    if(ticks < s_bl_max_ticks)
    {
        ticks ++;
        if(NULL != lastexpiry)
        {
            *lastexpiry = 0;
        }
    }
    else
    {
        if(NULL != lastexpiry)
        {
            *lastexpiry = 1;
        }
    }
    
    return(ee_res_OK);
}

extern void bl_run_application(void)
{
    const eEsharinfoPartable_t *partTbl;
 
    s_bl_led_flash(led_red);
    hal_led_off(led_yellow);
    hal_led_off(led_red);
    partTbl = bl_partitiontable_get();
    hal_sys_executenowataddress(partTbl->proc_application.addr);
}

extern uint32_t bl_address_get(void)
{
    uint32_t canadr;
#if defined(UPDTBL)
    canadr = s_bl_updt_canaddress;
#else
    //sharinfo_loader_canaddress_get(&canadr);
	ee_sharedstorage_deviceinfo_canaddress_get(&canadr);
#endif
    return(canadr);
}

extern const eEsharinfoPartable_t* bl_partitiontable_get(void)
{
    const eEsharinfoPartable_t *ptab = NULL;
#if defined(UPDTBL)
    static const eEsharinfoPartable_t s_sharinfo_ptab_data =
    {
        .strg_shro_procinfo_loader  = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRO_LOADERINFO_SIZE,    .addr = EE_MEMMAP_STRG_SHRO_LOADERINFO_ADDR},
        .strg_shro_boardinfo        = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRO_BRDINFO_SIZE,       .addr = EE_MEMMAP_STRG_SHRO_BRDINFO_ADDR},
        .strg_shro_partable         = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRO_PTABLE_SIZE,        .addr = EE_MEMMAP_STRG_SHRO_PTABLE_ADDR},
        .strg_shrw_ipcdata          = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRW_IPROCCOMM_SIZE,     .addr = EE_MEMMAP_STRG_SHRW_IPROCCOMM_ADDR},
        .strg_shrw_procinfo_applic  = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRW_APPLICINFO_SIZE,    .addr = EE_MEMMAP_STRG_SHRW_APPLICINFO_ADDR},
        .strg_shrw_deviceinfo       = { .type = ee_strg_emuleeprom, .size = EE_MEMMAP_STRG_SHRW_DEVINFO_SIZE,       .addr = EE_MEMMAP_STRG_SHRW_DEVINFO_ADDR},
        .strg_priv_application      = 
                                    { 
                                        .type = ee_strg_emuleeprom, 
                                        .size = EE_MEMMAP_STRG_APPL_SIZE+EE_MEMMAP_STRG_SHRO_SIZE+EE_MEMMAP_STRG_SHRW_SIZE,
                                        .addr = EE_MEMMAP_STRG_APPL_ADDR
                                    },
        .proc_loader                =  
                                    { 
                                        .addr = EE_MEMMAP_APPLICATION_ROM_ADDR,     
                                        .size = EE_MEMMAP_APPLICATION_ROM_SIZE 
                                    },
        .proc_application           = 
                                    { 
                                        .addr = EE_MEMMAP_LOADER_ROM_ADDR,          
                                        .size =  EE_MEMMAP_LOADER_ROM_SIZE
                                    }
    };

    ptab = &s_sharinfo_ptab_data;
#else
    //sharinfo_loader_partiontable_get(&ptab);
	ee_sharedstorage_partiontable_get(&ptab);
#endif

    return(ptab);
}

extern void bl_address_set(uint32_t adr)
{
#if defined(UPDTBL)
    s_bl_updt_canaddress = adr;
#else
    //sharinfo_loader_canaddress_set(adr);
	ee_sharedstorage_deviceinfo_canaddress_set(adr);
#endif
}

extern void bl_canfilters_set(uint32_t adr)
{
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (CAN_MSG_CLASS_LOADER|adr), hal_can_frameID_std);
                                               
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 1, (CAN_MSG_CLASS_LOADER|0xF), hal_can_frameID_std);

    board_func_can_filter_polling_msg_set(0, 0x70F, 2, adr);
}

extern void bl_boardinfo_getversion(uint8_t *major, uint8_t *minor)
{
    *major = s_bl_moduleinfo.info.entity.version.major;
    *minor = s_bl_moduleinfo.info.entity.version.minor;
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_bl_hal_init(void)
{
    extern hal_cfg_t *hal_cfgBL;
    uint32_t size = 0;
    uint32_t* memory = NULL;
    
    static uint8_t flash[1024];
#define CRC16_USE_CRC16TABLE_IN_ROM
#ifndef CRC16_USE_CRC16TABLE_IN_ROM
    static uint16_t crc16table[256];
#else
    static uint16_t* crc16table = NULL;
#endif
    static hal_eeprom_cfg_t eepromcfg;
    static hal_crc_cfg_t crccfg;

    // --- hal 
    
    hal_base_memory_getsize(hal_cfgBL, &size);
    // get memory
    if(0 != size)
    {
        memory = (uint32_t*)calloc(size/4, sizeof(uint32_t));   
    }
    hal_base_initialise(hal_cfgBL, memory);

    
    hal_sys_systeminit();

    hal_led_init(led_yellow, NULL);
    hal_led_init(led_red, NULL);
    hal_led_off(led_yellow);
    hal_led_off(led_red);


    eepromcfg.flashpagebuffer   = flash;
    eepromcfg.flashpagesize     = sizeof(flash);    
    hal_eeprom_init(hal_eeprom_emulatedflash, &eepromcfg);
    
    crccfg.order                = hal_crc_order_16;
    crccfg.polynomial           = hal_crc_poly_crc16_ccitt;
    crccfg.crctblram            = crc16table; // not when hal_crc_poly_crc16_ccitt uses a precalculated const internal table
    hal_crc_init(hal_crc0, &crccfg);

}

static void s_bl_can_start(void)
{
    uint32_t canaddr;

    hal_can_cfg_t cfg =
    {
        .runmode        = hal_can_runmode_noisr_0q,
        .baudrate       = hal_can_baudrate_1mbps,
        .priorx         = hal_int_priorityNONE,
        .priotx         = hal_int_priorityNONE,
        .callback_on_rx = NULL,
        .arg_cb_rx      = NULL,
        .callback_on_tx = NULL,
        .arg_cb_tx      = NULL,
    };

s_bl_updt_canaddress = BOOTLOADER_UPDT_CANADDRESS;
#if defined(UPDTBL)
    canaddr = s_bl_updt_canaddress;
#else
    //sharinfo_loader_canaddress_get(&canaddr);
	ee_sharedstorage_deviceinfo_canaddress_get(&canaddr);
#endif
    
    hal_can_init(hal_can_port1, &cfg);
    
    bl_canfilters_set(canaddr);
    
    hal_can_enable(hal_can_port1);
}

//This function takes 600 milliseconds
static void s_bl_led_flash(hal_led_t led)
{
	uint8_t i;
	for(i=0; i<15; i++)
	{  
		hal_led_on(led);
		hal_arch_dspic_delay(20000); //20msec
        hal_led_off(led); 
        hal_arch_dspic_delay(20000); //20msec
	}
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

