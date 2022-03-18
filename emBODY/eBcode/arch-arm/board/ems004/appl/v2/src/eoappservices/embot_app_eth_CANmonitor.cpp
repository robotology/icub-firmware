
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_CANmonitor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "EOtheErrorManager.h"
#include "EoError.h"
#include "embot_core_binary.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::eth::CANmonitor::Impl
{   
    Config _config {};
    bool active {false};
    
    State state {State::OK}; 
    
    embot::core::Time timeoflastcheck {0};
    embot::core::Time timeoflastreport {0};
    embot::core::Time timeofdisappearance {0};
    
    MAP boards2touch {0, 0};

    bool transmissionisactive {false};
    bool allboardsarealive {false};    
    eOerrmanDescriptor_t errdes = {0};
    
    Impl() = default;

    
    ~Impl()
    {

    }
    
    bool configure(const Config &cfg);
    bool add(eObrd_canlocation_t loc);
    bool rem(eObrd_canlocation_t loc);
    bool setcheckperiod(embot::core::relTime r);
    
    bool start();
    bool stop();
    bool tick();
    bool touch(eObrd_canlocation_t loc);
    
    static constexpr bool enablelog {false};
    void log(const std::string &str)
    {
        if(enablelog)
        {
            alwayslog(str);  
        }            
    }
    
    void alwayslog(const std::string &str)
    {
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print(str + " @ " + tf.to_string());         
    }
    
};

bool embot::app::eth::CANmonitor::Impl::configure(const Config &cfg)
{
//    log("configure()");
    
    _config = cfg;
    active = false;
    
    stop();
    
    errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress      = _config.servicecategory;
    errdes.par16              = 0;
    errdes.par64              = 0;
       
    return true;
}


bool embot::app::eth::CANmonitor::Impl::start()
{
//    log("start()");
    // i start only if ...
    // _config.periodofcheck is != 0;
    // and i start also if _config.target is empty, 
    
    if(0 == _config.periodofcheck)
    {
        return false;
    }

    boards2touch = _config.target;
    state = State::OK;
    allboardsarealive = true;
    active = true;
    timeoflastcheck = timeoflastreport = embot::core::now();
    return true;
}

bool embot::app::eth::CANmonitor::Impl::stop()
{  
//    log("stop()");  
    boards2touch.clear();
    state = State::OK;
    allboardsarealive = false;
    active = false;
    timeoflastcheck = timeoflastreport = 0;
    return true;
}

