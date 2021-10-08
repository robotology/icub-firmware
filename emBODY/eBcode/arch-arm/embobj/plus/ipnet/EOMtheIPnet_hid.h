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
#ifndef _EOMTHEIPNET_HID_H_
#define _EOMTHEIPNET_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheIPnet_hid.h
    @brief      This header file implements hidden interface to the rtos IP net singleton.
    @author     marco.accame@iit.it
    @date       08/24/2011
**/

// marco.accame@iit.it on sept 2020:
// - substituted osal with embot::os when macro EMBOBJ_USE_EMBOT is defined

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtheIPnet.h"
#include "EOMtask.h"
#include "EOaction.h"
#include "EOtimer.h"    
#if !defined(EMBOBJ_USE_EMBOT)
#include "osal.h"
#else
#include "embot_os_rtos.h"
#endif
#include "ipal.h"
#include "EOpacket.h"    
      

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheIPnet.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------



typedef enum
{   // contains all possible command types that can be sent to the EOMtask tskproc
    cmdDoNONE       = 0,
    cmdDoARP        = 1,
    cmdAttachDTG    = 2,
    cmdDetachDTG    = 3
} EOMtheIPnetCmdCode;

typedef struct  
{   // is the command sent to the task tskprocl
#if !defined(EMBOBJ_USE_EMBOT)    
    osal_mutex_t            *mtxcaller;     // mutex used to stop other tasks to use the same command 
#else    
    embot::os::rtos::mutex_t *mtxcaller;     // mutex used to stop other tasks to use the same command  
#endif  // !defined(EMBOBJ_USE_EMBOT)  
    EOMtheIPnetCmdCode      opcode;         // the opcode of the command
    uint8_t                 repeatcmd;      // flag used to tell the receiver of the command to process it againg
    uint8_t                 result;         // the result of the command: 1 is OK
    uint16_t                par16b;         // first parameter
    uint32_t                par32b;         // second parameter
    uint32_t                par32x;         // third parameter
    uint64_t                par64x;         // fourth parameter
    uint32_t                tout;           // the timeout of the command 
#if !defined(EMBOBJ_USE_EMBOT)
    osal_semaphore_t        *blockingsemaphore;     // used by the caller to block until the end of the command execution.
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    osal_semaphore_t        *busysemaphore; // used by the caller to verify if another task (or the same task) is using the command
#endif
#else
    embot::os::rtos::semaphore_t *blockingsemaphore;
#if defined(IPNET_HAS_NON_BLOCKING_COMMAND)
    embot::os::rtos::semaphore_t *busysemaphore;
#endif    
#endif // !defined(EMBOBJ_USE_EMBOT)   
    EOtimer                 *stoptmr;       // used to drop the command after tout microsec even if the executer hasnt finished it
    EOaction                *stopact;       // the action to be done at expiry of the stop timer
} EOMtheIPnetCommand; 

/* @struct     EOMtheIPnet_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOMtheIPnet_hid 
{ 
    // base object
    EOVtheIPnet                     *ipnet;                 /*< the base object */
    
    // other stuff
    EOMtask                         *tskproc;               /*< the main task whcih process tcp/ip traffic and commands from other tasks */
    EOMtask                         *tsktick;               /*< the task which ticks the tcp/ip stack for updating its internal timing */
    EOMtheIPnetCommand              cmd;                    /*< the command used by other tasks to issue request to the EOMtheIPnet */
    EOpacket                        *rxpacket;              /*< contains the temporary packet received by the network */
    eOreltime_t                     maxwaittime;            /*< a generic wait time */
#if !defined(EMBOBJ_USE_EMBOT)
    osal_messagequeue_t             *dgramsocketready2tx;   /*< a fifo containing pointers of the datagram sockets which need to tx a packet  */
#else
    embot::os::rtos::messagequeue_t *dgramsocketready2tx;   /*< a fifo containing pointers of the datagram sockets which need to tx a packet  */
#endif
    eObool_t                        taskwakeuponrxframe;    /*< tells to send an evt to tskproc when there is a received eth frame */
    eObool_t                        active;
#if defined(IPAL_use_cfg2)    
    ipal_cfg2_t                     ipcfg2; 
    ipal_cfg2_eth_t                 ipcfg2_eth;
    ipal_cfg2_extfn_t               ipcfg2_extfn2;
#else
    ipal_cfg_t                      ipcfg;
#endif    
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------



// name of the tasks as it is shown in uvision

void tIPNETproc(void *p);

void tIPNETtick(void *p);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




