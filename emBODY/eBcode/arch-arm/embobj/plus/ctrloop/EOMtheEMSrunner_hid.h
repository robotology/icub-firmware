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
#ifndef _EOMTHEEMSRUNNER_HID_H_
#define _EOMTHEEMSRUNNER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSrunner_hid.h
    @brief      This header file implements hidden interface to the error manager
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"
#include "eOcfg_sm_EMSappl.h"
#include "EOtimer.h"
#include "osal.h"
#include "hal.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSrunner.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------

//#define EVIEWER_ENABLED

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMSrunner_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSrunner_hid 
{
    eOemsrunner_cfg_t   cfg;
    EOMtask*            task[eo_emsrunner_task_numberof];
    eOsmEventsEMSappl_t event;
    osal_timer_t*       osaltimer;
    eObool_t            cycleisrunning;
    eObool_t            safeDurationExpired[eo_emsrunner_task_numberof]; 
    eObool_t            overflownToNextTask[eo_emsrunner_task_numberof]; 
    hal_timer_t         haltimer_start[eo_emsrunner_task_numberof];
    hal_timer_t         haltimer_alert[eo_emsrunner_task_numberof];
    uint16_t            numofrxpackets;
    uint16_t            numofrxrops;
    uint16_t            numoftxpackets;
    uint16_t            numoftxrops;
    eOemsrunner_mode_t  mode;
    uint8_t             numofpacketsinsidesocket;
    osal_semaphore_t*   waitudptxisdone;
    osal_task_t*        osaltaskipnetexec;
};



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void tskEMSrunRX(void *p);
extern void tskEMSrunDO(void *p);
extern void tskEMSrunTX(void *p);


#if defined(EVIEWER_ENABLED)
void evRXstart(void);
void evRXalert(void);
void evDOstart(void);
void evDOalert(void);
void evTXstart(void);
void evTXalert(void);
void usrDef_RUNRecRopframe(void);
#endif

// default function for RX: it calls _beforedatagramreception(), _datagramreception(), _afterdatagramreception()
extern void eom_emsrunner_hid_userdef_taskRX_activity(EOMtheEMSrunner *p);

// default function for RX or TX: there is a transceiver error
extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p);

// default function for RX-before-datagram-reception: it is empty.
extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p);
// default function for RX-datagram-reception: it repeates upto xx times: get a pkt, call the transceiver, verifies if a quit evt has arrived.
extern void eom_emsrunner_hid_userdef_taskRX_activity_datagramreception(EOMtheEMSrunner *p);
// deafult function for RX-after-datagram-reception
extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p);


extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p);


extern void eom_emsrunner_hid_userdef_taskTX_activity(EOMtheEMSrunner *p);

extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskTX_activity_datagramtransmission(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p);


extern void eom_emsrunner_hid_userdef_onexecutionoverflow(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);

extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p);


// - inline functions

extern inline eObool_t eom_runner_hid_cansafelyexecute(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    eObool_t ret = eobool_false;

    switch(p->mode)
    {
        case eo_emsrunner_mode_besteffort:
        {
            ret = eobool_true;
        } break;
        
        case eo_emsrunner_mode_softrealtime:
        {
            ret = (eobool_false == p->safeDurationExpired[taskid]) ? (eobool_true) : (eobool_false);
        } break; 
        
        case eo_emsrunner_mode_hardrealtime:
        {
            ret = (eobool_false == p->safeDurationExpired[taskid]) ? (eobool_true) : (eobool_false);
        } break;  

        default:
        {
            ret = eobool_false;
        } break;
        
    }   
    
    return(ret);
    
}


extern inline eObool_t eom_runner_hid_signaloverflow(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid)
{
    eObool_t ret = eobool_false;

    switch(p->mode)
    {
        case eo_emsrunner_mode_besteffort:
        {
            ret = eobool_false;
        } break;
        
        case eo_emsrunner_mode_softrealtime:
        case eo_emsrunner_mode_hardrealtime:
        {
            ret = p->overflownToNextTask[taskid];
        } break;  

        default:
        {
            ret = eobool_false;
        } break;
        
    }   
    
    return(ret);
    
}


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

