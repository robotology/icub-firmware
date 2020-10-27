
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_sys.h"

#include "hal_trace.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace bsp {
    
    
    bool init(const embot::hw::Config &config)
    {          
        return true;
    }
    
    int print(const std::string &str)
    {
        return hal_trace_puts(str.c_str());
    }    

}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

