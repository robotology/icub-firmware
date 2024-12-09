

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
#include "embot_hw_bsp_config.h"
#include "theApplication_config.h"


#include "embot_app_eth_theETHmonitor.h"

namespace embot { namespace app { namespace eth {
    
    
    // can receive datagrams up to 768 in size and we can transmit datagrams of size 1416
    // 1416 is the highest possible value given the used IPAL, even when is lwip-based

    constexpr eOemssocket_cfg_t theHandler_EOMtheEMSsocket_Config
    {
        .inpdatagramnumber = 3,
        .outdatagramnumber = 2,
        .inpdatagramsizeof = 768,
        .outdatagramsizeof = 1416, //EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF
        .localport = 12345,
        .usemutex = eobool_true
    };
    
    
#if 0
    
     ---------------- ----....---- -------- ---------------- 
    | header         | data       | signat | time           |
     ---------------- ----....---- -------- ----------------
     fixed: 8B        var: 0->MAX  opt.: 4B optional: 8B
    
    ROPsize = 8 + sizeof(data) + ((header.plussignature == 1) ? 4 : 0) + ((header.plustime == 1) ? 8 : 0)
#endif

    // the biggest ROPs can be the following:
    // set<eOmn_service_cmmnds_t, 1sign, 0time>, where sizeof(eOmn_service_cmmnds_t) = 348 -> ROPsize = 360
    // set<eOmc_motor_config_t, 0sign, 0time>, where sizeof(eOmc_motor_config_t) = 120 -> ROPsize = 128
    // set<eOmc_joint_config_t, 0sign, 0time>, where sizeof(eOmc_joint_config_t) = 240 -> ROPsize = 248
    // so, a value of 384 is OK
    constexpr size_t maxsizeofROP = 384;    
    
    // we use ROP reply when we receive a ask<> and we transmit back a say<>
    // the format is the same as a set<> and the biggest ROP is:
    // say<eOmc_joint_config_t, 0sign, 0time>, where sizeof(eOmc_joint_config_t) = 240 -> ROPsize = 248
    // NOTE: we use a say<eOmn_service_command_result_t, 1sign, 0time> but 
    // sizeof(eOmn_service_command_result_t) is 32 so no problem in here.
    // as a result max ROPsize is now 248, so we can safely use a value between 248 and 384. 
    // 256 is the minimum value, 288 would be better
    constexpr size_t effectivecapacityofREPLYs = 256; // was 384
    constexpr size_t capacityofREPLYs = 28+effectivecapacityofREPLYs;    

    // the ropframe of occasional ROPs transports for instance diagnostics messages
    // one simple diagnostics message is sig<eOmn_info_basic_t, 0sign, 0time> that needs 24+8 = 32 bytes
    // one diagnostics message w/ text is sig<eOmn_info_status_t, 0sign, 0time> that needs 72+8 = 80 bytes 
    // the diagnostics messages are filled inside the ropframe at each 1ms tick until we reach max capacity 
    // and if still any present they are transmitted at the next cycle.
    // so, if the rate of emission is correct 128 is enough because it can host up to 4 simple messages    
    constexpr size_t effectivecapacityofOCCASIONALs = 128;
    constexpr size_t capacityofOCCASIONALs = 28+effectivecapacityofOCCASIONALs;
        
    // all the remaing space in the udp packet can be allocated to the regulars ROPs.
    // we have NREGs = 972 (+32) bytes available which we can use:
    // - n1 < N1 = NREGs*0.75 = 729 (753) available for MC
    // - n2 < N2 = NREgs*0.75 = 729 (753) available for all the rest (SK, As-ft, As-battery, AS-inertials3, ...)
    // as long as n1 + n2 < NREgs
    // are they enough?
    // now that we have at most 4 joints managed we can estimate the space in this way:
    // - MC will use at 1 eOmc_joint_status_t (sizeof = 96) and 1 eOmc_motor_status_t (sizeof = 24) per jomo
    //   each with a sig<xxx, 0sign, 0time> with a budget of (96+8)+(24+8) = 136 bytes per jomo.
    //   so, the full 4 jomos need 544 bytes. The N1 = 729 are well enough for now and for adding extra signalling.
    // - SK uses a sig<eOsk_status_t, 0sign, 0time> of 248+8 = 256 bytes for each skin patch
    // - AS inertials3 uses a sig<eOas_inertial3_status_t, 0sign, 0time> of 68+8 = 76 bytes
    // - AS ft uses a sig<eOas_ft_status_t, 0sign, 0time> of 56+8 = 64 bytes for each sensor (some boards have two)
    // - AS battery uses a sig<eOas_battery_status_t, 0sign, 0time> of 32+8 = 40 bytes
    // so, in N2 we can surely fit most entities but not all of them. the skin is the tricky one  
    // so, we can safely use MC-4jomos + AS-battery + AS-inertials3 + 2*AS-ft = 788 bytes
    // we cannot however add one skin becase it would become 1044 bytes
    constexpr size_t effectivecapacityofREGULARs = 972+32; 
    constexpr size_t capacityofREGULARs = 28+effectivecapacityofREGULARs;
    
    static_assert
    (   // capacityoftxpacket is theHandler_EOMtheEMSsocket_Config.outdatagramsizeof = 1416
        theHandler_EOMtheEMSsocket_Config.outdatagramsizeof >=  // 1416
        (
            28 +                                                // the header+footer of the ropframe
            effectivecapacityofREGULARs +                       // 972+32
            effectivecapacityofOCCASIONALs +                    // 128
            effectivecapacityofREPLYs                           // 256
                                                                // = 1416
        ), 
        "pls correct capacities of the ropframes"
    );    
    
    constexpr eOemstransceiver_cfg_t theHandler_EOMtheEMStransceiver_Config
    {
        .nvsetbrdcfg = &eonvset_BRDcfgBasic,
        .hostipv4addr = IPaddress(10, 0, 1, 104).v,
        .hostipv4port = 12345,
        .sizes = 
        {
            .capacityoftxpacket = theHandler_EOMtheEMSsocket_Config.outdatagramsizeof, //EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY, 
            .capacityofrop = maxsizeofROP,                          // EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPCAPACITY, was 384
            .capacityofropframeregulars = capacityofREGULARs,       // EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY
            .capacityofropframeoccasionals = capacityofOCCASIONALs, // EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY, 
            .capacityofropframereplies = capacityofREPLYs,          // EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY
            .maxnumberofregularrops = 32                            // EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFREGULARROPS         
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
