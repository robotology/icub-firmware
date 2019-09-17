// see copyright at end of file

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMTHEEMSAPPLCFG_CFG_H_
#define _EOMTHEEMSAPPLCFG_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSapplCfg_cfg.h
    @brief      This header file allow to configure the EMS application configuration singleton.
    @author     marco.accame@iit.it
    @date       07/02/2012
 **/


// -  default constants ----------------------------------------------------------------------------------------------- 

#if defined(USE_EMS4RD)
    // ok
#else
    #error --> cannot use this EOMtheEMSapplCfg_cfg.h file unless a ems4rd board
#endif
 
// - configuration wizard ---------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

//  <h> Remote Host (PC104 or other)

//  <o> host IP1 address <1-255>
//  <i> default: 10
#define EOMTHEEMSAPPLCFG_HOSTIPADDR1                            10

//  <o> host IP2 address <0-255>
//  <i> default: 255
#define EOMTHEEMSAPPLCFG_HOSTIPADDR2                            0

//  <o> host IP3 address <0-255>
//  <i> default: 72
#define EOMTHEEMSAPPLCFG_HOSTIPADDR3                            1

//  <o> host IP4 address <1-255>
//  <i> default: 205
#define EOMTHEEMSAPPLCFG_HOSTIPADDR4                            104


//  </h>Remote Host

// <h> Choice of EMS board
// <i> It holds ...


//  <o> ID of the EMS board     <255=> Unique FW

#define EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD     255

#if EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD == 255
    #undef EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD
#endif
      
// </h>Choice of EMS board

// <h> Info about EMS application
// <i> It holds configuration for ...

//  <o> name           <0=> EOMemsApplEBx, where x is the same number of the chosen EBx  
#define EOMTHEEMSAPPLCFG_xcwd3254385r3r432      0

//  <h> version
//  <o> major           <0-255> 
#define EOMTHEEMSAPPLCFG_VERSION_MAJOR          103
//  <o> minor           <0-255> 
#define EOMTHEEMSAPPLCFG_VERSION_MINOR          28

//  </h>version

//  <h> build date
//  <o> year            <2010-2020> 
#define EOMTHEEMSAPPLCFG_BUILDDATE_YEAR         2019
//  <o> month           <1-12> 
#define EOMTHEEMSAPPLCFG_BUILDDATE_MONTH        9
//  <o> day             <1-31> 
#define EOMTHEEMSAPPLCFG_BUILDDATE_DAY          17
//  <o> hour            <0-23> 
#define EOMTHEEMSAPPLCFG_BUILDDATE_HOUR         11
//  <o> minute          <0-59> 
#define EOMTHEEMSAPPLCFG_BUILDDATE_MIN          55
//  </h>build date
// </h>Info 


// <h> Configuration of EMS application
// <i> It holds configuration for .


// <h> System specifics (as required by EOMtheSystem)
// <i> Keeps configuration used by object EOMtheSystem.


//  <h> code offset
//  <o> value           <0=> as in eEmemorymap.h (but EENV_EAPPLICATION_FORCE_CODE_OFFSET_TO_ZERO forces it to zero)
#define sdfefe001       0
//  </h>code offset


//  <h> hal
//  <o> info            <0=> as in hal_cfg.h
#define sdfefe001       0
//  </h>hal


//  <h> osal
//  <o> info            <0=> as osal_cfg.h
#define sdfefe002       0
//  </h>osal

//  <h> memory pool
//  <o> memory          <0=> from heap
#define sdfefe003       0
//  </h>memory pool

//  <h> error manager
//  <o> on error   <0=>   redefine __weak eom_emsapplcfg_hid_userdef_OnError() 
#define sdfefe004       0
//  <o> printf on debug window enabled   <0=>   false  <1=>   true
#define EOMTHEEMSAPPLCFG_HALTRACE_ENABLED    1
//  </h>error manager


//  <h> timer manager        
//  <i> info: the timer manager uses a task to receive execution orders from the timers based on OSAL timers. 

//  <o> settings            <1=> default: priority = 240, stack size = 512, msg queue = 8 
//                          <0=> as in user defined                               
#define TMRMAN_USEDEFAULT   1

//  <h> user defined

//  <o> task priority <2-251>
//  <i> default: 240
#define TMRMAN_TMP_PRIO   241

