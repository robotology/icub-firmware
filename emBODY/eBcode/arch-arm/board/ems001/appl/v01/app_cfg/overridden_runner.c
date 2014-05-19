
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

//appl
#include "EOMtheEMSappl.h"
#include "EOtheEMSApplBody.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoManagement.h"

#include "EOemsController_hid.h" 
#include "OPCprotocolManager_Cfg.h" 
#include "EOtheEMSapplDiagnostics.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EOarray.h"

#include "eventviewer.h"


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
//if defined SET_DESIRED_CURR_IN_ONLY_ONE_MSG, the appl sends all desered current to 2fon in only one msg
#define SET_DESIRED_CURR_IN_ONLY_ONE_MSG
#define runner_timeout_send_diagnostics         1000
#define runner_countmax_check_ethlink_status    5000 //every one second

#define COUNT_WATCHDOG_VIRTUALSTRAIN_MAX        50



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

//RX
static void s_eom_emsrunner_hid_read_can_messages(eOcanport_t port, eObool_t all, uint8_t max);


//DO
static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p);



//utils
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p);

#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
#else
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
#endif
EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_checkEthLinks(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t motionDoneJoin2Use = 0;
static uint16_t count_ethlink_status = 0;
static uint8_t count_watchdog_virtaulStrain = 0;
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
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    if(applrunMode__2foc == runmode)
    {
         eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReaderHandle(eo_emsapplBody_GetHandle()));
    }
    
    #if defined(EVIEWER_ENABLED)   
    if(0 == event_view)
    {   
        eventviewer_load(EVIEWER_userDef_hwErrCntr, userDef_hwErrCntr);
        event_view = 1;
    }
    #endif    


}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    eOresult_t                      res;
    EOappTheDB                      *db_ptr = eo_emsapplBody_GetDataBaseHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_SkinCanLocation_t    skincanloc;
    eOsk_skinId_t                   skId;
    uint8_t                         numofRXcanframe = 0;
    uint8_t                         port;
    EOappCanSP                      *cansp = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOarray_of_10canframes          *arrayof10canframes_ptr = NULL;
        
    
    for(port=eOcanport1; port<eo_appCanSP_emscanportnum; port++)
    {
        skincanloc.emscanport = (eOcanport_t)port;
        res = eo_appTheDB_GetSkinId_BySkinCanLocation(eo_appTheDB_GetHandle(), &skincanloc, &skId);
        if(eores_OK == res)
        {
            //reset netvar
            res = eo_appTheDB_GetSkinStArray10CanFramesPtr(eo_appTheDB_GetHandle(), skId,  &arrayof10canframes_ptr);
            eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinStArray10CanFramesPtr");
            
            eo_array_Reset((EOarray*)arrayof10canframes_ptr);
            
            numofRXcanframe = 10;//if skin is conencted i should read 10 messages max, because net var has size of 10 messages
        }
        else
        {
            res = eo_appCanSP_GetNumOfRecCanframe(cansp, (eOcanport_t)port, &numofRXcanframe);
            eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetNumOfRecCanframe");
        }
    
         #ifdef _GET_CANQUEUE_STATISTICS_
        eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(port, numofRXcanframe);
        #endif
        eo_appCanSP_read(cansp, (eOcanport_t)port, numofRXcanframe, NULL); 
    }

}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(emsappbody_ptr);

    /* TAG_ALE */
//     if(applrunMode__skinAndMc4 == runmode)
//     {
//         #warning VALE--> remove this code after test on semaphore-can
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//     }
    

    switch(runmode)
    {
        case applrunMode__2foc:
        {
            s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(p);
        }break;
        
        case applrunMode__mc4Only:
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(p);
        }break;
        
        case applrunMode__skinOnly:
        {
            return; //currently nothing to do 
        }//break;
        
        default:
        {
            return;
        }
    };
  
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{

    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint8_t numoftxframe_p1, numoftxframe_p2;
    
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numoftxframe_p1);
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2, &numoftxframe_p2);
    
