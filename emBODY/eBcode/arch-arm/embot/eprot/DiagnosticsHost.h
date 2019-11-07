
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBO_APP_DIAGNOSTICSHOST_H_
#define _EMBO_APP_DIAGNOSTICSHOST_H_

#include "embot_common.h"
#include "embot_utils.h"
#include "embot_eprot_rop.h"

namespace embot { namespace app {
           
    class DiagnosticsHost 
    {
    public:
        struct Config
        {
            bool concurrentuse {false};
            size_t ropcapacity {128};
            embot::eprot::rop::fpOnROP onrop {nullptr};
            Config() = default;
            constexpr Config(bool cu, size_t rc, embot::eprot::rop::fpOnROP o) 
               : concurrentuse(cu), ropcapacity(rc), onrop(o) {}
            bool isvalid() const { return (!onrop) && (ropcapacity >= 40); }
        };         
               
        DiagnosticsHost();  
        ~DiagnosticsHost();

        // usage: init(), then accept(ipv4, rxropframe). 

        bool init(const Config &config);
        bool initted() const;
        bool accept(const embot::eprot::IPv4 &ipv4, const embot::utils::Data &ropframedata, embot::eprot::rop::fpOnROP onrop = nullptr);  
    
    private:    
        struct Impl;
        Impl *pImpl;
    };  
    

}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
