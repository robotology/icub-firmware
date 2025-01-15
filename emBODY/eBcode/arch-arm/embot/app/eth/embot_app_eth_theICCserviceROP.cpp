
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theICCserviceROP.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"

using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::eth::icc::theICCserviceROP::Impl
{
    bool _initted {false};
    Config _config {};
            
    constexpr static const char _objname[] = "theICCserviceROP";
        
    Impl() = default;
    
    bool initialise(const Config &config);
             
    bool ping(embot::core::relTime timeout);
    bool ask(ItemROP::Variable &var, embot::core::relTime timeout);
    bool sig(const ItemROP::Variable &var);
    bool set(const ItemROP::Variable &var, embot::core::relTime timeout);

    
    bool set(fpOnROPrx onroprx);
        
private:    
        
    // it waits for ....
    embot::os::rtos::semaphore_t *reply_semaphore {nullptr}; // used to wait for the reply (ack, nak, say)
    
    ItemROP rxrop {};
    ItemROP reply{};
        
    bool send(const ItemROP &txrop, embot::core::relTime timeout = embot::core::reltimeWaitForever)
    {
        bool r {false};
        if(embot::app::eth::icc::ItemROP::CMD::none != txrop.cmd)
        {            
            r = embot::app::eth::icc::theICCservice::getInstance().put(_config.pipe, txrop.item(), timeout);
//            embot::app::eth::icc::theICCservice::getInstance().flush(_config.pipe, timeout);
        }
        
        return r;
    }
        
    // it manages the incoming Item (s)
    static void onRXitem(void *p) 
    { 
        Impl* pimpl = reinterpret_cast<Impl*>(p);
        
        bool release_the_semaphore {false};
        
        pimpl->rxrop.clear();
        pimpl->reply.clear();
        pimpl->reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, {});
            
        // i get one item only w/ 
        if(true == pimpl->get(pimpl->rxrop))
        {
            // if i have a proper ItemROP
            switch(pimpl->rxrop.cmd)
            {
                case embot::app::eth::icc::ItemROP::CMD::ack: // it must be a reply to a set<> / ping
                case embot::app::eth::icc::ItemROP::CMD::say: // it must be a reply to a ask<>
                case embot::app::eth::icc::ItemROP::CMD::nak: // it must be a reply to a set<> / ask<>
                {
                    // it must be a reply to a ask<>. if the task cmd was a ask, then we
                    // 1. unlock the sem                    
                    release_the_semaphore =  true;
                    
                } break;
                
                case embot::app::eth::icc::ItemROP::CMD::ping:
                {
                    // i must send back an ack
                    pimpl->reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, pimpl->rxrop.var);
                    pimpl->send(pimpl->reply.item());
                    
                } break;
 
                case embot::app::eth::icc::ItemROP::CMD::ask:
                case embot::app::eth::icc::ItemROP::CMD::set:
                {
                    // i must call the user defined callback 
                    pimpl->_config.onroprx(pimpl->rxrop, pimpl->reply);
                    // and send a reply (if not CMD::none)
                    pimpl->send(pimpl->reply.item());
                    
                } break;

                case embot::app::eth::icc::ItemROP::CMD::sig:
                {
                    // i must call the user defined callback 
                    pimpl->_config.onroprx(pimpl->rxrop, pimpl->reply);
                    // but not send a reply                    
                } break;
                
                default: {} break;
            }
                       
        }    
        
        // finally, if waiting for ... we release it
        if(true == release_the_semaphore)
        {
            embot::os::rtos::semaphore_release(pimpl->reply_semaphore); 
        }
    }
        
    bool get(ItemROP &irx);
          
};


bool embot::app::eth::icc::theICCserviceROP::Impl::get(ItemROP &roprx)
{
    bool r {false}; 
    
    // i must get only one .... so, if any, i just get the first
    Item rxi {};
    size_t s {0};
    
    s = embot::app::eth::icc::theICCservice::getInstance().input(_config.pipe);
    
    if(0 == s)
    {
        return r;
    }

    size_t remaining {0};
    embot::app::eth::icc::theICCservice::getInstance().get(_config.pipe, rxi, remaining);
    if(ItemType::ROP == rxi.type)
    {
        roprx.load(rxi);
        r = true;
    }
    
    // the others (if any) ... i throw them away
            
    for(size_t i=0; i<remaining; i++)
    {
        size_t re {0};
        embot::app::eth::icc::theICCservice::getInstance().get(_config.pipe, rxi, re);
    }    
             
    return r;
}


// qui devo settare una funzione che parsi gli item appena arrivano e li trsaforma in ItemROP etc....
bool embot::app::eth::icc::theICCserviceROP::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    _config = config;
    
    if(nullptr == _config.onroprx)
    {
        _config.onroprx = embot::app::eth::icc::theICCserviceROP::defOnROPrx;
    }
    
    reply_semaphore = embot::os::rtos::semaphore_new(2, 0);
    
    // configure theICCservice in the proper way.
    
    // 1. i impose the execution of onRXitem() when an item is received so that it unlocks the waiting semaphore
    embot::core::Callback cbk {onRXitem, this};    
    embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, cbk);
    // 2. i may force an immediate transmission and remove the use of the flush() but never mind for now
    embot::app::eth::icc::theICCservice::getInstance().set(_config.pipe, embot::app::eth::icc::theICCservice::modeTX::instant);
        
    _initted = true;
    return true;
}



