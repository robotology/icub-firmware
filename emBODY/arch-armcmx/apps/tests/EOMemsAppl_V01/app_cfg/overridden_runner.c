
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
#include "EoSensors.h"
#include "EoManagement.h"
// #include "eOcfg_nvsEP_sk.h"   //==> included to clear skin array
// #include "eOcfg_nvsEP_as.h"   //==> included to clear mais array
// #include "eOcfg_nvsEP_mc.h"   //==>inlcluded to get joint's nvindex





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
//da usare finche' si usa proto test
int16_t pwm_out = 0;
int32_t encoder_can = 0;
int32_t posref_can = 0;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//RX
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOtheEMSapplBody *p);

//DO
static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p);


//utils
static void s_eom_emsrunner_hid_readSkin(EOtheEMSapplBody *p);
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_eom_emsrunner_hid_readMc4andMais(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
/* TAG_ALE*/


extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    if(applrunMode__2foc == runmode)
    {
         eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReaderHandle(eo_emsapplBody_GetHandle()));
    }    
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode  =  eo_emsapplBody_GetAppRunMode(emsappbody_ptr);

    switch(runmode)
    {
        case applrunMode__skinOnly:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__2foc:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__mc4Only:
        {
            ;
        }break;

        default:
        {
            ;
        };
    };
    
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
    eOresult_t res;
    
#ifdef _USE_PROTO_TEST_
    eOmc_setpoint_t     mySetPoint_current = 
    {
        EO_INIT(.type)       eomc_setpoint_current,
        EO_INIT(.to)
        {
            EO_INIT(.current)
            {
                EO_INIT(.value)     0
            }   
        }
    };
#endif
   
//following activities are independent on runmode
    

#ifdef _USE_PROTO_TEST_
    mySetPoint_current.to.current.value = pwm_out;
    eo_appCanSP_SendSetPoint(p->cfg.appCanSP_ptr, 3, &mySetPoint_current);  
    ((int32_t*)payload)[0]=encoder_can;
    ((int32_t*)payload)[1]=posref_can;

    eo_appCanSP_SendMessage_TEST(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), payload);    
        
#else        
        
    res = eo_appCanSP_StartTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    if(eores_OK != res)
    {
        return;
    }        

    res = eo_appCanSP_StartTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    if(eores_OK != res)
    {
        return;
    }        
#endif        


}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    uint8_t a =1;
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eo_appCanSP_WaitTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    eo_appCanSP_WaitTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    a =a;
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOtheEMSapplBody *p)
{
    s_eom_emsrunner_hid_readSkin(p); 
}

static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOtheEMSapplBody *p)
{
	/* TAG_ALE*/
    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, 4, NULL); 
}
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOtheEMSapplBody *p)
{
        
    /* 1) get can frames about skin and process them*/
    s_eom_emsrunner_hid_readSkin(p);
    
    /* 2) get can frame from mais and mc4 board and process them */
    s_eom_emsrunner_hid_readMc4andMais(p);
    
    //something else about mc4 ??????

}

static void s_eom_emsrunner_hid_readSkin(EOtheEMSapplBody *p)
{
    eOsk_skinId_t     sId = 0;//only one skin. (evenif skin is composed by more skin board, here we see the skin like a unique identity)
//    eOskin_status_t     *skstatus_ptr;
    EOarray_of_10canframes *arrayof10canframes_ptr = NULL;
    eOresult_t res;

//uncomment this code when status and config nvvar are used.   
//     res = eo_appTheDB_GetSkinStatusPtr(eo_appTheDB_GetHandle(), sId,  &skstatus_ptr);
//     eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinNVMemoryRef");

//     eo_array_Reset((EOarray*)(&skstatus_ptr->arrayof10canframes));

    
    //1) get pointer to nv array and reset it

    res = eo_appTheDB_GetSkinStArray10CanFramesPtr(eo_appTheDB_GetHandle(), sId,  &arrayof10canframes_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinStArray10CanFramesPtr");
    
    eo_array_Reset((EOarray*)arrayof10canframes_ptr);
    
    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport2, 10, NULL); //10 is the max size of sk_array

}

static void s_eom_emsrunner_hid_readMc4andMais(EOtheEMSapplBody *p)
{
    eOsnsr_maisId_t                 sId = 0; //only one mais per ems
    eOsnsr_mais_status_t            *sstatus_ptr;
    eOresult_t                      res;
    
    //1) reset nv array
    res = eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSnrMaisStatusPtr");

    //reset array
    sstatus_ptr->the15values.head.size = 0;
    sstatus_ptr->the15values.data[0] = 0;

    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, 2, NULL); //2...boh!!!

}



static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t res;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__SET_DISIRED_CURRENT
    };
    eOmc_setpoint_t     mySetPoint_current = 
    {
        EO_INIT(.type)       eomc_setpoint_current,
        EO_INIT(.to)
        {
            EO_INIT(.current)
            {
                EO_INIT(.value)     0
            }   
        }
    };
#warning VALE --> put here for cicle to send all setpoints
    mySetPoint_current.to.current.value = pwmList[0];
    
    /*Since in run mode the frame are sent on demnad...here i can punt in tx queue frame to send.
    they will be sent by transmitter */
    res = eo_appCanSP_SendCmd2Joint(eo_emsapplBody_GetCanServiceHandle(p), 3/*jid*/, msgCmd, (void*)&mySetPoint_current);
    return(res);
}



static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          res;
    EOtheEMSapplBody    *emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint32_t            encvalue;
    int16_t             *pwm;

    if (eo_appEncReader_isReady(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr)))
    {     
        res = eo_appEncReader_GetValue(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr), eOeOappEncReader_encoder3, &encvalue);
    }

    if(eores_OK != res)
    {
        eo_emsController_SkipEncoders();
    }
    else
    {
        eo_emsController_ReadEncoders((int32_t*)&encvalue);
    }
        
    /* 2) pid calc */
    pwm = eo_emsController_PWM();
     
    pwm_out = pwm[3];
        
#ifndef _USE_PROTO_TEST_
        /* 4) prepare and punt in rx queue new setpoint */
        s_eom_emsrunner_hid_SetCurrentsetpoint(emsappbody_ptr, pwm, 0);
#endif   

    s_eom_emsrunner_hid_UpdateJointstatus(p);
    /*Note: motor status is updated with data sent by 2foc by can */
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
        
        eo_emsController_GetJointStatus(jId, &jstatus_ptr->basic);
        
        eo_emsController_GetActivePidStatus(jId, &jstatus_ptr->ofpid); 
        
        #error VALE--> aggiungi qui getmotionDone
    }
}

static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    eOresult_t                      res;
    uint16_t                        numofjoint;
    eOmc_jointId_t                  jId;
    eOmc_joint_status_t             *jstatus_ptr;
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__MOTION_DONE
    };
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for(jId = 0; jId<numofjoint; jId++)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //error
        }
        
        if(jstatus_ptr->basic.motionmonitorstatus != eomc_motionmonitorstatus_setpointnotreachedyet)
        {
            /* if motionmonitorstatus is different from _setpointnotreachedyet, 
            that is it is _notmonitored or _setpointisreached, 
            i don't need to send motion done message, so return. */
            continue;
        }

        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, jId, msgCmd, NULL);

    }   
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




