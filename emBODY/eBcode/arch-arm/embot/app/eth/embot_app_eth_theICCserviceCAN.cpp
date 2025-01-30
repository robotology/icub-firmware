
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theICCserviceCAN.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::eth::icc::theICCserviceCAN::Impl
{
    bool _initted {false};
    Config _config {};
            
    constexpr static const char _objname[] = "theICCserviceCAN";
        
    Impl() = default;
    
    bool initialise(const Config &config);
             
    bool set(embot::app::eth::icc::theICCservice::modeTX modetx);
    bool set(const embot::core::Callback &onrx);  
    bool set(const ItemCANframeParser itemparser);
    
    bool put(const ItemCANframe &item, embot::core::relTime timeout);
    size_t output() const;
    bool flush(embot::core::relTime timeout);
    bool flush(const embot::core::Callback &cbk);
    bool flushed() const;
    
    size_t input() const;
    bool get(ItemCANframe &item, size_t &remaining);
    bool parse();
        
private:   
       
          
};



// qui devo settare una funzione che parsi gli item appena arrivano e li trsaforma in ItemROP etc....
bool embot::app::eth::icc::theICCserviceCAN::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    _config = config;
            
    // configure theICCservice in the proper way.
    embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, nullptr);
    
    set(_config.modetx);
    set(_config.onrx);
    set(_config.itemparser); // it also checks if it is nullptr and corrects it
   
    
//    // 1. i impose the execution of onRXitem() when an item is received so that it unlocks the waiting semaphore
//    embot::core::Callback cbk {onRXitem, this};    
//    embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, cbk);
//    // 2. i may force an immediate transmission and remove the use of the flush() but never mind for now
//    // embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, embot::app::eth::icc::theICCservice::modeTX::instant);
        
    _initted = true;
    return _initted;
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::set(embot::app::eth::icc::theICCservice::modeTX modetx)
{
    _config.modetx = modetx;
    // propagate it to icc
    return embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, _config.modetx);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::set(const embot::core::Callback &onrx)
{
    _config.onrx = onrx;
    // propagate it to icc
    return embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, _config.onrx);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::set(const ItemCANframeParser itemparser)
{
    _config.itemparser = (nullptr == itemparser) ? ItemCANframeParserDummy : itemparser;
    // do not propagate it to icc, because its parsing is always disabled.
    return true;
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::put(const ItemCANframe &item, embot::core::relTime timeout)
{
    return embot::app::eth::icc::theICCservice::getInstance().put(_config.pipe, item.item(), timeout);
}

size_t embot::app::eth::icc::theICCserviceCAN::Impl::output() const
{
    return embot::app::eth::icc::theICCservice::getInstance().output(_config.pipe);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::flush(embot::core::relTime timeout)
{
    return embot::app::eth::icc::theICCservice::getInstance().flush(_config.pipe, timeout);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::flush(const embot::core::Callback &cbk)
{
    return embot::app::eth::icc::theICCservice::getInstance().flush(_config.pipe, cbk);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::flushed() const
{
    return embot::app::eth::icc::theICCservice::getInstance().flushed(_config.pipe);  
}

size_t embot::app::eth::icc::theICCserviceCAN::Impl::input() const
{
    return embot::app::eth::icc::theICCservice::getInstance().input(_config.pipe);
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::get(ItemCANframe &item, size_t &remaining)
{
    bool r {false};
    
    Item rxi {};
    if(true == embot::app::eth::icc::theICCservice::getInstance().get(_config.pipe, rxi, remaining))
    {
        if(embot::app::eth::icc::ItemType::CANframe == rxi.type)
        {
            item.load(rxi);
            r = true;
        }
    }
    
    return r;
}

bool embot::app::eth::icc::theICCserviceCAN::Impl::parse()
{    
    if(false == _initted)
    {
        return false;
    }  

    bool r {false};
    Item rxi {};
    size_t s {0};
    
    s = input();
        
    for(size_t i=0; i<s; i++)
    {
        size_t remaining {0};
        if(true == embot::app::eth::icc::theICCservice::getInstance().get(_config.pipe, rxi, remaining))
        {
            if(embot::app::eth::icc::ItemType::CANframe == rxi.type)
            {
                ItemCANframe icf {rxi};
                if(nullptr !=  _config.itemparser)
                {
                    _config.itemparser(icf);
                    r = true;
                }
            }
        }
    }    
    
    return r;    
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::icc::theICCserviceCAN& embot::app::eth::icc::theICCserviceCAN::getInstance()
{
    static theICCserviceCAN* p = new theICCserviceCAN();
    return *p;
}

embot::app::eth::icc::theICCserviceCAN::theICCserviceCAN()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::icc::theICCserviceCAN::~theICCserviceCAN() { }

bool embot::app::eth::icc::theICCserviceCAN::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::icc::theICCserviceCAN::set(embot::app::eth::icc::theICCservice::modeTX modetx)
{
    return pImpl->set(modetx);
}

bool embot::app::eth::icc::theICCserviceCAN::set(const embot::core::Callback &onrx)
{
    return pImpl->set(onrx);
}

bool embot::app::eth::icc::theICCserviceCAN::set(const ItemCANframeParser itemparser)
{
    return pImpl->set(itemparser);
}

bool embot::app::eth::icc::theICCserviceCAN::put(const ItemCANframe &item, embot::core::relTime timeout)
{
    return pImpl->put(item, timeout); 
}

size_t embot::app::eth::icc::theICCserviceCAN::output() const
{
    return pImpl->output();
}

bool embot::app::eth::icc::theICCserviceCAN::flush(embot::core::relTime timeout)
{
    return pImpl->flush(timeout);
}

bool embot::app::eth::icc::theICCserviceCAN::flush(const embot::core::Callback &cbk)
{
    return pImpl->flush(cbk);
}

bool embot::app::eth::icc::theICCserviceCAN::flushed() const
{
    return pImpl->flushed();    
}

size_t embot::app::eth::icc::theICCserviceCAN::input() const
{
    return pImpl->input();
}

bool embot::app::eth::icc::theICCserviceCAN::get(ItemCANframe &item, size_t &remaining)
{
    return pImpl->get(item, remaining); 
}

bool embot::app::eth::icc::theICCserviceCAN::parse()
{
    return pImpl->parse();
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


