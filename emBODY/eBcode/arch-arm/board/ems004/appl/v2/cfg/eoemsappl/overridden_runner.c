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


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"
#include "EOarray.h"
#include "EOtheErrorManager.h"
#include "EoError.h"


#include "EOMtheEMSappl.h"
//#include "EOtheEMSApplBody.h"


#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoManagement.h"

#include "EOtheEntities.h"

//#include "EOtheCANdiscovery.h"

#include "EOemsController_hid.h" 
#include "OPCprotocolManager_Cfg.h" 

#include "EOarray.h"

#include "eventviewer.h"


#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

//#include "EOtheMotionDone.h"
//#include "EOtheVirtualStrain.h"

#include "EOtheInertial.h"

#include "EOtheCANdiscovery2.h"

#include "EOtheMotionController.h"

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


    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
    


static EOarray* s_getSkinDataArray(eOcanport_t port);

//DO
//static void s_taskDO_activity_2foc(EOMtheEMSrunner *p);

//static void s_taskDO_activity_mc4(EOMtheEMSrunner *p);



//utils
//static void s_UpdateJointstatus(EOMtheEMSrunner *p);


//static eOresult_t s_SetCurrentSetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);


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
    static uint32_t iter = 0;

    iter ++;    
  
}


extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    // i tick the can-discovery. 
    // this function does something only if a discovery is active and if the search timer period has expired.
    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
    
    // i manage the can-bus reception. so far i dont care about skin, thus i parse everything
    eo_canserv_ParseAll(eo_canserv_GetHandle());
    
    
    
#if !defined(DEBUG_REMOVE_OLD_SERVICES)    
    
    rimosso
    // other stuff.
    
    uint8_t                         numofRXcanframe = 0;
    uint8_t                         port;
    
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
    
#endif //!defined(DEBUG_REMOVE_OLD_SERVICES)
    

}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    
    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
    
    
#if !defined(DEBUG_REMOVE_OLD_SERVICES)
    
#if !defined(TEST_EB2_EB4_WITHOUT_MC)
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode = eo_emsapplBody_GetAppRunMode(emsappbody_ptr);


    switch(runmode)
    {
        case applrunMode__2foc:
        {
            s_taskDO_activity_2foc(p);
        } break;
        
        case applrunMode__mc4Only:
        case applrunMode__skinAndMc4:
        {
            s_taskDO_activity_mc4(p);
        } break;
        
        case applrunMode__skinOnly:
        {
            //currently nothing to do 
        } break;
        
        default:
        {

        } break;
    }
#endif
    
#endif //!defined(DEBUG_REMOVE_OLD_SERVICES)
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{      
    eo_canserv_TXstartAll(eo_canserv_GetHandle());
}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{  
    // do things ...


    
    // KEEP IT LAST: wait until can tx is all done
    const uint32_t timeout = 3*osal_reltime1ms;
    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), timeout);   

    return;
    

#if !defined(DEBUG_REMOVE_OLD_SERVICES)
    
    // if we have skin and we have just transmitted the regulars, then we must reset the status->array containing the
    // can frames, so that at the rx cycle we can put some more canframes inside.
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
    
    
    
    // we could refresh status in here ... but only if we have just sent to robotinterface the previous status
    if(eobool_true == eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle()))
    {
        eo_inertial_RefreshStatusOfEntity(eo_inertial_GetHandle());
    }
    
    
    // now we wait for the can tx to finish. 
    // diagnostics about tx failure within the specified timeout is managed internally 
    const uint32_t timeout = 3*osal_reltime1ms;
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, timeout);
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport2, timeout);   

#endif // #if !defined(DEBUG_REMOVE_OLD_SERVICES)    
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

static EOarray* s_getSkinDataArray(eOcanport_t port)
{   // tells if the can port has a skin entity in it 
    static EOarray* arraysofcandata[2] = {NULL, NULL};
    static eObool_t evaluated[2] = {eobool_false, eobool_false};
    

    if(eobool_false == evaluated[port])
    {
        evaluated[port] = eobool_true;       
        eOcanmap_location_t loc = {0};
        
        // is the first skin with index 0 or with index 1 on this can bus? 
        
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, 0, 0), &loc, NULL, NULL))
        {   // found a skin-0
            if(port == loc.port)
            {   // and it is on the same can port passed as function argument
                eOsk_status_t * skinstatus = eo_entities_GetSkinStatus(eo_entities_GetHandle(), 0);
                if(NULL != skinstatus)
                {
                    arraysofcandata[port] = (EOarray*)(&skinstatus->arrayofcandata);
                }                
            }
            
        }
        else if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, 1, 0), &loc, NULL, NULL))
        {   // found a skin-1
            if(port == loc.port)
            {   // and it is on the same can port passed as function argument
                eOsk_status_t * skinstatus = eo_entities_GetSkinStatus(eo_entities_GetHandle(), 1);
                if(NULL != skinstatus)
                {
                    arraysofcandata[port] = (EOarray*)(&skinstatus->arrayofcandata);
                }                
            }            
        }
    }
        
    return(arraysofcandata[port]);    
}




