
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_eth_Service_impl_mc_AGENTs.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceMC.h"


#include "embot_app_eth_Service_impl.h"

#include "EOtheCANprotocol.h"
#include "EOtheCANmapping.h"


#include "embot_app_eth_theErrorManager.h"
#include "EoError.h"
#include "EOtheErrorManager.h"

#include "embot_os_theScheduler.h"




namespace embot::app::eth::service::impl::mc {    

    // struct AGENTs
    
    bool AGENTs::supported(const eOmn_serv_configuration_t *sc)
    {
        return (nullptr == sc) ? false : supported(static_cast<embot::app::eth::Service::Type>(sc->type));
    }         
        
    void AGENTs::initialise(embot::app::eth::service::impl::Core *p)
    {
        _p2core = p;
        
        _mcfoc.initialise(p);
        _mcadvfoc.initialise(p);        
                
        _theactiveagent = &_mcnone;
            
        _mcservicetype = embot::app::eth::Service::Type::none;
        for(size_t i=0; i<maxjomos; i++)
        {
            _joints[i] = reinterpret_cast<eOmc_joint_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i));
            _motors[i] = reinterpret_cast<eOmc_motor_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i));
        }             
    }
    
    void AGENTs::clear()
    {
        _theactiveagent = &_mcnone;
        _mcservicetype = embot::app::eth::Service::Type::none;        
    }
    
    bool AGENTs::load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc)
    {
        bool r {true};
                
        _mcservicetype = static_cast<embot::app::eth::Service::Type>(sc->type);
        
        switch(_mcservicetype)
        {
            case embot::app::eth::Service::Type::MC_foc:
            {
                _theactiveagent = &_mcfoc;             
            } break;

            case embot::app::eth::Service::Type::MC_advfoc:
            {
                _theactiveagent = &_mcadvfoc;                
            } break; 

            default:
            {
                _theactiveagent = &_mcnone;
            } break;
        }
        
        r = _theactiveagent->load(serv, sc);        
                
        return r;
    }
 
    
    size_t AGENTs::numberofjomos() const
    {
        return _theactiveagent->numberofjomos();
    }
    
    
    eOmc_joint_t * AGENTs::joint(size_t n) const
    {
        return (n < _theactiveagent->numberofjomos()) ? (_joints[n]) : (nullptr);        
    }
    
    eOmc_motor_t * AGENTs::motor(size_t n) const
    {
        return (n < _theactiveagent->numberofjomos()) ? (_motors[n]) : (nullptr);
    }    

    embot::app::eth::Service::Type AGENTs::type() const
    {
        return _mcservicetype;
    }        
 
   
    bool AGENTs::verifyactivate(embot::app::eth::Service::OnEndOfOperation onend)
    {
        constexpr bool andactivate {true};
        return _theactiveagent->verify(onend, andactivate);        
    }   

    bool AGENTs::forceidle()
    {
        return _theactiveagent->forceidle();
    }
    
    bool AGENTs::deactivate()
    {
        return _theactiveagent->deactivate(); 
    }  
 
    
    bool AGENTs::stop()
    {
        return _theactiveagent->stop();        
    }
    
    
    bool AGENTs::start()
    {        
        return _theactiveagent->start();
    }

    IFagent * AGENTs::activeagent() 
    { 
        return _theactiveagent; 
    }    


} // namespace embot::app::eth {  




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


