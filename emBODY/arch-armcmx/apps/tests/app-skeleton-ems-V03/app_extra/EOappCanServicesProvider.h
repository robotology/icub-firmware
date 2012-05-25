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
#ifndef _EOAPPCANSERVICESPROVIDER_H_
#define _EOAPPCANSERVICESPROVIDER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOappCanServicesProvider.h
    @brief      This file provides CAN services to ems application.
                These services pertain to communication with icub-can-board by CAN bus.
                Therefore, these object is tied to icub-can-protocol and icub-world in general
    @author     valentina.gaggero@iit.it
    @date       02/14/2012
**/

/** @defgroup eo_app_canServicesProvider Object eOappCanSP
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoMotionControl.h"
#include "EoBoards.h"
#include "EOfifoWord.h"
#include "EOicubCanProto_specifications.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct EOappCanSP_hid   EOappCanSP;

//this declaretion is equal to eo_emsCanNetTopo_jointOrMotorCanLocation_t.
typedef struct
{
    eOcanport_t         emscanport;	
    uint8_t             canaddr;        /**< CAN address of board liked with joint/motor.
                                             if this struct is used with icubCanProtocol canaddr's values must be complient with eo_icubCanProto_canBoardAddress_t*/
    uint8_t             axis;	        /**< number of joint/ motor, said axis generally.
                                             if this struct is used with icubCanProtocol axis's values must be complient with eo_icubCanProto_motorAxis_t*/
} eo_appCanSP_canLocation;


typedef struct
{
    uint32_t dummy;
} eOappCanSP_cfg_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg)
    @brief      Create a new application CAN Services Provider.
                Creating this obj entails in creations of other obj used by EOappCanSP to provides CAN services 
    @param      cfg        configuration for the obj. (currently is not used)
    @return     NULL in case of errors (for example CAN peripheral's error) or pointer to EOappCanSP on successfully 
 **/
extern EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg);



/** @fn         eOresult_t eo_appCanSP_GetConnectedJoints(EOappCanSP *p, EOfifoWord *connectedJointsList)
    @brief      fill @e connectedJointsList with joint id connected to ems
    @param      p                       target obj
    @param      connectedJointsList     in output contains a list of joint id connected to ems 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
extern eOresult_t eo_appCanSP_GetConnectedJoints(EOappCanSP *p, EOfifoWord *connectedJointsList); 


/** @fn         eOresult_t eo_appCanSP_GetConnectedMotors(EOappCanSP *p, EOfifoWord *connectedMotorsList)
    @brief      fill @e connectedMotorsList with motor id connected to ems
    @param      p                       target obj
    @param      connectedMotorsList     in output contains a list of motor id connected to ems 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
extern eOresult_t eo_appCanSP_GetConnectedMotors(EOappCanSP *p, EOfifoWord *connectedMotorsList);



/** @fn         eOresult_t eo_appCanSP_GetConnectedSensors(EOappCanSP *p, EOfifoWord *connectedSensorsList)
    @brief      fill @e connectedSensorsList with Sensor id connected to ems
    @param      p                       target obj
    @param      connectedJSensorsList     in output contains a list of Sensor id connected to ems 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
extern eOresult_t eo_appCanSP_GetConnectedSensors(EOappCanSP *p, EOfifoWord *connectedSensorsList);



/** @fn         extern eOresult_t eo_appCanSP_SendCmd2Joint(EOappCanSP *p, eOmc_jointId_t jId, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr)
    @brief      send a command to a joint
    @param      p               target obj
    @param      jid             joint destination of command
    @param      msgCmd          command to send
    @param      val_ptr         command's params. it can be null, if command doesn't need param.
    @return     eores_OK on success
                eores_NOK_nullpointer if p is null, or eores_NOK_nodata the jid is not connected with this ems board,
                or eores_NOK_generic something else error case. //TODO: check better error!!!
 **/
extern eOresult_t eo_appCanSP_SendCmd2Joint(EOappCanSP *p, eOmc_jointId_t jId, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr);


/** @fn         extern eOresult_t eo_appCanSP_SendCmd2Joint(EOappCanSP *p, eOmc_jointId_t jId, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr)
    @brief      return joint's can location ad the board type where it is connected to. 
    @param      p               target obj
    @param      jid             the joint
    @param      canLoc          in output @e canLoc contains info about joint's can Location
    @param      boardType       if not null, in output contains board type where the joint is connected to
    @return     eores_OK on success
                eores_NOK_nullpointer if p is null, or eores_NOK_nodata the jid is not connected with this ems board,
                or eores_NOK_generic something else error case. //TODO: check better error!!!
 **/
