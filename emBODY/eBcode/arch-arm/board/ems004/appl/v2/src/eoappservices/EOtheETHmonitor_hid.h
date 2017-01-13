/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHEETHMONITOR_HID_H_
#define _EOTHEETHMONITOR_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"
#include "osal_semaphore.h"
#include "hal_ethtransceiver.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheETHmonitor.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eObool_t                            on;
    eObool_t                            previouson;
    hal_ethtransceiver_phystatus_t      phy;
    hal_ethtransceiver_phyerrorinfo_t   rxcrc;    
} eOethmonitor_port_status_t;

enum { eOethmonitor_numberofports = 3 };

struct EOtheETHmonitor_hid
{
    eObool_t                    initted;
    eObool_t                    enabled;
    eObool_t                    newresultsavailable;
    EOMtask*                    taskworker;
    osal_semaphore_t*           semaphoreworker;
    osal_semaphore_t*           alertsemaphore;
    EOMtask*                    task2alert;
    eOevent_t                   alertevent;
    uint8_t                     upmask;
    uint64_t                    lastsequencenumbererror;
    uint16_t                    lastnumberofseqnumbererrors;
    eOethmonitor_port_status_t  portstatus[eOethmonitor_numberofports];  // for hal_ethtransceiver_phy0 (P2) and hal_ethtransceiver_phy1 (P3) and hal_ethtransceiver_phy1 (rmii)
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

void eo_ethmonitor(void *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



