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

#include <stdio.h>
#include <stdlib.h>

#include "lock_guard"

extern void tskEMScfg(void *p)
{
    eom_task_Start((EOMtask*)p);
}
EOMtheEMSDiagnostic::EOMtheEMSDiagnostic()
{
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

    //Mutex init
    mutexNode_=eom_mutex_New();
    mutexUdpPackage_=eom_mutex_New();
           
    txpkt_ = eo_packet_New(0);
    eo_packet_Size_Set(txpkt_, 0); 
 
    rawdata = new uint8_t[rawcapacity_];    
    
    embot::app::DiagnosticsNode::Config config {}; // to be filled properly after
    node_ = new embot::app::DiagnosticsNode;
    node_->init(config);    
        
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
    //TODO add RX msg code

    return true;
}

eOresult_t EOMtheEMSDiagnostic::transmitUdpPackage()
{
    if(!traceIsActive_)
        return eores_OK;//no transmit necessary
    
    if(node_->getNumberOfROPs()==0)
    {
        return eores_OK;//nothing to transmit
    }

    //Manage connection
    eOresult_t res;
    if((!connected2host_) || (remoteAddr_ != hostaddress_))
    {
        res = connect(remoteAddr_);
        if(eores_OK != res)
        {
            hal_trace_puts("ERROR - connect");
            return(res);
        }
    }
    
    //Manage packet to send
    {
        size_t sizeofropframe = 0;
        lock_guard<EOVmutexDerived> lockNode(mutexNode_);        
        if(!node_->prepare(sizeofropframe))
        {
            hal_trace_puts("ERROR - prepare ROP");
            return eores_NOK_generic;
        }
        
        if(sizeofropframe > rawcapacity_)
        {
            hal_trace_puts("ERROR - size ROP");
            return eores_NOK_generic;  //TODO something
        }    
        
        embot::utils::Data datainropframe { rawdata, rawcapacity_};
        bool thereisarop = node_->retrieve(datainropframe);   
        
        lock_guard<EOVmutexDerived> lockUdp(mutexUdpPackage_);                
        eo_packet_Full_LinkTo(txpkt_, remoteAddr_, remotePort_, sizeofropframe, datainropframe.getU08ptr());                
        eo_packet_Addressing_Set(txpkt_, remoteAddr_, remotePort_);
        res = eo_socketdtg_Put(socket_, txpkt_);   
        
        eo_packet_Size_Set(txpkt_, 0);        
    }        
    if(res!=eores_OK)
        hal_trace_puts("ERROR - Udp msg not sent."); //TODO something
    return(res);
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


bool EOMtheEMSDiagnostic::send(const embot::eprot::diagnostics::InfoBasic &ib, bool flush)
{   
    {
        lock_guard<EOVmutexDerived> lock(mutexNode_);
        bool res=node_->add(ib);
        if(!res)
        {
            hal_trace_puts("ERROR - ROP not added.");//TODO something
            return false;
        }
    }
    if(flush || forceFlush_)
    {  
        eom_task_SetEvent(task_, diagnosticEvent_evt_packet_tobesent);          
    }   
    return true;
}  

extern "C"
{
    void initDiagnostic()
    {
        EOMtheEMSDiagnostic::Params param;
        EOMtheEMSDiagnostic::instance().initialise(param);
    }
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

