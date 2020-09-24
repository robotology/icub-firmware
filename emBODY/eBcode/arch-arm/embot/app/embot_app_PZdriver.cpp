
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_PZdriver.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

// ...

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


     
struct embot::app::PZdriver::Impl
{  
    Config _config {};    

    uint32_t _somevalues[4] {};
    
    Impl() 
    {
        // ....
    }
    
    ~Impl()
    {
        // ...
    }
    
    bool init(const Config &config)
    {
        _config = config;
        // ...
        
        //_config.pzlut->get(0, _somevalues); 
        
        return true;
    } 
    
    bool start()
    {
        // ...
        
        return true;
    } 
    
    bool stop()
    {
        // ...
        
        return true;
    }     

    bool set(const setpoint_t &value)
    {
        // ...
        
        return true;
    }     
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

    


embot::app::PZdriver::PZdriver()
: pImpl(new Impl)
{ 

}

embot::app::PZdriver::~PZdriver()
{   
    delete pImpl;
}

bool embot::app::PZdriver::init(const Config &config)
{
    return pImpl->init(config);
}


bool embot::app::PZdriver::start()
{
    return pImpl->start();
}

bool embot::app::PZdriver::stop()
{
    return pImpl->stop();
}


bool embot::app::PZdriver::set(const setpoint_t &value)
{
    return pImpl->set(value);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


