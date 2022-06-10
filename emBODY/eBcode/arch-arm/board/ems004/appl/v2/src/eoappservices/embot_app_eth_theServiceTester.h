
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THESERVICETESTER_H_
#define __EMBOT_APP_ETH_THESERVICETESTER_H_

#include "embot_core.h"

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoManagement.h"
#if defined(USE_EMBOT_theServices)  
#else
#include "EOtheServices.h"
#endif

#include <array>
#include <memory>


namespace embot { namespace app { namespace eth {
      
    class theServiceTester
    {
    public:
        static theServiceTester& getInstance();
                
    public:
    
        enum class Caller : std::uint8_t { none, threadCFG, threadRUN };
             
        struct Config
        {
            static constexpr size_t maxregulars {8};
            eOmn_serv_category_t category {eomn_serv_category_ft};
            const eOmn_serv_configuration_t *servconfig {nullptr};
            embot::core::fpWorker setconfig {nullptr};
            embot::core::fpWorker txstart {nullptr};
            size_t sizeofregulars {0};
            std::array<eOprotID32_t, maxregulars> regulars {0, 0, 0, 0, 0, 0, 0, 0};
            constexpr Config() = default;
            constexpr Config(eOmn_serv_category_t c, const eOmn_serv_configuration_t *sc, 
                             embot::core::fpWorker scfg, embot::core::fpWorker txst, 
                             size_t sr, std::array<eOprotID32_t, maxregulars> r) 
                             : category(c), servconfig(sc), setconfig(scfg), txstart(txst), sizeofregulars(sr), regulars(r) {} 
        };
        
        bool initialise(const Config &config);  
        bool tick(Caller caller);        
           
    private:
        theServiceTester(); 
        ~theServiceTester(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    };       
    

}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