//  <o> task stack size <256-1024:128>
//  <i> default: 512
#define TMRMAN_TMP_STACK   1024

//  <o> task message queue size <1-16>
//  <i> default: 8
#define TMRMAN_TMP_MSGQ   12

//  </h>user defined 


#if (1 == TMRMAN_USEDEFAULT)
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_PRIORITYof                 240
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_STACKSIZEof                1024
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_QUEUESIZEof                12
#else
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_PRIORITYof                 TMRMAN_TMP_PRIO
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_STACKSIZEof                TMRMAN_TMP_STACK
    #define EOMTHEEMSAPPLCFG_TMRMAN_TASK_QUEUESIZEof                TMRMAN_TMP_MSGQ
#endif

//   </h>timer manager 


//  <h> callback manager        
//  <i> info: the callback manager uses a task to receive execution orders of callbacks. 

//  <o> settings            <1=> default: priority = 202, stack size = 512, msg queue = 8 
//                          <0=> as in user defined                               
#define CBKMAN_USEDEFAULT   1

//  <h> user defined

//  <o> task priority <2-251>
//  <i> default: 202
#define CBKMAN_TMP_PRIO   203

//  <o> task stack size <256-1024:128>
//  <i> default: 512
#define CBKMAN_TMP_STACK   2048

//  <o> task message queue size <1-16>
//  <i> default: 8
#define CBKMAN_TMP_MSGQ   12

//  </h>user defined 


#if (1 == CBKMAN_USEDEFAULT)
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_PRIORITYof                 202
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_STACKSIZEof                2048
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_QUEUESIZEof                12
#else
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_PRIORITYof                 CBKMAN_TMP_PRIO
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_STACKSIZEof                CBKMAN_TMP_STACK
    #define EOMTHEEMSAPPLCFG_CBKMAN_TASK_QUEUESIZEof                CBKMAN_TMP_MSGQ
#endif

//   </h>callback manager 


// </h>System




// <h> IP network (as required by EOMtheIPnet)
// <i> Keeps ...

//  <h> ipal
//  <o> info            <0=> as ipal_cfg.h
#define sdfefe005       0
//  </h>ipal


//  <h> local addresses (mac address, ipv4 address, ipv4 netmask)       
//  <i> info: those used by ... 

//  <o> addresses       <0=> from eeprom 
//                      <1=> from ipal_cfg.h                               
#define ADDR_USEIPAL   0

#if (1 == ADDR_USEIPAL)
    #define EOMTHEEMSAPPLCFG_IPADDR_FROM_ENVIRONMENT        0
#else
    #define EOMTHEEMSAPPLCFG_IPADDR_FROM_ENVIRONMENT        1
#endif

//   </h>local addresses 


//   <h> proc and tick tasks of EOMtheIPnet     
//   <i> defefe.

//  <o> settings            <1=> default: prio = 220/219, stack = 1024/128, idletime proc = 20 ms, period tick = see ipal_cfg.h, 
//                          <0=> as in user defined                               
#define IPNET_USEDEFAULT    1

//  <h> user defined

//  <o> proc task priority <2-251>
//  <i> default: 220
#define IPNET_TMP_PRIO1   220

//  <o> proc task stack size <256-1024:128>
//  <i> default: 1024
#define IPNET_TMP_STACK1   1024

//  <o> proc task max idletime [ms] <1-200>
//  <i> default: 20
#define IPNET_TMP_IDLE1   20


//  <o> tick task priority <2-251>
//  <i> default: 219
#define IPNET_TMP_PRIO2   219

//  <o> tick task stack size <128-256:128>
//  <i> default: 128
#define IPNET_TMP_STACK2   256


//  <o> tick task period     <0=> as specified in ipal_cfg->sys_timetick in ipal_cfg.h
#define ASDGFdecdew1234     0 

//  </h>user defined 

// <e> proc task waken up by ETH ISR
// <i> If enabled, the ETH ISR forces a wake up of the proc task if EOMtheIPnet
#define EOMTHEEMSAPPLCFG_IPNETPROC_WAKEUPONRXFRAME     1
// </e>proc task


#if (1 == IPNET_USEDEFAULT)
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_PRIORITYof              220
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_STACKSIZEof             1024
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_MAXIDLETIMEof           20000
    #define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_PRIORITYof              219
    #define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_STACKSIZEof             128
