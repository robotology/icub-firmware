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

#ifndef _ICUBCANPROTO_BOOTLOADERMESSAGES_H_
#define _ICUBCANPROTO_BOOTLOADERMESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_bootloaderMessagges.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands relating to bootLoader
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
/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_LOADER
/***************************************************************************/
#define ICUBCANPROTO_BL_BOARD                 0x00 
#define ICUBCANPROTO_BL_ADDRESS               0x01 
#define ICUBCANPROTO_BL_START                 0x02 
#define ICUBCANPROTO_BL_DATA                  0x03 
#define ICUBCANPROTO_BL_END                   0x04 
#define ICUBCANPROTO_BL_ERROR                 0x05 
#define ICUBCANPROTO_BL_GET_ADDITIONAL_INFO   0x0C 
#define ICUBCANPROTO_BL_SET_ADDITIONAL_INFO   0x0D 
#define ICUBCANPROTO_BL_BROADCAST             0xFF 

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
