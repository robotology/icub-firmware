/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       eom_emsappl_extra.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEmemorymap.h"
#include "EOVtheSystem.h"
#include "EOMtask.h"
#include "EOpacket.h"
#include "EOsocketDatagram.h"
#include "EOMmutex.h"
#include "EOaction_hid.h"
#include "EOtheErrorManager.h"

#include "osal.h"
#include "hal.h"
#include "ipal.h"

#include "stdlib.h"

#include "eom_emsappl_specialise.h"


// -- nvs part
#define USE_EB2

// all that is enough for the local board

#if     defined(USE_EB2)
#include "eOcfg_EPs_eb2.h"
#elif   defined(USE_EB7)
#include "eOcfg_EPs_eb7.h"
#endif

#include "EOtheBOARDtransceiver.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_emsappl_extra.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define     USE_PROTOCOLTRANSCEIVER
#undef      USE_ECHOER
#undef      USE_ASIMM


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(USE_ECHOER) || defined(USE_ASIMM)
static void s_eom_emsappl_extra_echoer_asimm_init(void);
#endif

#if     defined(USE_ECHOER)
static void s_eom_emsappl_extra_echoer_receive(EOpacket* rxpkt);
#endif

#if     defined(USE_ASIMM)
static void s_eom_emsappl_extra_asimm_receive(EOpacket* rxpkt);
#endif


#if     defined(USE_PROTOCOLTRANSCEIVER)
static void s_eom_emsappl_extra_protocoltransceiver_init(void);
static void s_eom_emsappl_extra_protocoltransceiver_receive(EOpacket* rxpkt);
static void s_eom_emsappl_extra_protocoltransceiver_addoccasionalrop(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

EOpacket* s_mytxpkt = NULL;


// -- nvs part

static EOtransceiver *ems00txrx = NULL;

static const eOipv4addr_t nvs_pc104_ipaddress       = EO_COMMON_IPV4ADDR(10, 0, 0, 2);

static const eOipv4port_t nvs_base_endpoint_iport   = 33333;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsappl_extra_transceiver_init(void)
{    
    // init transceiver
#if     defined(USE_ECHOER) || defined(USE_ASIMM)

    s_eom_emsappl_extra_echoer_asimm_init();

#elif   defined(USE_PROTOCOLTRANSCEIVER)

    s_eom_emsappl_extra_protocoltransceiver_init();

#endif


    // impose that on rx pkt the tasl udp calls ...

#if     defined(USE_ECHOER) 

    eom_emsappl_specialise_onpktreceived_set(s_eom_emsappl_extra_echoer_receive);

#elif    defined(USE_ASIMM)

    eom_emsappl_specialise_onpktreceived_set(s_eom_emsappl_extra_asimm_receive);

#elif   defined(USE_PROTOCOLTRANSCEIVER)

    eom_emsappl_specialise_onpktreceived_set(s_eom_emsappl_extra_protocoltransceiver_receive);

#endif    

    
       
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(USE_ECHOER) || defined(USE_ASIMM)
static void s_eom_emsappl_extra_echoer_asimm_init(void)
{
    s_mytxpkt = eo_packet_New(512);
    eo_packet_Size_Set(s_mytxpkt, 512);
}
#endif

#if     defined(USE_ASIMM)
static void s_eom_emsappl_extra_echoer_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == eom_emsappl_specialise_connect(remaddr))
    {
        return;
    }


    // then send back teh very same pkt

    eom_emsappl_specialise_transmit(rxpkt);

}
#endif


#if     defined(USE_ASIMM)
static void s_eom_emsappl_extra_asimm_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == eom_emsappl_specialise_connect(remaddr))
    {
        return;
    }


    // then send back teh 260 bytes pkt

    eom_emsappl_specialise_transmit(s_mytxpkt);

}
#endif


#if     defined(USE_PROTOCOLTRANSCEIVER)

static void s_eom_emsappl_extra_protocoltransceiver_init(void)
{
#if     defined(USE_EB2)
    eOboardtransceiver_cfg_t boardtxrxcfg = 
    {
        .vectorof_endpoint_cfg          = eo_cfg_EPs_vectorof_eb2,
        .hashfunction_ep2index          = eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index,
        .remotehostipv4addr             = nvs_pc104_ipaddress,
        .remotehostipv4port             = nvs_base_endpoint_iport,
        .sizes                          = 
        {
            .capacityofpacket               = 1024,
            .capacityofrop                  = 256,
            .capacityofropframeregulars     = 768,
            .capacityofropframeoccasionals  = 128,
            .capacityofropframereplies      = 128,
            .maxnumberofregularrops         = 32
        }
    };    
#elif   defined(USE_EB7)
    eOboardtransceiver_cfg_t boardtxrxcfg = 
    {
        .vectorof_endpoint_cfg          = eo_cfg_EPs_vectorof_eb7,
        .hashfunction_ep2index          = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index,
        .remotehostipv4addr             = nvs_pc104_ipaddress,
        .remotehostipv4port             = nvs_base_endpoint_iport,
        .tobedefined                    = 0
    };    
#endif    
    ems00txrx = eo_boardtransceiver_Initialise(&boardtxrxcfg);

}

static void s_eom_emsappl_extra_protocoltransceiver_receive(EOpacket* rxpkt)
{
    uint16_t numofrops_ems00_rx;
    uint16_t numofrops_ems00_tx;
    eOabstime_t txtime;
    EOpacket *txpkt = NULL;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    // get addressing of received packet
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);


    // -- process a received packet: rxpkt
    eo_transceiver_Receive(ems00txrx, rxpkt, &numofrops_ems00_rx, &txtime);

    // -- add an occasional packet
    s_eom_emsappl_extra_protocoltransceiver_addoccasionalrop();


    // -- retrieve the packet to transmit: txpkt
    eo_transceiver_Transmit(ems00txrx, &txpkt, &numofrops_ems00_tx);


    // -- set the remote address and port 
    eo_packet_Addressing_Set(txpkt, remaddr, remport);

    // -- attempt to connect.
    if(eobool_false == eom_emsappl_specialise_connect(remaddr))
    {
        return;
    }

    // -- assign the packet to the transmitter
    eom_emsappl_specialise_transmit(txpkt);

}

static void s_eom_emsappl_extra_protocoltransceiver_addoccasionalrop(void)
{
    static uint32_t step = 1;

    return;

    // i add an extra rop only once every 5 transmissions

    if(3==(step%10))
    {
        eo_transceiver_ropinfo_t ropinfo;

        ropinfo.ropcfg      = eok_ropconfig_basic;
        ropinfo.ropcode     = eo_ropcode_sig;
        ropinfo.nvep        = endpoint_mc_leftlowerleg;
        ropinfo.nvid        = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)0, jointNVindex_jstatus__ofpid);
        eo_transceiver_rop_occasional_Load(ems00txrx, &ropinfo);

//            snprintf(str, sizeof(str)-1, "               - added a sig<__boardinfo>");
//            hal_trace_puts(str); 
    }



    step += 2;
}


#endif



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