#ifdef _GET_CANQUEUE_STATISTICS_
    eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport1, numoftxframe_p1);
    eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport2, numoftxframe_p2);
#endif

}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOresult_t res[2];
 
 //before wait can, check link status!!
    count_ethlink_status ++;
    if(runner_countmax_check_ethlink_status == count_ethlink_status)
    {
        //uint8_t link1_isup;
        //uint8_t link2_isup;
        //this func chacks if one of link change state and notify it.
        //the pkt arrived on pc104 backdoor when one link change down->up.
        //eo_theEMSdgn_checkEthLinksStatus_quickly(eo_theEMSdgn_GetHandle(), &link1_isup, &link2_isup);
        if(eo_dgn_cmds.signalCanStatistics)
        {
            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_canQueueStatistics , 0);
        }
        count_ethlink_status = 0;
    }
    
    
    res[0] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    res[1] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    if((eores_NOK_timeout ==  res[0]) || (eores_NOK_timeout ==  res[1]))
    {
        eo_dgn_emsapplcore.core.runst.cantxfailuretimeoutsemaphore++;
        eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);

    }
    
}


extern void eom_emsrunner_hid_userdef_onfailedtransmission(EOMtheEMSrunner *p)
{
    eOemsrunner_diagnosticsinfo_t *dgn_ptr = eom_emsrunner_GetDiagnosticsInfoHandle(p);
    if(NULL == dgn_ptr)
    {
        return;
    }
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);

}

extern void eom_emsrunner_hid_userdef_onemstransceivererror(EOMtheEMStransceiver *p)
{
    eOemstransceiver_diagnosticsinfo_t* dgn_ptr = eom_emstransceiver_GetDiagnosticsInfoHandle(p);
    
    if(NULL == dgn_ptr)
    {
        return;
    }
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);
    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    
#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
    return(s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(p, pwmList, size));
#else	
    return(s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(p, pwmList,size));
#endif
}

#ifndef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t 				err;
	eOmeas_current_t            value;
    eOresult_t 				    res = eores_OK;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_DISIRED_CURRENT
    };	

#warning VALE --> solo per test
    pwmList[0] = 0x11AA;
    pwmList[1] = 0x12AA;
    pwmList[2] = 0x13AA;
    pwmList[3] = 0x14AA;
    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());

    for (uint8_t jid = 0; jid <numofjoint; jid++)
    {
        value = pwmList[jid];
    
        /*Since in run mode the frame are sent on demnad...here i can punt in tx queue frame to send.
        they will be sent by transmitter */

        err = eo_appCanSP_SendCmd2Joint(eo_emsapplBody_GetCanServiceHandle(p), jid, msgCmd, (void*)&value);
        
        if (err != eores_OK)
        {
            res = err;
        }
    }
    
    return(res);
   
}
#endif

#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t 				                res = eores_OK;
    int16_t                                 pwm_aux[4] = {0, 0, 0, 0};
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         dest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_periodicMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT
    };


    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for (uint8_t jid = 0; jid <numofjoint; ++jid)
    {
        res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jid,  &canLoc, NULL);
        if(eores_OK != res)
        {
            return(res); //i think i'll be never here
        }
        
        pwm_aux[canLoc.addr-1] = pwmList[jid];    
    }
    //since msg is periodic, all 2foc received it so dest is useless.
    dest.dest = 0;
    eo_appCanSP_SendCmd(eo_emsapplBody_GetCanServiceHandle(p), canLoc.emscanport, dest, msgCmd, (void*)pwm_aux);
    
    return(res);    
}
#endif


