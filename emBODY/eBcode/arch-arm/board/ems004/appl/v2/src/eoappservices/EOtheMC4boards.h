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

#ifdef __cplusplus
extern "C" {
#endif

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
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMC4boards_hid EOtheMC4boards;


typedef float eOmc4boards_conv_encoder_factor_t;  /**< expressed in encoder_ticks/idegree  */
typedef float eOmc4boards_conv_encoder_offset_t;  /**< expressed in idegree  */
typedef float eOmc4boards_conv_torque_factor_t;  /**< expressed in torque_ticks/Nm  */


typedef struct
{
    eOmc_mc4shifts_t        shifts;
    uint16_t                broadcastflags; // use an | combination of (1<<x) where x is a value from eOmc_mc4broadcast_t    
} eOmc4boards_config2_t;


typedef enum
{
    eomc4_velocitycontext_toTICKS                   = 0,    // just conversion to ticks/sec
    eomc4_velocitycontext_toCAN_signed              = 1,    // to can with signed velocity expressed in ticks/ms but also scaled by velocity-shift
    eomc4_velocitycontext_toCAN_unsigned            = 2,    // to can with unsigned velocity expressed in ticks/ms but also scaled by velocity-shift
    eomc4_velocitycontext_toCAN_positionsetpoint    = 3     // to can specially prepared for pos setpoint (unsigned velocity expressed in 100*ticks/ms) 
} eOmc4boards_velocitycontext_t;
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config2_t *cfg2);

extern eOresult_t eo_mc4boards_LoadShifts(EOtheMC4boards *p, eOmc_mc4shifts_t shifts);

extern eOresult_t eo_mc4boards_LoadBroadcastFlags(EOtheMC4boards *p, uint16_t flags);

extern EOtheMC4boards* eo_mc4boards_GetHandle(void);

extern eObool_t eo_mc4boards_AreThere(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_BroadcastStart(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_BroadcastStop(EOtheMC4boards *p);

extern eOresult_t eo_mc4boards_Config(EOtheMC4boards *p);

typedef enum 
{   // put in here the types of motioncontrol can frames

    eo_mc4_canframe_unknown = 255    
} eOmc4boards_canframe_t;

extern eOresult_t eo_mc4boards_AcceptCANframe(EOtheMC4boards *p, eOcanframe_t *frame, eOcanport_t port, eOmc4boards_canframe_t cftype);



extern eOresult_t eo_mc4boards_Convert_encoderfactor_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_factor_t factor);
extern eOresult_t eo_mc4boards_Convert_encoderoffset_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_offset_t offset);


extern void eo_mc4boards_Convert_maxJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max);
extern void eo_mc4boards_Convert_minJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min);
extern icubCanProto_position_t eo_mc4boards_Convert_maxJointPos_Get(EOtheMC4boards *p, uint8_t joint);
extern icubCanProto_position_t eo_mc4boards_Convert_minJointPos_Get(EOtheMC4boards *p, uint8_t joint);
extern void eo_mc4boards_Convert_maxMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max);
extern void eo_mc4boards_Convert_minMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min);
extern icubCanProto_position_t eo_mc4boards_Convert_maxMotorPos_Get(EOtheMC4boards *p, uint8_t joint);
extern icubCanProto_position_t eo_mc4boards_Convert_minMotorPos_Get(EOtheMC4boards *p, uint8_t joint);

extern eOmeas_position_t eo_mc4boards_Convert_Position_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t pos);
extern icubCanProto_position_t eo_mc4boards_Convert_Position_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_position_t pos);


extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel, eOmc4boards_velocitycontext_t context);
extern eOmeas_velocity_t eo_mc4boards_Convert_Velocity_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel);

extern eOmeas_acceleration_t eo_mc4boards_Convert_Acceleration_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc);
extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc);

extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN_abs__NEW(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc);


extern icubCanProto_stiffness_t eo_mc4boards_Convert_impedanceStiffness_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_stiffness_t stiff);
extern eOmeas_stiffness_t eo_mc4boards_Convert_impedanceStiffness_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_stiffness_t s_stiff);

extern icubCanProto_damping_t eo_mc4boards_Convert_impedanceDamping_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_damping_t i_damping);
extern eOmeas_damping_t eo_mc4boards_Convert_impedanceDamping_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_damping_t s_damping);

extern icubCanProto_torque_t eo_mc4boards_Convert_torque_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_torque_t i_torque);
extern eOmeas_torque_t eo_mc4boards_Convert_torque_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_torque_t s_torque);



/** @}            
    end of group eo_EOtheMC4boards
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


