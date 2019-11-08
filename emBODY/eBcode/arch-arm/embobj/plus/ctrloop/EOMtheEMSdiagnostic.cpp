/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Luca Tricerri
 * email:   luca.tricerri@iit.it
 * website: https://github.com/icub-tech-iit
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/



// public interface

#include "EOMtheEMSDiagnostic.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "lock_guard"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "OPCprotocolManager.h"
#include "OPCprotocolManager_Cfg.h"

#include "EOaction.h"
#include "EOMtheIPnet.h"
#include "EOMmutex.h"
#include "EOpacket.h"
#include "EOsocketDatagram.h"
#include "EOMtask.h"

#include "osal.h"
#include "hal_trace.h"

// --

#define TEST_DECODE
    
    
#if defined(TEST_DECODE)

    
    // to be placed in the pimpl of the main object and conditionally included
    class Decoder
    {
    public:
        struct Config
        {
            constexpr static size_t minropcapacity = 40;
            size_t ropcapacity {384};
            Config() = default;
            constexpr Config(int t) : ropcapacity(t) {}
            bool isvalid() const { return ropcapacity >= minropcapacity; }
        };      
        
        Decoder();        
        ~Decoder(); 
    
        bool init(const Config &config);
        bool initted() const;        
        bool decode(uint8_t *ropframe, uint16_t sizeofropframe, const embot::eprot::IPv4 &ipv4 = {"10.0.1.98"}); 
        
    private:    
        struct Impl;
        Impl *pImpl;        
    };

        
    Decoder *decoder {nullptr};
    
#endif

// --


extern void tskEMScfg(void *p)
{
    eom_task_Start((EOMtask*)p);
}
EOMtheEMSDiagnostic::EOMtheEMSDiagnostic()
{
    txpkt_=eo_packet_New(0);
}

EOMtheEMSDiagnostic& EOMtheEMSDiagnostic::instance()
{
    static EOMtheEMSDiagnostic instance;
    return instance;
}

bool EOMtheEMSDiagnostic::initialise(const Params& cfg)
{
    params_=cfg;
        
    if(NULL == eom_ipnet_GetHandle())
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eom_emsdiscoverylistener_Initialise(): EOMtheIPnet not started yet", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }

     mutexBody_=eom_mutex_New();
    
    // create the socket    
    socket_ = eo_socketdtg_New(cfg.inpdatagramnumber_, EOMDiagnosticUdpMsg::getSize(), (eobool_true == cfg.usemutex_) ? (eom_mutex_New()) : (NULL), 
                                                               cfg.outdatagramnumber_, EOMDiagnosticUdpMsg::getSize(), (eobool_true == cfg.usemutex_) ? (eom_mutex_New()) : (NULL)
                                                              );    
    // create the rx packet
    rxpkt_ = eo_packet_New(EOMDiagnosticUdpMsg::getSize());
    
    
    
    txpkt2_ = eo_packet_New(0);
    eo_packet_Size_Set(txpkt2_, 0); 
    rawcapacity = 513;    
    rawdata = new uint8_t[rawcapacity];    
    
    embot::app::DiagnosticsNode::Config config {}; // to be filled properly after
    node = new embot::app::DiagnosticsNode;
    node->init(config);
        
#if defined(TEST_DECODE)
    decoder =  new Decoder;
    decoder->init({});
#endif        
        

    // create the task
    task_ = eom_task_New(eom_mtask_EventDriven, 
                        params_.taskpriority_, 
                        params_.taskstacksize_, 
                        [](EOMtask* task ,uint32_t t){
                            EOMtheEMSDiagnostic::instance().taskStartup(task,t);
                        },
                        [](EOMtask* task ,uint32_t t){
                            EOMtheEMSDiagnostic::instance().taskRun(task,t);
                        }, 
                        (eOevent_t)0, 
                        wakeupTask_, 
                        NULL, 
                        tskEMScfg,
                        "diagnostic");
                                                                                
    
                             
    return true;                                                
}


void EOMtheEMSDiagnostic::taskStartup(EOMtask*,uint32_t t)
{
    // open the socket
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, diagnosticConfigEvent_evt_packet_received, task_);
    
    eo_socketdtg_Open(socket_, localPort_, eo_sktdir_TXRX, eobool_false,NULL, onrx, NULL);   
    
    eo_errman_Trace(eo_errman_GetHandle(), "called _task_startup()", s_eobj_ownname);                            
}

