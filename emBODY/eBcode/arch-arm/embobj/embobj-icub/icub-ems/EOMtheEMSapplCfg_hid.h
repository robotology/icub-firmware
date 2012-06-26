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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMTHEEMSAPPLCFG_HID_H_
#define _EOMTHEEMSAPPLCFG_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSapplCfg_hid.h
    @brief      This header file implements hidden interface to the EMS application configuration singleton.
    @author     marco.accame@iit.it
    @date       06/25/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOtheBOARDtransceiver.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSapplCfg.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#warning --> metti il nome della board sotto ifndef. metti anche altre cosette.

#define EOMTHEEMSAPPLCFG_HOSTIPADDR1                            10
#define EOMTHEEMSAPPLCFG_HOSTIPADDR2                            255
#define EOMTHEEMSAPPLCFG_HOSTIPADDR3                            72
#define EOMTHEEMSAPPLCFG_HOSTIPADDR4                            205

#define EOMTHEEMSAPPLCFG_HOSTIPPORT                             33333


#define EOMTHEEMSAPPLCFG_TMRMAN_TASK_PRIORITYof                 240
#define EOMTHEEMSAPPLCFG_TMRMAN_TASK_STACKSIZEof                512
#define EOMTHEEMSAPPLCFG_TMRMAN_TASK_QUEUESIZEof                8

#define EOMTHEEMSAPPLCFG_CBKMAN_TASK_PRIORITYof                 202
#define EOMTHEEMSAPPLCFG_CBKMAN_TASK_STACKSIZEof                512
#define EOMTHEEMSAPPLCFG_CBKMAN_TASK_QUEUESIZEof                8


#define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_PRIORITYof              220
#define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_STACKSIZEof             1024
#define EOMTHEEMSAPPLCFG_IPNETPROC_TASK_MAXIDLETIMEof           20000
#define EOMTHEEMSAPPLCFG_IPNETPROC_WAKEUPONRXFRAME              1
#define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_PRIORITYof              219
#define EOMTHEEMSAPPLCFG_IPNETTICK_TASK_STACKSIZEof             128


#define EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFSOCKETS                  1
#define EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFDATAGRAMSENQUEUEDINTX    4


#define EOMTHEEMSAPPLCFG_IPADDR_FROM_ENVIRONMENT                0

#define EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMNUMBER                  3
#define EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF                  768
#define EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMNUMBER                  2
#define EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF                  1024
#define EOMTHEEMSAPPLCFG_SOCKET_LOCALPORT                       12345

#define EOMTHEEMSAPPLCFG_ERROBJ_TASK_PRIORITYof                 40
#define EOMTHEEMSAPPLCFG_ERROBJ_TASK_STACKSIZEof                512


#define EOMTHEEMSAPPLCFG_CFGOBJ_TASK_PRIORITYof                 50
#define EOMTHEEMSAPPLCFG_CFGOBJ_TASK_STACKSIZEof                512

#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_PRIORITYof              250
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_PRIORITYof              251
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_PRIORITYof              252

#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_STACKSIZEof             1024
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_STACKSIZEof             1024
#define EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_STACKSIZEof             1024


#define EOMTHEEMSAPPLCFG_RUNOBJ_PERIOD                          1000
#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_AFTER                        0
#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME                     300
#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER                        400
#define EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME                     250
#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER                        700
#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME                     250

#define EOMTHEEMSAPPLCFG_RUNOBJ_RX_MAXPACKETS                   3
#define EOMTHEEMSAPPLCFG_RUNOBJ_TX_MAXPACKETS                   1

#define EOMTHEEMSAPPLCFG_RUNOBJ_MODE_AT_STARTUP                 0

// - definition of the hidden struct implementing the object ----------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// <h> Configuration of EMS application
// <i> It holds configuration for .


// <h> System specifics 
// <i> Keeps configuration used by object EOMtheSystem.

//   <o> hal            <0=> see hal_cfg.h
//   <i> defefe.
#ifndef sdfefe0
 #define sdfefe0      0
#endif


//   <o> osal           <0=> see osal-cfg.h
//   <i> defefe.
#ifndef sdfefe1
 #define sdfefe1      0
#endif

//   <o> error manager  <0=>   redefine eom_emsapplcfg_hid_userdef_OnError() 
//   <i> defefe.
#ifndef sdfefe2
 #define sdfefe2      0
#endif

//   <o> timer manager  <0=>   task priority is 240, stack is 512, message queue size is 8 
//   <i> defefe.
#ifndef sdfefe3
 #define sdfefe3      0
#endif


//   <o> callback manager  <0=>   task priority is 202, stack is 512, message queue size is 8 
//   <i> defefe.
#ifndef sdfefe4
 #define sdfefe4      0
#endif



// </h>System


// <h> Network Specifics 
// <i> Keeps ...

//   <o> ipal            <0=> see ipal_cfg.h
//   <i> defefe.
#ifndef sdfefe00
 #define sdfefe00      0
#endif


//   <o> EOMtheIPnet            <0=> proc/tick prio and stacksize are 220/219, 1024/128. idletime is 20milli. IPnet is waken-up by the ETH ISR 
//   <i> defefe.
#ifndef sdfefe01
 #define sdfefe01      0
#endif


//   <o> sockets in EOMtheIPnet <0=> max 1 UDP socket with 4 packets in output queue 
//   <i> defefe.
#ifndef sdfefe02
 #define sdfefe02      0
#endif


// </h>Networks


// </h>






// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

