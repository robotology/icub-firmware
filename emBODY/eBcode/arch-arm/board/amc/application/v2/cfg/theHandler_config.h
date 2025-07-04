

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
    
    
    // can receive datagrams up to 768 in size and we can transmit datagrams of size 1472
    // 1472 is the highest possible value given the used IPAL, even when is lwip-based
    // see UDP Maximum Packet Size (1472 bytes) in: https://www.keil.com/support/man/docs/rlarm/rlarm_udp_get_buf.htm
    // also: stm32h7xx_hal_eth.h defined ETH_MAX_PAYLOAD to be 1500. we must remove 20 bytes fro IP header and 8 bytes for UDP and we have 1472
    
    constexpr size_t IPALmaxsizeofUDPframe {1472};

    constexpr eOemssocket_cfg_t theHandler_EOMtheEMSsocket_Config
    {
        .inpdatagramnumber = 3,
        .outdatagramnumber = 2,
        .inpdatagramsizeof = 768,
        .outdatagramsizeof = IPALmaxsizeofUDPframe, 
        .localport = 12345,
        .usemutex = eobool_true
    };
    
    
#if 0
    
    ROPframe
     ---------------- ----....---- ---------------- 
    | ROPFrameHeader | BODY       | ROPFrameFooter |
     ---------------- ----....---- ----------------
    fixed: 24         var          fixed: 4
    
    BODY = {ROP}_n
    sizeof(ROPframe) = 28 + sizeof(BODY)
    
    ROP
     ---------------- ----....---- -------- ---------------- 
    | header         | data       | signat | time           |
     ---------------- ----....---- -------- ----------------
     fixed: 8B        var: 0->MAX  opt.: 4B optional: 8B
    
    ROPsize = 8 + sizeof(data) + ((header.plussignature == 1) ? 4 : 0) + ((header.plustime == 1) ? 8 : 0)
