
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"

static uint8_t s_initted = 0;

void hal2_init();

extern stm32hal_res_t stm32hal_init(const stm32hal_config_t *cfg)
{
    if(1 == s_initted)
    {
        return stm32hal_res_OK;
    }
    
    hal2_init();
    
    s_initted = 1;
    
    return stm32hal_res_OK;
}


// -
// - hal 2 porting
// -



#include "embot_core.h"

#ifndef HAL_SYS_CFG_STACKSIZE
 #define HAL_SYS_CFG_STACKSIZE      0x00003000
#endif


#ifndef HAL_SYS_CFG_HEAPSIZE
 #define HAL_SYS_CFG_HEAPSIZE      0x0001FC00
#endif

static void on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);

static void* myheap_new(uint32_t size);
static void myheap_delete(void* mem);

constexpr hal_core_cfg_t hal_cfg 
{
    {
        {
            on_fatalerror,
            nullptr,
            nullptr,
            myheap_new,
            myheap_delete
        } // extfn
    }, // hal_base_cfg_t      basecfg;    /**< the configuration of the base module */
    {
        0
    }, // hal_mpu_cfg_t       mpucfg;     /**< the configuration of the mpu module */
    {
        0
    }, //hal_flash_cfg_t     flashcfg;   /**< the confoguration of the flash module */
    {
        HAL_SYS_CFG_STACKSIZE,
        HAL_SYS_CFG_HEAPSIZE,
    } //hal_sys_cfg_t       syscfg;     /**< the configuration of the sys module */    
};

static void on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    errorcode = errorcode;
    if(nullptr != errormsg)
    {
        hal_trace_puts(errormsg);
    }

    if(hal_fatalerror_warning == errorcode)
    {
        return;
    }
    

    for(;;)
    {
        embot::core::wait(1000);
//        embot::hw::led::toggle();
    }
}

static void* myheap_new(uint32_t size)
{
    return calloc(size, 1);   
}

static void myheap_delete(void* mem)
{
    free(mem);
}


const hal_core_cfg_t * getconfig()
{
    return &hal_cfg;
}

void hal2_init()
{
    hal_core_init(getconfig());
    hal_core_start();    
}
    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



