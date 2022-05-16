
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theBackdoor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "embot_os_rtos.h"
#include "EOMtheIPnet.h"
#include "EOsocketDatagram.h"
#include "EOMmutex.h"
#include "eEcommon.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::eth::theBackdoor::Impl
{
    static constexpr embot::os::Event evtRXframe = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
    Config _config {};
    bool _initted {false};
    
    embot::os::EventThread *thr {nullptr};
    EOsocketDatagram* socket {nullptr};
    EOpacket *rxpkt {nullptr};
    bool connected2host {false};
    eOipv4addr_t hostaddress {0};
    embot::os::rtos::mutex_t *mtx {nullptr};
        
    Impl() = default;      
    
    bool initialise(const Config &config);
    
    static void alert(void* p);
    static void startup(embot::os::Thread *t, void *p);
    static void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p);   
    bool transmit(EOpacket* txpkt);   
    bool connect(eOipv4addr_t remaddr);    
};



void tBACKDOOR(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::app::eth::theBackdoor::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    embot::core::print("embot::app::eth::theBackdoor::Impl::initialise()");    
    
    if(nullptr == eom_ipnet_GetHandle())
    {
        embot::core::print("embot::app::eth::theBackdoor::Impl::initialise(): cannot start because EOMtheIPnet is not started yets");
    }

    // create the socket    
    socket = eo_socketdtg_New(  _config.socket.size.inputmaxdatagrams, _config.socket.size.inputmaxdatagramsize, eom_mutex_New(),
                                _config.socket.size.outputmaxdatagrams, _config.socket.size.outputmaxdatagramsize, eom_mutex_New()
                             );
                                 
    // create the rx packet
    rxpkt = eo_packet_New(_config.socket.size.inputmaxdatagramsize);    

    // create the mutex
    mtx = embot::os::rtos::mutex_new();
    
    constexpr embot::core::relTime timeout {1000*embot::core::time1millisec};

    embot::os::EventThread::Config tCfg { 
        _config.thread.stacksize, 
        _config.thread.priority, 
        startup, this,
        timeout,
        onevent,
        "tBACKDOOR"
    };
       
        
    // create the server thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(tCfg, tBACKDOOR);
    
    _initted = true;
    return true;
}


void embot::app::eth::theBackdoor::Impl::alert(void* p)
{
    embot::app::eth::theBackdoor::Impl *impl = reinterpret_cast<embot::app::eth::theBackdoor::Impl*>(p);
    impl->thr->setEvent(evtRXframe);
}

void embot::app::eth::theBackdoor::Impl::startup(embot::os::Thread *t, void *p)
{
    embot::app::eth::theBackdoor::Impl *impl = reinterpret_cast<embot::app::eth::theBackdoor::Impl*>(p);
    
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetCallback(onrx, embot::app::eth::theBackdoor::Impl::alert, impl, nullptr);
    
    eOresult_t res = eores_NOK_generic;
    res = eo_socketdtg_Open(impl->socket, impl->_config.socket.localport, eo_sktdir_TXRX, eobool_false, 
                            NULL, onrx, NULL);   
    
    if(eores_OK != res)
    {
        embot::core::print("tBACKDOOR cannot open the socket");
    }    
}

void embot::app::eth::theBackdoor::Impl::onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }

    embot::app::eth::theBackdoor::Impl *impl = reinterpret_cast<embot::app::eth::theBackdoor::Impl*>(p);
    
    if(true == embot::core::binary::mask::check(eventmask, evtRXframe)) 
    {   
//        embot::core::TimeFormatter tf(embot::core::now());        
//        embot::core::print("onevent: evtRXframe received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

//        EOpacket* replypkt = nullptr;
        eOsizecntnr_t remainingrxpkts = 0;  
            
        // 1. get the packet.    
        if(eores_OK == eo_socketdtg_Get(impl->socket, impl->rxpkt, eok_reltimeZERO))
        {                 
            // 2. process the packet with the user-defined callback
            embot::os::rtos::mutex_take(impl->mtx, embot::core::reltimeWaitForever);
            if(nullptr != impl->_config.onreception)
            {
                impl->_config.onreception(impl->rxpkt);
            }
            embot::os::rtos::mutex_release(impl->mtx);
 
        }
            
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again   
        eo_socketdtg_Received_NumberOf(impl->socket, &remainingrxpkts);
        
        if(remainingrxpkts > 0)
        {
            t->setEvent(evtRXframe); 
        }     
    
    }
}


bool embot::app::eth::theBackdoor::Impl::transmit(EOpacket* txpkt)
{
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {0};
        
    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);
    
    if((false == connected2host) || (remaddr != hostaddress))
    {
        if(true != connect(remaddr))
        {
            return false;
        }
    }
    
    eOresult_t r = eo_socketdtg_Put(socket, txpkt);  
    
    return eores_OK == r;    
}

bool embot::app::eth::theBackdoor::Impl::connect(eOipv4addr_t remaddr)
{
    eOresult_t res = eores_OK;
    
    if((false == connected2host) || (remaddr != hostaddress))
    {
        connected2host = false;
        hostaddress = remaddr;
        
        res = eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), remaddr, 5*EOK_reltime1sec);
 
        if(eores_OK == res)
        {
            connected2host = true;  
        }            
    }
    
    return connected2host;
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theBackdoor& embot::app::eth::theBackdoor::getInstance()
{
    static theBackdoor* p = new theBackdoor();
    return *p;
}

embot::app::eth::theBackdoor::theBackdoor()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theBackdoor::~theBackdoor() { }
        

bool embot::app::eth::theBackdoor::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theBackdoor::transmit(EOpacket *txp)
{
    return pImpl->transmit(txp);
}

bool embot::app::eth::theBackdoor::set(fpOnPacket onrx)
{
    // a mutex is required in here to avoid damage
    embot::os::rtos::mutex_take(pImpl->mtx, embot::core::reltimeWaitForever);
    pImpl->_config.onreception = onrx;
    embot::os::rtos::mutex_release(pImpl->mtx);
    
    return true;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