void EOMtheEMSDiagnostic::taskRun(EOMtask*,uint32_t t)
{
    eOevent_t evt = (eOevent_t)t;

    if(!evt)
        {
            transmitUdpPackage();        
        }
  
    if(eobool_true == eo_common_event_check(evt, diagnosticConfigEvent_evt_packet_received))
        {
            processEventRxPacket();
        }

    if(eobool_true == eo_common_event_check(evt, diagnosticEvent_evt_packet_tobesent))
        {
            transmitUdpPackage();
        }            
}

void EOMtheEMSDiagnostic::processEventRxPacket()
{
    EOpacket* rxpkt = rxpkt_;
    eOsizecntnr_t remainingrxpkts = 0;    
    
    // 1. get the packet.    
    if(eores_OK == eo_socketdtg_Get(socket_, rxpkt, eok_reltimeZERO))
    {    
        // 2. process the packet 
        manageArrivedMessage(rxpkt);        
    }

    // 3. if another packet is in the rx fifo, send a new event to process its retrieval again   
    eo_socketdtg_Received_NumberOf(socket_, &remainingrxpkts);
    if(remainingrxpkts > 0)
    {
            eom_task_SetEvent(task_, diagnosticConfigEvent_evt_packet_received); 
    }     
}

bool EOMtheEMSDiagnostic::manageArrivedMessage(EOpacket* package)
{
    uint8_t *data = NULL;
    uint16_t size = 0;
    eo_packet_Payload_Get(rxpkt_,&data,&size);
    EOMDiagnosticUdpMsg msg;
    msg.parse(data,size);

   for(uint16_t index=0;index<msg.getCurrentRopNumber();++index)
    {
        EOMDiagnosticRopMsg rop;
        msg.getRop(rop,index);
        switch(rop.getCode())
        {
            case DiagnosticRopCode::empty:
                break;            
            case DiagnosticRopCode::startlog:
                traceIsActive_=true;
                hal_trace_puts("Start log msg rx from pc104");
                break;
            case DiagnosticRopCode::stoplog:
                traceIsActive_=false;
                hal_trace_puts("Stop log msg rx from pc104");
                break;
            case DiagnosticRopCode::forceflush:
                forceFlush_=true;
                hal_trace_puts("Force flush msg rx from pc104");
                break;                            
            case DiagnosticRopCode::unforceflush:
                forceFlush_=false;
                hal_trace_puts("Unforce flush msg rx from pc104");
                break;                                        
            default:
                hal_trace_puts("ERROR - Unknown msg rx from pc104");
                EOMDiagnosticRopMsg toSend(EOMDiagnosticRopMsg::Info{(uint16_t)DiagnosticRopCode::diag,(uint16_t)DiagnosticRopSeverity::error,(uint16_t)DiagnosticRopString::wrongcommand,2,3,4,5,6,0});                            
                sendDiagnosticMessage(toSend,false);
                break;            
        }
    }

    return true;
}