#else
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_PRIORITYof              IPNET_TMP_PRIO1
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_STACKSIZEof             IPNET_TMP_STACK1
    #define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_MAXIDLETIMEof           (1000*IPNET_TMP_IDLE1)
    #define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_PRIORITYof              IPNET_TMP_PRIO2
    #define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_STACKSIZEof             IPNET_TMP_STACK2
#endif


//  </h>proc and tick tasks


//  <h> UDP sockets managed by EOtheIPnet

//  <o> number of UDP sockets <1-4>
//  <i> default: 1
#define EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFSOCKETS   3

//  <o>  max number of packets in wait for transmission <1-16>
//  <i> default: 4
#define EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFDATAGRAMSENQUEUEDINTX   8

//  </h>UDP sockets 


//  </h>IP network


//  <h> Discovery Listener configuration (for EOMtheEMSdiscoveryListener)

//  <o> IP listening port <3333=> 3333
//  <i> default: 3333
#define EOMTHEEMSAPPLCFG_DISCOVERY_LOCALPORT                       3333


//  <h> datagrams in socket

//  <o> max number of input datagrams <1-8>
//  <i> default: 2
#define EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMNUMBER                  2

//  <o> max size of input datagrams <16-64>
//  <i> default: 16
#define EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMSIZEOF                  16

//  <o> max number of output datagrams <1-8>
//  <i> default: 1 
#define EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMNUMBER                  1

//  <o> max size of output datagrams <16-128>
//  <i> default: 32
#define EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMSIZEOF                  128

//  </h>datagrams in socket

//  <h> destination address

//  <o> IP address <0=> The same as Remote Host  <1=> The same as the incoming packet
//  <i> default: 1
#define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR_MODE                   1

#if   (0 == EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR_MODE)
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR1    EOMTHEEMSAPPLCFG_HOSTIPADDR1
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR2    EOMTHEEMSAPPLCFG_HOSTIPADDR2
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR3    EOMTHEEMSAPPLCFG_HOSTIPADDR3
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR4    EOMTHEEMSAPPLCFG_HOSTIPADDR4
#elif (1 == EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR_MODE)
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR1    127
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR2    0
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR3    0
    #define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR4    1    
#endif

//  <o> IP port <3333=> 3333
//  <i> default: 3333
#define EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEPORT                3333

//  </h>destination address


//  <h> protocol

//  <o> type of protocol    <0=> ETHloader reduced set: only CMD_SCAN, CMD_RESET, and CMD_UPD_ONCE
#define EOMTHEEMSAPPLCFG_DISCOVERY_PROTOCOL                0

//  </h>protocol

//  <h> listener task

//  <o> task priority <2-251>
//  <i> default: 41
#define EOMTHEEMSAPPLCFG_LISTENER_TASK_PRIORITYof   41

//  <o> task stack size <256-1024:128>
//  <i> default: 512
#define EOMTHEEMSAPPLCFG_LISTENER_TASK_STACKSIZEof   1024

//  </h>listener task


// </h>Discovery Listener



// <e> Backdoor configuration
// <i> Enable backdoor (thus EOMtheEMSbackdoor, EOMtheEMSbackdoortransceiver, OPCprotocolManager)
// <i> Default: disabled
#ifndef EOMTHEEMSAPPLCFG_BACKDOOR_ENABLED
 #define EOMTHEEMSAPPLCFG_BACKDOOR_ENABLED       0
#endif

//  <o> IP listening port <4444=> 4444
//  <i> default: 4444
#define EOMTHEEMSAPPLCFG_BACKDOOR_LOCALPORT                       4444


//  <h> datagrams in socket

//  <o> max number of input datagrams <0-8>
//  <i> default: 2
#define EOMTHEEMSAPPLCFG_BACKDOOR_INPDGRAMNUMBER                  0

//  <o> max size of input datagrams <0-64>
//  <i> default: 16
#define EOMTHEEMSAPPLCFG_BACKDOOR_INPDGRAMSIZEOF                  0

//  <o> max number of output datagrams <0-8>
//  <i> default: 2 
#define EOMTHEEMSAPPLCFG_BACKDOOR_OUTDGRAMNUMBER                  0

