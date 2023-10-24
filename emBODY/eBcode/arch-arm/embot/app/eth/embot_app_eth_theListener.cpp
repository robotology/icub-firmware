
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theListener.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "EOMtheIPnet.h"
#include "EOsocketDatagram.h"
#include "EOMmutex.h"
#include "eEcommon.h"
#include "embot_os_theScheduler.h"
#include "embot_app_eth_theErrorManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 
#include "embot_app_eth_theUpdaterParser.h"


struct embot::app::eth::theListener::Impl
{
    static constexpr embot::os::Event evtRXframe = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
    Config _config {};
    bool _initted {false};
    
    embot::os::EventThread *thr {nullptr};
    EOsocketDatagram* socket {nullptr};
    EOpacket *rxpkt {nullptr};
    EOpacket *replypkt {nullptr};
    bool connected2host {false};
    eOipv4addr_t hostaddress {0};
        
    Impl() = default;      
    
    bool initialise(const Config &config);
    
    static void alert(void* p);
    static void startup(embot::os::Thread *t, void *p);
    static void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p);   
    void transmit(EOpacket* txpkt);   
    bool connect(eOipv4addr_t remaddr);    
};

//embot::app::eth::theListener::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      

void tLISTENER(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::app::eth::theListener::Impl::initialise(const Config &config)
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
    
    embot::core::print("embot::app::eth::theListener::Impl::initialise()");
    
    
    if(nullptr == eom_ipnet_GetHandle())
    {
        embot::core::print("embot::app::eth::theListener::Impl::initialise(): cannot start because EOMtheIPnet is not started yets");
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eom_emsdiscoverylistener_Initialise(): EOMtheIPnet not started yet", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }

    // create the socket    
    socket = eo_socketdtg_New(  _config.socket.size.inputmaxdatagrams, _config.socket.size.inputmaxdatagramsize, eom_mutex_New(),
                                _config.socket.size.outputmaxdatagrams, _config.socket.size.outputmaxdatagramsize, eom_mutex_New()
                             );
                                 
//    socket = eo_socketdtg_New(  cfg.inpdatagramnumber, cfg.inpdatagramsizeof, (eobool_true == cfg.usemutex) ? (eom_mutex_New()) : (NULL), 
//                                cfg.outdatagramnumber, cfg.outdatagramsizeof, (eobool_true == cfg.usemutex) ? (eom_mutex_New()) : (NULL)
//                                );        
    // create the rx packet
    rxpkt = eo_packet_New(_config.socket.size.inputmaxdatagramsize);    


    embot::app::eth::theUpdaterParser::Config cfg1 {_config.hostaddress, _config.socket.size.outputmaxdatagramsize, nullptr}; 
    embot::app::eth::theUpdaterParser::getInstance().initialise(cfg1);
    
    constexpr embot::core::relTime timeout {1000*embot::core::time1millisec};

    embot::os::EventThread::Config tCfg { 
        _config.thread.stacksize, 
        _config.thread.priority, 
        startup, this,
        timeout,
        onevent,
        "tLISTENER"
    };
       
        
    // create the server thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(tCfg, tLISTENER);
    
    _initted = true;
    return true;
}

void embot::app::eth::theListener::Impl::alert(void* p)
{
    embot::app::eth::theListener::Impl *impl = reinterpret_cast<embot::app::eth::theListener::Impl*>(p);
    impl->thr->setEvent(evtRXframe);
}

void embot::app::eth::theListener::Impl::startup(embot::os::Thread *t, void *p)
{
    embot::app::eth::theListener::Impl *impl = reinterpret_cast<embot::app::eth::theListener::Impl*>(p);
    
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetCallback(onrx, embot::app::eth::theListener::Impl::alert, impl, nullptr);
    
    eOresult_t res = eores_NOK_generic;
    res = eo_socketdtg_Open(impl->socket, impl->_config.socket.localport, eo_sktdir_TXRX, eobool_false, 
                            NULL, onrx, NULL);   
    
    if(eores_OK != res)
    {
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theListener", thr}, {}, "cannot open its socket");        
    }    
}

void embot::app::eth::theListener::Impl::onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }

    embot::app::eth::theListener::Impl *impl = reinterpret_cast<embot::app::eth::theListener::Impl*>(p);
    
    if(true == embot::core::binary::mask::check(eventmask, evtRXframe)) 
    {   
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("onevent: evtRXframe received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

        EOpacket* replypkt = nullptr;
        eOsizecntnr_t remainingrxpkts = 0;  
            
        // 1. get the packet.    
        if(eores_OK == eo_socketdtg_Get(impl->socket, impl->rxpkt, eok_reltimeZERO))
        {                 
            // 2. process the packet with the transceiver
           
            if(embot::app::eth::theUpdaterParser::getInstance().parse(impl->rxpkt))
            {
                if(embot::app::eth::theUpdaterParser::getInstance().getreply(&replypkt))
                {
                    impl->transmit(replypkt);
                }
            }       
        }
            
        // 5. if another packet is in the rx fifo, send a new event to process its retrieval again   
        eo_socketdtg_Received_NumberOf(impl->socket, &remainingrxpkts);
        
        if(remainingrxpkts > 0)
        {
            t->setEvent(evtRXframe); 
        }     
    
    }
}

void embot::app::eth::theListener::Impl::transmit(EOpacket* txpkt)
{
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {0};
        
    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);
    
    if((false == connected2host) || (remaddr != hostaddress))
    {
        if(true != connect(remaddr))
        {
            return;
        }
    }
    
    eo_socketdtg_Put(socket, txpkt);    
}

bool embot::app::eth::theListener::Impl::connect(eOipv4addr_t remaddr)
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

embot::app::eth::theListener& embot::app::eth::theListener::getInstance()
{
    static theListener* p = new theListener();
    return *p;
}

embot::app::eth::theListener::theListener()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theListener::~theListener() { }
        

bool embot::app::eth::theListener::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

//bool embot::app::eth::theListener::synchronise()
//{
//    return pImpl->synchronise();
//}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


