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
#include "EOtheEMSApplBody.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoManagement.h"

#include "EOtheProtocolWrapper.h"

#include "EOemsController_hid.h" 
#include "OPCprotocolManager_Cfg.h" 
#include "EOtheEMSapplDiagnostics.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EOarray.h"

#include "eventviewer.h"

#include <stdio.h>
#include <string.h>


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

#define COUNT_WATCHDOG_VIRTUALSTRAIN_MAX        10
#define COUNT_BETWEEN_TWO_UPDATES_MAx           200 /* equal to timeout in mc4 before mc4 considers useless strain values
                                                       see macro "STRAIN_SAFE" in iCub\firmware\motorControllerDsp56f807\common_source_code\include\strain_board.h*/




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

/*
extern uint64_t startofcycletime;
    
extern uint64_t eom_emsrunner_rxstart;
extern uint64_t eom_emsrunner_dostart;
extern uint64_t eom_emsrunner_txstart;
    
extern uint64_t eom_emsrunner_rxduration;
extern uint64_t eom_emsrunner_doduration;
extern uint64_t eom_emsrunner_txduration;

extern uint64_t eom_emsrunner_rxprevduration;
extern uint64_t eom_emsrunner_doprevduration;
extern uint64_t eom_emsrunner_txprevduration;
*/

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static EOarray* s_getSkinDataArray(eOcanport_t port);

//RX
// marco.accame: commented it out on nov 26 2014 because it is not used and the compiler complains
//static void s_eom_emsrunner_hid_read_can_messages(eOcanport_t port, eObool_t all, uint8_t max);


//DO
static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p);



//utils
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p);

//#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
//#else
//static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
//#endif
EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_checkEthLinks(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t motionDoneJoin2Use = 0;
static uint16_t count_ethlink_status = 0;
static uint8_t count_watchdog_virtaulStrain = 0;
static uint8_t count_between_two_updates = 0;
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
    static uint32_t count = 0;
    count ++;
    
    if(1 == count)
    {
        eo_emsapplBody_SignalDetectedCANboards(eo_emsapplBody_GetHandle());
    }
    
    /*    
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    if(applrunMode__2foc == runmode)
    {
         eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReaderHandle(eo_emsapplBody_GetHandle()));
    }
    */
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
    uint8_t                         numofRXcanframe = 0;
    uint8_t                         port;
    EOappCanSP                      *cansp = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    for(port=eOcanport1; port<eo_appCanSP_emscanportnum; port++)
    {
        // marco.accame: the following strategy is ok if the bus containing skin does not contain any other board type
        // the bus has only skin? then i read at most a number of canframes equal to what the skin-data buffer can host
        // else i read all the content of the rx can buffer.
        
        // step 1: decide the max number of can frames to read
        
        EOarray *arrayofcandata = s_getSkinDataArray((eOcanport_t)port);
        if(NULL != arrayofcandata)
        {   // ok, we have a skin   

            // i read from bus at most the number of can frame that the array can host. 
            // that does not depend on the fact that in this cycle we transmit the regulars.
            // the array is emptied only after the regulars are transmitted, so that non can frame is lost.
            // this opertion is done in function eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission()
            numofRXcanframe = eo_array_Available(arrayofcandata);
//            if(eobool_false == eom_emsrunner_CycleIsTransmitting(eom_emsrunner_GetHandle()))
//            {   // if we dont transmit in this cycle then i dont read any frame from this bus
//                numofRXcanframe = 0;                
//            }
//            else
//            {   // we read at most the capacity of the skin patch. but we also need to reset the array so that new can frames can be pushed back inside
//                numofRXcanframe = eo_array_Capacity(arrayofcandata);
//                eo_array_Reset(arrayofcandata);                
//            }  
        }
        else
        {   // in this bus we dont have skin, thus ... i read everything in the rx can buffer
            eo_appCanSP_GetNumOfRecCanframe(cansp, (eOcanport_t)port, &numofRXcanframe);
        }
        
    
//        // step x: update diagnostics (marco.accame: maybe we remove it later on)
//        #ifdef _GET_CANQUEUE_STATISTICS_
//        eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(port, numofRXcanframe);
//        #endif
        
        // step 2: read the can frames. this function also parses them and triggers associated action ...
        eo_appCanSP_read(cansp, (eOcanport_t)port, numofRXcanframe, NULL); 
    }
}





extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode = eo_emsapplBody_GetAppRunMode(emsappbody_ptr);

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
        } break;
        
        case applrunMode__mc4Only:
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(p);
        } break;
        
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
    uint8_t numofframes2betransmitted_can1 = 0;
    uint8_t numofframes2betransmitted_can2 = 0;
    
    // i enable transmission on both can buses. functions are not blocking as the transmission is done by the ISRs
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numofframes2betransmitted_can1);
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2, &numofframes2betransmitted_can2);
    
//#ifdef _GET_CANQUEUE_STATISTICS_
//    eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport1, numoftxframe_p1);
//    eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport2, numoftxframe_p2);
//#endif
}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOresult_t res[2] = {eores_NOK_generic, eores_NOK_generic};
 
    // before wait can, check link status!!
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
        
    // if we have skin and we have just transmitted the regulars, then we must reset the status->array containing the
    // can frames, so that at the rx cycle we can put some more canframes inside.
    uint8_t port = 0;
    EOarray *arrayofcandata = NULL;
    for(port=eOcanport1; port<eo_appCanSP_emscanportnum; port++)
    {
        if(NULL != (arrayofcandata = s_getSkinDataArray((eOcanport_t)port)))
        {   // ok, we have a skin   
            if(eobool_true == eom_emsrunner_CycleHasJustTransmittedRegulars(eom_emsrunner_GetHandle()))
            {   // ok, we can reset the array
                eo_array_Reset(arrayofcandata);
            }            
        }
    }
    
    // now we wait for the can tx to finish    
    res[0] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    res[1] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    
    if((eores_NOK_timeout ==  res[0]) || (eores_NOK_timeout ==  res[1]))
    {
        eo_dgn_emsapplcore.core.runst.cantxfailuretimeoutsemaphore++;
        // marco.accame: for now we dont use this but the error manager inside function eo_appCanSP_wait_XXX()
        //eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);
    }
    
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
        
    
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
    // marco.accame: for now i remove the action of this object and i call only the errormanager. for later we can have both the error handlers
    //eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);

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
    
    // marco.accame: for now i remove the action of this object and i call only the errormanager. for later we can have both the error handlers
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
    //eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , runner_timeout_send_diagnostics);    
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
        
       // ok, now i prepare the value of retvalues[port] but i do it only once
        eOappTheDB_SkinCanLocation_t skincanloc = {0};
        skincanloc.emscanport = (eOcanport_t)port;
        eOsk_skinId_t skid =  255;

        if(eores_OK == eo_appTheDB_GetSkinId_BySkinCanLocation(eo_appTheDB_GetHandle(), skincanloc, &skid)) 
        {
            // ok, i can attempt to retrieve the pointer
            eOsk_status_t * skinstatus = eo_protocolwrapper_GetSkinStatus(eo_protocolwrapper_GetHandle(), skid);
            if(NULL != skinstatus)
            {
                arraysofcandata[port] = (EOarray*)(&skinstatus->arrayofcandata);
            }
        }        
    }
        
    return(arraysofcandata[port]);    
}



EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    
//#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
    return(s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(p, pwmList, size));
//#else	
//    return(s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(p, pwmList,size));
//#endif
}