//  <o> max size of output datagrams <0-256>
//  <i> default: 32
#define EOMTHEEMSAPPLCFG_BACKDOOR_OUTDGRAMSIZEOF                  0

//  </h>datagrams in socket

//  <h> destination address

//  <o> IP address <0=> The same as Remote Host then the one from incoming request
//  <i> default: 1
#define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR_MODE                   0

#if   (0 == EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR_MODE)
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR1    EOMTHEEMSAPPLCFG_HOSTIPADDR1
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR2    EOMTHEEMSAPPLCFG_HOSTIPADDR2
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR3    EOMTHEEMSAPPLCFG_HOSTIPADDR3
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR4    EOMTHEEMSAPPLCFG_HOSTIPADDR4
#elif (1 == EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR_MODE)
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR1    127
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR2    0
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR3    0
    #define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR4    1    
#endif

//  <o> IP port <4444=> 4444
//  <i> default: 4444
#define EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEPORT                4444

//  </h>destination address


//  <h> protocol

//  <o> type of protocol    <0=> OPC protocol
#define EOMTHEEMSAPPLCFG_BACKDOOR_PROTOCOL                0

//  </h>protocol

//  <h> backdoor task

//  <o> task priority <0-251>
//  <i> default: 41. If 0, then the backdoor is not enabled
#define EOMTHEEMSAPPLCFG_BACKDOOR_TASK_PRIORITYof   0

//  <o> task stack size <256-1024:128>
//  <i> default: 512
#define EOMTHEEMSAPPLCFG_BACKDOOR_TASK_STACKSIZEof   512

//  </h>backdoor task


// </e>Backdoor configuration

//  <h> Socket configuration (for EOMtheEMSsocket)

//  <o> IP listening port <1-65000>
//  <i> default: 12345
#define EOMTHEEMSAPPLCFG_SOCKET_LOCALPORT                       12345

//  <h> datagrams in socket

//  <o> max number of input datagrams <1-8>
//  <i> default: 3
#define EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMNUMBER                  3

//  <o> max size of input datagrams <64-1500>
//  <i> default: 768
#define EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF                  768

//  <o> max number of output datagrams <1-8>
//  <i> default: 2 (it can be higher than max number of packets that the task TX emits in 1 ms because in real time modes the IPnet may not be fast enough to transmit them)
#define EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMNUMBER                  2

//  <o> max size of output datagrams <64-1500>
//  <i> default: 1024
#define EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF                  1408

//  </h>datagrams in socket


// </h>Socket configuration


//  <h> Transceiver configuration

//  <h> destination address

//  <o> IP address <0=>    The same as Remote Host
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_HOSTIPADDR2USE             0

//  <o> IP port <1-65000>
//  <i> default: 33333
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_HOSTIPPORT                12345

//  </h>destination address

//  <h> protocol

//  <o> type of protocol    <0=> nv ropframe (udp contains a frame of rops on network variables)
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_PROTOCOL                0

//  <h> format

//  <h> ropframe


//  <o> capacity of the whole ropframe    <1=> automatic (equal to the max between the capacity of input and output datagrams manipulated by the socket)
//  <i> must be higher than the sum of regulars, occasionals, and replies
#define ROPFRAMECAPACITY_AUTOMATIC  1  

#if(1 == ROPFRAMECAPACITY_AUTOMATIC)
//     #if(EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF > EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF)
//         #define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY   EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF
//     #else
//         #define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY   EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF
//     #endif  
 

    #define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY   (EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF)

#else
    #error specify capacity of ropframe
#endif

//  <o> capacity of the ropframe of regular rops    <16-1440:8>
//  <i> default: 768
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY  1024  

//  <o> capacity of the ropframe of occasional rops    <16-1440:8>
//  <i> default: 128
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY  128  

//  <o> capacity of the ropframe of reply rops    <16-1440:8>
//  <i> default: 128
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY  256  



#if( (EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY) < (EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY+EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY+EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY)) 
    #error --> the sum of sizes regulars, occasionals and replies must be lower than the size of the complete ropframe
#endif

//  </h>ropframe

//  <h> rop

//  <o> capacity of a single rop            <8-512:8>
//  <i> default: 256
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPCAPACITY  384  