static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          res;
    EOtheEMSapplBody    *emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint32_t            encvalue[4] = {(uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID};
    int16_t             pwm[4];
//     static uint8_t             mask = 0;
//     static uint32_t             count_mask = 0;
//     #define     COUNT_MASK_MAX      2000

    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());

    if (eo_appEncReader_isReady(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr)))
    {    
        for (uint8_t enc = 0; enc < numofjoint; ++enc)
        {
            res = eo_appEncReader_GetValue(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr), (eOappEncReader_encoder_t)enc, &(encvalue[enc]));
            
            if (res != eores_OK)
            {
                //if (enc == 3) ++hysto_error[encvalue[3]];
                
                encvalue[enc] = (uint32_t)ENC_INVALID;
            }
        }        
    }

    eo_emsController_ReadEncoders((int32_t*)encvalue);
        
    /* 2) pid calc */
    eo_emsController_PWM(pwm);

    /* 3) prepare and punt in rx queue new setpoint */
    s_eom_emsrunner_hid_SetCurrentsetpoint(emsappbody_ptr, pwm, 4); //4: eo_emsController_PWM fills an arry of 4 item 
                                                                    //because max num of mortor for each ems is 4 
 
    /* 4) update joint status */
    s_eom_emsrunner_hid_UpdateJointstatus(p);
    /*Note: motor status is updated with data sent by 2foc by can */

    s_checkEthLinks();

}


static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p)
{
    eOmc_joint_status_t             *jstatus_ptr;
    eOmc_jointId_t                  jId;
    eOresult_t                      res;
    uint16_t                        numofjoint;
    
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    for(jId = 0; jId<numofjoint; jId++)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //error
        }
        
        //eo_emsController_GetJointStatus(jId, &jstatus_ptr->basic);
        eo_emsController_GetJointStatus(jId, jstatus_ptr);
        
        eo_emsController_GetActivePidStatus(jId, &jstatus_ptr->ofpid);
        
        if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus_ptr->basic.motionmonitorstatus)
        {
            /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
            - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
            message because the setpoint is alredy reached. this means that:
                - if monitormode is forever, no new set point has been configured 
                - if monitormode is _untilreached, the joint reached the setpoint already.
            - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
            message because pc104 is not interested in getting motion done.
            */
            if(eo_emsController_GetMotionDone(jId))
            {
                jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
            }
        }
        
        
    }
}

static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    eOresult_t                              res;
    uint8_t                                 send_virtualStrainData;
    uint16_t                                numofjoint;
    eOmc_joint_status_t                     *jstatus_ptr;
    uint16_t                                *virtStrain_ptr;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    EOappTheDB                              *db_ptr; 
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__MOTION_DONE
    };
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    db_ptr = eo_appTheDB_GetHandle();
    
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(db_ptr);

    res = eo_appTheDB_GetJointStatusPtr(db_ptr, motionDoneJoin2Use, &jstatus_ptr);
    if(eores_OK != res)
    {
        return; //error
    }
    
    if(jstatus_ptr->basic.motionmonitorstatus == eomc_motionmonitorstatus_setpointnotreachedyet)
    {
        /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
        - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
        message because the setpoint is alredy reached. this means that:
            - if monitormode is forever, no new set point has been configured 
            - if monitormode is _untilreached, the joint reached the setpoint already.
        - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
        message because pc104 is not interested in getting motion done.
        */
        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, motionDoneJoin2Use, msgCmd, NULL);
    }
    
    motionDoneJoin2Use++;
    if(motionDoneJoin2Use == numofjoint)
    {
        motionDoneJoin2Use = 0;
    }
    
 

    //prepare virtual strain data
    if(eo_appTheDB_IsVirtualStrainDataUpdated(db_ptr))
    {
        send_virtualStrainData = 1;
        count_watchdog_virtaulStrain = 0;
    }
    else
    {
        count_watchdog_virtaulStrain++;
        if(count_watchdog_virtaulStrain <= COUNT_WATCHDOG_VIRTUALSTRAIN_MAX)
        {
            send_virtualStrainData = 1;
        }
        else
        {
            send_virtualStrainData = 0;
        }

    }
    
    
    
    if(send_virtualStrainData)
    {
        eo_appTheDB_GetVirtualStrainDataPtr(db_ptr, &virtStrain_ptr);
        
        res = eo_appTheDB_GetJointCanLocation(db_ptr, 0,  &canLoc, NULL);
        if(eores_OK != res)
        {
            return;
        }

        //set destination of all messages 
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, 12); //virtual ft sensor has address 12
        
        //set command (calss + id) and send it
        msgCmd.class = icubCanProto_msgCmdClass_periodicAnalogSensor;
        msgCmd.cmdId = ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&virtStrain_ptr[0]);
        
        msgCmd.cmdId = ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&virtStrain_ptr[3]);
    }
    s_checkEthLinks();
    
