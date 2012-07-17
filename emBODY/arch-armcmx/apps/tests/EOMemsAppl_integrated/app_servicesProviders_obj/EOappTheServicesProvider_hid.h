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
#ifndef _EOAPPTHESERVICESPROVIDER_HID_H_
#define _EOAPPTHESERVICESPROVIDER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappTheServicesProvider.h
    @brief     This file provides hidden interface to TheServicesProvider singleton.
    @author    valentina.gaggero@iit.it
    @date      07/16/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOarray.h"
#include "EoManagement.h"

//services
#include "EOappCanServicesProvider.h"
#include "EOappTheCanBoardsManager.h"
#include "EOappTheNVmapRef.h"
#include "EOappEncodersReader.h"
#include "EOemsController.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappTheServicesProvider.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef enum
{
    eO_appTheSP_st__NOTinited   = 0,
    eO_appTheSP_st__inited      = 1,
} eO_appTheSP_states_hid_t;



struct EOappTheSP_hid
{
    eOappTheSP_cfg_t                    cfg;
    eO_appTheSP_states_hid_t            st;
    eOmn_appl_runMode_t                 appRunMode;
    EOarray                             *jointsList;        /**<  List of joints used by the application  */
    EOarray                             *motorsList;        /**<  List of motors used by the application  */
    EOarray                             *sensorsStrainList; /**<  List of sensors-strain used by the application */
    EOarray                             *sensorsMaisList;   /**<  List of sensors-mais used by the application */
    EOarray                             *skinList;          /**<  List of sensors used by the application */
    struct
    {
        EOappCanSP                          *appCanSP;
        EOappTheCanBrdsMng                  *appTheCanBrdsMng;
        EOappTheNVmapRef                    *appTheNVmapRef;
        EOappEncReader                      *appEncReader;
        EOemsController                     *emsController;
    }srv;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



