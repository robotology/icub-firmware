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

#ifndef _ICUBCANPROTO_SKINMESSAGES_H_
#define _ICUBCANPROTO_SKINMESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_skinMessages.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands relating to skin
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
// ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR 
/****************************************************************************/
#define ICUBCANPROTO_POL_SK_CMD__TACT_SETUP         76 //0x4C
#define ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG        77 //0x4D
#define ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2        78 //0x4E
#define ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP     79 //0x4F
#define ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG     80 //0x50

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