#endif

    constexpr size_t ROPframe_headfoot {28}; 
    constexpr size_t ROPheadersize {8};
    constexpr size_t ROPsignaturesize {4};

    // the biggest ROPs can be the following:
    // set<eOmn_service_cmmnds_t, 1sign, 0time>, where sizeof(eOmn_service_cmmnds_t) = 348 -> ROPsize = 348+8+4 = 360
    // set<eOmc_motor_config_t, 1sign, 0time>, where sizeof(eOmc_motor_config_t) = 156 -> ROPsize = 164 + 4 = 168 
    // set<eOmc_joint_config_t, 1sign, 0time>, where sizeof(eOmc_joint_config_t) = 240 -> ROPsize = 248 + 4 = 252 
    // so, a value of 384 is OK for ROP input
    // it could be bigger (512), but ... the smaller the better on the board because we dont want to waste RAM
    constexpr size_t maxsizeofROP = 384;   

    // i add a check
    static_assert    
    (
        (sizeof(eOmn_service_cmmnds_t)+ROPheadersize+ROPsignaturesize) <= maxsizeofROP,
        "pls correct maxsizeofROP"
    );
    
    // we use ROP reply when we receive a ask<> and we transmit back a say<>
    // the format is the same as a set<> and the biggest ROP emitted by the ETH board is:
    // say<eOmc_joint_config_t, 1sign, 0time>, where sizeof(eOmc_joint_config_t) = 240 -> ROPsize = 240 + 8 + 4 = 252
    // NOTE: we use a say<eOmn_service_command_result_t, 1sign, 0time> but 
    // sizeof(eOmn_service_command_result_t) is 32 so no problem in here.
    // so, we must fill at least one say<> of the biggest size inside the reply ropframe
    // we have multiple possibilities in here: 
    // (1) i compute each time the max size of the ROP containing the biggest variable and i set maxsizeofROPreply just a little higher than that.
    //     if i forget to rise maxsizeofROPreply when i make a variable bigger i can rely on the eo_errman_Trace() inside EOropframe.c
    // (2) i just set maxsizeofROPreply = maxsizeofROP, so i have more clarity on how a reply ROP is.
    //     but i potentially waste bytes for the regulars...
    constexpr size_t maxsizeofROPreply = maxsizeofROP; // use option 2
    constexpr size_t effectivecapacityofREPLYs = maxsizeofROPreply;  
    constexpr size_t capacityofREPLYs = 28+effectivecapacityofREPLYs;    

    // the ropframe of occasional ROPs transports for instance diagnostics messages
    // one simple diagnostics message is sig<eOmn_info_basic_t, 0sign, 0time> that needs 24+8 = 32 bytes
    // one diagnostics message w/ text is sig<eOmn_info_status_t, 0sign, 0time> that needs 72+8 = 80 bytes 
    // the diagnostics messages are filled inside the ropframe at each 1ms tick until we reach max capacity 
    // and if still any present they are transmitted at the next cycle.
    // so, if the rate of emission is correct 128 is enough because it can host up to 4 simple messages    
    constexpr size_t effectivecapacityofOCCASIONALs = 128;
    constexpr size_t capacityofOCCASIONALs = ROPframe_headfoot+effectivecapacityofOCCASIONALs;
        
    // all the remaing space in the udp packet can be allocated to the regulars ROPs.
    // the max size of teh UDP frame is 1472, of which 1472-28 = 1444 reserved for the ROPs.
    // with the above values of 384 reseved to reply ROPs and 128 to occasional ROPs, we have bytesREGs = 932 remaining.
    // are they enough to transmit all the regular ROPs? not in every case.
    // we have some legacy robots where the ETH board must manage 12 joints. we already cycled in two rounds of 6 but
    // now since 4 july 2025 we shall cycle in three rounds of 4, so we shall have max 4 joints to transmit in a ropframe
    // so: at each cycle we can have up to a maximum of bytesREGs = 932 bytes for any combination of up to 4 jomos, skin, etc.
    // NOTE-1: w/ EOTRANSMITTER_ROPFRAMES_use100percent defined that uses more memory ....
    // how much do we need for each?
    // - MC will use at 1 eOmc_joint_status_t (sizeof = 96) and 1 eOmc_motor_status_t (sizeof = 24) per jomo
    //   each with a sig<xxx, 0sign, 0time> with a budget of (96+8)+(24+8) = 136 bytes per jomo.
    //   so, the full 4 jomos need 544 bytes. OK and we still have some margin to increase
    // - SK uses a sig<eOsk_status_t, 0sign, 0time> of 248+8 = 256 bytes for each skin patch
    // - AS inertials3 uses a sig<eOas_inertial3_status_t, 0sign, 0time> of 68+8 = 76 bytes
    // - AS ft uses a sig<eOas_ft_status_t, 0sign, 0time> of 56+8 = 64 bytes for each sensor (some boards have two)
    // - AS battery uses a sig<eOas_battery_status_t, 0sign, 0time> of 32+8 = 40 bytes  
    // - AS mais  uses a sig<eOas_mais_status_t, 0sign, 0time> of 40+8 = 48 bytes 
    // SO: in the bytesREGs = 932 we can accomodate: 
    // - MC-4jomo (544) + SK (256) + mais (48) = 848 and we have margin of other 21 bytes for each jomo 
    //   [that is the case of the lower arms w/ 12 joints after that i have cycled in three rather than 2 times because if we
    //    cycle twice we would have MC6-jomo (816) + 256 + 48 = 1120 > bytesREGs
    // - MC-4jomo (544) + FT-2 (128) + Inertials (76) = 748 
    // - etc.
    // NOTE: in case we need for more we shall reduce maxsizeofROPreply from 384 to be > sizeof(eOmc_joint_config_t) + 8 + 4 = 252
    
    constexpr size_t effectivecapacityofREGULARs = IPALmaxsizeofUDPframe - ROPframe_headfoot - effectivecapacityofREPLYs - effectivecapacityofOCCASIONALs; 
    constexpr size_t capacityofREGULARs = ROPframe_headfoot+effectivecapacityofREGULARs;
    
    static_assert
    (   // capacityoftxpacket is theHandler_EOMtheEMSsocket_Config.outdatagramsizeof = 1472
        theHandler_EOMtheEMSsocket_Config.outdatagramsizeof >=  // 1472
        (
            ROPframe_headfoot +                                 // 28 
            effectivecapacityofREGULARs +                       // 1472-28-128-384 = 932
            effectivecapacityofOCCASIONALs +                    // 128
            effectivecapacityofREPLYs                           // 384
                                                                // = 1472
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
            .capacityoftxpacket = theHandler_EOMtheEMSsocket_Config.outdatagramsizeof, 
            .capacityofrop = maxsizeofROP,                          
            .capacityofropframeregulars = capacityofREGULARs,       
            .capacityofropframeoccasionals = capacityofOCCASIONALs, 
            .capacityofropframereplies = capacityofREPLYs,          
            .maxnumberofregularrops = 32                                   
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
        .modeatstartup = eo_emsrunner_mode_synchronized,
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
