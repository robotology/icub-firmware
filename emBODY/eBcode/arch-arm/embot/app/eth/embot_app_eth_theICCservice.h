

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCSERVICE_H_
#define __EMBOT_APP_ETH_THEICCSERVICE_H_

#include "embot_core.h"
#include "embot_app_eth.h"


#include "embot_app_eth_mc_messaging.h"

namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theICCservice` is responsible to ... 
    
#endif
    
        
    class theICCservice
    {
    public:
        static theICCservice& getInstance();
                

        bool initialise();
    
        bool add(const embot::app::eth::mc::messaging::Location &des, const embot::prot::can::Frame &frame);
       
        // IFsomething
//        bool Verify(const Config &config, const OnEndOfOperation &onverify, bool activateafterverify) override;         
//        bool Activate(const Config &config) override;
//        bool Deactivate() override;
//        bool SendReport() override;         
//        bool StartReading() override;                          
//        bool IsReadingAvailable() override;       
//        bool Read(uint8_t jomo, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary) override;
//        bool Diagnostics_Tick() override;
//        embot::app::eth::encoder::v1::Type GetType(const embot::app::eth::encoder::v1::Target &target) override;
//        bool Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler) override;
    

    private:
        theICCservice(); 
        ~theICCservice(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
