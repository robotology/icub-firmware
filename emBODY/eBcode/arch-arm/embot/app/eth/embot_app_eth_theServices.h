
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
                
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return 0 != tbd; }
        };
        
        // tells if a ID32 is relevant to the calling service
        using fpIsID32relevant = bool (*)(eOprotID32_t id32);
               
        bool initialise(const Config &config); 
        
        bool load(embot::app::eth::Service *s);
        embot::app::eth::Service* get(embot::app::eth::Service::Category cat);
        embot::app::eth::Service* get(const eOmn_service_cmmnds_command_t *command);
        
        bool process(eOmn_service_cmmnds_command_t *command);
        
        bool stop(); // stop and deactivate every service
        bool tick(); // tick all activated services        

        // setREGULARS() is called by a given Service object which gives its own id32ofregulars than MUST be non nullptr.
        // id32ofregulars can be either nullptr or not
        // - id32ofregulars is nullptr: setregulars() makes sure that the ID32 values inside id32ofregulars are removed from the transceiver 
        //   and then it clears id32ofregulars
        // - id32ofregulars is not nullptr: setregulars() makes sure that id32ofregulars contains ONLY what inside arrayofid32 and updates the transceiver.        
        bool setREGULARS(EOarray* id32ofregulars, const eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem);   
        
        // as a better alternative ... these are better methods
        bool add(const std::vector<eOprotID32_t> &id32s, fpIsID32relevant fpISOK, uint8_t &added);
        bool rem(const std::vector<eOprotID32_t> &id32s, uint8_t &removed);
        
        bool load(const eOprot_EPcfg_t &epcfg);         
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
