<<<<<<< HEAD
/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
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


#include "hal_trace.h"


extern void tskEMScfg(void *p)
{
	 eom_task_Start((EOMtask*)p);
}

EOMtheEMSDiagnostic::EOMtheEMSDiagnostic()
{
	txpkt_=eo_packet_New(0);	
	eo_packet_Full_LinkTo(txpkt_, remoteAddr_, remotePort_, udpPacketDataSize_, udpPacketData_.data());
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

    // create the socket    
    socket_ = eo_socketdtg_New(cfg.inpdatagramnumber_, cfg.inpdatagramsizeof_, (eobool_true == cfg.usemutex_) ? (eom_mutex_New()) : (NULL), 
                                                               cfg.outdatagramnumber_, udpPacketDataSize_, (eobool_true == cfg.usemutex_) ? (eom_mutex_New()) : (NULL)
                                                              );    
    // create the rx packet
    rxpkt_ = eo_packet_New(cfg.inpdatagramsizeof_);
     
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


void EOMtheEMSDiagnostic::taskStartup(EOMtask*,unsigned int)
{
    // open the socket
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, diagnosticConfigEvent_evt_packet_received, task_);
    
    eo_socketdtg_Open(socket_, localPort_, eo_sktdir_TXRX, eobool_false,NULL, onrx, NULL);   
    
    eo_errman_Trace(eo_errman_GetHandle(), "called _task_startup()", s_eobj_ownname);                            

}

void EOMtheEMSDiagnostic::taskRun(EOMtask*,unsigned int t)
{
	// the event that we have received
    eOevent_t evt = (eOevent_t)t;

    if(!evt)
		{
			transmitUdpPackage();		
		}
  
    if(eobool_true == eo_common_event_check(evt, diagnosticConfigEvent_evt_packet_received))
		{
			processEventRxPacket();
		}

    if(eobool_true == eo_common_event_check(evt, diagnosticEvent_evt_packet_send))
		{
			transmitUdpPackage();
		}
				
		//hal_trace_puts("taskRun end");				
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

bool EOMtheEMSDiagnostic::manageArrivedMessage(EOpacket*)
{
	transmitTest();
	return true;
}

eOresult_t EOMtheEMSDiagnostic::transmitUdpPackage()
{
		if(!currentBufferSize_)
			return eores_OK;//nothing to transmit
		
		createUdpPacketData();
    
	  eOresult_t res;
    
    eo_packet_Addressing_Set(txpkt_, remoteAddr_, remotePort_);
    
    if((!connected2host_) || (remoteAddr_ != hostaddress_))
    {
        res = connect(remoteAddr_);
        if(eores_OK != res)
        {
            return(res);
        }
    }
    
    res = eo_socketdtg_Put(socket_, txpkt_);
    
		currentBufferSize_=0;
		udpPacketData_.fill(0);
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
            return(res);
        }
        connected2host_ = eobool_true;        
    }
    
    return(eores_OK);   
}

bool EOMtheEMSDiagnostic::sendDiagnosticMessage(void *msg,uint8_t size)
{
	if(size>EOMDiagnosticRopMsg::getSize())
	{
		return false;//TODO
	}
	
	if(currentBufferSize_>txBuffersizeSize_)	
	{
		return false;//TODO
	}

	EOMDiagnosticRopMsg tmp(msg,size);
	
	//mutex TODO
	txBuffer_.at(currentBufferSize_)=tmp;
	currentBufferSize_++;
	return true;
}

bool EOMtheEMSDiagnostic::sendDiagnosticMessage(EOMDiagnosticRopMsg& msg)
{
	if(currentBufferSize_>txBuffersizeSize_)	//si puo' evitare
	{
		return false;//TODO
	}

	//mutex TODO
	txBuffer_.at(currentBufferSize_)=msg;
	currentBufferSize_++;
	return true;
}

void EOMtheEMSDiagnostic::transmitTest()
{
	EOMDiagnosticRopMsg toSend(EOMDiagnosticRopMsg::Info{1,2,3,4,5,6,0,0,7});
	sendDiagnosticMessage(toSend);
	
	EOMDiagnosticRopMsg toSend1(EOMDiagnosticRopMsg::Info{10,20,30,40,50,60,0,0,70});
	sendDiagnosticMessage(toSend1);
	
	transmitUdpPackage();
}


bool EOMtheEMSDiagnostic::createUdpPacketData()
{
	header_.updateHeader(currentBufferSize_*EOMDiagnosticRopMsg::getSize(),currentBufferSize_,0);
	
	createUdpHeader();
	createUdpBody();
	createUdpFooter();
	
	eo_packet_Full_LinkTo(txpkt_, remoteAddr_, remotePort_, udpPacketDataSize_, udpPacketData_.data());
	return true;
}

