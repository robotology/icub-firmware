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
#ifndef _EOCFG_ICUBCNAPROTO_MOTORBOARDMSGFUNCTION_H_
#define _EOCFG_ICUBCNAPROTO_MOTORBOARDMSGFUNCTION_H_




/** @file       eOcfg_icubCanProto_motorBoardMsgFunctions.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/

/** @defgroup eo_icubCanProto Configuation of the messages lookup tables.
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "EOicubCanProto.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

//********************** P A R S E R       POLLING     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_pol_mb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_test(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getControlMode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__motionDone(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDebugParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBoardId(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMinPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosStictionParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorqueStictionParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBackemfParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getModelParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getI2TParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOpenLoopParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);



//********************** F O R M E R       POLLING      F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_former_pol_mb_unexpected_cmd(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_test(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_dummy_cmd(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerRun(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerIdle(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__toggleVerbose(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__calibrateEncoder(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__enablePwmPad(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__disablePwmPad(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getControlMode(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__motionDone(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setControlMode(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setAdditionalInfo(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setSpeedEtimShift(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDebugParam(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDebugParam(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__positionMove(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__velocityMove(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setEncoderPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredTorque(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__stopTrajectory(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBoardId(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMinPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMinPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxVelocity(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentLimit(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBcastPolicy(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelShift(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOffsetAbsEncoder(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelTimeout(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceOffset(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorqueStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorqueStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBackemfParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBackemfParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setModelParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getModelParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredCurrent(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPeriodicContents(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setI2TParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getI2TParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCmdPos(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setInteractionMode(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOpenLoopParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOpenLoopParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMotorConfig(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);

//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__2foc(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__status(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__overflow(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__print(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__debug(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__motorPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__motorSpeed(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__additionalStatus(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
//********************** F O R M E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_former_per_mb_cmd__emsto2foc_desiredcurrent(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);



/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




