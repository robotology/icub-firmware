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
#ifndef _EOCFG_ICUBCNAPROTO_SENSORBOARDMSGLOOKUPTBL_H_
#define _EOCFG_ICUBCNAPROTO_SENSORBOARDMSGLOOKUPTBL_H_




/** @file       eOcfg_icubCanProto_sensorBoardMsgLookupTbl.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/

/** @defgroup eo_icubCanProto Configuation of the motor board messages
              lookup tables (one for parser and one for former).
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "EOconstLookupTbl.h"



// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//this is the mesdsages class lookup table: it associates parser and former tbl for a given class 
extern const EOconstLookupTbl        icubCanProto_pollingSensorBoardMsg_parser_LUTbl;
extern const EOconstLookupTbl* const icubCanProto_pollingSensorBoardMsg_parser_LUTbl__ptr;

extern const EOconstLookupTbl        icubCanProto_pollingSensorBoardMsg_former_LUTbl;
extern const EOconstLookupTbl* const icubCanProto_pollingSensorBoardMsg_former_LUTbl__ptr;


extern const EOconstLookupTbl        icubCanProto_periodicSensorBoardMsg_parser_LUTbl;
extern const EOconstLookupTbl* const icubCanProto_periodicSensorBoardMsg_parser_LUTbl__ptr;
// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