bool embot::app::eth::CANmonitor::Impl::tick()
{
    if(false == active)       
    {
        return true;
    }
    
    bool checknow = false;
    bool regularreportnow = false;
    bool forcereport = false;
    embot::core::Time timenow = embot::core::now();
    
    if((timeoflastcheck + _config.periodofcheck) < timenow)
    {
        checknow = true;
    }
    
    if((timeoflastreport + _config.periodofreport) < timenow)
    {
        regularreportnow = true;
    }  
        
    MAP boards2report {};
    
    if(true == checknow)
    {
        timeoflastcheck = timenow;
        
        bool allboardstouched = boards2touch.empty();
        
        if(allboardsarealive)
        {
            if(allboardstouched)
            {   // it is the case in which everything goes fine: the boards keeps on transmitting 
                state = State::OK;
                boards2report = _config.target; // if we report we report the target
                forcereport = false; // we dont force a report
                boards2touch = _config.target; // re-init the boards2touch for new touches
                
                allboardsarealive = true;
            }
            else
            {   // we have just lost contact w/ some boards
                state = State::justLOST;
                timeofdisappearance = timenow;
                boards2report = boards2touch; // if we report we report what we miss
                forcereport = true; // and we force a report
                boards2touch = _config.target; // re-init the boards2touch for new touches
                
                allboardsarealive = false; // there are some boards which are not alive anymore
            }
        }
        else
        {
            if(allboardstouched)
            {   // we have just recovered from a MISSING situation
                state = State::justFOUND;
                boards2report = _config.target; // if we report we report the target
                forcereport = true; // and we force a report
                boards2touch = _config.target; // re-init the boards2touch for new touches
                
                allboardsarealive = true; // all alive again
            }
            else
            {   // sad story: we dont have recovered yet. some boards are still missing
                state = State::stillLOST;
                boards2report = boards2touch; // if we report we report what we miss
                forcereport = false; // we dont force a report
                boards2touch = _config.target; // re-init the boards2touch for new touches
                
                allboardsarealive = false; // there are some boards which are not alive anymore               
            }            
        }
    }
         

    // is the report enabled given the current state and the configured mode? and is it required a forced report or a regular report?
    if((true == reportenabled(state, _config.reportmode)) && ((true == forcereport) || (true == regularreportnow)))
    {
//        log("tick(reporting)");
        timeoflastreport = timenow;
        
        switch(state)
        {
            case State::OK:
            {
//                alwayslog("OK");
                errdes.par16 = 0;
                errdes.par64 = boards2report.getcompact();        
                errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_monitor_regularcontact);              
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, _config.ownername, &errdes); 
            } break;
            
            case State::justLOST:
            {
//                alwayslog("jLOST");
                errdes.par16 = 0;
                errdes.par64 = boards2report.getcompact();        
                errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_monitor_lostcontact);              
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, _config.ownername, &errdes);                
            } break;                

            case State::justFOUND:
            {
//                alwayslog("JFOUND");
                uint64_t mspassed = (timenow-timeofdisappearance)/1000;
                errdes.par16 = 0;
                errdes.par64 = (mspassed << 32) | boards2report.getcompact();        
                errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_monitor_retrievedcontact);              
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, _config.ownername, &errdes);                
            } break; 

            case State::stillLOST:
            {
//                alwayslog("sLOST");
                uint64_t mspassed = (timenow-timeofdisappearance)/1000;
                errdes.par16 = 0;
                errdes.par64 = (mspassed << 32) | boards2report.getcompact();        
                errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_monitor_stillnocontact);              
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, _config.ownername, &errdes);
            } break;                  
        }           
    }
            
    
    return true;
}


bool embot::app::eth::CANmonitor::Impl::touch(eObrd_canlocation_t loc)
{
    if(false == active)       
    {
        return true;
    }
    
    if(_config.target.check(loc))
    {
        boards2touch.clear(loc);
    }
    
    return true;
}


bool embot::app::eth::CANmonitor::Impl::add(eObrd_canlocation_t loc)
{
    _config.target.set(loc);
    return true;
}

bool embot::app::eth::CANmonitor::Impl::rem(eObrd_canlocation_t loc)
{
    _config.target.clear(loc);
    return true;
}


bool embot::app::eth::CANmonitor::Impl::setcheckperiod(embot::core::relTime r)
{
    _config.periodofcheck = r;
    return true;
}

embot::app::eth::CANmonitor::CANmonitor()
: pImpl(new Impl())
{ 

}

embot::app::eth::CANmonitor::~CANmonitor()
{   
    delete pImpl;
}

bool embot::app::eth::CANmonitor::configure(const Config &cfg)
{
    return pImpl->configure(cfg);
}

bool embot::app::eth::CANmonitor::start()
{
    return pImpl->start();
}

bool embot::app::eth::CANmonitor::stop()
{
    return pImpl->stop();
}

bool embot::app::eth::CANmonitor::tick()
{
    return pImpl->tick();
}

bool embot::app::eth::CANmonitor::touch(eObrd_canlocation_t loc)
{
    return pImpl->touch(loc);
}

bool embot::app::eth::CANmonitor::add(eObrd_canlocation_t loc)
{
    return pImpl->add(loc);
}

bool embot::app::eth::CANmonitor::rem(eObrd_canlocation_t loc)
{
    return pImpl->rem(loc);
}

bool embot::app::eth::CANmonitor::setcheckperiod(embot::core::relTime r)
{
    return pImpl->setcheckperiod(r);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