//  <o> max number of regular rops          <1-64>
//  <i> default: 32
#define EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFREGULARROPS  32  


//  <o> max number of confermation request rops          <0-16>
//  <i> default: 8

#define EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFCONFREQROPS 0

//  <o> max number of occasional rops       <0=> until the ropframe reserved to regulars is full of bytes
#define ASFIDANKEN098675234323215  0  

//  <o> max number of reply rops            <0=> until the ropframe reserved to replies is full of bytes
#define ASFIDANKEN098672346780425  0  

//  </h>rop

//  </h>format

//  </h>protocol




//  <h> proxy
//  <i> the proxy redirectes all ROPs related to variables effectively owned by another device (e.g., over CAN)
//  <i> and manages replies in the following way: 
//  <i> - ack/nak: are managed in relation to the IP board. The ROP with conf request flagged is acked/naked as soon is decoded.
//  <i> - say:     the ask<> is forwarded, info about the reply are stored inside a list and the say<> is sent oly when the 
//  <i>            data from the proxied device arrives. if the timeout expires, the reply is not sent anymore. 

//  <o> mode     <0=> DISABLED    <1=> ENABLED

#define EOMTHEEMSAPPLCFG_PROXY_MODE     1

//  <h> reply container
//  <i> it contains the details of the received ask<>: id32, plus-time, plus-signature, etc. 
//  <i> the details are stored in a list of a given capacity for up to a given timeout

//  <o> capacity of the reply container                     <0-32>
//  <i> default: 16
//  <i> it is the max number of reply information that can be stored
#define EOMTHEEMSAPPLCFG_PROXY_MAXNUMOFREPLYROPS 24

//  <o> maximum time a reply can be contained (in ms)       <0-100000>
//  <i> default: 10. 
//  <i> it is the max time a reply can be stored in the container before the say<> is considered lost
#define EOMTHEEMSAPPLCFG_PROXY_REPLYTIMEOUTMILLISEC     1000

//  </h> reply container

//  </h> proxy




//  </h>Transceiver configuration


//  <h> Configurator object configuration

//  <o> task priority <2-251>
//  <i> default: 50
#define EOMTHEEMSAPPLCFG_CFGOBJ_TASK_PRIORITYof   50

//  <o> task stack size <256-8192:128>
//  <i> default: 512
#define EOMTHEEMSAPPLCFG_CFGOBJ_TASK_STACKSIZEof   8192

//  </h>Configurator object configuration


//  <h> Error object configuration

//  <o> task priority <2-251>
//  <i> default: 40
#define EOMTHEEMSAPPLCFG_ERROBJ_TASK_PRIORITYof   40

//  <o> task stack size <256-4096:128>
//  <i> default: 512
#define EOMTHEEMSAPPLCFG_ERROBJ_TASK_STACKSIZEof   2048

//  </h>Error object configuration


//  <h> Runner object configuration

//  <h> priorities
//  <i> it is important put the priorities of the three tasks higher than any other task in the system

//  <o> RX task priority <2-251>
//  <i> default: 247
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_PRIORITYof   249

//  <o> DO task priority <2-251>
//  <i> default: 248
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_PRIORITYof   248

//  <o> TX task priority <2-251>
//  <i> default: 249
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_PRIORITYof   247

//  </h>priorities 

//  <h> stack sizes 

//  <o> RX task stack size <256-4096:128>
//  <i> default: 1024
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_STACKSIZEof   4096

//  <o> DO task stack size <256-4096:128>
//  <i> default: 1024
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_STACKSIZEof   4096

//  <o> TX task stack size <256-4096:128>
//  <i> default: 1024
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_STACKSIZEof   4096

//  </h>stack sizes

//  <e0> enable custom timings
//	<i> If checked, the timings defined below will be used. If not, the default (adaptive) values will be used in the control loop

#define EOMTHEEMSAPPLCFG_RUNOBJ_CUSTOMIZED 0

