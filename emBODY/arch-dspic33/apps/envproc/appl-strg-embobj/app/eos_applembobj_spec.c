
/* @file       eos_applembobj_spec.c
	@brief      This file keeps the module info of the updatre
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"

#include "eEcommon.h"
#include "eEmemorymap.h"

#include "hal.h"

#include "EOStheSystem.h"

#include "eos_applembobj_info.h"


#include "eEsharedStorage.h"
#include "eEprivateStorage.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eos_applembobj_spec.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eos_applembobj_spec_user_manage_error(void);

static void s_eos_applembobj_spec_user_initstorages(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
	uint32_t	dummy;
	uint16_t	dimmy;
	uint16_t	demmy;
} myPrivateData_t;


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// -- application info about the application

static const eEmoduleInfo_t s_moduleinfo_application = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
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
        .name           = "appl-dspic33"
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


// default private data

static const myPrivateData_t s_myprivate_data_default =
{
	.dummy	= 0x12345678,
    .dimmy	= 0x1111,
	.demmy	= 0xabcd
};

static myPrivateData_t* s_myprivate_data_ptr = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eos_applembobj_spec_hal_init(void)
{
    extern const hal_cfg_t* hal_cfgAPP;
    uint32_t sizememory;
    uint32_t* memory = NULL;

    // how much memory do we need?
    hal_base_memory_getsize(hal_cfgAPP, &sizememory);
    
    if(0 != sizememory)
    {
        memory = (uint32_t*)calloc(sizememory/4, sizeof(uint32_t));   
    }
    
    // give the memory
    hal_base_initialise(hal_cfgAPP, memory);
}


extern void eos_applembobj_spec_sys_on_tick(void)
{
    static uint32_t tick_num = 0;

    if(1000 == ++tick_num)
    {
        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_1000TICKS);
        tick_num = 0;
    }

}

extern void eos_applembobj_spec_errman_on_error(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
//    hal_can_frame_t frame;

    if( (eo_errortype_weak == errtype) || (eo_errortype_fatal == errtype) )
    {
        //notify error    
        //frame.id = my_6sg_cfg.gen_ee_data.board_address;
        //frame.id_type = hal_can_frameID_std;
        //frame.frame_type = hal_can_frame_data;
        //frame.size = 5;
        //memcpy(frame.data, s_error_str, 5);
        // 
        //hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
        
        
        for(;;);
          
    }
    else // eo_errortype_info or eo_errortype_warning
    {
        return;
    }
}



extern void eos_applembobj_spec_foop_on_event(uint32_t evtpos, eOevent_t evtmsk)
{

    switch(evtpos) 
    {

        case 0: 
        {

        } break;

        case 1:  
        {
            
        } break;

        case 2:  
        {
            
        } break;


        case 3: 
        {
            
        } break;

        case 4: 
        {

            //hal_led_toggle(led_yellow);

        } break;
        
        case 10:            // EVT_1000TICKS
        {
            #warning --> add processing of 1000 ticks ... toggles a led.
			hal_led_toggle(hal_led1);
        } break;
        
        default:
        {
        
        } break;
        
    }
    
}


extern void eos_applembobj_spec_user_init(void)
{
    #warning --> add in here whatever is needed

	// we init the led and we put it on.
	hal_led_init(hal_led1, 0);

	hal_led_on(hal_led1);


	s_eos_applembobj_spec_user_initstorages();


}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eos_applembobj_spec_user_manage_error(void)
{
	for(;;);
}


static uint32_t s_canaddr = 7;

static void s_eos_applembobj_spec_user_initstorages(void)
{
	#warning --> add in here initialisation of shared storage and of private storage.

	// 1. init eeprom and crc.

    static uint8_t flash[1024];
#define CRC16_USE_CRC16TABLE_IN_ROM
#ifndef CRC16_USE_CRC16TABLE_IN_ROM
    static uint16_t crc16table[256];
#else
    static uint16_t* crc16table = NULL;
#endif
    static hal_eeprom_cfg_t eepromcfg;
    static hal_crc_cfg_t crccfg;

    ee_privatestorage_cfg_t privstrgcfg;

    eepromcfg.flashpagebuffer   = flash;
    eepromcfg.flashpagesize     = sizeof(flash);    
    hal_eeprom_init(hal_eeprom_emulatedflash, &eepromcfg);
    
    crccfg.order                = hal_crc_order_16;
    crccfg.polynomial           = hal_crc_poly_crc16_ccitt;
    crccfg.crctblram            = crc16table; // not when hal_crc_poly_crc16_ccitt uses a precalculated const internal table
    hal_crc_init(hal_crc0, &crccfg);

	// 2. init shared storage

	if(ee_res_OK != ee_sharedstorage_init(ee_procApplication, &s_moduleinfo_application, NULL, 0))
    {
        s_eos_applembobj_spec_user_manage_error();
    }


	// 3. get something from shared storage. the can address for instance.

	if(ee_res_OK != ee_sharedstorage_deviceinfo_canaddress_get(&s_canaddr))
    {
         s_eos_applembobj_spec_user_manage_error(); 
    }

	// 4. init private storage

	privstrgcfg.size 	= sizeof(myPrivateData_t);
    privstrgcfg.defval	= &s_myprivate_data_default;

	ee_privatestorage_init(&privstrgcfg, (void**)&s_myprivate_data_ptr);

	 
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



