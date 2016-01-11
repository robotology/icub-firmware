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
// empty-section    


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
  
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
}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    // so far we tick only the motion control.
    eo_motioncontrol_Tick(eo_motioncontrol_GetHandle());
    
    // however, we could also tick others ....
    // TODO: see if i can move all the _Tick() in the do phase.
    
    // eo_ethmonitor_Tick(eo_ethmonitor_GetHandle()); // if we do it in here, then in eb2/eb4 it warns about execution time overflow
}


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
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - removed code 
// --------------------------------------------------------------------------------------------------------------------


// maybe: USE_ONLY_QE, motiondon is not different, etc.
#if 0



// want to send a canframe with pwm onto can bus. 
static eOresult_t s_SetCurrentSetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOcanport_t port = eOcanport1;
    
    // now i need to assign port and command with correct values.
    
    // i manage 2foc boards. they are at most 4. they must be all in the same can bus. 
    int16_t pwmValues[4] = {0, 0, 0, 0};
    
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {
        eOcanmap_location_t loc = {0};
        // search the address of motor i-th and fill the pwmValues[] in relevant position.
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i, 0), &loc, NULL, NULL))
        {
            port = (eOcanport_t)loc.port;  // marco.accame: i dont check if the port is not always the same ... it MUST be.          
            if((loc.addr > 0) && (loc.addr <= 4))
            {
                pwmValues[loc.addr-1] = pwmList[i];
            }            
        }        
    }
    
    // ok, now i fill command and location
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_periodicMotorControl;    
    command.type  = ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT;
    command.value = &pwmValues[0];
    
    eOcanmap_location_t location = {0};
    location.port = port;
    location.addr = 0; // marco.accame: we put 0 just because it is periodic and this is the source address (the EMS has can address 0).
    location.insideindex = eocanmap_insideindex_first; // because all 2foc have motor on index-0. 

    // and i send the command
    return(eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location));   
}


static void s_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          							res;
    EOtheEMSapplBody    							*emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint32_t            							encvalue[4] = {0}; 
    hal_encoder_errors_flags					    encflags[4] = {0};
    int16_t             							pwm[4] = {0};
    EOappEncReader      							*app_enc_reader = eo_emsapplBody_GetEncoderReader(emsappbody_ptr);
    uint8_t             							error_mask = 0;

    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
        
#ifndef USE_ONLY_QE
    
    uint8_t spistream0 = 0;
    uint8_t spistream1 = 0;
    
    for(uint8_t i=0; i<30; ++i)
    {
        if (eo_appEncReader_isReady(app_enc_reader))
        {
            //stop_read = osal_system_abstime_get();
            break;
        }
        else
        {
            if (!eo_appEncReader_isReadySPI_stream0(app_enc_reader)) ++spistream0;
            if (!eo_appEncReader_isReadySPI_stream1(app_enc_reader)) ++spistream1;
            hal_sys_delay(5);
        }
    }
    
    
    if (eo_appEncReader_isReady(app_enc_reader))
    {    
        for (uint8_t jo=0; jo<numofjomos; jo++)
        {
            uint32_t extra = 0;
            res = eo_appEncReader_GetJointValue(app_enc_reader, jo, &(encvalue[jo]), &extra, &(encflags[jo]));
            if (res != eores_OK)
            {
                error_mask |= 1<<(jo<<1);
                //encvalue[enc] = (uint32_t)ENC_INVALID;
            }
        }        
    }
    else
    {
        error_mask = 0xAA; // timeout
    }

    // Restart the reading of the encoders
    eo_appEncReader_StartRead(app_enc_reader);
		
#endif // USE_ONLY_QE
    
    eo_emsController_AcquireAbsEncoders((int32_t*)encvalue, error_mask);
    
    eo_emsController_CheckFaults();
        
    /* 2) pid calc */
    eo_emsController_PWM(pwm);

    /* 3) prepare and punt in rx queue new setpoint */
    s_SetCurrentSetpoint(emsappbody_ptr, pwm, 4); //4: eo_emsController_PWM fills an arry of 4 item 
                                                                    //because max num of mortor for each ems is 4 
 
    /* 4) update joint status */
    s_UpdateJointstatus(p);

    /*Note: motor status is updated with data sent by 2foc by can */

}


static void s_UpdateJointstatus(EOMtheEMSrunner *p)
{
    eOmc_joint_status_t             *jstatus = NULL;
    eOmc_motor_status_t             *mstatus = NULL;
    eOmc_jointId_t                  jId;

    uint8_t                         transmit_decoupled_pwms = 1;
    
    
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());    

    for(jId = 0; jId<numofjomos; jId++)
    {
        jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jId);
        if(NULL == jstatus)
        {
            return; //error
        }
        
        eo_emsController_GetJointStatus(jId, jstatus);
        
        eo_emsController_GetActivePidStatus(jId, &jstatus->ofpid);
        
        if (transmit_decoupled_pwms) 
        {  
            //this functions is used to get the motor PWM after the decoupling matrix
            eo_emsController_GetPWMOutput(jId, &jstatus->ofpid.generic.output);
        }

        jstatus->modes.ismotiondone = eo_emsController_GetMotionDone(jId);
        
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
//                jstatus->ismotiondone = eo_emsController_GetMotionDone(jId);
//            }
//        }
        
        
    }
    
    
    for(jId = 0; jId<numofjomos; jId++)
    {
        mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), jId);
        if(NULL == mstatus)
        {
            return; //error
        }
        
        eo_emsController_GetMotorStatus(jId, mstatus);
    }
}


static void s_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    
    // motion done
    eo_motiondone_Tick(eo_motiondone_GetHandle());
    
    // virtual strain
    eo_virtualstrain_Tick(eo_virtualstrain_GetHandle());
    
}

#endif // 0


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




