
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_THECOMM_H_
#define __EMBOT_APP_BLDC_THECOMM_H_


#include <memory>
#include <vector>
#include "embot_prot_can.h"
#include "embot_app_bldc_COMM.h"
#include "embot_app_application_CANagentCORE.h"
#include "embot_os_common.h"

namespace embot { namespace app { namespace bldc {
    
    
    class theCOMM : public embot::app::bldc::COMM
    {
    public:
        
        static theCOMM& getInstance();
    
        struct Config
        {
            embot::os::Priority priority {embot::os::Priority::high44};
            uint16_t tCOMM_stacksize {4*1024};
            embot::core::relTime tCOMM_timeout {50*embot::core::time1millisec};
            embot::app::application::CANagentCORE *canagentcore {nullptr};             
            uint8_t tCOMMmaxINPcanframes {32};
            uint8_t tCOMMmaxOUTcanframes {32};  
                      
          
            constexpr Config() = default;
            constexpr Config(embot::os::Priority p, uint16_t st, embot::core::relTime to, embot::app::application::CANagentCORE *ca, uint8_t r, uint8_t t) 
                : priority(p), tCOMM_stacksize(st), tCOMM_timeout(to), canagentcore(ca), tCOMMmaxINPcanframes(r), tCOMMmaxOUTcanframes(t)
                {}
            constexpr bool isvalid() const {
                bool notok = (tCOMM_stacksize < 1024) || (tCOMM_timeout < embot::core::time1millisec) ||
                             // it may be nullptr ... || (nullptr == canagentcore)
                             (tCOMMmaxINPcanframes == 0) || (tCOMMmaxOUTcanframes == 0);                    
                return !notok; 
            }
        };
        

        static constexpr size_t MAXcapacity = 999;   
                
        bool initialise(const Config &config);   


//        bool subscribe(Direction dir, const embot::os::Action &action);  

        bool add(const embot::prot::can::Frame &frame) override;
        bool add(const std::vector<embot::prot::can::Frame> &frames) override;      

        // retrieval methods
        bool get(size_t &remaining, embot::prot::can::Frame &frame) override;
        bool get(size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve = COMM::MAXcapacity) override;          
        
        
    private:
        theCOMM(); 
        ~theCOMM(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
}}} // namespace embot { namespace app { namespace bldc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
