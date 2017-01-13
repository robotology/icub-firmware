/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHEMEMS_HID_H_
#define _EOTHEMEMS_HID_H_


#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"
#include "osal_semaphore.h"
#include "hal_ethtransceiver.h"

#include "EOvector.h"
#include "EOtimer.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheMEMS.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheMEMS_hid
{
    EOvector*                   fifoofinertialdata;
    osal_semaphore_t*           semaphorefifo;
    eObool_t                    initted;
    eObool_t                    enabled;
    eObool_t                    newresultsavailable;
    EOMtask*                    taskworker;
    EOtimer*                    ticktimer;
    eOmems_sensor_cfg_t         config; // only one, so far
    eObool_t                    sensorisinitted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

void eo_mems(void *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



