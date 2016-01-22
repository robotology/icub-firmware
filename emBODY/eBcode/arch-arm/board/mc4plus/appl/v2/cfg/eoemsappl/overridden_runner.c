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

#include "EoCommon.h"

#include "EOtheErrorManager.h"
#include "EoError.h"


#include "EOtheCANservice.h"
#include "EOtheInertials.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheMotionController.h"
#include "EOtheSKIN.h"
#include "EOtheETHmonitor.h"

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
// empty-section


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

//static void s_overriden_runner_CheckAndUpdateExtFaults(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// emoty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    // tick of currents watchdog  
//    eo_currents_watchdog_Tick(eo_currents_watchdog_GetHandle());
}


extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    // i tick the can-discovery. 
    // this function does something only if a discovery is active and if the search timer period has expired.
    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
    
    // i manage the can-bus reception. i parse everything. 
    // it will be the can parser functions which will call the relevant objects which will do what they must.
    // as an example, the broadcast skin can frames are always parsed and are given to EOtheSKIN which will decide what to do with them
    eo_canserv_ParseAll(eo_canserv_GetHandle());    
    
 
    //check and update faults mask for the motor
    eo_motioncontrol_extra_ManageEXTfault(eo_motioncontrol_GetHandle());
}


//extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
//{
//    // we should also handle a skin case in here (see comments at the end of this function), but at the moment we make it work with the mais
//    
//    if (eo_canserv_GetHandle() != NULL)
//    {
//        uint8_t numofRXcanframe = 0;
//        numofRXcanframe = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), eOcanport1);
//        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, numofRXcanframe, NULL);
//    }
//    /*
//    for(port=eOcanport1; port<eOcanports_number; port++)
//    {
//        // marco.accame: the following strategy is ok if the bus containing skin does not contain any other board type
//        // the bus has only skin? then i read at most a number of canframes equal to what the skin-data buffer can host
//        // else i read all the content of the rx can buffer.
//        
//        // step 1: decide the max number of can frames to read
//        
//        EOarray *arrayofcandata = s_getSkinDataArray((eOcanport_t)port);
//        if(NULL != arrayofcandata)
//        {   // ok, in this can bus we have a skin 

//            // i read from bus at most the number of can frame that the array can host. 
//            // that does not depend on the fact that in this cycle we transmit the regulars.
//            // the array is emptied only after the regulars are transmitted, so that no can frame is lost.
//            // this operation is done in function eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission()
//            numofRXcanframe = eo_array_Available(arrayofcandata);
//        }
//        else
//        {   // in this can bus we dont have skin, thus ... i read everything in the rx can buffer
//            numofRXcanframe = eo_canserv_NumberOfFramesInRXqueue(eo_canserv_GetHandle(), (eOcanport_t)port);
//        }

//        // step 2: read the can frames. this function also parses them and triggers associated action ...
//        
//        eo_canserv_Parse(eo_canserv_GetHandle(), (eOcanport_t)port, numofRXcanframe, NULL);
//    }
//    */
//    
//    //check and update faults mask for the motor
//    s_overriden_runner_CheckAndUpdateExtFaults();
//}



extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    // so far we tick only the motion control.
    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
    
    // however, we could also tick others ....
    // TODO: see if i can move all the _Tick() in the do phase.
    
    // eo_ethmonitor_Tick(eo_ethmonitor_GetHandle()); // if we do it in here, then in eb2/eb4 it warns about execution time overflow
}


//extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
//{  
//    eo_mcserv_Actuate(eo_mcserv_GetHandle());    
//}


//extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
//{
//    if (eo_canserv_GetHandle() != NULL)
//    {
//        eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport1, NULL);
//    }
//}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
    uint8_t txcan1frames = 0;
    uint8_t txcan2frames = 0;

    eo_canserv_TXstartAll(eo_canserv_GetHandle(), &txcan1frames, &txcan2frames);
    
    eom_emsrunner_Set_TXcanframes(eom_emsrunner_GetHandle(), txcan1frames, txcan2frames);
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{  
    eObool_t prevTXhadRegulars = eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle());
    
    // ticks some services ... 
    // marco.accame: i put them in here just after tx phase. however, we can move it even in eom_emsrunner_hid_userdef_taskDO_activity() 
    // because eom_emsrunner_CycleHasJustTransmittedRegulars() keeps memory of previous tx cycle.
    eo_skin_Tick(eo_skin_GetHandle(), prevTXhadRegulars); 
    
    eo_inertials_Tick(eo_inertials_GetHandle(), prevTXhadRegulars); 
    
    eo_ethmonitor_Tick(eo_ethmonitor_GetHandle());


    
    // ABSOLUTELY KEEP IT LAST: wait until can tx started by eo_canserv_TXstartAll() in eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission() is all done
    const eOreltime_t timeout = 3*EOK_reltime1ms;
    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);   

    return; 
}


//extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
//{
//    // we should also handle a skin case in here (see comments at the end of this function), but at the moment we make it work with the mais
//    
//    if (eo_canserv_GetHandle() != NULL)
//    {
//        // wait for the can tx to finish. 
//        // diagnostics about tx failure within the specified timeout is managed internally 
//        const uint32_t timeout = 3*osal_reltime1ms;
//        eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, timeout);
//    }
//    
//    /*
//    uint8_t port = 0;
//    EOarray *arrayofcandata = NULL;
//    for(port=eOcanport1; port<eOcanports_number; port++)
//    {
//        if(NULL != (arrayofcandata = s_getSkinDataArray((eOcanport_t)port)))
//        {   // ok, in this can bus we have a skin   
//            if(eobool_true == eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle()))
//            {   // ok, we can reset the array of can data
//                eo_array_Reset(arrayofcandata);
//            }            
//        }
//    }
//    */
//}


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


//extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
//{
//    eOerrmanDescriptor_t errdes = {0};
//    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_udptxfailure);
//    errdes.par16            = 0;
//    errdes.par64            = 0;
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = 0;    
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes);         
//}

//extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
//{
//    eOerrmanDescriptor_t errdes = {0};
//    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runner_transceivererror);
//    errdes.par16            = 0;
//    errdes.par64            = 0;
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = 0;    
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSrunner", &errdes); 
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static void s_overriden_runner_CheckAndUpdateExtFaults(void)
//{
//    #warning marco.accame: implement in motion-controller object the external faults
////    if (eo_mcserv_AreMotorsExtFaulted(eo_mcserv_GetHandle()))
////    {
////        uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
////        uint32_t state = 0;
////        //set the fault mask for ALL the motors
////        for (uint8_t i = 0; i < numofjomos; i++)
////        {
////            state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),i);
////            if((state & MOTOR_EXTERNAL_FAULT) == 0) //external fault bit not set
////            {
////                //simulate the CANframe used by 2FOC to signal the status
////                uint64_t fault_mask = (((uint64_t)(state | MOTOR_EXTERNAL_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
////                eo_motor_set_motor_status(eo_motors_GetHandle(),i, (uint8_t*)&fault_mask);
////            }
////        }
////    }
//    return;
//}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