eOresult_t EOMtheEMSDiagnostic::transmitUdpPackage()
{
#if 1
    if(!traceIsActive_)
        return eores_OK;
    
    //uint64_t start = osal_system_abstime_get();    
     
    {
        lock_guard<EOVmutexDerived> lock(mutexBody_);

        if(0 == eo_packet_Size_Get(txpkt2_))
        {
            return eores_OK;//nothing to transmit
        }
    }
//    eo_packet_Full_LinkTo(txpkt_, remoteAddr_, remotePort_,EOMDiagnosticUdpMsg::getSize(), udpMsgRaw_.data());

    //uint64_t end = osal_system_abstime_get();    
    //hal_trace_puts(std::to_string(end-start).c_str());     

    
    eOresult_t res;

    eo_packet_Addressing_Set(txpkt2_, remoteAddr_, remotePort_);

    if((!connected2host_) || (remoteAddr_ != hostaddress_))
    {
        res = connect(remoteAddr_);
        if(eores_OK != res)
        {
            hal_trace_puts("ERROR - connect");
            return(res);
        }
    }

    res = eo_socketdtg_Put(socket_, txpkt2_);
    
    eOresult_t tmp=res;
    if(res!=eores_OK)
        hal_trace_puts("ERROR - Udp msg not sent.");
    else
    {
        //Cambiare Luca
        lock_guard<EOVmutexDerived> lock(mutexBody_);
        eo_packet_Size_Set(txpkt2_, 0);        
    }
    return(res);
#else
    
    if(!traceIsActive_)
        return eores_OK;
    
    //uint64_t start = osal_system_abstime_get();    
     
    {
        lock_guard<EOVmutexDerived> lock(mutexBody_);
        if(!txUdpMsg_.createUdpPacketData(udpMsgRaw_))
            return eores_OK;//nothing to transmit
    }
    eo_packet_Full_LinkTo(txpkt_, remoteAddr_, remotePort_,EOMDiagnosticUdpMsg::getSize(), udpMsgRaw_.data());

    //uint64_t end = osal_system_abstime_get();    
    //hal_trace_puts(std::to_string(end-start).c_str());     

    
    eOresult_t res;

    eo_packet_Addressing_Set(txpkt_, remoteAddr_, remotePort_);

    if((!connected2host_) || (remoteAddr_ != hostaddress_))
    {
        res = connect(remoteAddr_);
        if(eores_OK != res)
        {
            hal_trace_puts("ERROR - connect");
            return(res);
        }
    }

    res = eo_socketdtg_Put(socket_, txpkt_);
    
    eOresult_t tmp=res;
    if(res!=eores_OK)
        hal_trace_puts("ERROR - Udp msg not sent.");
    else
    {
        //Cambiare Luca
        lock_guard<EOVmutexDerived> lock(mutexBody_);
        txUdpMsg_.reset();        
    }
    return(res);
#endif
}

eOresult_t EOMtheEMSDiagnostic::connect(eOipv4addr_t remaddr)
{
    eOresult_t res = eores_OK;

    if((eobool_false == connected2host_) || (remaddr != hostaddress_))
    {
        connected2host_ = eobool_false;
        hostaddress_    = remaddr;
        
        res = eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), remaddr, 5*EOK_reltime1sec);
 
        if(eores_OK != res)
        {
                        hal_trace_puts("ERROR - eom_ipnet_ResolveIP");
            return(res);
        }
        connected2host_ = eobool_true;        
    }
    
    return(eores_OK);   
}

bool EOMtheEMSDiagnostic::sendDiagnosticMessage(EOMDiagnosticRopMsg& msg,bool flush)
{   
    bool out;
    {
        msg.data_.time_=osal_system_abstime_get();
        lock_guard<EOVmutexDerived> lock(mutexBody_);
        out=txUdpMsg_.addRop(msg);
    }
    
    if(flush || forceFlush_)
    {
        eom_task_SetEvent(task_, diagnosticEvent_evt_packet_tobesent); 
    }
    
    return out;
}

bool EOMtheEMSDiagnostic::sendDiagnosticMessage(EOMDiagnosticRopMsg::Info& info,bool flush)
{       
    bool out;
    {
        info.time_=osal_system_abstime_get();
        lock_guard<EOVmutexDerived> lock(mutexBody_);
        out=txUdpMsg_.addRop(info);
    }
    if(flush || forceFlush_)
    {
        eom_task_SetEvent(task_, diagnosticEvent_evt_packet_tobesent); 
    }
    
    return out;
}

EOMDiagnosticRopMsg& EOMtheEMSDiagnostic::getRopForModify(bool res)
{
    //Not to be used
    return txUdpMsg_.getRopForModify(res);
}

extern "C"
{
    void initDiagnostic()
    {
        EOMtheEMSDiagnostic::Params param;
        EOMtheEMSDiagnostic::instance().initialise(param);
    }
}