//#ifndef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
//static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
//{
//    eOresult_t 				err;
//	eOmeas_current_t            value;
//    eOresult_t 				    res = eores_OK;
//    eOicubCanProto_msgCommand_t msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_DISIRED_CURRENT
//    };	
//
//#warning VALE --> solo per test
//    pwmList[0] = 0x11AA;
//    pwmList[1] = 0x12AA;
//    pwmList[2] = 0x13AA;
//    pwmList[3] = 0x14AA;
//    uint16_t numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
//
//    for (uint8_t jid = 0; jid <numofjoint; jid++)
//    {
//        value = pwmList[jid];
//    
//        /*Since in run mode the frame are sent on demnad...here i can punt in tx queue frame to send.
//        they will be sent by transmitter */
//
//        err = eo_appCanSP_SendCmd2Joint(eo_emsapplBody_GetCanServiceHandle(p), jid, msgCmd, (void*)&value);
//        
//        if (err != eores_OK)
//        {
//            res = err;
//        }
//    }
//    
//    return(res);
//   
//}
//#endif

//#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
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


    uint16_t numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
    
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
    eo_appCanSP_SendCmd(eo_emsapplBody_GetCanServiceHandle(p), (eOcanport_t)canLoc.emscanport, dest, msgCmd, (void*)pwm_aux);
    
    return(res);    
}
//#endif

//extern eOresult_t send_diagnostics_to_server(const char *str, uint32_t signature, uint8_t plustime);

static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          							res;
    EOtheEMSapplBody    							*emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint32_t            							encvalue[4]; // = {(uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID};
    hal_encoder_errors_flags					    encflags[4];
    int16_t             							pwm[4];
    EOappEncReader      							*app_enc_reader = eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr);
    uint8_t             							error_mask = 0;

    uint16_t numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    //uint64_t start_read = osal_system_abstime_get();
    //uint64_t enc1_delta = start_read - eo_appEncReader_startSPI1(app_enc_reader);
    //uint64_t stop_read = 0;
    
    uint8_t spi1 = 0, spi3 = 0;
    
    for(uint8_t i=0; i<30; ++i)
    {
        if (eo_appEncReader_isReady(app_enc_reader))
        {
            //stop_read = osal_system_abstime_get();
            break;
        }
        else
        {
            if (!eo_appEncReader_isReadySPI1(app_enc_reader)) ++spi1;
            if (!eo_appEncReader_isReadySPI3(app_enc_reader)) ++spi3;
            hal_sys_delay(5);
        }
    }
    
    /*
    static char msg[31];
    
    if (spi1 || spi3)
    {
        snprintf(msg,sizeof(msg),"ENC TOUT %d %d",spi1,spi3);
        send_diagnostics_to_server(msg, 0xffffffff, 1);        
    }
    */
    
    if (eo_appEncReader_isReady(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr)))
    {    
        for (uint8_t enc = 0; enc < numofjoint; ++enc)
        {
            res = eo_appEncReader_GetValue(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr), (eOappEncReader_encoder_t)enc, &(encvalue[enc]), &(encflags[enc]));
            
            if (res != eores_OK)
            {
                error_mask |= 1<<(enc<<1);
                //encvalue[enc] = (uint32_t)ENC_INVALID;
            }
        }        
    }
    else
    {
        error_mask = 0xAA; // timeout
    }

    // Restart the reading of the encoders
    eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReaderHandle(eo_emsapplBody_GetHandle()));
		
    eo_emsController_AcquireAbsEncoders((int32_t*)encvalue, error_mask);
        
    eo_emsController_CheckFaults();
    
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
    eOmc_joint_status_t             *jstatus = NULL;
    eOmc_motor_status_t             *mstatus = NULL;
    eOmc_jointId_t                  jId;
    uint16_t                        numofjoint;
    EOappTheDB                      *db = eo_appTheDB_GetHandle();
    uint16_t                        numofmotors;
    uint8_t                         transmit_decoupled_pwms = 1;
    
    numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(db);
    for(jId = 0; jId<numofjoint; jId++)
    {
        jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
        if(NULL == jstatus)
        {
            return; //error
        }
        
        //eo_emsController_GetJointStatus(jId, &jstatus_ptr->basic);
        eo_emsController_GetJointStatus(jId, jstatus);
        
        eo_emsController_GetActivePidStatus(jId, &jstatus->ofpid);
        
        if (transmit_decoupled_pwms) 
        {  
            //this functions is used to get the motor PWM after the decoupling matrix
            eo_emsController_GetPWMOutput(jId, &jstatus->ofpid.output);
        }
        
        if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus->basic.motionmonitorstatus)
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
                jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
            }
        }
        
        
    }
    
    
    
    numofmotors =  eo_appTheDB_GetNumberOfConnectedMotors(db);
    for(jId = 0; jId<numofmotors; jId++)
    {
        mstatus = eo_protocolwrapper_GetMotorStatus(eo_protocolwrapper_GetHandle(), jId);
        if(NULL == mstatus)
        {
            return; //error
        }
        
        eo_emsController_GetMotorStatus(jId, mstatus);
    }
}

