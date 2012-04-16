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
#ifndef _EOICUBCANPROTO_H_
#define _EOICUBCANPROTO_H_


/** @file       EoicubCanProto.h
	@brief      This header implements public interface to iCub CAN protocol
	@author     valentina.gaggero@iit.it
	@date       03/08/2012
**/

/** @defgroup eo_icubCanProto Object EoicubCanProto
    this object provide function to parse and form can frame in according with icub can protocol.
   
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoUtilities.h"
#include "EoMotionControl.h"
#include "EoconstLookuptbl.h"
#include "EOemsCanNetworkTopology.h"


#include "EOicubCanProto_specifications.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EoicubCanProto_hid EoicubCanProto
    @brief      .....
 **/  
typedef struct EOicubCanProto_hid EOicubCanProto;




typedef struct
{
    const EOconstLookupTbl* const msgClasses_LUTbl__ptr;  /**< the lookup table that contains for each message class 
                                                               lookup table indexed by can message of that class. 
                                                               The last lookup table contains parser and former funaction for
                                                               each can message*/
    EOemsCanNetTopo         *emsCanNetTopo__ptr;          /**< reference to configuration of ems can network topology */
} eo_icubCanProto_cfg_t;

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

//currently does not exist default config.
//extern const eo_icubCanProto_cfg_t eo_icubCanProto_cfg_default; 

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern EOicubCanProto* eo_icubCanProto_New(const eo_icubCanProto_cfg_t *cfg)
    @brief      create an new icubCanProto. Usually exist one icubCanProto for application.   
    @param      cfg         configuration of icub can protocol             
    @return     eores_OK.
 **/
extern EOicubCanProto* eo_icubCanProto_New(eo_icubCanProto_cfg_t *cfg); 



/** @fn         extern eOresult_t eo_icubCanProto_parseCanFrame(hal_can_frame_t *frame)
    @brief      Parse a reaceived can frame.
                if the received frame is formatting in according with icub can protocol,
                the object calls function call back memorized in lookup table.
    @param      p               reference to the object
    @param      frame           The received frame.  
    @return     returns the result of function call back or eores_NOK_nullpointer
                if frame or p are NULL.
 **/
extern eOresult_t eo_icubCanProto_ParseCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX);




/** @fn         extern eOresult_t  eo_icubCanProto_formCanFrame
    @brief      formatting the can frame given in last param in according with icub can Protocol.
                Given the command the object invokes the associated call back function to form the frame.
    @param      p               reference to the object
    @param      command         the command to send
    @param      dest            the destination of the frame
    @param      value           the value of the command (the argument). can be NULL.
    @param      frame           in output contains the frame formtted correctly
    @return     eores_NOK_nullpointer if p or frame are NULL, else the result of callback function.
 **/
extern eOresult_t eo_icubCanProto_FormCanFrame(EOicubCanProto* p,
                                               eo_icubCanProto_msgCommand_t command, 
                                               eo_icubCanProto_msgDestination_t dest,
                                               void *value,
                                               eOcanframe_t *frame);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group eo_icubCanProto  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


