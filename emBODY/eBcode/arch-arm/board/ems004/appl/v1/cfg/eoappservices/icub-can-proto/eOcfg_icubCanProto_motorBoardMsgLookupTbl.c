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


/* @file       eOcfg_icubCanProto_motorBoardMsgLookupTbl.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

#include "EOconstLookupTbl.h"
#include "EOconstLookupTbl_hid.h"

#include "eOcfg_icubCanProto_motorBoardMsgFunctions.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_icubCanProto_motorBoardMsgLookupTbl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

/* Since in the fist 64th messages most of them are obsolete, the table start at msg 64 and manage other such as exception*/
#define eo_icubCanProto_pollingMotorBoardMsg_inParserTbl_max          (ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS -64 +1) 
#define eo_icubCanProto_pollingMotorBoardMsg_inFormerTbl_max          (ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG -64 +1) 
#define eo_icubCanProto_pollingMotorBoardMsg_maxNum                    ICUBCANPROTO_POL_MC_CMD_MAXNUM

#define eo_icubCanProto_periodicMotorBoardMsg_inParserTbl_max   13 // until ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS 
#define eo_icubCanProto_periodicMotorBoardMsg_inFormerTbl_max   1  // only  ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
#define eo_icubCanProto_periodicMotorBoardMsg_maxNum            ICUBCANPROTO_PER_MC_MSG_MAXNUM



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_icubCanProto_exceptionMsgpollingMotorBoard_parser(EOicubCanProto *p, icubCanProto_msgCommand_cmdId_t cmdId, eOcanframe_t *frame, eOcanport_t canPort);
static eOresult_t s_eo_icubCanProto_exceptionMsgpollingMotorBoard_former(EOicubCanProto *p, icubCanProto_msgCommand_cmdId_t cmdID, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);


