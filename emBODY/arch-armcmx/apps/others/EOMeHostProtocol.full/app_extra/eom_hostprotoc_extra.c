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

/* @file       eom_hostprotoc_extra.c
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

#include "eom_hostprotoc_specialise.h"



// -- gpio part

#include "EOMtheCallbackManager.h"

#include "EOtheGPIO.h"
#include "eOcfg_GPIO_MCBSTM32c.h"
#include "EOMtheGPIOManager.h"

#include "EOioPinOutputManaged.h"
#include "EOioPinInputManaged.h"
#include "EOioPinOutput.h"


// -- nvs part

// all that is enough for the host board
#include "eOcfg_EPs_eb7.h"
#include "EOhostTransceiver.h"







// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_hostprotoc_extra.h"

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
static void s_eom_hostprotoc_extra_echoer_asimm_init(void);
#endif

#if     defined(USE_ECHOER)
static void s_eom_hostprotoc_extra_echoer_receive(EOpacket* rxpkt);
#endif

#if     defined(USE_ASIMM)
static void s_eom_hostprotoc_extra_asimm_receive(EOpacket* rxpkt);
#endif


#if     defined(USE_PROTOCOLTRANSCEIVER)
static void s_eom_hostprotoc_extra_protocoltransceiver_init(void);
static void s_eom_hostprotoc_extra_protocoltransceiver_receive(EOpacket* rxpkt);
static void s_eom_hostprotoc_extra_protocoltransceiver_addoccasionalrop(void);

static void s_eom_hostprotoc_extra_protocoltransceiver_ask_the_board(void);
static void s_eom_hostprotoc_extra_protocoltransceiver_configure_regular_rops_on_board(void);
static void s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid);
static void s_eom_hostprotoc_extra_protocoltransceiver_transmit_ropframe(void);
#endif



static void s_callback_button_user(void *arg);
static void s_callback_button_tamp(void *arg);
static void s_callback_button_wkup(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

EOpacket* s_mytxpkt = NULL;


// -- nvs part

static EOtransceiver *pc104txrx = NULL;

static const eOipv4addr_t nvs_ems00_ipaddress       = EO_COMMON_IPV4ADDR(10, 0, 0, 3);
//static const eOipv4addr_t nvs_ems00_ipaddress       = EO_COMMON_IPV4ADDR(10, 255, 36, 127);


static const eOipv4port_t nvs_base_endpoint_iport   = 33333;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void eom_hostprotoc_extra_environment_init(void)
{
    EOaction s_fullaction;

    // start gpio handler
    eom_gpioman_Initialise(eo_gpio_Initialise(eo_cfg_gpio_mcbstm32c_Get()), NULL);

    // init three callbacks on USER, TAMP, and WKUP
    eo_action_SetCallback(&s_fullaction, s_callback_button_user, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_iopininpman_ActionOn_Register(eo_iopininpman_GetHandle(iopinID_Iman_mcbstm32c_BUTTON_USER), 
                                     &s_fullaction, eo_iopinTrig_OnRiseStay, 3*eok_reltime100ms);

    eo_action_SetCallback(&s_fullaction, s_callback_button_tamp, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_iopininpman_ActionOn_Register(eo_iopininpman_GetHandle(iopinID_Iman_mcbstm32c_BUTTON_TAMP), 
                                     &s_fullaction, eo_iopinTrig_OnRiseStay, 3*eok_reltime100ms);

    eo_action_SetCallback(&s_fullaction, s_callback_button_wkup, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));
    eo_iopininpman_ActionOn_Register(eo_iopininpman_GetHandle(iopinID_Iman_mcbstm32c_BUTTON_WKUP), 
                                     &s_fullaction, eo_iopinTrig_OnRiseStay, 3*eok_reltime100ms);

}

extern void eom_hostprotoc_extra_transceiver_init(void)
{    
    // init transceiver
#if     defined(USE_ECHOER) || defined(USE_ASIMM)

    s_eom_hostprotoc_extra_echoer_asimm_init();

#elif   defined(USE_PROTOCOLTRANSCEIVER)

    s_eom_hostprotoc_extra_protocoltransceiver_init();

#endif


    // impose that on rx pkt the tasl udp calls ...

#if     defined(USE_ECHOER) 

    eom_hostprotoc_specialise_onpktreceived_set(s_eom_hostprotoc_extra_echoer_receive);

#elif    defined(USE_ASIMM)

    eom_hostprotoc_specialise_onpktreceived_set(s_eom_hostprotoc_extra_asimm_receive);

#elif   defined(USE_PROTOCOLTRANSCEIVER)

    eom_hostprotoc_specialise_onpktreceived_set(s_eom_hostprotoc_extra_protocoltransceiver_receive);

#endif    

    
       
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(USE_ECHOER) || defined(USE_ASIMM)
static void s_eom_hostprotoc_extra_echoer_asimm_init(void)
{
    s_mytxpkt = eo_packet_New(512);
    eo_packet_Size_Set(s_mytxpkt, 512);
}
#endif

#if     defined(USE_ASIMM)
static void s_eom_hostprotoc_extra_echoer_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == eom_hostprotoc_specialise_connect(remaddr))
    {
        return;
    }


    // then send back teh very same pkt

    eom_hostprotoc_specialise_transmit(rxpkt);

}
#endif


#if     defined(USE_ASIMM)
static void s_eom_hostprotoc_extra_asimm_receive(EOpacket* rxpkt)
{
    eOipv4addr_t remaddr;
    eOipv4port_t remport;

    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    // at first attempt to connect.

    if(eobool_false == eom_hostprotoc_specialise_connect(remaddr))
    {
        return;
    }


    // then send back teh 260 bytes pkt

    eom_hostprotoc_specialise_transmit(s_mytxpkt);

}
#endif


#if     defined(USE_PROTOCOLTRANSCEIVER)

static void s_eom_hostprotoc_extra_protocoltransceiver_init(void)
{
    eOhosttransceiver_cfg_t hosttxrxcfg = 
    {
        .vectorof_endpoint_cfg          = eo_cfg_EPs_vectorof_eb7,
        .hashfunction_ep2index          = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index,
        .remoteboardipv4addr            = nvs_ems00_ipaddress,
        .remoteboardipv4port            = nvs_base_endpoint_iport,
        .tobedefined                    = 0
    };  

    EOhostTransceiver *hosttxrx = NULL;

    hosttxrx = eo_hosttransceiver_New(&hosttxrxcfg);
    pc104txrx = eo_hosttransceiver_Transceiver(hosttxrx);

}

static void s_eom_hostprotoc_extra_protocoltransceiver_receive(EOpacket* rxpkt)
{
    uint16_t numofrops_ems00_rx;
    static uint32_t count = 0;
//    uint16_t numofrops_ems00_tx;
    eOabstime_t txtime;
//    EOpacket *txpkt = NULL;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    char str[128];
    uint8_t *ipv4array;

    // get addressing of received packet
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);

    ipv4array = (uint8_t*)&remaddr;

    snprintf(str, sizeof(str)-1, "xxx: cont = %d --> received from ip = %d.%d.%d.%d", count++, ipv4array[0], ipv4array[1], ipv4array[2], ipv4array[3]); 
    hal_trace_puts(str);


    // -- process a received packet: rxpkt
    eo_transceiver_Receive(pc104txrx, rxpkt, &numofrops_ems00_rx, &txtime);

    snprintf(str, sizeof(str)-1, "zzz - a total of %d rops", numofrops_ems00_rx); 
    hal_trace_puts(str);


//    // -- add an occasional packet
//    s_eom_hostprotoc_extra_protocoltransceiver_addoccasionalrop();
//
//
//    // -- retrieve the packet to transmit: txpkt
//    eo_transceiver_Transmit(ems00txrx, &txpkt, &numofrops_ems00_tx);
//
//
//    // -- set the remote address and port 
//    eo_packet_Addressing_Set(txpkt, remaddr, remport);

//    // -- attempt to connect.
//    if(eobool_false == eom_hostprotoc_specialise_connect(remaddr))
//    {
//        return;
//    }
//
//    // -- assign the packet to the transmitter
//    eom_hostprotoc_specialise_transmit(txpkt);

}

static void s_eom_hostprotoc_extra_protocoltransceiver_addoccasionalrop(void)
{
    static uint32_t step = 1;
    char str[128];


    if(4==(step%10))
    {

        eo_transceiver_ropinfo_t ropinfo;

        ropinfo.ropcfg      = eok_ropconfig_basic;
        ropinfo.ropcode     = eo_ropcode_ask;
        ropinfo.nvep        = endpoint_mc_leftlowerleg;
        ropinfo.nvid        = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)0, jointNVindex_jconfig__motionmonitormode);
        
        eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);

        snprintf(str, sizeof(str)-1, "               - added a ask<__jcfg_motionmonitormode j0>");
        hal_trace_puts(str); 

    }
    else if(8==(step%10))
    {
        eo_transceiver_ropinfo_t ropinfo;
        eo_cfg_nvsEP_mc_lowerleg_t* locllleg = eo_cfg_nvsEP_eb7_Get_remotelyownedRAM(endpoint_mc_leftlowerleg, eo_nvscfg_ownership_local);

        ropinfo.ropcfg      = eok_ropconfig_basic;
        ropinfo.ropcode     = eo_ropcode_set;
        ropinfo.nvep        = endpoint_mc_leftlowerleg;
        ropinfo.nvid        = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)1, jointNVindex_jconfig__maxpositionofjoint);

        locllleg->joints[1].jconfig.maxpositionofjoint = 5;
        
        eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);

        snprintf(str, sizeof(str)-1, "               - added a set<__jcfg_maxpositionofjoint j1, %d>", locllleg->joints[1].jconfig.maxpositionofjoint);
        hal_trace_puts(str); 

    }



    if(3==(step%10))
    {
        eo_transceiver_ropinfo_t ropinfo;

        ropinfo.ropcfg      = eok_ropconfig_basic;
        ropinfo.ropcode     = eo_ropcode_ask;
        ropinfo.nvep        = endpoint_mc_leftlowerleg;
        ropinfo.nvid        = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)1, jointNVindex_jconfig__minpositionofjoint);
//        eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);

//            snprintf(str, sizeof(str)-1, "               - added a ask<__minpositionofjoint j1>");
//            hal_trace_puts(str); 
    }



    step += 2;
}


#endif



static void s_eom_hostprotoc_extra_protocoltransceiver_configure_regular_rops_on_board(void)
{
    eo_cfg_nvsEP_mn_comm_t* locmn = eo_cfg_nvsEP_eb7_Get_remotelyownedRAM(endpoint_mn_comm, eo_nvscfg_ownership_local);
    eOmn_ropsigcfg_command_t* ropsigcmd = (eOmn_ropsigcfg_command_t*) & locmn->ropsigcfgcommand; 
    EOarray *array = (EOarray*) & ropsigcmd->array;

    eOropSIGcfg_t sigcfg;
    char str[128];
    static uint8_t reset = 0;
    
    ropsigcmd->cmmnd = ropsigcfg_cmd_assign; 

    eo_array_Reset(array);

    if(0 == reset)
    {

        sigcfg.ep = endpoint_mc_leftlowerleg;
        sigcfg.id = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)0, jointNVindex_jstatus__basic);
        sigcfg.plustime = 0;
        eo_array_PushBack(array, &sigcfg);
    
    
        sigcfg.ep = endpoint_mc_leftlowerleg;
        sigcfg.id = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_jointNumber_t)1, jointNVindex_jstatus__basic);
        sigcfg.plustime = 0;
        eo_array_PushBack(array, &sigcfg);
    
        sigcfg.ep = endpoint_mc_leftlowerleg;
        sigcfg.id = eo_cfg_nvsEP_mc_motor_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_motorNumber_t)0, motorNVindex_mstatus__basic);
        sigcfg.plustime = 0;
        eo_array_PushBack(array, &sigcfg);
    
        sigcfg.ep = endpoint_mc_leftlowerleg;
        sigcfg.id = eo_cfg_nvsEP_mc_motor_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_motorNumber_t)1, motorNVindex_mstatus__basic);
        sigcfg.plustime = 0;
        eo_array_PushBack(array, &sigcfg);
    
#if 0
#endif

    }

    s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, 0, commNVindex__ropsigcfgcommand));  

    if(1 == reset)
    {
        snprintf(str, sizeof(str)-1, "added a set<__ropsigcfgassign, empty-list>");
        hal_trace_puts(str);     
    }
    else
    {
        snprintf(str, sizeof(str)-1, "added a set<__ropsigcfgassign, list>");
        hal_trace_puts(str);        
    }    


    reset = (0==reset) ? (1) : (0);
}

static void s_eom_hostprotoc_extra_protocoltransceiver_ask_the_board(void)
{
    char str[128];

    s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eo_ropcode_ask, endpoint_mc_leftlowerleg, eo_cfg_nvsEP_mc_motor_NVID_Get(endpoint_mc_leftlowerleg, (eOcfg_nvsEP_mc_motorNumber_t)0, motorNVindex_mconfig__maxcurrentofmotor));  
    
    snprintf(str, sizeof(str)-1, "added a ask<_mconfig__maxcurrentofmotor m0>");
    hal_trace_puts(str); 
}

static void s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid)
{
    eo_transceiver_ropinfo_t ropinfo;

    ropinfo.ropcfg      = eok_ropconfig_basic;
    ropinfo.ropcode     = opc;
    ropinfo.nvep        = ep;

    ropinfo.nvid = nvid;
    eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);
}

static void s_eom_hostprotoc_extra_protocoltransceiver_transmit_ropframe(void)
{
    EOpacket* txpkt;
    
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    uint16_t numofrops_tx;

    // -- retrieve the packet to transmit: txpkt
    eo_transceiver_Transmit(pc104txrx, &txpkt, &numofrops_tx);

    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);

    // -- attempt to connect.
    if(eobool_false == eom_hostprotoc_specialise_connect(remaddr))
    {
        return;
    }

    // -- assign the packet to the transmitter
    eom_hostprotoc_specialise_transmit(txpkt);
}



static void s_callback_button_user(void *arg)
{
    char str[128];

    snprintf(str, sizeof(str)-1, "called callback on BUTTON_USER: tx a ask<>");
    hal_trace_puts(str);

    s_eom_hostprotoc_extra_protocoltransceiver_ask_the_board();

    s_eom_hostprotoc_extra_protocoltransceiver_transmit_ropframe();
}

static void s_callback_button_tamp(void *arg)
{
    char str[128];

    snprintf(str, sizeof(str)-1, "called callback on BUTTON_TAMP: tx a set<regulars>");
    hal_trace_puts(str);

    s_eom_hostprotoc_extra_protocoltransceiver_configure_regular_rops_on_board();

    s_eom_hostprotoc_extra_protocoltransceiver_transmit_ropframe();
}

static void s_callback_button_wkup(void *arg)
{
    char str[128];

    snprintf(str, sizeof(str)-1, "called callback on BUTTON_WKUP: tx a ropframe");
    hal_trace_puts(str);

    s_eom_hostprotoc_extra_protocoltransceiver_transmit_ropframe();
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



