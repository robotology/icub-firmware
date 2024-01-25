

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
        bool _initted {false};
        
        bool _active {false};  
        bool _started {false};
        
        embot::app::eth::Service::State _state {embot::app::eth::Service::State::idle};
        eOmn_serv_configuration_t serviceconfig {};  
            
        uint8_t capacityOfRegulars {0};    
        EOarray* id32ofRegulars {nullptr};
        
                    
        constexpr Core() = default; 

        bool initted() const { return _initted; }        
        
        void initialise(uint8_t maxregulars)
        {          
            if(true == _initted)
            {
                return;
            }
            
            capacityOfRegulars = maxregulars;
            id32ofRegulars = eo_array_New(maxregulars, sizeof(uint32_t), nullptr);
            
            clear();
            
            _initted = true;
        } 
        
        void clear()
        {
            _active = _started = false;
            _state = embot::app::eth::Service::State::idle;
            std::memset(&serviceconfig, 0, sizeof(serviceconfig));            
            eo_array_Reset(id32ofRegulars);
        }
        
        void state(embot::app::eth::Service::State s)
        {
            _state = s;
            _active = state2active(s);
            _started = state2started(s);
        }

        bool active() const
        {
            return _active;
        }

        bool started() const
        {
            return _started;
        }
        
        embot::app::eth::Service::State state() const
        {
            return _state;
        }
        
        static bool state2active(embot::app::eth::Service::State s)
        {
            return (s == embot::app::eth::Service::State::activated) || (s == embot::app::eth::Service::State::started);
        }
        
        static bool state2started(embot::app::eth::Service::State s)
        {
            return (s == embot::app::eth::Service::State::started);
        }        
    };

} // namespace embot::app::eth::service::impl {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
