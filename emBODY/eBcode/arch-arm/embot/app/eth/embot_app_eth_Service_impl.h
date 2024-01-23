

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_H_

#if 0
    
    we use this .h file to describe all data structure that is needed to implement the service. 
    in here we have:
    - use (if required) of legacy embobj C code
    - use of modern embot C++ code

    some code parts can also be inside embot_app_eth_Service.cpp   

#endif

#include "embot_app_eth_Service.h"



#include "EOtheCANdiscovery2.h"
#include "EOtheCANprotocol.h"
#include "EOvector.h"
#include "EOarray.h"

namespace embot::app::eth::service::impl {
    
    struct eOmnConfig : public embot::app::eth::Service::Config 
    {
        const eOmn_serv_configuration_t *eomnservcfg {nullptr}; 
        
        Type type() const override { return Type::eOmn_serv; }            
        const void * memory() const override { return eomnservcfg; }
                
        constexpr eOmnConfig() = default;
        constexpr eOmnConfig(const eOmn_serv_configuration_t *sc) : eomnservcfg(sc) {} 
    };

    struct ServiceOperation2
    {
        embot::app::eth::Service::OnEndOfOperation onendoperation {nullptr};  
        embot::app::eth::Service *service {nullptr};
        const embot::app::eth::Service::Config * servcfg {nullptr}; 
        
        ServiceOperation2() = default;
        
        bool isvalid() { return (nullptr != servcfg) && (nullptr != onendoperation) && (nullptr != service); }
        void load(embot::app::eth::Service *s, const embot::app::eth::Service::Config *c, embot::app::eth::Service::OnEndOfOperation eop)
        {
            onendoperation = eop;
            service = s;
            servcfg = c;
        }
        
        void onend(bool ok)
        {
            if(isvalid())
            {  // ok tells the caller that the operation was successful
               onendoperation(service, ok);
            }
        }
    };
    
    // it was used as an interface for the callback for legacy services
    
    struct ServiceOperation
    {
        using OnEnd = void (*)(Service *s, const eOmn_serv_configuration_t *sc, bool operationisok);
        const eOmn_serv_configuration_t * servcfg {nullptr};
        OnEnd onendoperation {nullptr};   
        embot::app::eth::Service *service {nullptr};
        
        ServiceOperation() = default;
        
        bool isvalid() { return (nullptr != servcfg) && (nullptr != onendoperation) && (nullptr != service); }
        void load(embot::app::eth::Service *s, const eOmn_serv_configuration_t *c, OnEnd eop)
        {
            service = s;
            servcfg = c;
            onendoperation = eop;
        }
        
        void onend(bool ok)
        {
            if(isvalid())
            {  // ok tells the caller that the operation was successful
               onendoperation(service, servcfg, ok);
            }
        }
    };    
    
    struct Paraphernalia
    {   
        bool initted {false};
        bool active {false};  
        bool activateafterverify {false};
        bool started {false};
        ServiceOperation::OnEnd onverifyactivate {nullptr};
        void *onverifyarg {nullptr};
        embot::app::eth::Service::State state {embot::app::eth::Service::State::idle};
        const eOmn_serv_configuration_t* tmpcfg {nullptr};
        eOmn_serv_configuration_t servconfig {};
            
        constexpr Paraphernalia() = default;
            
        void initialise()
        {
            clear();
            initted = true;
        }
        
        void clear()
        {
            active = activateafterverify = started = false;
            onverifyactivate = nullptr;
            onverifyarg = nullptr;
            state = embot::app::eth::Service::State::idle;
            tmpcfg = nullptr;
            std::memset(&servconfig, 0, sizeof(servconfig));
        }
    };
    
    struct CANtools
    {
        EOvector* boardproperties {nullptr};    // of eObrd_canproperties_t
        EOvector* entitydescriptor {nullptr};   // of eOcanmap_entitydescriptor_t
        EOarray* discoverytargets {nullptr};    // of eOcandiscovery_target_t
        eOcandiscovery_onstop_t ondiscoverystop {nullptr}; 
        eOcanprot_command_t command {};  
            
        constexpr CANtools() = default; 
            
        void initialise(size_t nboards, size_t nentities, size_t ntargets)
        {
            if(nullptr != boardproperties)
            {
                return;
            }
            boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), nboards, nullptr, 0, nullptr, nullptr);
            entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), nentities, nullptr, 0, nullptr, nullptr);
            discoverytargets = eo_array_New(sizeof(eOcandiscovery_target_t), ntargets, nullptr);
        } 
        
        void clear()
        {
            if(nullptr == boardproperties)
            {
                return;
            }            
            eo_vector_Clear(boardproperties);
            eo_vector_Clear(entitydescriptor);
            eo_array_Reset(discoverytargets);
        }            
    };

    struct Diagnostics
    {
        //EOtimer* reportTimer {nullptr};
        eOreltime_t reportPeriod {5*1000*1000};  
        //eOerrmanDescriptor_t errorDescriptor {};
        //eOerrmanErrorType_t errorType {eo_errortype_info};
        uint8_t errorCallbackCount {0};
        uint8_t repetitionOKcase {0};
        
        constexpr Diagnostics() = default;
        
        void initialise() {}
        void clear() {};            
    };

    struct Core
    {
        Paraphernalia paraphernalia {};
        CANtools cantools {};    
        EOarray* id32ofRegulars {nullptr};
        std::vector<eOprotID32_t> regulars {};
        Diagnostics diagnostics {};
                    
        constexpr Core() = default;  
        
        void initialise(size_t nboards, size_t nentities, size_t ntargets, size_t nregulars)
        {
            paraphernalia.initialise();
            cantools.initialise(nboards, nentities, ntargets);
            id32ofRegulars = eo_array_New(nregulars, sizeof(uint32_t), nullptr);
            regulars.reserve(nregulars);
            diagnostics.initialise();
        } 
        
        void clear()
        {
            paraphernalia.clear();
            cantools.clear();     
            eo_array_Reset(id32ofRegulars);
            regulars.clear();
            diagnostics.clear();
        }
    };

} // namespace embot::app::eth::service::impl {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
