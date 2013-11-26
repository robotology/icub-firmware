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
#ifndef _EODEB_EOPROTOPARSER_HID_H_
#define _EODEB_EOPROTOPARSER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       eODeb_eoProtoParser_hid.h
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "eODeb_eoProtoParser.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

struct eODeb_eoProtoParser_hid
{
    eODeb_eoProtoParser_cfg_t     cfg;
    uint8_t                       initted;
};
// - declaration of extern hidden functions ---------------------------------------------------------------------------



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