//     for(jId = 0; jId<numofjoint; jId++)
//     {
//         res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
//         if(eores_OK != res)
//         {
//             return; //error
//         }
//         
//         if(jstatus_ptr->basic.motionmonitorstatus == eomc_motionmonitorstatus_setpointnotreachedyet)
//         {
//             /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
//             - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
//             message because the setpoint is alredy reached. this means that:
//                 - if monitormode is forever, no new set point has been configured 
//                 - if monitormode is _untilreached, the joint reached the setpoint already.
//             - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
//             message because pc104 is not interested in getting motion done.
//             */
//             eo_appCanSP_SendCmd2Joint(appCanSP_ptr, jId, msgCmd, NULL);
//         }

//     }   
}




#define ethLinksCount_max 3000 //5 sec
static void s_checkEthLinks(void)
{
    static uint32_t ethLinksCount;
    static uint32_t ethLinksCount_partial;
    static uint8_t linknum = 255;
    
    ethLinksCount++;
    
    if((ethLinksCount< ethLinksCount_max) && (255 == linknum))
    {
        return;
    }

    if(255 == linknum)
    {
        linknum = 0;
        #if defined(EVIEWER_ENABLED)    
        evEntityId_t prev = eventviewer_switch_to(EVIEWER_userDef_hwErrCntr);
        #endif
        
        eo_theEMSdgn_checkEthLinkErrors(eo_theEMSdgn_GetHandle(), linknum);
        
        #if defined(EVIEWER_ENABLED)    
        eventviewer_switch_to(prev);
        #endif  
        linknum++;
    }
    else
    {
        if(ethLinksCount_partial == 20)
        {
            ethLinksCount_partial = 0;
            #if defined(EVIEWER_ENABLED)    
            evEntityId_t prev = eventviewer_switch_to(EVIEWER_userDef_hwErrCntr);
            #endif
            
            eo_theEMSdgn_checkEthLinkErrors(eo_theEMSdgn_GetHandle(), linknum);
            
            #if defined(EVIEWER_ENABLED)    
            eventviewer_switch_to(prev);
            #endif  
            linknum++;
            if(linknum == 3)
            {
                if((eo_dgn_cmds.signalEthCounters) && (eo_theEMSdgn_EthLinksInError(eo_theEMSdgn_GetHandle())) )
                //if((eo_dgn_cmds.signalEthCounters) )
                {
                    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsperiph , 0);
                }

                linknum = 255;
                ethLinksCount = 0;
            }
        }
    }
    ethLinksCount_partial++;
}


static void s_eom_emsrunner_hid_read_can_messages(eOcanport_t port, eObool_t all, uint8_t max)
{
    EOappCanSP*  cansp = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    uint8_t      numofRXcanframe = 0;
    eOresult_t   res;
    
    if(all)
    {
        res = eo_appCanSP_GetNumOfRecCanframe(cansp, port, &numofRXcanframe);
        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetNumOfRecCanframe");
    }
    else
    {
        numofRXcanframe = max;
    }
    
    #ifdef _GET_CANQUEUE_STATISTICS_
        eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(port, numofRXcanframe);
    #endif
    eo_appCanSP_read(cansp, port, numofRXcanframe, NULL); 

}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