#if (EOMTHEEMSAPPLCFG_RUNOBJ_CUSTOMIZED == 1)
	//  <o> period [usec] <100-10000:100>
	//  <i> default: 1000
	#define EOMTHEEMSAPPLCFG_RUNOBJ_PERIOD         1000
    
    
	//  <o> tx decimation factor [integer] <1-10:1>
	//  <i> default: 1
	#define EOMTHEEMSAPPLCFG_RUNOBJ_TXDECIMATIONFACTOR         1    

	//  <o> RX start after [usec] <0-10000:10>
	//  <i> default: 0
	#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_AFTER       0

	//  <o> RX safe execution [usec] <0-10000:10>
	//  <i> default: 300
	//  <i> B2/B4: 	 350
	#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME    300

	//  <o> DO start after [usec] <0-10000:10>
	//  <i> default: 400
	//  <i> B2/B4: 	 450
	#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER       400

	//  <o> DO safe execution [usec] <0-10000:10>
	//  <i> default: 200
	//  <i> B2/B4: 	 50
	#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME    200

	//  <o> TX start after [usec] <0-10000:10>
	//  <i> default: 700
	//  <i> B2/B4: 	 550
	#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER       700

	//  <o> TX safe execution [usec] <0-10000:10>
	//  <i> default: 250
	//  <i> B2/B4: 	 350
	#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME    250
#else
	#define EOMTHEEMSAPPLCFG_RUNOBJ_PERIOD      1000
    #define EOMTHEEMSAPPLCFG_RUNOBJ_TXDECIMATIONFACTOR         1
    
	#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_AFTER    0
	
//	#if ((EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD == 2) || (EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD == 4))
//		#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME     350
//		#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER   	    450
//		#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME     50
//		#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER   		550
//		#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME     350
//	#else
		#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME     300
		#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER   		400
		#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME     200
		#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER   		700
		#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME     250
//	#endif
#endif

//  </e> enable custom timings

//  <h> hardware timers
//  <i> specify the hal timers used for triggering the phases of the runner plus the safetime alerts
//  <i> all the timers must be different.


//  <o> hal_timer for start rx
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//  <i> default: hal_timer6
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTRX   5

//  <o> hal_timer for start do
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//  <i> default: hal_timer7
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTDO   6

//  <o> hal_timer for start tx
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//  <i> default: hal_timer11
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTTX   10

//  <o> hal_timer for alert rx
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//                           <255=> hal_timerNONE
//  <i> default: hal_timer12
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTRX   255

//  <o> hal_timer for alert do
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//                           <255=> hal_timerNONE
//  <i> default: hal_timer13
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTDO   255

//  <o> hal_timer for alert tx
//                           <0=> hal_timer1 <1=> hal_timer2 <2=> hal_timer3 <3=> hal_timer4 <4=> hal_timer5 <5=> hal_timer6 <6=> hal_timer7 
//                           <7=> hal_timer8 <8=> hal_timer9 <9=> hal_timer10 <10=> hal_timer11 <11=> hal_timer12 <12=> hal_timer13 <13=> hal_timer14
//                           <255=> hal_timerNONE
//  <i> default: hal_timer14
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTTX   255


#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTRX   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTRX))
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTDO   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTDO))
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTTX   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_STARTTX))
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTRX   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTRX))
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTDO   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTDO))
#define EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTTX   ((hal_timer_t)(EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_N_ALERTTX))


//  </h> hardware timers


//  <h> behaviour

//  <o> running mode       <0=> best effort <1=> soft realtime <2=> hard realtime
#define EOMTHEEMSAPPLCFG_RUNOBJ_MODE_AT_STARTUP 0
//  <i> default: best effort

//  <o> max number of packets parsed at each RX cycle <0-8>
//  <i> default: 3
#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_MAXPACKETS                   3

//  <o> max number of packets transmitted at each TX cycle <0-1>
//  <i> default: 1
#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_MAXPACKETS                   1


#if(EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMNUMBER < EOMTHEEMSAPPLCFG_RUNOBJ_RX_MAXPACKETS)
    #error EOMtheEMSapplCfg_cfg --> socket must be able to receive at least the number of packets the RX task processes in 1 ms 
#endif

#if(EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMNUMBER < EOMTHEEMSAPPLCFG_RUNOBJ_TX_MAXPACKETS)
    #error EOMtheEMSapplCfg_cfg --> socket must be able to accept for tx at least the number of packets the TX task processes in 1 ms
#endif

//  </h>behaviour


//  </h>Runner object configuration
    
    

// </h>
    
    
//  <h> Overridable functions
    
    
//  <h> EOMtheEMStransceiver    
 
