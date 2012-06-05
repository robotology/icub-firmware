/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOMAPPSYSCONTROLLER_HID_H_
#define _EOMAPPSYSCONTROLLER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOMappSysController.h
    @brief      his header file implements hidden interface to EOMappSysController obj
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOcommon.h"
#include "EOMtask.h"
#include "EOarray.h"

#include "EOmanagement.h"
#include "EOtheBOARDtransceiver.h"

//application services provioders
#include "EOethBaseModule.h"
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
//application actors
#include "EOMappDataCollector.h"
#include "EOMappDataTransmitter.h"
#include "EOMappMotorController.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMappTheSysController.h"

// - definition of the hidden struct implementing the object ----------------------------------------------------------
typedef enum
{
    eOm_appTheSysController_st__NOTinited   = 0,
    eOm_appTheSysController_st__inited      = 1,
    eOm_appTheSysController_st__started     = 2,

} eOm_appTheSysController_states_hid_t;

// typedef enum
// {
//     eOm_appTheSysController_runMode__skinOnly      = 0,
//     eOm_appTheSysController_runMode__mc4Only       = 1,
//     eOm_appTheSysController_runMode__skinAndMc4    = 2,
//     eOm_appTheSysController_runMode__2foc          = 3
// } eOm_appTheSysController_apRunMode_hid_t;

typedef enum
{
    eOm_appTheSysController_sockRunMode__onEvent    = 0,
    eOm_appTheSysController_sockRunMode__onDemand   = 1
} eOm_appTheSysController_socketRunMode_hid_t;

struct EOMappTheSysController_hid
{
    EOMappTheSysController_cfg_t            cfg;              /**< configuration data */
    EOMtask                                 *mytask;          /**< pointer to my task */
    EOarray                              *jointsList;  /**<  List of joints used by the application  */
    EOarray                              *motorsList;  /**<  List of motors used by the application  */
    EOarray                              *sensorsList; /**<  List of sensors used by the application */
    EOarray                              *skinList;    /**<  List of sensors used by the application */

    /* services provider hendler*/
    struct
    {
        EOappCanSP                              *appCanSP_ptr;       /**< pointer to can service provider*/
        EOappEncReader                          *appEncReader_ptr;   /**< pointer to encoder reader*/
        EOethBaseModule                         *ethMod_ptr;         /**< pointer to eth module */
        EOappTheNVmapRef                        *appTheNVmapRef_ptr; /**< pointer to nv map references */
        EOtransceiver                           *transceiver;        /**< pointer to the board transceiver*/
    } srv;

    /* actors */ 
    struct
    {
        EOMappDataCollector                     *appDataCollector_ptr;
        EOMappMotorController                   *appMotorController_ptr;
        EOMappDataTransmitter                   *appDataTransmitter_ptr;
    } act;    

    /* states*/
    /* retptr->appl_state and   retptr->appl_runMode are meaningless until st is eOm_appTheSysController_st__started*/
    eOmn_appl_state_t                        appl_state;   /* state of application*/
    eOmn_appl_runMode_t                      appl_runMode; /* run mode of application*/
    eOm_appTheSysController_states_hid_t     st;                /**< status of the singleton */
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



