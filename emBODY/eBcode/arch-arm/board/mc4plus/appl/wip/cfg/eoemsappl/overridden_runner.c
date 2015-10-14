/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdio.h"

//general
#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOarray.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

//appl
#include "EOMtheEMSappl.h"
//#include "EOtheEMSApplBody.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoManagement.h"


//#include "EOemsController_hid.h" 

#include "EOarray.h"

#include "eventviewer.h"

#include <stdio.h>
#include <string.h>

#include "EOtheServices.h"

#include "EOtheEntities.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



#if defined(EVIEWER_ENABLED)
#define EVIEWER_userDef_IDbase             (ev_ID_first_usrdef+1)
//#define EVIEWER_userDef_RUNRecRopframe     (EVIEWER_userDef_IDbase +1) see definition in EOMtheEMSrunner.c
#define EVIEWER_userDef_hwErrCntr          (EVIEWER_userDef_IDbase +3)
#endif

#if defined(EVIEWER_ENABLED)
void userDef_hwErrCntr(void){}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void    s_overriden_runner_CheckAndUpdateExtFaults(void);
static void    s_overriden_runner_UpdateMotorsStatus(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if defined(EVIEWER_ENABLED) 
static uint8_t event_view = 0;
#endif 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
  //here we could update some of the values which are updated via CAN callback for the EMS with mc 2foc-based (e.g. currents of the motors OR rotor position etc...)
    
  //update motors status
  s_overriden_runner_UpdateMotorsStatus();
  
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    // we should also handle a skin case in here (see comments at the end of this function), but at the moment we make it work with the mais
    
    if (eo_canserv_GetHandle() != NULL)
    {
        uint8_t numofRXcanframe = 0;
        numofRXcanframe = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), eOcanport1);
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, numofRXcanframe, NULL);
    }
    /*
    for(port=eOcanport1; port<eOcanports_number; port++)
    {
        // marco.accame: the following strategy is ok if the bus containing skin does not contain any other board type
        // the bus has only skin? then i read at most a number of canframes equal to what the skin-data buffer can host
        // else i read all the content of the rx can buffer.
        
        // step 1: decide the max number of can frames to read
        
        EOarray *arrayofcandata = s_getSkinDataArray((eOcanport_t)port);
        if(NULL != arrayofcandata)
        {   // ok, in this can bus we have a skin 

            // i read from bus at most the number of can frame that the array can host. 
            // that does not depend on the fact that in this cycle we transmit the regulars.
            // the array is emptied only after the regulars are transmitted, so that no can frame is lost.
            // this operation is done in function eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission()
            numofRXcanframe = eo_array_Available(arrayofcandata);
        }
        else
        {   // in this can bus we dont have skin, thus ... i read everything in the rx can buffer
            numofRXcanframe = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), (eOcanport_t)port);
        }

        // step 2: read the can frames. this function also parses them and triggers associated action ...
        
        eo_canserv_Parse(eo_canserv_GetHandle(), (eOcanport_t)port, numofRXcanframe, NULL);
    }
    */
    
    //check and update faults mask for the motor
    s_overriden_runner_CheckAndUpdateExtFaults();
}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{  
    //check and update faults mask for the motor
    //s_overriden_runner_CheckAndUpdateExtFaults();
    
    eo_mcserv_Actuate(eo_mcserv_GetHandle());    
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    if (eo_canserv_GetHandle() != NULL)
    {
        eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport1, NULL);
    }
}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    // we should also handle a skin case in here (see comments at the end of this function), but at the moment we make it work with the mais
    
    if (eo_canserv_GetHandle() != NULL)
    {
        // wait for the can tx to finish. 
        // diagnostics about tx failure within the specified timeout is managed internally 
        const uint32_t timeout = 3*osal_reltime1ms;
        eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, timeout);
    }
    
    /*
    uint8_t port = 0;
    EOarray *arrayofcandata = NULL;
    for(port=eOcanport1; port<eOcanports_number; port++)
    {
        if(NULL != (arrayofcandata = s_getSkinDataArray((eOcanport_t)port)))
        {   // ok, in this can bus we have a skin   
            if(eobool_true == eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle()))
            {   // ok, we can reset the array of can data
                eo_array_Reset(arrayofcandata);
            }            
        }
    }
    */
}


extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes);         
}

extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_transceivererror);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_overriden_runner_CheckAndUpdateExtFaults(void)
{
    if (eo_mcserv_AreMotorsExtFaulted(eo_mcserv_GetHandle()))
    {
        uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
        //set the fault mask for ALL the motors
        for (uint8_t i = 0; i < numofjomos; i++)
        {
            uint32_t state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),i);
            if((state & MOTOR_EXTERNAL_FAULT) == 0) //external fault bit not set
            {
                //uint8_t fault_mask[6] = {0x4, 0x0, 0x0, 0x0, 0x0, 0x0}; //setting only the external fault bit
                uint8_t fault_mask[6] = {0x0, 0x4, 0x0, 0x0, 0x0, 0x0}; //byte0 -> idle state for the joint, byte1 -> ext fault
                eo_motor_set_motor_status(eo_motors_GetHandle(),i, fault_mask);
            }
        }
    }
    return;
}

static void s_overriden_runner_UpdateMotorsStatus(void)
{
  eOmc_motor_t* mot;
  eOmc_joint_t* jnt;    
  int16_t  mot_curr = 0;
  uint32_t mot_pos  = 0;    
      
  uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());  
  for (uint8_t i = 0; i < numofjomos; i++)
  {       
    if((NULL == (jnt = eo_mcserv_GetJoint(eo_mcserv_GetHandle(), i))) ||
       (NULL == (mot = eo_mcserv_GetMotor(eo_mcserv_GetHandle(), i)))) 
    {
        continue;        
    }
    
    //Get values using MCService interfaces
    mot_curr = eo_mcserv_GetMotorCurrent (eo_mcserv_GetHandle(), i);
    
    //rotor position update inside the control loop, reading using the EOappEncodersReader 
    /*
    //mot_pos  = eo_mcserv_GetMotorPositionRaw(eo_mcserv_GetHandle(), i);
    //converting in iCub degrees the rotorposition
    float divider = mot->config.rotorEncoderResolution;
    if(0.0f == divider)
    {
        mot_pos = 0;       
    }
    else
    {
        if(divider < 0)
        {
            divider = -divider;
        }
    
        mot_pos  = (float) (mot_pos * 65535.0) / mot->config.rotorEncoderResolution;
    }
    */
    
    
   
    //update structure which will be broadcasted
    //eo_emsController_AcquireMotorEncoder(i, mot_curr, 0, (int32_t) mot_pos);
    eo_emsController_AcquireMotorCurrent(i, mot_curr);
  }
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