//// want to send a canframe with pwm onto can bus. 
//static eOresult_t s_SetCurrentSetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
//{
//    eOcanport_t port = eOcanport1;
//    
//    // now i need to assign port and command with correct values.
//    
//    // i manage 2foc boards. they are at most 4. they must be all in the same can bus. 
//    int16_t pwmValues[4] = {0, 0, 0, 0};
//    
//    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
//    uint8_t i=0;
//    for(i=0; i<numofjomos; i++)
//    {
//        eOcanmap_location_t loc = {0};
//        // search the address of motor i-th and fill the pwmValues[] in relevant position.
//        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i, 0), &loc, NULL, NULL))
//        {
//            port = (eOcanport_t)loc.port;  // marco.accame: i dont check if the port is not always the same ... it MUST be.          
//            if((loc.addr > 0) && (loc.addr <= 4))
//            {
//                pwmValues[loc.addr-1] = pwmList[i];
//            }            
//        }        
//    }
//    
//    // ok, now i fill command and location
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_periodicMotorControl;    
//    command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
//    command.value = &pwmValues[0];
//    
//    eOcanmap_location_t location = {0};
//    location.port = port;
//    location.addr = 0; // marco.accame: we put 0 just because it is periodic and this is the source address (the EMS has can address 0).
//    location.insideindex = eocanmap_insideindex_first; // because all 2foc have motor on index-0. 

//    // and i send the command
//    return(eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location));   
//}


//static void s_taskDO_activity_2foc(EOMtheEMSrunner *p)
//{
//    eOresult_t          							res;
//    EOtheEMSapplBody    							*emsappbody_ptr = eo_emsapplBody_GetHandle();
//    uint32_t            							encvalue[4] = {0}; 
//    hal_encoder_errors_flags					    encflags[4] = {0};
//    int16_t             							pwm[4] = {0};
//    EOappEncReader      							*app_enc_reader = eo_emsapplBody_GetEncoderReader(emsappbody_ptr);
//    uint8_t             							error_mask = 0;

//    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
//        
//    uint8_t spistream0 = 0;
//    uint8_t spistream1 = 0;
//    
//    for(uint8_t i=0; i<30; ++i)
//    {
//        if (eo_appEncReader_isReady(app_enc_reader))
//        {
//            //stop_read = osal_system_abstime_get();
//            break;
//        }
//        else
//        {
//            if (!eo_appEncReader_isReadySPI_stream0(app_enc_reader)) ++spistream0;
//            if (!eo_appEncReader_isReadySPI_stream1(app_enc_reader)) ++spistream1;
//            hal_sys_delay(5);
//        }
//    }
//    
//    
//    if (eo_appEncReader_isReady(app_enc_reader))
//    {    
//        for (uint8_t jo=0; jo<numofjomos; jo++)
//        {
//            uint32_t extra = 0;
//            res = eo_appEncReader_GetJointValue(app_enc_reader, jo, &(encvalue[jo]), &extra, &(encflags[jo]));
//            if (res != eores_OK)
//            {
//                error_mask |= 1<<(jo<<1);
//                //encvalue[enc] = (uint32_t)ENC_INVALID;
//            }
//        }        
//    }
//    else
//    {
//        error_mask = 0xAA; // timeout
//    }

//    // Restart the reading of the encoders
//    eo_appEncReader_StartRead(app_enc_reader);
//		
//    eo_emsController_AcquireAbsEncoders((int32_t*)encvalue, error_mask);
//    
//    eo_emsController_CheckFaults();
//        
//    /* 2) pid calc */
//    eo_emsController_PWM(pwm);

//    /* 3) prepare and punt in rx queue new setpoint */
//    s_SetCurrentSetpoint(emsappbody_ptr, pwm, 4); //4: eo_emsController_PWM fills an arry of 4 item 
//                                                                    //because max num of mortor for each ems is 4 
// 
//    /* 4) update joint status */
//    s_UpdateJointstatus(p);

//    /*Note: motor status is updated with data sent by 2foc by can */

//}


//static void s_UpdateJointstatus(EOMtheEMSrunner *p)
//{
//    eOmc_joint_status_t             *jstatus = NULL;
//    eOmc_motor_status_t             *mstatus = NULL;
//    eOmc_jointId_t                  jId;

//    uint8_t                         transmit_decoupled_pwms = 1;
//    
//    
//    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());    

//    for(jId = 0; jId<numofjomos; jId++)
//    {
//        jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jId);
//        if(NULL == jstatus)
//        {
//            return; //error
//        }
//        
//        eo_emsController_GetJointStatus(jId, jstatus);
//        
//        eo_emsController_GetActivePidStatus(jId, &jstatus->ofpid);
//        
//        if (transmit_decoupled_pwms) 
//        {  
//            //this functions is used to get the motor PWM after the decoupling matrix
//            eo_emsController_GetPWMOutput(jId, &jstatus->ofpid.output);
//        }
//        
//        if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus->basic.motionmonitorstatus)
//        {
//            /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
//            - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
//            message because the setpoint is alredy reached. this means that:
//                - if monitormode is forever, no new set point has been configured 
//                - if monitormode is _untilreached, the joint reached the setpoint already.
//            - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
//            message because pc104 is not interested in getting motion done.
//            */
//            if(eo_emsController_GetMotionDone(jId))
//            {
//                jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
//            }
//        }
//        
//        
//    }
//    
//    
//    for(jId = 0; jId<numofjomos; jId++)
//    {
//        mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), jId);
//        if(NULL == mstatus)
//        {
//            return; //error
//        }
//        
//        eo_emsController_GetMotorStatus(jId, mstatus);
//    }
//}


//static void s_taskDO_activity_mc4(EOMtheEMSrunner *p)
//{
//    
//    // motion done
//    eo_motiondone_Tick(eo_motiondone_GetHandle());
//    
//    // virtual strain
//    eo_virtualstrain_Tick(eo_virtualstrain_GetHandle());
//    
//}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




