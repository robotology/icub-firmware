
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


//extern const hal_cfg_t     s_hal_cfg;



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




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

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

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



