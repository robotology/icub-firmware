
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theMSGbroker.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"

#include <array>

#include "embot_os_rtos.h"
#include "embot_os_Thread.h"
#include "embot_os_Action.h"


// --------------------------------------------------------------------------------------------------------------------
// - defines
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::bldc {



} // end of namespace


struct embot::app::bldc::theMSGbroker::Impl
{
    Impl() = default;  
    
    embot::os::Action _onTX {}; 
    embot::os::Action _onRX {}; 
        
    Config _config {};
    bool _initted {false};
    
    embot::os::rtos::mutex_t *_rxm {nullptr};
    std::vector<embot::app::bldc::MSG> _rxMSGvector {};
    embot::os::rtos::mutex_t *_txm {nullptr};    
    std::vector<embot::app::bldc::MSG> _txMSGvector {};     
        
    // the initialization code
    bool initialise(Config config);       

    size_t size(Direction dir) const;

    bool add(Direction dir, const embot::app::bldc::MSG &msg);
    bool add(Direction dir, const std::vector<embot::app::bldc::MSG> &msgs);        

    bool rem(Direction dir, size_t &remaining, embot::app::bldc::MSG &msg);
    bool rem(Direction dir, size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve);  
        
    bool subscribe(Direction dir, const embot::os::Action &action);
};

      
bool embot::app::bldc::theMSGbroker::Impl::initialise(const Config config)
{
    if(true == _initted)
    {
        return true;
    }
    
    embot::core::print("embot::app::bldc::theMSGbroker::Impl::initialise()");
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    _rxm = embot::os::rtos::mutex_new();
    _txm = embot::os::rtos::mutex_new();
    _rxMSGvector.reserve(_config.rxCapacity);
    _txMSGvector.reserve(_config.txCapacity);
    
    _initted = true;
    return _initted;
}


size_t embot::app::bldc::theMSGbroker::Impl::size(Direction dir) const
{
    size_t r {0};
    
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        r = _rxMSGvector.size();
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        r = _txMSGvector.size();
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::add(Direction dir, const embot::app::bldc::MSG &msg)
{
    bool r {false};
    
    constexpr size_t s = 1; // surely 1 is compatible w/ capacity
        
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)        
        if((_rxMSGvector.size() + s) > _config.rxCapacity)
        {
            _rxMSGvector.erase(_rxMSGvector.begin());
        }
#endif
        _rxMSGvector.push_back(msg);
        _onRX.execute();
        r = true;
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)        
        if((_txMSGvector.size() + s) > _config.txCapacity)
        {
            _txMSGvector.erase(_txMSGvector.begin());
        }
#endif
        _txMSGvector.push_back(msg);
        _onTX.execute();
        r = true;
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::add(Direction dir, const std::vector<embot::app::bldc::MSG> &msgs)
{
    bool r {false};
    
    if(true == msgs.empty())
    {
        return r;
    }

#if defined(SHARED_CIRCULAR)
    // we limit the number of insertable msgs to capacity
    const size_t s = std::min(msgs.size(), (Direction::INP == dir) ? _config.rxCapacity : _config.txCapacity);
#else    
    const size_t s = msgs.size();
#endif 
    
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);        
#if defined(SHARED_CIRCULAR)
        int toremove = s - (_config.rxCapacity - _rxMSGvector.size());        
        if(toremove > 0)
        {
            _rxMSGvector.erase(_rxMSGvector.begin(), _rxMSGvector.begin() + toremove);
        }
#endif
        _rxMSGvector.insert(_rxMSGvector.end(), msgs.begin(), msgs.begin() + s);
        _onRX.execute();
        r = true;
        embot::os::rtos::mutex_release(_rxm);
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)
        int toremove = s - (_config.txCapacity - _txMSGvector.size());        
        if(toremove > 0)
        {
            _txMSGvector.erase(_txMSGvector.begin(), _txMSGvector.begin() + toremove);
        }
#endif
        _txMSGvector.insert(_txMSGvector.end(), msgs.begin(), msgs.begin() + s);
        _onTX.execute();
        r = true;
        embot::os::rtos::mutex_release(_txm);        
    }  
    
    return r;
}


bool embot::app::bldc::theMSGbroker::Impl::rem(Direction dir, size_t &remaining, embot::app::bldc::MSG &msg)
{
    bool r {false};

    remaining = 0;
    embot::app::bldc::MSG ff {};
            
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        if(!_rxMSGvector.empty())
        {
            ff = _rxMSGvector.front();
            msg = ff;
            _rxMSGvector.erase(_rxMSGvector.begin());
            remaining = _rxMSGvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        if(!_txMSGvector.empty())
        {
            ff = _txMSGvector.front();
            msg = ff;
            _txMSGvector.erase(_txMSGvector.begin());
            remaining = _txMSGvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::rem(Direction dir, size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve)
{
    bool r {false};

    remaining = 0;       
    retrieved = 0;
        
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        retrieved = std::min(max2retrieve, _rxMSGvector.size()); 
        if(retrieved > 0)
        {
            msgs.assign(_rxMSGvector.begin(), _rxMSGvector.begin() + retrieved);
            _rxMSGvector.erase(_rxMSGvector.begin(), _rxMSGvector.begin() + retrieved);
            remaining = _rxMSGvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        retrieved = std::min(max2retrieve, _txMSGvector.size()); 
        if(retrieved > 0)
        {
            msgs.assign(_txMSGvector.begin(), _txMSGvector.begin() + retrieved);
            _txMSGvector.erase(_txMSGvector.begin(), _txMSGvector.begin() + retrieved);
            remaining = _txMSGvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_txm);
    } 
    
    return r;
}


bool embot::app::bldc::theMSGbroker::Impl::subscribe(Direction dir, const embot::os::Action &action)
{
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        _onRX = action;
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        _onTX = action;
        embot::os::rtos::mutex_release(_txm);
    }  

    return true;
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::bldc::theMSGbroker& embot::app::bldc::theMSGbroker::getInstance()
{
    static theMSGbroker* p = new theMSGbroker();
    return *p;
}

embot::app::bldc::theMSGbroker::theMSGbroker()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::bldc::theMSGbroker::~theMSGbroker() { }
        
bool embot::app::bldc::theMSGbroker::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

size_t embot::app::bldc::theMSGbroker::size(Direction dir) const
{
    return pImpl->size(dir);
}

bool embot::app::bldc::theMSGbroker::add(Direction dir, const embot::app::bldc::MSG &msg)
{
    return pImpl->add(dir, msg);
}

bool embot::app::bldc::theMSGbroker::add(Direction dir, const std::vector<embot::app::bldc::MSG> &msgs)
{
    return pImpl->add(dir, msgs);
}

bool embot::app::bldc::theMSGbroker::rem(Direction dir, size_t &remaining, embot::app::bldc::MSG &msg)
{
    return pImpl->rem(dir, remaining, msg);
}

bool embot::app::bldc::theMSGbroker::rem(Direction dir, size_t &remaining, std::vector<embot::app::bldc::MSG> &msgs, size_t &retrieved, const size_t max2retrieve)
{
    return pImpl->rem(dir, remaining, msgs, retrieved, max2retrieve);
}

bool embot::app::bldc::theMSGbroker::subscribe(Direction dir, const embot::os::Action &action)
{
    return pImpl->subscribe(dir, action);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


