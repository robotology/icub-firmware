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
#ifndef _EOAPPTHEMCNVMAP_HID_H_
#define _EOAPPTHEMCNVMAP_HID_H_


/* @file       EOappTheMCNVmap_hid.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/17/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMotionControl.h"
#include "eOcfg_nvsEP_mc.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOappTheNVmapRef.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------




// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EONtheSystem_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  


struct EOappTheNVmapRef_hid 
{
    eOboolvalues_t     isInited; 
    EOnvsCfg           *nvsCfg;
    eOnvEP_t           mc_endpoint;
    eOnvEP_t           as_endpoint;
    void**             jointsList[jointNumberMAX];
    void**             motorsList[motorNumberMAX];
    void**             sensorsList[motorNumberMAX];   //  TODO: metti il num dei sensori
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

