

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEHANDLER_CONFIG_H_
#define __THEHANDLER_CONFIG_H_

#include "embot_app_eth.h"
#include "embot_app_eth_theHandler.h"

#include "EOMtheEMSsocket.h"

#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheEMSerror.h"

#include "embot_os_common.h"
#include "embot_hw_timer.h"
#include "embot_hw_bsp_amc_config.h"
#include "theApplication_config.h"


#include "embot_app_eth_theETHmonitor.h"

namespace embot { namespace app { namespace eth {

    constexpr eOemssocket_cfg_t theHandler_EOMtheEMSsocket_Config
    {
        .inpdatagramnumber = 3,
        .outdatagramnumber = 2,
        .inpdatagramsizeof = 768,
        .outdatagramsizeof = 1416, //EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF
        .localport = 12345,
        .usemutex = eobool_true
    };
    
    constexpr eOemstransceiver_cfg_t theHandler_EOMtheEMStransceiver_Config
    {
        .nvsetbrdcfg = &eonvset_BRDcfgBasic,
        .hostipv4addr = IPaddress(10, 0, 1, 104).v,
        .hostipv4port = 12345,
        .sizes = 
        {
            .capacityoftxpacket = theHandler_EOMtheEMSsocket_Config.outdatagramsizeof, //EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY, 
            .capacityofrop = 400,//                EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPCAPACITY, was 384
            .capacityofropframeregulars = 1024,//   EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY, 
            .capacityofropframeoccasionals = 128, //EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY, 
            .capacityofropframereplies = 400+28, //     EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY, was 400
            .maxnumberofregularrops = 32 //      EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFREGULARROPS         
        },
        .transprotection = eo_trans_protection_none,
        .nvsetprotection = eo_nvset_protection_none,
        .proxycfg = 
        {
            .mode = eoproxy_mode_enabled,
            .capacityoflistofropdes = 24,
            .replyroptimeout = 1000*embot::core::time1millisec,
            .mutex_fn_new = nullptr,
            .transceiver = nullptr        
        }        
    };
    
    constexpr eOemsconfigurator_cfg_t theHandler_EOMtheEMSconfigurator_Config
    {
        .taskpriority = embot::core::tointegral(embot::os::Priority::abovenorm36),
        .taskstacksize = 8*1024
    };  

    constexpr eOemserror_cfg_t theHandler_EOMtheEMSerror_Config
    {
        .taskpriority = embot::core::tointegral(embot::os::Priority::abovenorm35),
        .taskstacksize = 4*1024
    };   


#if defined(EMBOT_ENABLE_hw_timer_emulated)
        
    // 10: period (1ms) -> 10 ms, dostart (0.4ms) -> 4ms, safetx (0.25ms) -> 2.5 ms
    // 100: period (1ms) -> 100 ms, dostart (0.4ms) -> 40ms, safetx (0.25ms) -> 25 ms
    constexpr uint32_t sc {(theApplication_Config.OStick == 500*embot::core::time1microsec) ? 10 : 100};
    
#else
    constexpr uint32_t sc {1};
#endif
    
    constexpr embot::core::relTime runner_period = embot::core::time1millisec;
    constexpr embot::core::relTime runner_execRXafter = 0; // executes at beginning of period
    constexpr embot::core::relTime runner_safeRXexecutiontime = 300*embot::core::time1microsec;
    constexpr embot::core::relTime runner_execDOafter = 400*embot::core::time1microsec;
    constexpr embot::core::relTime runner_safeDOexecutiontime = 200*embot::core::time1microsec;
    constexpr embot::core::relTime runner_execTXafter = 700*embot::core::time1microsec;
    constexpr embot::core::relTime runner_safeTXexecutiontime = 250*embot::core::time1microsec;       

    constexpr eOemsrunner_cfg_t theHandler_EOMtheEMSrunner_Config
    {
        .taskpriority = 
        {
            embot::core::tointegral(embot::os::Priority::abovenorm39), // rx
            embot::core::tointegral(embot::os::Priority::abovenorm38), // do
            embot::core::tointegral(embot::os::Priority::abovenorm37)  // tx
            
        },
        .taskstacksize = 
        {
            4*1024, // rx
            4*1024, // do
            4*1024  // tx
        },
        .haltimerstart = 
        {
            static_cast<hal_timer_t>(embot::hw::TIMER::one),
            static_cast<hal_timer_t>(embot::hw::TIMER::two),
            static_cast<hal_timer_t>(embot::hw::TIMER::three)            
        },
        .haltimeralert = 
        {
            hal_timerNONE, 
            hal_timerNONE,
            hal_timerNONE
        },
        .period = sc*runner_period,
        .execRXafter = sc*runner_execRXafter, 
        .safeRXexecutiontime = sc*runner_safeRXexecutiontime,
        .execDOafter = sc*runner_execDOafter,
        .safeDOexecutiontime = sc*runner_safeDOexecutiontime,
        .execTXafter = sc*runner_execTXafter,
        .safeTXexecutiontime = sc*runner_safeTXexecutiontime,
        .maxnumofRXpackets = 3,
        .maxnumofTXpackets = 1,
        .modeatstartup = eo_emsrunner_mode_besteffort,
        .defaultTXdecimationfactor = 1        
    };      

//    constexpr eOipv4addr_t theHandler_EOMtheEMSsocket_hostIPaddress = IPaddress(10, 0, 1, 104).v;// EO_COMMON_IPV4ADDR(10, 0, 1, 104);
    
    constexpr embot::app::eth::theETHmonitor::Config theHandler_theETHmonitor_Config
    {
        {embot::os::Priority::belownorm22, 2*1024},
        100*embot::core::time1millisec,
        60*embot::core::time1second,
        {}        
    };
            
}}}


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
