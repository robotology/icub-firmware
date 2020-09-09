
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"

#include "hal.h"

#include "EOMtheSystem.h"

extern const eOmsystem_cfg_t* psyscfg;;

static uint8_t s_initted = 0;

extern stm32hal_res_t stm32hal_init(const stm32hal_config_t *cfg)
{
    if(1 == s_initted)
    {
        return stm32hal_res_OK;
    }
    
    hal_core_init(psyscfg->halcfg);
    hal_core_start();

    if(0 != psyscfg->codespaceoffset)
    {
        hal_sys_vectortable_relocate(psyscfg->codespaceoffset);
    }  
    
    s_initted = 1;
    
    return stm32hal_res_OK;
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



