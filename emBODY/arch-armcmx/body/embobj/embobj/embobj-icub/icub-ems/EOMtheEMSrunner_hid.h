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
#include "osal_timer.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSrunner.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMSrunner_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSrunner_hid 
{
    EOMtask*            task[eo_emsrunner_task_numberof];
    eOemsrunner_cfg_t   cfg;
    eObool_t            cycleisrunning;
    eOsmEventsEMSappl_t event;
    osal_timer_t*       osaltimer;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------


extern void eom_emsrunner_hid_userdef_taskRX_beforedatagramreception(void);
extern void eom_emsrunner_hid_userdef_taskRX_afterdatagramreception(uint16_t numberofrxrops, eOabstime_t txtimeofrxropframe);

extern void eom_emsrunner_hid_userdef_taskDO_activity(void);

extern void eom_emsrunner_hid_userdef_taskTX_beforedatagramtransmission(void);
extern void eom_emsrunner_hid_userdef_taskTX_afterdatagramtransmission(uint16_t numberoftxrops);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