static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    eOresult_t                              res;
    uint8_t                                 send_virtualStrainData;
    uint16_t                                numofjoint;
    eOmc_joint_status_t                     *jstatus = NULL;
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
    
    numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(db_ptr);

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), motionDoneJoin2Use);
    if(NULL == jstatus)
    {
        return; //error
    }
    
    if (jstatus->basic.controlmodestatus == eomc_controlmode_position
     || jstatus->basic.controlmodestatus == eomc_controlmode_mixed
     || jstatus->basic.controlmodestatus == eomc_controlmode_calib)
    {
       if(jstatus->basic.motionmonitorstatus == eomc_motionmonitorstatus_setpointnotreachedyet)
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
    }
    
    motionDoneJoin2Use++;
    if(motionDoneJoin2Use == numofjoint)
    {
        motionDoneJoin2Use = 0;
    }
    
 

    /*prepare virtual strain data:
      i send virtual strain data to mc4 if:
        - pc104 send me new values
        - or if have been not passede more then COUNT_BETWEEN_TWO_UPDATES_MAx millisec from last received value.
      More over, i send message every 10 millisec
    */
    send_virtualStrainData = 0;
    if(eo_appTheDB_IsVirtualStrainDataUpdated(db_ptr))
    {
        send_virtualStrainData = 1;
        count_watchdog_virtaulStrain = 0;
        count_between_two_updates = 0;
        eo_appTheDB_ClearVirtualStrainDataUpdatedFlag(db_ptr);
    }
    else
    {
        count_between_two_updates ++;
        count_watchdog_virtaulStrain++;
        
        if(count_between_two_updates <= COUNT_BETWEEN_TWO_UPDATES_MAx)
        {
            if(count_watchdog_virtaulStrain == COUNT_WATCHDOG_VIRTUALSTRAIN_MAX)
            {
                send_virtualStrainData = 1;
                count_watchdog_virtaulStrain = 0;
            }
            else
            {
                send_virtualStrainData = 0;
            }
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

        //#warning marco.accame: ecco qui il messaggio verso il can address 12 ...........................
        //set destination of all messages 
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, 12); // virtual ft sensor has address 12
        
        //set command (calss + id) and send it
        msgCmd.class = icubCanProto_msgCmdClass_periodicAnalogSensor;
        msgCmd.cmdId = ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR;
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&virtStrain_ptr[0]);
        
        msgCmd.cmdId = ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR;
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&virtStrain_ptr[3]);
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


// marco.accame: commented it out on nov 26 2014 because it is not used and the compiler complains
//static void s_eom_emsrunner_hid_read_can_messages(eOcanport_t port, eObool_t all, uint8_t max)
//{
//    EOappCanSP*  cansp = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    uint8_t      numofRXcanframe = 0;
//    eOresult_t   res;
//    
//    if(all)
//    {
//        res = eo_appCanSP_GetNumOfRecCanframe(cansp, port, &numofRXcanframe);
//        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "err in GetNumOfRecCanframe", "EOMtheEMSrunner", &eo_errman_DescrTobedecided);
//    }
//    else
//    {
//        numofRXcanframe = max;
//    }
//    
//    #ifdef _GET_CANQUEUE_STATISTICS_
//        eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(port, numofRXcanframe);
//    #endif
//    eo_appCanSP_read(cansp, port, numofRXcanframe, NULL); 
//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




