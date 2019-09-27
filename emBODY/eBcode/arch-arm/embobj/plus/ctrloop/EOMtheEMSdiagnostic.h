/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Luca Tricerri
 * email:   marco.accame@iit.it,luca.tricerri@iit.it
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


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMTHEEMSDIAGNOSTIC_H_
#define _EOMTHEEMSDIAGNOSTIC_H_

/** @file       EOMtheEMSDiagnostic.h
	@brief      This header file implements public interface to the EMS diagnostic singleton.
 	@author     luca.tricerri@iit.it
	@date       01/08/2019
 **/

/** @defgroup eom_theEMSDiagnostic Singleton EOMtheEMSDiagnostic 
    
    The .....  
  
    @{		
 **/

#include <array>

#include "EoCommon.h"
#include "EOpacket.h"
#include "EOMtask.h"
#include "EOsocketDatagram.h"

#include "EOMDiagnosticRopMsg.h"
#include "EOMDiagnosticUdpHeader.h"
#include "EOMDiagnosticUdpFooter.h"

class EOMtheEMSDiagnostic
{
    public:
	
		//constexpr static uint8_t	txpktSize_{10};
		//using DiagnosticMsg=std::array<uint8_t,txpktSize_>;
		
		class Params
		{
			public:
			uint8_t taskpriority_{41};
			uint16_t taskstacksize_{2048};  
			eOsizecntnr_t inpdatagramnumber_{2};
			eOsizecntnr_t outdatagramnumber_{1};
			eOsizeitem_t inpdatagramsizeof_{64};
			eOsizeitem_t outdatagramsizeof_{64};
			eObool_t usemutex_{eobool_true};   
		};
		
		enum DiagnosticEvents :uint32_t
		{   // must be single bits: 1, 2, 4, 8, 16, 32, 64 etc.
			timeout = 0,
			diagnosticConfigEvent_evt_packet_received = 1,
			diagnosticEvent_evt_packet_send = 2,
		};

    static EOMtheEMSDiagnostic& instance();
    bool initialise(const Params &emsbackdoortransceivercfg);
		
		bool sendDiagnosticMessage(void *msg,uint8_t size);
		bool sendDiagnosticMessage(EOMDiagnosticRopMsg& msg);
   	
    private:

    EOMtheEMSDiagnostic();
    EOMtheEMSDiagnostic(const EOMtheEMSDiagnostic&)=delete;
    EOMtheEMSDiagnostic(EOMtheEMSDiagnostic&&)=delete;
    EOMtheEMSDiagnostic& operator=(const EOMtheEMSDiagnostic&)=delete;
    EOMtheEMSDiagnostic& operator=(EOMtheEMSDiagnostic&)=delete;
		
		eOresult_t connect(eOipv4addr_t remaddr);			

		//task
    void taskStartup(EOMtask*,unsigned int);
    void taskRun(EOMtask*,unsigned int);			
		uint8_t wakeupTask_{10};

		Params params_;
		
		EOMtask* task_{nullptr};		
		EOsocketDatagram* socket_{nullptr};		  
		EOpacket *rxpkt_{nullptr};

		//buffer
		constexpr static uint8_t	txBuffersizeSize_{10};
		std::array<EOMDiagnosticRopMsg,txBuffersizeSize_> txBuffer_;
		uint8_t currentBufferSize_{0};
		void transmitTest();
			
		//udp packet
		constexpr static uint16_t getSize(){return (EOMDiagnosticUdpHeader::getSize()+EOMDiagnosticRopMsg::getSize()+EOMDiagnosticUdpFooter::getSize());};
		EOpacket* txpkt_{nullptr};
		constexpr static uint16_t	udpPacketDataSize_{52*txBuffersizeSize_};//(EOMDiagnosticUdpHeader::getSize()+EOMDiagnosticRopMsg::getSize()+EOMDiagnosticUdpFooter::getSize())=52};
		EOMDiagnosticUdpHeader header_;
		EOMDiagnosticUdpFooter footer_;
		std::array<uint8_t,udpPacketDataSize_>	udpPacketData_;
		bool createUdpPacketData();
		bool createUdpHeader();
		bool createUdpBody();		
		bool createUdpFooter();
		
		//process event
		void processEventRxPacket();
		eOresult_t transmitUdpPackage();
		bool manageArrivedMessage(EOpacket*);
    
		bool connected2host_{false};  
		
    eOipv4addr_t hostaddress_{EO_COMMON_IPV4ADDR_LOCALHOST};
		const eOipv4addr_t remoteAddr_{EO_COMMON_IPV4ADDR(10,0,1,104)};
    const eOipv4port_t remotePort_{11000};
		const eOipv4port_t localPort_{11000};
					
		static constexpr char s_eobj_ownname[]{"EOMtheEMSdiagnostic"};		
};

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
