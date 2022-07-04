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
#include "EOtimer.h"
#if !defined(EMBOBJ_USE_EMBOT)    
#include "osal.h"
#else
//#include "embot_os_rtos.h" 
// cannot include a c++ file because ... 
// this file EOMtheEMSrunner_hid.h is used by a .cpp file, so ... __cplusplus is defined, so ... linkage is C style
// and as embot_os_rtos.h contains C++ only .h files it give errors.
// so, the easiest solution is to avoid using embot::os::rtos::semaphore_t* and using void* instead
#endif

#include "hal_timer.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSrunner.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------



typedef struct
{
    eOabstime_t         timestarted;
    eOabstime_t         timestopped;
    eOabstime_t         duration[2];    // 0 is current iteration, 1 is previous iteration
    volatile eObool_t   isexecuting;
    volatile eObool_t   isabout2overflow;
    volatile eObool_t   isoverflown;
} eOemsrunner_tasktiming_t;


typedef struct
{
    eObool_t                    cycleisrunning;
    uint64_t                    iterationnumber;
    eOemsrunner_tasktiming_t    tasktiming[eo_emsrunner_task_numberof];    
} eOemsrunner_cycletiming_t;


/** @struct     EOMtheEMSrunner_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSrunner_hid 
{
    eOemsrunner_cfg_t           cfg;
    EOMtask*                    task[eo_emsrunner_task_numberof];
    eOsmEventsEMSappl_t         event;
    hal_timer_t                 haltimer_start[eo_emsrunner_task_numberof];
    hal_timer_t                 haltimer_safestop[eo_emsrunner_task_numberof];
    uint16_t                    numofrxpackets;
    uint16_t                    numofrxrops;
    uint16_t                    numoftxpackets;
    uint16_t                    numoftxrops;
    eOemsrunner_mode_t          mode;
    uint8_t                     numofpacketsinsidesocket;
#if !defined(EMBOBJ_USE_EMBOT)      
    osal_semaphore_t*           waitudptxisdone;     
#else
    void*                       waitudptxisdone;
#endif    
    uint8_t                     usedTXdecimationfactor;
    eOtransmitter_ropsnumber_t  txropsnumberincycle;
    uint8_t                     txcan1frames;
    uint8_t                     txcan2frames;
    eOemsrunner_cycletiming_t   cycletiming;
    eObool_t                    isrunning;
};



// - declaration of extern hidden functions ---------------------------------------------------------------------------

// so that we can see it on uvision
void tRX(void *p);
void tDO(void *p);
void tTX(void *p);


// default overridable function (weakly defined) for: transceiver error
extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p);

// default overridable function (weakly defined) for: failed transmission
extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p);

// default overridable functions (weakly defined) for: rx, do, tx
extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p);
extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

