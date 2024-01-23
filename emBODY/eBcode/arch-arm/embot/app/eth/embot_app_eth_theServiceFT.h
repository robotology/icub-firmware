

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THESERVICEFT_H_
#define __EMBOT_APP_ETH_THESERVICEFT_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_app_eth_Service.h"

#include "EoCommon.h"
#include "EoProtocol.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
    
    class theServiceFT : Service
    {
    public:
        static theServiceFT& getInstance();
                
    public:
                        
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return (666 == tbd) ? false : true; }
        }; 
        
        eOresult_t initialise(const Config &config);  
        
        Service* service();
        
        // interface of Service
        Category category() const override;
        Type type() const override;
        State state() const override;
        void set(State s) override;
        
        bool verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate) override;
        bool deactivate() override;
        bool start() override;
        bool stop() override;
        bool setregulars(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem) override;
        bool tick(bool resetstatus = false) override;
        bool report() override;
        bool process(const DescriptorCANframe &canframedescriptor) override;
        bool process(const DescriptorROP &ropdescriptor) override;

    private:        
                     
    private:
        theServiceFT(); 
        ~theServiceFT(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
