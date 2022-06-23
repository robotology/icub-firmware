
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THESERVICES_H_
#define __EMBOT_APP_ETH_THESERVICES_H_

#include "embot_core.h"

#include "embot_app_eth_Service.h"

#include "EoCommon.h"
#include "EoProtocol.h"

#include <array>
#include <memory>


namespace embot { namespace app { namespace eth {
      
    class theServices
    {
    public:
        static theServices& getInstance();
                
    public:
    
//        enum class Caller : std::uint8_t { none, threadCFG, threadRUN };
             
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return 0 != tbd; }
        };
        
        
        bool initialise(const Config &config); 
        
        bool load(embot::app::eth::Service *s);
        embot::app::eth::Service* get(embot::app::eth::Service::Category cat);
        embot::app::eth::Service* get(const eOmn_service_cmmnds_command_t *command);
        
        bool process(eOmn_service_cmmnds_command_t *command);
        
        bool stop(); // stop and deactivate every service
        bool tick(); // tick all activated services        

        using fpIsID32relevant = bool (*)(uint32_t id32);
        bool setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem);
            
        bool synch(Service::Category category, Service::State state);  
        
    private:
        theServices(); 
        ~theServices(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       
    

}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