//  <o> eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t exp_seqnum) <0=> called when the EOreceiver detects a wrong sequence number
#define cercervced12345cevr4vr3bvrte 0
    
// </h>EOMtheEMStransceiver        
 

//  <h> EOMtheEMSappl    
 
//  <o> eom_emsappl_hid_userdef_initialise()      <0=> called inside eom_emsappl_Initialise() just before entering the state machine
#define assfef443f43gfvr4vr3bvrte 0
    
// </h>EOMtheEMSappl    
    
    
//  <h> EOMtheEMSconfigurator    
 
//  <o> eom_emsconfigurator_hid_userdef_DoJustAfterPacketParsing()      <0=> called in state CFG just after a datagram is received and parsed
#define assfef443f43gde43fcrec4vr3bvrte 0
    
// </h>EOMtheEMSconfigurator    


//  <h> EOMtheEMSerror    
 
//  <o> eom_emserror_hid_userdef_DoJustAfterPacketReceived()      <0=> called in state ERR just after a datagram is received and parsed
#define assfef443f43wdws56743fcrec4 0
    
// </h>EOMtheEMSerror   



//  <h> EOMtheEMSrunner    
 
//  <o> eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception()      <0=> called in state RUN-RX just before attempting to retrieve a datagram from EOMtheEMSsocket
#define assfef443ferdfws56743fcrec4 0
    
//  <o> eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception()      <0=> called in state RUN-RX just after processing of datagram reception
#define assfef443ferdfsded56743fcrec4 0  


//  <o> eom_emsrunner_hid_userdef_taskDO_activity()      <0=> called in state RUN-DO as the only action to be performed
#define assfef443ferdfs123456743fcrec4 0  


//  <o> eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission()      <0=> called in state RUN-TX before transmission of the datagram
#define assfe666erdfs123456743fcrec4 0  
    
//  <o> eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission()      <0=> called in state RUN-TX after transmission of the datagram
#define assfe66999fs123456743fcrec4 0     
    
    
//  <o> eom_emsrunner_hid_userdef_onexecutionoverflow()      <0=> called in state RUN if in realtime mode any of RX-DO-TX has overflown to the next one
#define assfe3333355fs123456743fcrec4 0  


//  <o> eom_emsrunner_hid_userdef_onfailedtransmission()      <0=> called inside RUN-TX if teh datagram could not be transmitted
#define assfe66999999906743fcrec4 0  

// </h>EOMtheEMSrunner   


//  <h> EOMtheEMSbackdoortransceiver    
 
//  <o> eom_emsbackdoortransceiver_hid_userdef_get_OPCprotocolManager_cfg()      <0=> used to allow the backdoortransceiver to manage a given OPC protocol
#define assfef443f43wdws56743fcrsd43d234 0
    
// </h>EOMtheEMSbackdoortransceiver   


//  </h>Overridable functions    
    


// -------------------------------------------------------------------------------------------------------------------
// derived macros
// -------------------------------------------------------------------------------------------------------------------

// in here we put only the macros required for the reference application
#if     defined(ICUB_MEC_V1)
    #define NAME_PREFIX     aEMS
    //#define NAME_POSTFIX    v1mec
    #define NAME_POSTFIX    gen04
#elif   defined(ICUB_MEC_V2)
    #define NAME_PREFIX     aEMS
    #define NAME_POSTFIX    v2mec
#elif   defined(CER)
    #define NAME_PREFIX     aEMS
    #define NAME_POSTFIX    cer    
#else
    //#error -> specify either ICUB_MEC_V1 or ICUB_MEC_V2
    #define NAME_PREFIX     aEMS
    #define NAME_POSTFIX    all
#endif    
  

#define PPCAT_NX(A, B, C)   A ## B ## C
#define PPCAT(A, B, C)      PPCAT_NX(A, B, C)

#define STRINGIZE_NX(A)     #A
#define STRINGIZE(A)        STRINGIZE_NX(A)


        #define     NUM     00
        #define     EOMTHEEMSAPPLCFG_NAME                   STRINGIZE(PPCAT(NAME_PREFIX, NUM, NAME_POSTFIX))

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// keep at the end because the configuration wizard can parse not many lines of code ...
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

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