bool EOMtheEMSDiagnostic::send(const embot::eprot::diagnostics::InfoBasic &ib, bool flush)
{   
    bool ret = true;
    
    node->add(ib);
    
    if(flush)
    {
        size_t sizeofropframe = 0;
        if(true == node->prepare(sizeofropframe))
        {
            if(sizeofropframe <= rawcapacity)
            {
                embot::utils::Data datainropframe { rawdata, rawcapacity};
                bool thereisarop = node->retrieve(datainropframe);   
                eo_packet_Full_LinkTo(txpkt2_, remoteAddr_, remotePort_, sizeofropframe, datainropframe.getU08ptr());  
#if defined(TEST_DECODE)
                // i must ... 
                decoder->decode(datainropframe.getU08ptr(), sizeofropframe); // default ipv4 = 10.0.1.98 
#endif                  
                eom_task_SetEvent(task_, diagnosticEvent_evt_packet_tobesent);                
            }
                

            //etc.
        }        
        
    }
    

//    {
//        msg.data_.time_=osal_system_abstime_get();
//        lock_guard<EOVmutexDerived> lock(mutexBody_);
//        out=txUdpMsg_.addRop(msg);
//    }
//    
//    if(flush || forceFlush_)
//    {
//        eom_task_SetEvent(task_, diagnosticEvent_evt_packet_tobesent); 
//    }
//    
    return ret;
}


#if defined(TEST_DECODE)
// pimpl of decode

    #include "DiagnosticsHost.h" 
    #include "EoError.h"


struct Decoder::Impl
{
    bool _initted {false};
    Config _config {};
    embot::app::DiagnosticsHost *_host {nullptr};
    embot::app::DiagnosticsHost::Config _configdiaghost { false, 513, ropdecode};
      

    Impl() : _host(new embot::app::DiagnosticsHost)
    {
    }
        
    ~Impl()
    {
        delete _host;            
    }
            
    static bool ropdecode(const embot::eprot::IPv4 &ipv4, const embot::eprot::rop::Descriptor &rop)
    {       
        // in here we just print out, hence we use a string // or a std:;string
        char textout[128] = {0};
        
        // i accept only sig<>
        if(embot::eprot::rop::OPC::sig != rop.opcode)
        {
            return false;
        }
        
        switch(rop.id32)
        {
            
            case embot::eprot::diagnostics::InfoBasic::id32:
            {
                embot::eprot::diagnostics::InfoBasic *ib = reinterpret_cast<embot::eprot::diagnostics::InfoBasic*>(rop.value.getU08ptr());

                uint64_t tt = ib->timestamp;
                uint32_t sec = tt/(1000*1000);
                uint32_t tmp = tt%(1000*1000);
                uint32_t msec = tmp / 1000;
                uint32_t usec = tmp % 1000;                    
                const char *text = eoerror_code2string(ib->code); 
                char buf[16] = {0};
                if(rop.hassignature() || rop.hastime())
                {
                    snprintf(textout, sizeof(textout), "from %s [sig = 0x%x, tim = %lld] -> @ s%d m%d u%d -> %s", ipv4.tostring(buf, sizeof(buf)), rop.signature, rop.time, sec, msec, usec, text);
                }
                else
                {
                    snprintf(textout, sizeof(textout), "from %s [no sig, no tim] -> @ s%d m%d u%d: %s", ipv4.tostring(buf, sizeof(buf)), sec, msec, usec, text);
                }
                
                // and use also ib->flags + ib->par16 + ib->par64
                
                hal_trace_puts(textout);
            } break;
            
            default:
            {
                hal_trace_puts("unknown");
            } break;
        }
           
        return true;
    }
    
        
    bool init(const Config &config)
    {  
        if(initted())
        {
            return false;
        }
        
        if(false == config.isvalid())
        {
            return false;
        } 
        
        _config = config;
        
        _configdiaghost.ropcapacity = _config.ropcapacity;
        _host->init(_configdiaghost);     
        
        _initted = true;
        
        return true;        
    }
    
    bool initted() const
    {
        return _initted;
    }
        
    bool decode(uint8_t *ropframe, uint16_t sizeofropframe, const embot::eprot::IPv4 &ipv4 = {"10.0.1.98"})
    {
        if(!initted())
        {
            return false;
        }
        
        embot::utils::Data data(ropframe, sizeofropframe);
        return _host->accept(ipv4, data);        
    }  
    
};

Decoder::Decoder()
: pImpl(new Impl)
{ 
}

Decoder::~Decoder()
{   
    delete pImpl;
}

bool Decoder::init(const Config &config)
{
    return pImpl->init(config);
}

bool Decoder::initted() const
{
    return pImpl->initted();
}

bool Decoder::decode(uint8_t *ropframe, uint16_t sizeofropframe, const embot::eprot::IPv4 &ipv4)
{
    return pImpl->decode(ropframe, sizeofropframe, ipv4);
}
   
#endif // #if defined(TEST_DECODE)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

