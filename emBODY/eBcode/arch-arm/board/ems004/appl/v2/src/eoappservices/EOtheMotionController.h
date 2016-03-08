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
#ifndef _EOTHEMOTIONCONTROLLER_H_
#define _EOTHEMOTIONCONTROLLER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMotionController.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheMotionController Object EOtheMotionController
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"

#include "EOtheServices.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMotionController_hid EOtheMotionController;



//typedef eOresult_t (*eOmotcon_onendofoperation_fun_t) (EOtheMotionController* p, eObool_t operationisok);

enum { eo_motcon_standardJOMOs = 4, eo_motcon_maxJOMOs = 12 };


typedef enum
{
    eo_motcon_mode_NONE         = eomn_serv_NONE,
    eo_motcon_mode_foc          = eomn_serv_MC_foc,
    eo_motcon_mode_mc4          = eomn_serv_MC_mc4,
    eo_motcon_mode_mc4plus      = eomn_serv_MC_mc4plus,
    eo_motcon_mode_mc4plusmais  = eomn_serv_MC_mc4plusmais   
} eOmotioncontroller_mode_t;
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheMotionController* eo_motioncontrol_Initialise(void);

extern EOtheMotionController* eo_motioncontrol_GetHandle(void);

// we can call them if _Initialise() was called
extern eOmn_serv_state_t eo_motioncontrol_GetServiceState(EOtheMotionController *p);
extern eOresult_t eo_motioncontrol_SendReport(EOtheMotionController *p);
extern eOmotioncontroller_mode_t eo_motioncontrol_GetMode(EOtheMotionController *p);



// so far we support only eo_motcon_mode_foc and eo_motcon_mode_mc4

extern eOresult_t eo_motioncontrol_Verify(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify);

extern eOresult_t eo_motioncontrol_Activate(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg);

extern eOresult_t eo_motioncontrol_Deactivate(EOtheMotionController *p);


// to be called once before to be ticked
extern eOresult_t eo_motioncontrol_Start(EOtheMotionController *p);

extern eOresult_t eo_motioncontrol_SetRegulars(EOtheMotionController *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);

// it ticks every ms of the control-loop
extern eOresult_t eo_motioncontrol_Tick(EOtheMotionController *p);

extern eOresult_t eo_motioncontrol_Stop(EOtheMotionController *p);


// others, needed by mc4plus original code by davide.pollarolo. see if they can be moved elsewhere or if they are really needed
extern eOresult_t eo_motioncontrol_extra_MotorEnable(EOtheMotionController *p, uint8_t jomo);

extern eOresult_t eo_motioncontrol_extra_FaultDetectionEnable(EOtheMotionController *p);

extern eObool_t eo_motioncontrol_extra_AreMotorsExtFaulted(EOtheMotionController *p);

//extern eOresult_t eo_motioncontrol_extra_SetMotorFaultMask(EOtheMotionController *p, uint8_t jomo, uint8_t* fault_mask);

//extern uint32_t eo_motioncontrol_extra_GetMotorFaultMask(EOtheMotionController *p, uint8_t jomo);

extern uint16_t eo_motioncontrol_extra_GetMotorCurrent(EOtheMotionController *p, uint8_t jomo);

extern uint32_t eo_motioncontrol_extra_GetMotorAnalogSensor(EOtheMotionController *p, uint8_t jomo);

extern uint32_t eo_motioncontrol_extra_GetMotorPositionRaw(EOtheMotionController *p, uint8_t jomo);

extern void eo_motioncontrol_extra_ResetQuadEncCounter(EOtheMotionController *p, uint8_t jomo);

extern eObool_t eo_motioncontrol_extra_IsMotorEncoderIndexReached(EOtheMotionController *p, uint8_t jomo);

extern int16_t eo_motioncontrol_extra_GetSuppliedVoltage(EOtheMotionController *p);

// others, one make by marco.accame which groups actions

// why dont we put it inside _Tick() ????
extern eOresult_t eo_motioncontrol_extra_ManageEXTfault(EOtheMotionController *p);




/** @}            
    end of group eo_EOtheMotionController
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


