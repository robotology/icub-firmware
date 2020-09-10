
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_sys.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal_trace.h"


namespace embot { namespace hw { namespace sys {
       

    int puts(const std::string &str) 
    {
        return puts(str.c_str());
    }
    
    int puts(const char* str) 
    {    
        return hal_trace_puts(str);  
    }

    

}}} // namespace embot { namespace hw { namespace sys { 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

