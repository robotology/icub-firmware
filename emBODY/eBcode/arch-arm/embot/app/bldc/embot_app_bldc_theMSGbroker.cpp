
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
    std::vector<embot::prot::can::Frame> _rxCANvector {};
    embot::os::rtos::mutex_t *_txm {nullptr};    
    std::vector<embot::prot::can::Frame> _txCANvector {};     
        
    // the initialization code
    bool initialise(Config config);       

    size_t size(Direction dir) const;

    bool add(Direction dir, const embot::prot::can::Frame &frame);
    bool add(Direction dir, const std::vector<embot::prot::can::Frame> &frames);        

    bool rem(Direction dir, size_t &remaining, embot::prot::can::Frame &frame);
    bool rem(Direction dir, size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve);  
        
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
    _rxCANvector.reserve(_config.rxCapacity);
    _txCANvector.reserve(_config.txCapacity);
    
    _initted = true;
    return _initted;
}


size_t embot::app::bldc::theMSGbroker::Impl::size(Direction dir) const
{
    size_t r {0};
    
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        r = _rxCANvector.size();
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        r = _txCANvector.size();
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::add(Direction dir, const embot::prot::can::Frame &frame)
{
    bool r {false};
    
    constexpr size_t s = 1; // surely 1 is compatible w/ capacity
        
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)        
        if((_rxCANvector.size() + s) > _config.rxCapacity)
        {
            _rxCANvector.erase(_rxCANvector.begin());
        }
#endif
        _rxCANvector.push_back(frame);
        _onRX.execute();
        r = true;
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)        
        if((_txCANvector.size() + s) > _config.txCapacity)
        {
            _txCANvector.erase(_txCANvector.begin());
        }
#endif
        _txCANvector.push_back(frame);
        _onTX.execute();
        r = true;
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::add(Direction dir, const std::vector<embot::prot::can::Frame> &frames)
{
    bool r {false};
    
    if(true == frames.empty())
    {
        return r;
    }

#if defined(SHARED_CIRCULAR)
    // we limit the number of insertable frames to capacity
    const size_t s = std::min(frames.size(), (Direction::INP == dir) ? _config.rxCapacity : _config.txCapacity);
#else    
    const size_t s = frames.size();
#endif 
    
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);        
#if defined(SHARED_CIRCULAR)
        int toremove = s - (_config.rxCapacity - _rxCANvector.size());        
        if(toremove > 0)
        {
            _rxCANvector.erase(_rxCANvector.begin(), _rxCANvector.begin() + toremove);
        }
#endif
        _rxCANvector.insert(_rxCANvector.end(), frames.begin(), frames.begin() + s);
        _onRX.execute();
        r = true;
        embot::os::rtos::mutex_release(_rxm);
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
#if defined(SHARED_CIRCULAR)
        int toremove = s - (_config.txCapacity - _txCANvector.size());        
        if(toremove > 0)
        {
            _txCANvector.erase(_txCANvector.begin(), _txCANvector.begin() + toremove);
        }
#endif
        _txCANvector.insert(_txCANvector.end(), frames.begin(), frames.begin() + s);
        _onTX.execute();
        r = true;
        embot::os::rtos::mutex_release(_txm);        
    }  
    
    return r;
}


bool embot::app::bldc::theMSGbroker::Impl::rem(Direction dir, size_t &remaining, embot::prot::can::Frame &frame)
{
    bool r {false};

    remaining = 0;
    embot::prot::can::Frame ff {};
            
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        if(!_rxCANvector.empty())
        {
            ff = _rxCANvector.front();
            frame = ff;
            _rxCANvector.erase(_rxCANvector.begin());
            remaining = _rxCANvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        if(!_txCANvector.empty())
        {
            ff = _txCANvector.front();
            frame = ff;
            _txCANvector.erase(_txCANvector.begin());
            remaining = _txCANvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_txm);
    }  
    
    return r;
}

bool embot::app::bldc::theMSGbroker::Impl::rem(Direction dir, size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve)
{
    bool r {false};

    remaining = 0;       
    retrieved = 0;
        
    if(Direction::INP == dir)
    {
        embot::os::rtos::mutex_take(_rxm, embot::core::reltimeWaitForever);
        retrieved = std::min(max2retrieve, _rxCANvector.size()); 
        if(retrieved > 0)
        {
            frames.assign(_rxCANvector.begin(), _rxCANvector.begin() + retrieved);
            _rxCANvector.erase(_rxCANvector.begin(), _rxCANvector.begin() + retrieved);
            remaining = _rxCANvector.size();
            r = true;            
        }
        embot::os::rtos::mutex_release(_rxm); 
    }
    else
    {
        embot::os::rtos::mutex_take(_txm, embot::core::reltimeWaitForever);
        retrieved = std::min(max2retrieve, _txCANvector.size()); 
        if(retrieved > 0)
        {
            frames.assign(_txCANvector.begin(), _txCANvector.begin() + retrieved);
            _txCANvector.erase(_txCANvector.begin(), _txCANvector.begin() + retrieved);
            remaining = _txCANvector.size();
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

bool embot::app::bldc::theMSGbroker::add(Direction dir, const embot::prot::can::Frame &frame)
{
    return pImpl->add(dir, frame);
}

bool embot::app::bldc::theMSGbroker::add(Direction dir, const std::vector<embot::prot::can::Frame> &frames)
{
    return pImpl->add(dir, frames);
}

bool embot::app::bldc::theMSGbroker::rem(Direction dir, size_t &remaining, embot::prot::can::Frame &frame)
{
    return pImpl->rem(dir, remaining, frame);
}

bool embot::app::bldc::theMSGbroker::rem(Direction dir, size_t &remaining, std::vector<embot::prot::can::Frame> &frames, size_t &retrieved, const size_t max2retrieve)
{
    return pImpl->rem(dir, remaining, frames, retrieved, max2retrieve);
}

bool embot::app::bldc::theMSGbroker::subscribe(Direction dir, const embot::os::Action &action)
{
    return pImpl->subscribe(dir, action);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


