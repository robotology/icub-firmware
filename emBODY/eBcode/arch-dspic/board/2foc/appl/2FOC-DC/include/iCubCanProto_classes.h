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

#ifndef _ICUBCANPROTO_CLASSES_H_
#define _ICUBCANPROTO_CLASSES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_classes.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands classes.
    @author     valentina.gaggero@iit.it
    @date       12/03/2012
    @ingroup    iCubCanProtocol
**/

/** @ingroup    iCubCanProtocol
    @{
 **/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------
/****************************************************************************/
// ICUBCANPROTO_POL_MB_CMD_ messages class
/****************************************************************************/
#define ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL               0x00
#define ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL              0x01
#define ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR               0x02
#define ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR              0x03
#define ICUBCANPROTO_CLASS_PERIODIC_SKIN                      0x04
#define ICUBCANPROTO_CLASS_PERIODIC_INERTIALSENSOR            0x05
#define ICUBCANPROTO_CLASS_BOOTLOADER                         0x07
// marco.accame: is this macro ICUBCANPROTO_CLASS_MAXNUM used? 
//               if, so, after the addition of new class ICUBCANPROTO_CLASS_PERIODIC_INERTIALSENSOR, it should become 0x06 
//               (the ICUBCANPROTO_CLASS_BOOTLOADER is not strangely not counted). 
#define ICUBCANPROTO_CLASS_MAXNUM                             0x05


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
