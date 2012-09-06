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
#ifndef _EOAPPMEASURESCONVERTER_H_
#define _EOAPPMEASURESCONVERTER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappMeasuresConverter.h
    @brief      This file provides interface to converter obj.
                this obj convertes position, veleocity, acceleration, torque, stiffness, etc. 
                expressed in encoder/sensor ticks in "icub measure unit". see file "EoMeasures.h"
    @author     valentina.gaggero@iit.it
    @date       08/24/2012
**/


/** @defgroup   eo_app_measuresConverter Object EOappMeasConv
    NOTE: corrent doesn't need conversions because icub-can-proto and embobj-proto use the same mesurament unit  
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "eOcommon.h"
#include "EoMotionControl.h"
#include "EoMeasures.h"
#include "EOicubCanProto_specifications.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOappMeasConv_hid  EOappMeasConv;


typedef struct
{
    uint8_t     jointvelocityShift;
    uint8_t     motorVelocityEstimationShift;
    uint8_t     motorAccelerationEstimationShift;
    uint8_t     jointVelocityEstimationShift;
    uint8_t     jointAccelerationEstimationShift;
} eOappMeasConv_cfg_t;


typedef float eOappMeasConv_propRatio_t;    /**< encoder ticks/idegree  */
typedef float eOappMeasConv_encoderZero_t;  /**< express in idg  */


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
/** @fn         extern EOappMeasConv* eo_appEncReader_New(eOappMeasConv_cfg_t *cfg)
    @brief      Create a measures converter.
    @param      cfg        configuration for the obj. (currently is not used)
    @return     NULL in case of errors or pointer to EOappMeasConv on successfully 
 **/
extern EOappMeasConv* eo_appMeasConv_New(eOappMeasConv_cfg_t *cfg);


/** @fn         extern eOresult_t eo_appMeasConv_SetJntEncoderPropRatio(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_propRatio_t propRatio)
    @brief      Set EncoderPropRatio of joint with @e jId
    @param      p               referece to converter
    @param      jId             identifier of joint
    @param      propRatio       proportional ration of joint jId
    @return     - eores_NOK_nullpointer if p is NULL
                - eores_NOK_nodata if joint identified by @e jId is not managed by the given ems
                - eores_OK on success
 **/
extern eOresult_t eo_appMeasConv_SetJntEncoderPropRatio(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_propRatio_t propRatio);


/** @fn         extern eOresult_t eo_appMeasConv_SetJntEncoderZero(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_encoderZero_t zero)
    @brief      Set EncoderZero of encoder of joint with @e jId
    @param      p               referece to converter
    @param      jId             identifier of joint
    @param      propRatio       proportional ration of joint jId
    @return     - eores_NOK_nullpointer if p is NULL
                - eores_NOK_nodata if joint identified by @e jId is not managed by the given ems
                - eores_OK on success
 **/
extern eOresult_t eo_appMeasConv_SetJntEncoderZero(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_encoderZero_t zero);

/** @fn         extern eOresult_t eo_appMeasConv_position_E2I(EOappMeasConv *p)
    @brief      convert position measure from encoder ticks to icub degree (idg)
    @param      p        referece to converter
    @param      jId      joint identifier
    @return     - eores_NOK_nullpointer if p is NULL
                - eores_NOK_nodata if joint identified by @e jId is not managed by the given ems
                - eores_OK on success
 **/
extern eOresult_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos, eOmeas_position_t *i_pos_ptr);


/** @fn         extern eOresult_t eo_appMeasConv_position_E2I(EOappMeasConv *p)
    @brief      convert position measure from icub degree (idg) to encoder ticks 
    @param      p        referece to converter
    @param      jId      joint identifier
    @return     - eores_NOK_nullpointer if p is NULL
                - eores_NOK_nodata if joint identified by @e jId is not managed by the given ems
                - eores_OK on success
 **/
extern eOresult_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t *e_pos_ptr, eOmeas_position_t i_pos);


extern eOresult_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel, eOmeas_velocity_t *i_vel_ptr);
extern eOresult_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t *e_vel_ptr, eOmeas_velocity_t i_vel);

extern eOresult_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc, eOmeas_velocity_t *i_acc_ptr);
extern eOresult_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t *e_acc_ptr, eOmeas_velocity_t i_acc);


extern eOresult_t eo_appMeasConv_impedenceStiffness_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_stiffness_t *s_stiff, eOmeas_stiffness_t i_stiff);
extern eOresult_t eo_appMeasConv_impedenceStiffness_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_stiffness_t s_stiff, eOmeas_stiffness_t *i_stiff);

extern eOresult_t eo_appMeasConv_impedenceDamping_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_damping_t *s_damping, eOmeas_damping_t i_damping);
extern eOresult_t eo_appMeasConv_impedenceDamping_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_damping_t s_damping, eOmeas_damping_t *i_damping);

extern eOresult_t eo_appMeasConv_torque_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_torque_t *s_torque, eOmeas_torque_t i_torque);
extern eOresult_t eo_appMeasConv_torque_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_torque_t s_torque, eOmeas_torque_t *i_torque);

/** @}            
    end of group eo_app_measuresConverter
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


