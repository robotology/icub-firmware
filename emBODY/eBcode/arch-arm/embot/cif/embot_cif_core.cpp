
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


embot_cif_core_Config config {};
    
int pprint(const std::string &str)
{
    if(nullptr != config.print)
    {
        return config.print(str.c_str());
    }
    return 0;
}
    
extern void embot_cif_core_Init(const embot_cif_core_Config *cfg)
{
    if((nullptr != cfg) && (nullptr != cfg->timeget))
    {
        config = *cfg;
        embot::core::init({{config.timeinit, config.timeget}, {pprint}});
    }
}
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

