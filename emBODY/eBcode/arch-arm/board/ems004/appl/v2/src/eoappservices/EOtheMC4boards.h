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
#ifndef _EOTHEMC4BOARDS_H_
#define _EOTHEMC4BOARDS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMC4boards.h
    @brief      this object implements what is required for managing the MAIS.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheMC4boards Object EOtheMC4boards
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "iCubCanProto_types.h"
#include "EoMeasures.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMC4boards_hid EOtheMC4boards;


typedef float eOmc4boards_conv_encoder_factor_t;  /**< expressed in encoder_ticks/idegree  */
typedef float eOmc4boards_conv_encoder_offset_t;  /**< express in idg  */


typedef struct
{
    icubCanProto_velocityShift_t    velshift;
    icubCanProto_estimShift_t       estimshifts;
} eOmc4boards_shiftvalues_t;   


enum { eoemc4boards_broadcastpolicylistsize = 4 };
typedef struct
{
    uint8_t values[eoemc4boards_broadcastpolicylistsize];
} eOmc4boards_broadcastpolicy_t;       
    



typedef struct
{    
    eOmc4boards_broadcastpolicy_t   broadcastpolicy;  
    eOmc4boards_shiftvalues_t       shiftvalues;       
} eOmc4boards_config_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config_t *cfg);

extern EOtheMC4boards* eo_mc4boards_GetHandle(void);

extern eObool_t eo_mc4boards_AreThere(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_BroadcastStart(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_BroadcastStop(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_Config(EOtheMC4boards *p);


extern eOresult_t eo_mc4boards_Convert_encoderfactor_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_factor_t factor);
extern eOresult_t eo_mc4boards_Convert_encoderoffset_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_offset_t offset);

extern void eo_mc4boards_Convert_maxPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max);
extern void eo_mc4boards_Convert_minPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min);
extern icubCanProto_position_t eo_mc4boards_Convert_maxPos_Get(EOtheMC4boards *p, uint8_t joint);
extern icubCanProto_position_t eo_mc4boards_Convert_minPos_Get(EOtheMC4boards *p, uint8_t joint);

extern eOmeas_position_t       eo_mc4boards_Convert_Position_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t pos);
extern icubCanProto_position_t eo_mc4boards_Convert_Position_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_position_t pos);


extern eOmeas_velocity_t       eo_mc4boards_Convert_Velocity_fromCAN_abs(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel);
extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN_abs(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel);

extern eOmeas_velocity_t       eo_mc4boards_Convert_Velocity_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel);
extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel);


extern eOmeas_acceleration_t       eo_mc4boards_Convert_Acceleration_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc);
extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc);
extern eOmeas_acceleration_t       eo_mc4boards_Convert_Acceleration_fromCAN_abs(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc);
extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN_abs(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc);


extern icubCanProto_stiffness_t eo_mc4boards_Convert_impedanceStiffness_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_stiffness_t stiff);
extern eOmeas_stiffness_t       eo_mc4boards_Convert_impedanceStiffness_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_stiffness_t s_stiff);

extern icubCanProto_damping_t eo_mc4boards_Convert_impedanceDamping_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_damping_t i_damping);
extern eOmeas_damping_t       eo_mc4boards_Convert_impedanceDamping_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_damping_t s_damping);

extern icubCanProto_torque_t eo_mc4boards_Convert_torque_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_torque_t i_torque);
extern eOmeas_torque_t       eo_mc4boards_Convert_torque_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_torque_t s_torque);

extern uint8_t eo_mc4boards_VelocityEstimationShift_Get(EOtheMC4boards *p, uint8_t joint);
extern uint8_t eo_mc4boards_AccelerationEstimationShift_Get(EOtheMC4boards *p, uint8_t joint);

/** @}            
    end of group eo_EOtheMC4boards
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


