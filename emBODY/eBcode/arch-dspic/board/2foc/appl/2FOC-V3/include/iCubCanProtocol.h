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

#ifndef _ICUBCANPROTOCOL_H_
#define _ICUBCANPROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       iCubCanProtocol.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands.
    @author     valentina.gaggero@iit.it
    @date       12/03/2012
    @ingroup    iCubCanProtocol
**/

/** @defgroup    iCubCanProtocol
Definitions of the iCub CAN protocols commands
    @{
 **/

 
// - external dependencies --------------------------------------------------------------------------------------------

#include "iCubCanProto_classes.h"
#include "iCubCanProto_motorControlMessages.h"
#include "iCubCanProto_analogSensorMessages.h"
#include "iCubCanProto_skinMessages.h"
#include "iCubCanProto_inertialSensorMessages.h"
#include "iCubCanProto_bootloaderMessages.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

/** @} **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