bool embot::app::eth::icc::theICCserviceROP::Impl::ping(embot::core::relTime timeout)
{
    bool r {false};
    
    theICCservice::Pipe pipe {_config.pipe};
    
    
    embot::core::Time expiry = embot::core::now() + timeout;
    ItemROP txrop {ItemROP::CMD::ping, {}};
    send(txrop, timeout);
//    embot::app::eth::icc::theICCservice::getInstance().put(pipe, txrop.item(), timeout);
//    embot::app::eth::icc::theICCservice::getInstance().flush(pipe, timeout);
    
    // now wait for a ack<> or a timeout
        
    r = embot::os::rtos::semaphore_acquire(reply_semaphore, timeout);
        
    if(true == r)
    {
        // we have a reply 
        
        // it must be a ack, if not we return false
        if(embot::app::eth::icc::ItemROP::CMD::ack != rxrop.cmd)
        {
            r = false;
        }

    }

    return r;    
}

bool embot::app::eth::icc::theICCserviceROP::Impl::ask(ItemROP::Variable &var, embot::core::relTime timeout)
{
    bool r {false};
    
    theICCservice::Pipe pipe {_config.pipe};
        
    embot::core::Time expiry = embot::core::now() + timeout;
    ItemROP txrop {ItemROP::CMD::ask, var};
    send(txrop, timeout);
//    embot::app::eth::icc::theICCservice::getInstance().put(pipe, txrop.item(), timeout);
//    embot::app::eth::icc::theICCservice::getInstance().flush(pipe, timeout);
    
    // now wait for a say<> or a nak<>
        
    r = embot::os::rtos::semaphore_acquire(reply_semaphore, timeout);
        
    if(true == r)
    {
        // we have content inside rxrop
        
        // it must be a say
        if(embot::app::eth::icc::ItemROP::CMD::say == rxrop.cmd)
        {
            std::memmove(var.memory, rxrop.data, var.descriptor.size);
        }
        else
        {   // it must be a nak
            r = false;
        }
    }
       
    return r;
}
//#warning .... say() is not used
//bool embot::app::eth::icc::theICCserviceROP::Impl::say(const ItemROP::Variable &var)
//{
//    bool r {false};
//    
//    theICCservice::Pipe pipe {_config.pipe};
//    
//    embot::core::relTime timeout {100*1000};
//    
//    embot::core::Time expiry = embot::core::now() + timeout;
//    ItemROP ir {ItemROP::CMD::say, var};
//    embot::app::eth::icc::theICCservice::getInstance().put(pipe, ir.item(), timeout);
//    r = embot::app::eth::icc::theICCservice::getInstance().flush(pipe, timeout);
//    
//    // nothing to wait for
//        
//    return r;
//}

bool embot::app::eth::icc::theICCserviceROP::Impl::sig(const ItemROP::Variable &var)
{
    bool r {false};
    
    theICCservice::Pipe pipe {_config.pipe};
    
    embot::core::relTime timeout {100*1000};
    
    embot::core::Time expiry = embot::core::now() + timeout;
    ItemROP txrop {ItemROP::CMD::sig, var};
    r = send(txrop, timeout);
//    embot::app::eth::icc::theICCservice::getInstance().put(pipe, ir.item(), timeout);
//    r = embot::app::eth::icc::theICCservice::getInstance().flush(pipe, timeout);
    
    // nothing to wait for
        
    return r;
}

bool embot::app::eth::icc::theICCserviceROP::Impl::set(const ItemROP::Variable &var, embot::core::relTime timeout)
{
    bool r {false};
        
    theICCservice::Pipe pipe {_config.pipe};
    
    embot::core::Time expiry = embot::core::now() + timeout;
    ItemROP txrop {ItemROP::CMD::set, var};
    send(txrop, timeout);
//    embot::app::eth::icc::theICCservice::getInstance().put(pipe, txrop.item(), timeout);
//    embot::app::eth::icc::theICCservice::getInstance().flush(pipe, timeout);
    
    // now wait for a ack<> or a nak<>
    
    r = embot::os::rtos::semaphore_acquire(reply_semaphore, timeout);
        
    if(true == r)
    {
        // we have content inside rxrop
        
        if(embot::app::eth::icc::ItemROP::CMD::ack == rxrop.cmd)
        {
            r = true;
        }
        else
        {   // it must be a nak
            r = false;
        }
    }
       
    return r;
}


bool embot::app::eth::icc::theICCserviceROP::Impl::set(fpOnROPrx onroprx)
{ 
    _config.onroprx = (nullptr == onroprx) ? embot::app::eth::icc::theICCserviceROP::defOnROPrx : onroprx;    
    return true;
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::icc::theICCserviceROP& embot::app::eth::icc::theICCserviceROP::getInstance()
{
    static theICCserviceROP* p = new theICCserviceROP();
    return *p;
}

embot::app::eth::icc::theICCserviceROP::theICCserviceROP()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::icc::theICCserviceROP::~theICCserviceROP() { }

bool embot::app::eth::icc::theICCserviceROP::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::icc::theICCserviceROP::ping(embot::core::relTime timeout)
{
    return pImpl->ping(timeout);
}

bool embot::app::eth::icc::theICCserviceROP::ask(ItemROP::Variable &var, embot::core::relTime timeout)
{
    return pImpl->ask(var, timeout);
}

bool embot::app::eth::icc::theICCserviceROP::sig(const ItemROP::Variable &var)
{
    return pImpl->sig(var);
}

bool embot::app::eth::icc::theICCserviceROP::set(const ItemROP::Variable &var, embot::core::relTime timeout)
{
    return pImpl->set(var, timeout);
}

bool embot::app::eth::icc::theICCserviceROP::set(fpOnROPrx onroprx)
{
    return pImpl->set(onroprx);
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


