
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_THECTRL_H_
#define __EMBOT_APP_BLDC_THECTRL_H_


#include <memory>
#include <vector>
#include "embot_app_bldc_COMM.h"
#include "embot_app_application_theCANagentCORE.h"
#include "embot_os_common.h"

namespace embot { namespace app { namespace bldc {
           
    class theCTRL
    {
    public:
        
        static theCTRL& getInstance();
 
        using OnStartup = void (*)(embot::prot::can::Address adr);    
        using OnTick = void (*)(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output);
    
        struct Config
        {            
            embot::os::Priority priority {embot::os::Priority::high44};
            uint16_t tCTRL_stacksize {6*1024};
            embot::core::relTime tCTRL_period {1*embot::core::time1millisec};  
            
            embot::app::application::CANagentCORE *canagentcore {nullptr}; 
            embot::app::bldc::COMM *comm {nullptr};
            
            OnStartup onstartup {nullptr};
            OnTick ontick {nullptr};
                        
          
            constexpr Config() = default;
            constexpr Config(embot::os::Priority p, uint16_t st, embot::core::relTime pe,
                             embot::app::application::CANagentCORE *ca, embot::app::bldc::COMM *co, 
                             OnStartup ost, OnTick ot) 
                            :   priority(p), tCTRL_stacksize(st), tCTRL_period(pe),
                                canagentcore(ca), comm(co), 
                                onstartup(ost), ontick(ot)             
                    {}
            constexpr bool isvalid() const {
                bool notok = (tCTRL_stacksize < 1024) || (tCTRL_period < 500) ||
                             // they may be nullptr ... (nullptr == comm) || (nullptr == canagentcore) ||
                             (nullptr == ontick);                  
                return !notok; 
            }
        };            
                  

        static constexpr size_t MAXcapacity = 999;   
                
        bool initialise(const Config &config);   

        bool subscribe(embot::app::bldc::COMM *comm);        

        
    private:
        theCTRL(); 
        ~theCTRL(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
}}} // namespace embot { namespace app { namespace bldc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
