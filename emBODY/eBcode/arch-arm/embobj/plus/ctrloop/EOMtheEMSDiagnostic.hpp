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

#pragma once


/** @file       EOMtheEMSbackdoortransceiver.h
	@brief      This header file implements public interface to the EMS diagnostic singleton.
 	@author     luca.tricerri@iit.it
	@date       01/08/2019
 **/

/** @defgroup eom_theemsbackdoortransceiver Singleton EOMtheEMSbackdoortransceiver 
    
    The .....  
  
    @{		
 **/

#include "EoCommon.h"

typedef struct EOpacket_hid EOpacket;
typedef struct EOsocketDatagram_hid EOsocketDatagram;
typedef struct EOMtask_hid EOMtask;

class EOMtheEMSDiagnostic
{
    public:

    enum class Protocols : unsigned int
    {
        eobackdoor_protocol_userdef_opc = 0
    } ;
		
		enum DiagnosticEvents :uint32_t
		{   // must be single bits: 1, 2, 4, 8, 16, 32, 64 etc.
			diagnosticEvent_evt_packet_received = 1,
		};

    class Params
    {
        public:

        uint8_t                     taskpriority{41};
        uint16_t                    taskstacksize{512};   
        eOsizecntnr_t               inpdatagramnumber{2};
        eOsizecntnr_t               outdatagramnumber{1};
        eOsizeitem_t                inpdatagramsizeof{64};
        eOsizeitem_t                outdatagramsizeof{64};
        eOipv4port_t                localport{3334};
        eOipv4port_t                remoteport{3334};
        eOipv4addr_t                remoteipaddr{EO_COMMON_IPV4ADDR_LOCALHOST};
        eObool_t                    usemutex{eobool_true};   
        //eOemsdiscovery_protocol_t   discoveryprotocol{eodiscovery_protocol_ethloader_reduced};       
    };

    static EOMtheEMSDiagnostic& instance();

    bool initialise(const Params &emsbackdoortransceivercfg);
   
    EOMtheEMSDiagnostic(const EOMtheEMSDiagnostic&)=delete;
    EOMtheEMSDiagnostic(EOMtheEMSDiagnostic&&)=delete;
    EOMtheEMSDiagnostic& operator=(const EOMtheEMSDiagnostic&)=delete;
    EOMtheEMSDiagnostic& operator=(EOMtheEMSDiagnostic&)=delete;

    void taskStartup(EOMtask*,unsigned int);
    void taskRun(EOMtask*,unsigned int);

    private:

    EOMtheEMSDiagnostic(){};
		
		bool manageArrivedMessage(EOpacket*);
		eOresult_t transmit(EOpacket* txpkt);
		eOresult_t connect(eOipv4addr_t remaddr);			

    Params              params_;
    EOMtask*            task_{nullptr};
    EOsocketDatagram*   socket_{nullptr};
    EOpacket            *rxpkt_{nullptr};
    EOpacket            *replypkt_{nullptr};
    eOipv4addr_t        hostaddress_{EO_COMMON_IPV4ADDR_LOCALHOST};
    eObool_t            connected2host_{eobool_false};  
		
		static constexpr char s_eobj_ownname[]{"EOMtheEMSdiscoverylistener"};		
};