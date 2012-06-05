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
#ifndef _EOAPCANSERVICESPROVIDER_HID_H_
#define _EOAPCANSERVICESPROVIDER_HID_H_


/*  @file       EOappCanServicesProvider_hid.h
    @brief      this file implemnts the hidden interfacto to EOappCanServicesProvider obj.
    @author     valentina.gaggero@iit.it
    @date       02/14/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

//embobj of application
#include "EOappCanServicesProvider.h"
#include "EOappEncodersReader.h"
#include "EOMappDataCollector.h"
#include "EOMappDataTransmitter.h"
#include "EOMappMotorController.h"
#include "EOMappTheSysController.h"
#include "EOappTheNVmapRef.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOappSkeletonEms_body.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    EOappCanSP                              *appCanSP_ptr;      /**< pointer to can service provider*/
    EOappEncReader                          *appEncReader_ptr;   /**< pointer to encoder reader*/
    EOethBaseModule                         *ethMod_ptr;         /**< pointer to eth module */
} eOm_appObjServicesProviders_t;


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



