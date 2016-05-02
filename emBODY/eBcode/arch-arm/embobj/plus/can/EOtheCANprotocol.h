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
#include "EOtheCANmapping.h"


// - public #define  --------------------------------------------------------------------------------------------------

// we mange teh following: 
// - class: it is eOcanprot_msgclass_t
// - source: it is the can address which sent the frame [0, 14]
// - destination: it is the can address where to send the frame [0, 14]
// - type: it is either the MSG or the CMD whcih describes the message (ICUBCANPROTO_PER_MC_MSG__POSITION, ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE, etc.).
// - idcan: it is the ID of the CAN frame
// - internal index: it is either 0 or 1 and is used in mc to specify a joint/motor

// retrieves the class of a frame
#define EOCANPROT_FRAME_GET_CLASS(frame)                        ( ((frame)->id & 0x700) >> 8 )

// retrieves the source address of a frame
#define EOCANPROT_FRAME_GET_SOURCE(frame)                       ( ((frame)->id & 0x0F0) >> 4 )

// retrieves the destination address of a frame (for polling messages only)
#define EOCANPROT_FRAME_POLLING_GET_DESTINATION(frame)          ( ((frame)->id & 0x00F) )

// retrievs the internal index (0 or 1) of a frame (for polling motion control frames only)
#define EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame)     ( ((frame)->data[0] & 0x80) >> 7)

// retrieves the message type of a frame (for the case of polling messages)
#define EOCANPROT_FRAME_POLLING_GET_TYPE(frame)                 ( ((frame)->data[0] & 0x7F) )

// retrieves the message type of a frame (for the case of periodic messages)
#define EOCANPROT_FRAME_PERIODIC_GET_TYPE(frame)                ( ((frame)->id & 0x00F) )

//#define EOCANPROT_MC_POL_CREATE_ID(orig, dest)  ( (ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL << 8) | (((orig)&0xf) << 4) | ((dest)&0xf) )  

// creates the CAN ID given the class, origin and destination addresses
#define EOCANPROT_CREATE_CANID(clss, orig, dest)                ( (((clss)&0xF) << 8) | (((orig)&0xF) << 4) | ((dest)&0xF) )  

// creates the CAN ID for periodic messages
#define EOCANPROT_CREATE_CANID_PERIODIC(clss, orig, type)       ( (((clss)&0xF) << 8) | (((orig)&0xF) << 4) | ((type)&0xF) )  

// creates the DATA[0] byte for MC polling messages given the internalindex and the message type
#define EOCANPROT_CREATE_POLLING_MC_DATA0(intindex, type)       ( (((intindex)&0x1) << 7) | ((type)&0x7F) )

// creates the DATA[0] byte for AS polling messages given the message type
#define EOCANPROT_CREATE_POLLING_AS_DATA0(type)                 ( ((type)&0x7F) )

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOtheCANprotocol_hid EOtheCANprotocol
    @brief      EOtheCANprotocol is an opaque struct.
 **/  
typedef struct EOtheCANprotocol_hid EOtheCANprotocol;

// eocanprot_msgclass_pollingSkin is a fake class, as it is used eocanprot_msgclass_pollingAnalogSensor
typedef enum
{
    eocanprot_msgclass_pollingMotorControl          = ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL,          // 0
    eocanprot_msgclass_periodicMotorControl         = ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL,         // 1
    eocanprot_msgclass_pollingAnalogSensor          = ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR,          // 2
    eocanprot_msgclass_periodicAnalogSensor         = ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR,         // 3
    eocanprot_msgclass_pollingSkin                  = ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR,          // 2
    eocanprot_msgclass_periodicSkin                 = ICUBCANPROTO_CLASS_PERIODIC_SKIN,                 // 4
    eocanprot_msgclass_periodicInertialSensor       = ICUBCANPROTO_CLASS_PERIODIC_INERTIALSENSOR        // 5
} eOcanprot_msgclass_t;

enum { eocanprot_msgclass_maxvalue = 5 };

enum { eocanprot_classperiodic_msgtypes_maxnumberof = 16, eocanprot_classpolling_msgtypes_maxnumberof = 128 };


/** @typedef    typedef struct eOcanprot_command_t
    @brief      Contains whatever is required to define a command over can.
 **/ 
typedef struct
{
    uint8_t     class;      /**< use eOcanprot_msgclass_t */
    uint8_t     type;       /**< they are: ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, etc. */
    void*       value;      /**< keeps a pointer to the value to be put inside the can frame */   
} eOcanprot_command_t;


/** @typedef    typedef struct eOcanprot_descriptor_t
    @brief      Contains whatever is required to form the can frame: the command and the location
 **/ 
typedef struct
{
    eOcanprot_command_t     cmd;        /**< the command */
    eObrd_canlocation_t     loc;        /**< the destination if polling message, the source if periodic */
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

