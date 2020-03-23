
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_cif_core.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"


#warning MEMO: do we need to call embot::os::init() instead?
#warning MEMO: do we need to redefine new / delete / new[] / delete[] as well?   
    
extern void embot_cif_core_Init(const embot_cif_core_Config *cfg)
{
    if((nullptr != cfg) && (nullptr != cfg->timeget))
    {
        embot::core::init({{cfg->timeinit, cfg->timeget}});
    }
}
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