bool EOMtheEMSDiagnostic::createUdpHeader()
{
	std::memcpy(udpPacketData_.data(),header_.data(),EOMDiagnosticUdpHeader::getSize());
	return true;
}
			
bool EOMtheEMSDiagnostic::createUdpFooter()
{
	uint16_t currentFooterAddress=currentBufferSize_*EOMDiagnosticRopMsg::getSize()+EOMDiagnosticUdpHeader::getSize();
	std::memcpy(udpPacketData_.data()+currentFooterAddress,footer_.data(),EOMDiagnosticUdpFooter::getSize());
	return true;	
}

bool EOMtheEMSDiagnostic::createUdpBody()
{
	//TODO mutex
	uint16_t currentROPStartAddress=EOMDiagnosticUdpHeader::getSize();
	for(int index=0;index<currentBufferSize_;++index)
	{
		currentROPStartAddress=currentROPStartAddress+index*EOMDiagnosticRopMsg::getSize();
		int tmp=EOMDiagnosticRopMsg::getSize();
		tmp=sizeof(EOMDiagnosticRopMsg::Info);
		std::memcpy(udpPacketData_.data()+currentROPStartAddress,txBuffer_[index].data(),EOMDiagnosticRopMsg::getSize());
	}	
	return true;	
}



=======
/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


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

#include "EOMtheEMSDiagnostic.hpp"

void tskEMScfg(void *p){};


EOMtheEMSDiagnostic& EOMtheEMSDiagnostic::instance()
{
    static EOMtheEMSDiagnostic instance;
    return instance;
}

bool EOMtheEMSDiagnostic::initialise(const Params& cfg)
{
		params_=cfg;
//    if(nullptr == cfg)
//    {
//        cfg = &eom_emsdiscoverylistener_DefaultCfg;
//    }
//    
//    memcpy(&s_emsdiscoverylistener_singleton.cfg, cfg, sizeof(eOemsdiscoverylistener_cfg_t));
    
    
    if(NULL == eom_ipnet_GetHandle())
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "eom_emsdiscoverylistener_Initialise(): EOMtheIPnet not started yet", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }

    // create the socket    
    socket_ = eo_socketdtg_New(cfg.inpdatagramnumber, cfg.inpdatagramsizeof, (eobool_true == cfg.usemutex) ? (eom_mutex_New()) : (NULL), 
                                                               cfg.outdatagramnumber, cfg.outdatagramsizeof, (eobool_true == cfg.usemutex) ? (eom_mutex_New()) : (NULL)
                                                              );    
    // create the rx packet
    rxpkt_ = eo_packet_New(cfg.inpdatagramsizeof);
    
    	   
    // create the task
    task_ = eom_task_New(eom_mtask_EventDriven, 
													cfg.taskpriority, 
													cfg.taskstacksize, 
													[](EOMtask* task ,uint32_t t){EOMtheEMSDiagnostic::instance().taskStartup(task,t);},
													[](EOMtask* task ,uint32_t t){EOMtheEMSDiagnostic::instance().taskRun(task,t);},  
													(eOevent_t)0, eok_reltimeINFINITE, NULL, 
													tskEMScfg, "tskEMSlis");
 
    return true;                                                
}


void EOMtheEMSDiagnostic::taskStartup(EOMtask*,unsigned int)
{
    // open the socket
    EOaction_strg astg = {0};
    EOaction *onrx = (EOaction*)&astg;
    eo_action_SetEvent(onrx, diagnosticEvent_evt_packet_received, task_);
    
    eo_socketdtg_Open(socket_, params_.localport, eo_sktdir_TXRX, eobool_false,NULL, onrx, NULL);   
    
    eo_errman_Trace(eo_errman_GetHandle(), "called _task_startup()", s_eobj_ownname);                            
}

void EOMtheEMSDiagnostic::taskRun(EOMtask*,unsigned int t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;
    EOpacket* rxpkt = rxpkt_;
    EOpacket* replypkt = NULL;
    eOsizecntnr_t remainingrxpkts = 0;    

    
    if(eobool_true != eo_common_event_check(evt, diagnosticEvent_evt_packet_received))
			return;
    
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
				eom_task_SetEvent(task_, diagnosticEvent_evt_packet_received); 
		} 
}

bool EOMtheEMSDiagnostic::manageArrivedMessage(EOpacket*)
{
	// In the case send also the replay
	return true;
}


eOresult_t EOMtheEMSDiagnostic::transmit(EOpacket* txpkt)
{
    eOresult_t res;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);
    
    if((eobool_false == connected2host_) || (remaddr != hostaddress_))
    {
        res = connect(remaddr);
        if(eores_OK != res)
        {
            return(res);
        }
    }
    
    res = eo_socketdtg_Put(socket_, txpkt);
    
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
            return(res);
        }
        connected2host_ = eobool_true;        
    }
    
    return(eores_OK);   
}


>>>>>>> Add files via upload