extern eOresult_t eo_appCanSP_GetJointCanLocation(EOappCanSP *p, eOmc_jointId_t jId, eo_appCanSP_canLocation *canLoc, eObrd_types_t *boardType);

/** @fn         extern eOresult_t eo_appCanSP_SendCmd2Motor(EOappCanSP *p, eOmc_motorId_t mId, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr)
    @brief      send a command to a motor
    @param      p               target obj
    @param      mid             motor destination of command
    @param      msgCmd          command to send
    @param      val_ptr         command's params. it can be null, if command doesn't need param.
    @return     eores_OK on success
                eores_NOK_nullpointer if p is null, or eores_NOK_nodata the mid is not connected with this ems board,
                or eores_NOK_generic something else error case. //TODO: check better error!!!
 **/
extern eOresult_t eo_appCanSP_SendCmd2Motor(EOappCanSP *p, eOmc_motorId_t mId, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr);


/** @fn         extern eOresult_t eo_appCanSP_GetMotorCanLocation(EOappCanSP *p, eOmc_motorId_t mId, eo_appCanSP_canLocation *canLoc, eObrd_types_t *boardType)
    @brief      return motor's can location ad the board type where it is connected to. 
    @param      p               target obj
    @param      jid             the motor
    @param      canLoc          in output @e canLoc contains info about joint's can Location
    @param      boardType       if not null, in output contains board type where the motor is connected to
    @return     eores_OK on success
                eores_NOK_nullpointer if p is null, or eores_NOK_nodata the mid is not connected with this ems board,
                or eores_NOK_generic something else error case. //TODO: check better error!!!
 **/
extern eOresult_t eo_appCanSP_GetMotorCanLocation(EOappCanSP *p, eOmc_motorId_t mId, eo_appCanSP_canLocation *canLoc, eObrd_types_t *boardType);



/** @fn         extern eOresult_t eo_appCanSP_SendCmd(EOappCanSP *p, eo_appCanSP_canLocation *canLocation, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr)
    @brief      send a command to a can location 
    @param      p               target obj
    @param      canLocation     destination of command
    @param      msgCmd          command to send
    @param      val_ptr         command's params. it can be null, if command doesn't need param.
    @return     eores_OK on success
                eores_NOK_nullpointer if p is null, or eores_NOK_nodata the mid is not connected with this ems board,
                or eores_NOK_generic something else error case. //TODO: check better error!!!
 **/
extern eOresult_t eo_appCanSP_SendCmd(EOappCanSP *p, eo_appCanSP_canLocation *canLocation, eo_icubCanProto_msgCommand_t msgCmd, void *val_ptr);



extern eOresult_t eo_appCanSP_read(EOappCanSP *p); //test porpouse
extern eOresult_t eo_appCanSP_SendMessage_TEST(EOappCanSP *p, eo_appCanSP_canLocation *canLocation, uint8_t *payload_ptr);

//following functions are obsolete!!!

/** @fn         extern eOresult_t eo_appCanSP_ConfigJoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_joint_config_t *cfg)
    @brief      sends can messages to can board to configure one of its joint 
    @param      p                       target obj
    @param      jId               identifies the joint to config.
    @param      cfg                     joint's configuration 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
//extern eOresult_t eo_appCanSP_ConfigJoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_joint_config_t *cfg);
extern eOresult_t eo_appCanSP_jointWholeConfig_Send(EOappCanSP *p, eOmc_jointId_t jId, eOmc_joint_config_t *cfg);


/** @fn         extern eOresult_t eo_appCanSP_ConfigJoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_joint_config_t *cfg)
    @brief      sends can messages to can board to configure one of its motor 
    @param      p                       target obj
    @param      jId               identifies the motor to config.
    @param      cfg                     motor's configuration 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
extern eOresult_t eo_appCanSP_ConfigMotor(EOappCanSP *p, eOmc_motorId_t mId, eOmc_motor_config_t *cfg);


/** @fn         extern eOresult_t eo_appCanSP_SendSetPoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_setpoint_t *setPoint)
    @brief      sends set point to joint. (this joint is managed by a CAN board)
    @param      p                       target obj
    @param      jId               identifies the joint.
    @param      cfg                     motor's configuration 
    @return     eores_OK or eores_NOK_nullpointer if p or connectedJointsList are null, or eores_NOK_nodata if no joint are connected to ems
 **/
extern eOresult_t eo_appCanSP_SendSetPoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_setpoint_t *setPoint);







/** @}            
    end of group eo_app_canServicesProvider
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