static eOresult_t s_eo_icubCanProto_pollingMotorBoardMsg_parser_excFn(void *arg);
static eOresult_t s_eo_icubCanProto_pollingMotorBoardMsg_former_excFn(void *arg);
static eOresult_t s_eo_icubCanProto_periodicMotorBoardMsg_parser_excFn(void *arg);
static eOresult_t s_eo_icubCanProto_periodicMotorBoardMsg_former_excFn(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

/**************************************************************************************************************/
/*                 POLLING     MOTOR     BOARD     MESSAGES     LOOKUP     TABLES                            */
/**************************************************************************************************************/

/* DECLARATION OF POLLING MOTOR BOARD PARSER BODY TBL */
static const eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t  s_pollingMotorBoardMsg_parserFn_list[eo_icubCanProto_pollingMotorBoardMsg_inParserTbl_max] = 
{
    {   // 64 ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION 			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 65 ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION 			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getMinPosition
    },

    {   // 66 ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 67 ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getMaxPosition
    },

    {   // 68 ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 69 ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getMaxVelocity
    },

    {   // 70 ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION  (obsolete)			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 71 ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION  (obsolete)			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 72 ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 73 ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 74 ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 75 ICUBCANPROTO_POL_MC_CMD__SET_OFFSET_ABS_ENCODER 			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 76 ICUBCANPROTO_POL_MC_CMD__GET_OFFSET_ABS_ENCODER 			
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getOffsetAbsEncoder
    },

    {   // 77 ICUBCANPROTO_POL_MC_CMD__SET_SMOOTH_PID  (obsolete)		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 78 ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 79 ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getTorquePid
    },

    {   // 80 ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 81 ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getTorquePidLimits
    },

    {   // 82  ICUBCANPROTO_POL_MC_CMD__SET_POS_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 83  ICUBCANPROTO_POL_MC_CMD__GET_POS_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getPosPid
    },

    {   // 84  ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 85  ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits
    },

    {   // 86  ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 87  ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 88  ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getImpedanceParams
    },

    {   // 89  ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 90  ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getImpedanceOffset
    },

    {   // 91  ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion
    },
    
    {   // 92  ICUBCANPROTO_POL_MC_CMD__SET_OPTICAL_ENC_RATIO		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },
    
    {   // 93  ICUBCANPROTO_POL_MC_CMD__SET_POS_STICTION_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },
    
    {   // 94  ICUBCANPROTO_POL_MC_CMD__GET_POS_STICTION_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getPosStictionParams
    },
    
    {   // 95  ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },
    
    {   // 96  ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getTorqueStictionParams
    },
    
    {   // 97  ICUBCANPROTO_POL_MC_CMD__SET_BACKEMF_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },
    
    {   // 98  ICUBCANPROTO_POL_MC_CMD__GET_BACKEMF_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getBackemfParams
    },
    
    {   // 99  ICUBCANPROTO_POL_MC_CMD__SET_MODEL_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },
    
    {   // 100  ICUBCANPROTO_POL_MC_CMD__GET_MODEL_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getModelParams
    },
    
    {   // 101  ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 102  ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getCurrentPid
    },

    {   // 103  ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 104  ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PIDLIMITS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getCurrentPidLimits
    },

    {   // 105  IICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 106  IICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getVelocityPid
    },

    {   // 107  IICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 108  IICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PIDLIMITS	
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 109  ICUBCANPROTO_POL_MC_CMD__SET_DISIRED_CURRENT		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 110  ICUBCANPROTO_POL_MC_CMD__GET_DISIRED_CURRENT		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getDesiredCurrent
    },

    {   // 111  ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 112  ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 113  ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS		
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getI2TParams
    },
    
    {   // 114  ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
    },

    {   // 115  ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
        EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__getOpenLoopParams
    }
    
 //   {   // 116  ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE
 //       EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
 //   },
 //   
 //   {   // 117  ICUBCANPROTO_POL_MC_CMD__GET_INTERACTION_MODE
 //       EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
 //   },
 //   
 //   {   // 118  ICUBCANPROTO_POL_MC_CMD__SET_TCFILTER_TYPE
 //       EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_unexpected_cmd
 //   },
 //   {   // 119  ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG
 //       EO_INIT(.parser)    eo_icubCanProto_parser_pol_mb_cmd__setMotorConfig
 //   }     
};        

/* DECLARATION OF POLLING MOTOR BOARD PARSER LOOKUP TBL */
const EOconstLookupTbl icubCanProto_pollingMotorBoardMsg_parser_LUTbl = 
{

    EO_INIT(.capacity)                      eo_icubCanProto_pollingMotorBoardMsg_inParserTbl_max,
    EO_INIT(.offset)                        64,
    EO_INIT(.exceptionMngFn)                s_eo_icubCanProto_pollingMotorBoardMsg_parser_excFn,
    EO_INIT(.itemsList)                     &s_pollingMotorBoardMsg_parserFn_list[0]
};

const EOconstLookupTbl* const icubCanProto_pollingMotorBoardMsg_parser_LUTbl__ptr = &icubCanProto_pollingMotorBoardMsg_parser_LUTbl;



/* DECLARATION OF POLLING MOTOR BOARD FORMER BODY TBL */
static const eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t  s_pollingMotorBoardMsg_formerFn_list[eo_icubCanProto_pollingMotorBoardMsg_inFormerTbl_max] = 
{
    {   // 64 ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION 			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setMinPosition
    },

    {   // 65 ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION 			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getMinPosition
    },

    {   // 66 ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setMaxPosition
    },

    {   // 67 ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getMaxPosition
    },

    {   // 68 ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setMaxVelocity
    },

    {   // 69 ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getMaxVelocity
    },

    {   // 70 ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION  (obsolete)			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },

    {   // 71 ICUBCANPROTO_POL_MC_CMD__GET_ACTIVE_ENCODER_POSITION  (obsolete)			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },

    {   // 72 ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setCurrentLimit
    },

    {   // 73 ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setBcastPolicy
    },

    {   // 74 ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setVelShift
    },

    {   // 75 ICUBCANPROTO_POL_MC_CMD__SET_OFFSET_ABS_ENCODER 			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setOffsetAbsEncoder
    },

    {   // 76 ICUBCANPROTO_POL_MC_CMD__GET_OFFSET_ABS_ENCODER 			
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getOffsetAbsEncoder
    },

    {   // 77 ICUBCANPROTO_POL_MC_CMD__SET_SMOOTH_PID  (obsolete)		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },

    {   // 78 ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setTorquePid
    },

    {   // 79 ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getTorquePid
    },

    {   // 80 ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setTorquePidLimits
    },

    {   // 81 ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits
    },

    {   // 82  ICUBCANPROTO_POL_MC_CMD__SET_POS_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setPosPid
    },

    {   // 83  ICUBCANPROTO_POL_MC_CMD__GET_POS_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getPosPid
    },

    {   // 84  ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setPosPidLimits
    },

    {   // 85  ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS	
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits
    },

    {   // 86  ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setVelTimeout
    },

    {   // 87  ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setImpedanceParams
    },

    {   // 88  ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS	
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams
    },

    {   // 89  ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setImpedanceOffset
    },

    {   // 90  ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET	
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset
    },

    {   // 91  ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getFirmwareVersion
    },
    
    {   // 92  ICUBCANPROTO_POL_MC_CMD__SET_OPTICAL_ENC_RATIO		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },

    {   // 93  ICUBCANPROTO_POL_MC_CMD__SET_POS_STICTION_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setPosStictionParams
    },

    {   // 94  ICUBCANPROTO_POL_MC_CMD__GET_POS_STICTION_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getPosStictionParams
    },

    {   // 95  ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_STICTION_PARAMS		
        EO_INIT(.former)   eo_icubCanProto_former_pol_mb_cmd__setTorqueStictionParams
    },

    {   // 96  ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_STICTION_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getTorqueStictionParams
    },

    {   // 97  ICUBCANPROTO_POL_MC_CMD__SET_BACKEMF_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setBackemfParams
    },

    {   // 98  ICUBCANPROTO_POL_MC_CMD__GET_BACKEMF_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getBackemfParams
    },

    {   // 99  ICUBCANPROTO_POL_MC_CMD__SET_MODEL_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getModelParams
    },

    {   // 100  ICUBCANPROTO_POL_MC_CMD__GET_MODEL_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getModelParams
    },

    {   // 101  ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setCurrentPid
    },

    {   // 102  ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getCurrentPid
    },

    {   // 103  ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setCurrentPidLimits
    },

    {   // 104  ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getCurrentPidLimits
    },

    {   // 105  IICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setVelocityPid
    },

    {   // 106  ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getVelocityPid
    },

    {   // 107  IICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setVelocityPidLimits
    },

    {   // 108  ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PIDLIMITS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getVelocityPidLimits
    },

    {   // 109  ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setDesiredCurrent
    },

    {   // 110  ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getDesiredCurrent
    },

    {   // 111  ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setPeriodicContents
    },

    {   // 112  ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setI2TParams
    },

    {   // 113  ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS		
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getI2TParams
    },

    {   // 114  ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setOpenLoopParams
    },

    {   // 115  ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__getOpenLoopParams
    },

    {   // 116  ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setInteractionMode
    },

    {   // 117  ICUBCANPROTO_POL_MC_CMD__GET_INTERACTION_MODE
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },
    
    {   // 118  ICUBCANPROTO_POL_MC_CMD__SET_TCFILTER_TYPE
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_unexpected_cmd
    },
    
    {   // 119  ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG
        EO_INIT(.former)    eo_icubCanProto_former_pol_mb_cmd__setMotorConfig
    }    
};        




/* DECLARATION OF POLLING MOTOR BOARD FORMER LOOKUP TBL */
const EOconstLookupTbl icubCanProto_pollingMotorBoardMsg_former_LUTbl = 
{

    EO_INIT(.capacity)                      eo_icubCanProto_pollingMotorBoardMsg_inFormerTbl_max,
    EO_INIT(.offset)                        64,
    EO_INIT(.exceptionMngFn)                s_eo_icubCanProto_pollingMotorBoardMsg_former_excFn,
    EO_INIT(.itemsList)                     &s_pollingMotorBoardMsg_formerFn_list[0]
};

const EOconstLookupTbl* const icubCanProto_pollingMotorBoardMsg_former_LUTbl__ptr = &icubCanProto_pollingMotorBoardMsg_former_LUTbl;










/**************************************************************************************************************/
/*                 PERIODIC     MOTOR     BOARD     MESSAGES     LOOKUP     TABLE                             */
/**************************************************************************************************************/

/* DECLARATION OF PERIODIC MOTOR BOARD PARSER BODY TBL */
static const eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t  s_periodicMotorBoardMsg_parserFn_list[eo_icubCanProto_periodicMotorBoardMsg_inParserTbl_max] = 
{
    {   // 0 ICUBCANPROTO_PER_MC_MSG__2FOC
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__2foc
    },

    {   // 1 ICUBCANPROTO_PER_MC_MSG__POSITION
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__position
    },

    {   // 2 ICUBCANPROTO_PER_MC_MSG__PID_VAL
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__pidVal
    },

    {   // 3 ICUBCANPROTO_PER_MC_MSG__STATUS
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__status
    },

    {   // 4 ICUBCANPROTO_PER_MC_MSG__CURRENT
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__current
    },

    {   // 5 ICUBCANPROTO_PER_MC_MSG__OVERFLOW
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__overflow
    },

    {   // 6 IICUBCANPROTO_PER_MC_MSG__PRINT
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__print 
    },

    {   // 7 ICUBCANPROTO_PER_MC_MSG__VELOCITY
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__velocity
    },

    {   // 8 ICUBCANPROTO_PER_MC_MSG__PID_ERROR
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__pidError
    },

    {   // 9 ICUBCANPROTO_PER_MC_MSG__DEBUG
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__debug
    },
    
    {   // 10 ICUBCANPROTO_PER_MC_MSG__MOTOR_POSITION
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__motorPosition
    },

    {   // 11 ICUBCANPROTO_PER_MC_MSG__MOTOR_SPEED
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__motorSpeed
    },
    
    {   // 12 ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS
        EO_INIT(.parser)    eo_icubCanProto_parser_per_mb_cmd__additionalStatus
    }
    
};        




/* DECLARATION OF PERIODIC MOTOR BOARD LOOKUP TBL */
extern const EOconstLookupTbl icubCanProto_periodicMotorBoardMsg_parser_LUTbl = 
{
    EO_INIT(.capacity)                      eo_icubCanProto_periodicMotorBoardMsg_inParserTbl_max,
    EO_INIT(.offset)                        0,
    EO_INIT(.exceptionMngFn)                s_eo_icubCanProto_periodicMotorBoardMsg_parser_excFn,                                        
    EO_INIT(.itemsList)                     &s_periodicMotorBoardMsg_parserFn_list[0]
};

extern const EOconstLookupTbl* const icubCanProto_periodicMotorBoardMsg_parser_LUTbl__ptr;





/* DECLARATION OF PERIODIC MOTOR BOARD FORMER BODY TBL */
static const eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t  s_periodicMotorBoardMsg_formerFn_list[eo_icubCanProto_periodicMotorBoardMsg_inFormerTbl_max] = 
{
    {   // 15 ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT 			
        EO_INIT(.former)    eo_icubCanProto_former_per_mb_cmd__emsto2foc_desiredcurrent
    }    
};        




/* DECLARATION OF PERIODIC MOTOR BOARD LOOKUP TBL */
extern const EOconstLookupTbl icubCanProto_periodicMotorBoardMsg_former_LUTbl = 
{
    EO_INIT(.capacity)                      eo_icubCanProto_periodicMotorBoardMsg_inFormerTbl_max,
    EO_INIT(.offset)                        15,
    EO_INIT(.exceptionMngFn)                s_eo_icubCanProto_periodicMotorBoardMsg_former_excFn,                                        
    EO_INIT(.itemsList)                     &s_periodicMotorBoardMsg_formerFn_list[0]
};

extern const EOconstLookupTbl* const icubCanProto_periodicMotorBoardMsg_former_LUTbl__ptr;






// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_icubCanProto_exceptionMsgpollingMotorBoard_parser(EOicubCanProto *p, icubCanProto_msgCommand_cmdId_t cmdId, 
                                                                         eOcanframe_t *frame, eOcanport_t canPort)
{                                                                      
    eOresult_t res;

    switch(cmdId)
    {
        case ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE:  /* 0 */
        {
            res = eo_icubCanProto_parser_test(p, frame, canPort);
        } break;


        case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:  /* 7 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getControlMode(p, frame, canPort);
        } break;
    
        case ICUBCANPROTO_POL_MC_CMD__MOTION_DONE:  /* 8 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__motionDone(p, frame, canPort);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO:  /* 12 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getAdditionalInfo(p, frame, canPort);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM:  /* 19 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getDebugParam(p, frame, canPort);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION:  /* 20 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getEncoderPosition(p, frame, canPort);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE:  /* 45*/
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(p, frame, canPort);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID:  /* 51 */
        {
            res = eo_icubCanProto_parser_pol_mb_cmd__getBoardId(p, frame, canPort);
        } break;
        
        default:
        {
            res = eo_icubCanProto_parser_pol_mb_unexpected_cmd(p, frame, canPort);
        }
    };


    return(res);        
}





static eOresult_t s_eo_icubCanProto_exceptionMsgpollingMotorBoard_former(EOicubCanProto *p, icubCanProto_msgCommand_cmdId_t cmdId, 
                                                                            void *nv_ptr,
                                                                            eOicubCanProto_msgDestination_t dest,
                                                                            eOcanframe_t *canFrame)
{
    eOresult_t res;

    switch(cmdId)
    {
        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN:  /* 1 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__controllerRun(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE:  /* 2 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__controllerIdle(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER:  /* 4 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__calibrateEncoder(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD:  /* 5 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__enablePwmPad(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD:  /* 6 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__disablePwmPad(p, nv_ptr, dest, canFrame);
        } break;


        case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:  /* 7 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getControlMode(p, nv_ptr, dest, canFrame);
        } break;
    
        case ICUBCANPROTO_POL_MC_CMD__MOTION_DONE:  /* 8 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__motionDone(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE:  /* 9 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setControlMode(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO:  /* 12 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getAdditionalInfo(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__SET_ADDITIONAL_INFO:  /* 13 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setAdditionalInfo(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT:  /* 14 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setSpeedEtimShift(p, nv_ptr, dest, canFrame);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_DEBUG_PARAM:  /* 18 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setDebugParam(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM:  /* 19 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getDebugParam(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION:  /* 20 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getEncoderPosition(p, nv_ptr, dest, canFrame);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE	:  /* 27 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__positionMove(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE:  /* 28 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__velocityMove(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION:  /* 29 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setEncoderPosition(p, nv_ptr, dest, canFrame);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE:  /* 44*/
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setDesiredTorque(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE:  /* 45*/
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getDesiredTorque(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY:  /* 46*/
        {
            res = eo_icubCanProto_former_pol_mb_cmd__stopTrajectory(p, nv_ptr, dest, canFrame);
        }break;

        case ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID:  /* 51 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__getBoardId(p, nv_ptr, dest, canFrame);
        } break;

        case ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION:  /* 62 */
        {
            res = eo_icubCanProto_former_pol_mb_cmd__setCmdPos(p, nv_ptr, dest, canFrame);
        } break;

        default:
        {
            res = eo_icubCanProto_former_pol_mb_unexpected_cmd(p, nv_ptr, dest, canFrame);
        }
    };


    return(res);        
}

static eOresult_t s_eo_icubCanProto_pollingMotorBoardMsg_parser_excFn(void *arg)
{  
    eo_icubCanProto_hid_exceptionFn_argument_t *exFn_arg = (eo_icubCanProto_hid_exceptionFn_argument_t *)arg;
    
    return(s_eo_icubCanProto_exceptionMsgpollingMotorBoard_parser(exFn_arg->iproto_ptr,exFn_arg->u.p_arg.cmdId, 
                                                           exFn_arg->u.p_arg.frame, exFn_arg->u.p_arg.canPort));
}
static eOresult_t s_eo_icubCanProto_pollingMotorBoardMsg_former_excFn(void *arg)
{
    eo_icubCanProto_hid_exceptionFn_argument_t *exFn_arg = (eo_icubCanProto_hid_exceptionFn_argument_t *)arg;

    return(s_eo_icubCanProto_exceptionMsgpollingMotorBoard_former(exFn_arg->iproto_ptr, exFn_arg->u.f_arg.cmdId,
                                                                  exFn_arg->u.f_arg.value, exFn_arg->u.f_arg.dest,
                                                                  exFn_arg->u.f_arg.frame));
}

static eOresult_t s_eo_icubCanProto_periodicMotorBoardMsg_parser_excFn(void *arg)
{
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_periodicMotorBoardMsg_former_excFn(void *arg)
{
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



