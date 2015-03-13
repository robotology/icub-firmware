/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _EOTHECANPROTOCOL_H_
#define _EOTHECANPROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheCANprotocol.h
	@brief      This header file implements public interface to the singleton which parses and form can frames in iCub.
 	@author     marco.accame@iit.it
	@date       12 mar 2015
 **/

/** @defgroup eo_thecanprotocol Singleton EOtheCANprotocol 
    
    It manages can protocol in icub.  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
    
#include "iCubCanProtocol.h"  


// - public #define  --------------------------------------------------------------------------------------------------


#define EOCANPROT_FRAME2SOURCEADDRESS(frame)    ( (frame->id & 0x0F0) >> 4 )

#define EOCANPROT_MC_FRAME2INTERNALINDEX(frame) ( (frame->data[0] & 0x80) >> 7)

#define EOCANPROT_MC_POL_CREATE_ID(orig, dest)  ( (ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL << 8) | ((orig&0xf) << 4) | (dest&0xf) )  
 
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOtheCANprotocol_hid EOtheCANprotocol
    @brief      EOtheCANprotocol is an opaque struct.
 **/  
typedef struct EOtheCANprotocol_hid EOtheCANprotocol;


typedef enum
{
    eocanprot_msgclass_pollingMotorControl          = ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL,          // 0
    eocanprot_msgclass_periodicMotorControl         = ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL,         // 1
    eocanprot_msgclass_pollingAnalogSensor          = ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR,          // 2
    eocanprot_msgclass_periodicAnalogSensor         = ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR,         // 3
    eocanprot_msgclass_periodicSkin                 = ICUBCANPROTO_CLASS_PERIODIC_SKIN                  // 4
} eOcanprot_msgclass_t;

enum { eocanprot_msgclass_numberofthem = 5 };


/** @typedef    typedef struct eOcanprot_descriptor_t
    @brief      Contains whatever is required to form the can frame.
 **/ 
typedef struct
{
    uint8_t     msgclass;               // they are: ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL, etc. they may be redefined as enum (see icubCanProto_msgCommand_class_t) at max they can be in 4 bits.
    uint8_t     msgidentifier;          // they are: ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, etc.  one byte is required
    uint8_t     destinationaddress;     // it is the can address. at max 4 bits
    uint8_t     internalindex;          // it is either 0 or 1 or none. it is used if the message is for a joint/motor inside the can board. otherwise it is not used.
    void*       value;                  // keeps a pointer to the value to be put inside the can frame.
} eOcanprot_descriptor_t;



typedef eOresult_t (*eOcanprot_fp_parser_t)(eOcanframe_t *frame, eOcanport_t port);

typedef eOresult_t (*eOcanprot_fp_former_t)(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);

typedef struct
{
    eOcanprot_fp_parser_t       parser;
    eOcanprot_fp_former_t       former;
} eOcanprot_functions_t;



/**	@typedef    typedef struct eOcanprot_cfg_t 
 	@brief      Contains the configuration for the EOtheCANprotocol. 
 **/
typedef struct
{
    uint32_t            nothingsofar;
} eOcanprot_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOtheCANprotocol * eo_canprot_Initialise(const eOcanprot_cfg_t *canprotcfg)
    @brief      Initialise the EOtheCANprotocol singleton 
    @arg        canprotcfg       The configuration. NULL is OK.
    @return     The handle to the EOtheCANprotocol
 **/
extern EOtheCANprotocol * eo_canprot_Initialise(const eOcanprot_cfg_t *canprotcfg);


/** @fn         extern EOtheCANprotocol * eo_canprot_GetHandle(void)
    @brief      Retrieve the EOtheCANprotocol singleton 
    @return     The handle to the EOtheCANprotocol
 **/
extern EOtheCANprotocol * eo_canprot_GetHandle(void);


/** @fn         extern eOresult_t eo_canprot_Parse(EOtheCANprotocol *p, eOcanframe_t *frame, eOcanport_t port) 
    @brief      It parses a can frame and executes associated actions. 
    @param      p               The singleton
    @param      frame           The input can frame.
    @param      port            The can port
    @return     eores_OK if the parsing is successful, eores_NOK_nullpointer in case of NULL parameters, eores_NOK_generic if the frame is not recognised.  
 **/
extern eOresult_t eo_canprot_Parse(EOtheCANprotocol *p, eOcanframe_t *frame, eOcanport_t port);


/** @fn         extern eOresult_t eo_canprot_Form(EOtheCANprotocol *p, eOcanprot_descriptor_t *descriptor, eOcanframe_t *outframe)
    @brief      It forms a can frame. 
    @param      p               The singleton
    @param      descriptor      A descriptor for the can frame to be formed. it contains class, cmdid, data, etc.
    @param      frame           The output can frame.
    @return     eores_OK if the parsing is successful, eores_NOK_nullpointer in case of NULL parameters, eores_NOK_generic if the frame is not recognised.  
 **/
extern eOresult_t eo_canprot_Form(EOtheCANprotocol *p, eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame);




/** @}            
    end of group eo_thecanprotocol  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

