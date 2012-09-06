
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
#include "EOappTheServicesProvider.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EoManagement.h"
#include "eOcfg_nvsEP_sk.h"   //==> included to clear skin array
#include "eOcfg_nvsEP_as.h"   //==> included to clear mais array
#include "eOcfg_nvsEP_mc.h"   //==>inlcluded to get joint's nvindex





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
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOappTheSP *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOappTheSP *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOappTheSP *p);

//DO
static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p);


//utils
static void s_eom_emsrunner_hid_readSkin(EOappTheSP *p);
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOappTheSP *p, int16_t *pwmList, uint8_t size);
static void s_eom_emsrunner_hid_readMc4andMais(EOappTheSP *p);
static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_UpdateMotorstatus(EOMtheEMSrunner *p);


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
    eOmn_appl_runMode_t runmode =  eo_appTheSP_GetAppRunMode(eo_appTheSP_GetHandle());
    if(applrunMode__2foc == runmode)
    {
         eo_appEncReader_StartRead(eo_appTheSP_GetEncoderReaderHandle(eo_appTheSP_GetHandle()));
    }    
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    EOappTheSP* appTheSP = eo_appTheSP_GetHandle();
    eOmn_appl_runMode_t runmode =  eo_appTheSP_GetAppRunMode(appTheSP);

    switch(runmode)
    {
        case applrunMode__skinOnly:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(appTheSP);
        }break;
        
        case applrunMode__2foc:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(appTheSP);
        }break;
        
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(appTheSP);
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
    EOappTheSP* appTheSP = eo_appTheSP_GetHandle();
    eOmn_appl_runMode_t runmode =  eo_appTheSP_GetAppRunMode(appTheSP);

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

    EOappTheSP* appTheSP = eo_appTheSP_GetHandle();
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

    eo_appCanSP_SendMessage_TEST(eo_appTheSP_GetCanServiceHandle(appTheSP), NULL, payload);    
        
#else        
        
    res = eo_appCanSP_StartTransmitCanFrames(eo_appTheSP_GetCanServiceHandle(appTheSP), eOcanport1);
    if(eores_OK != res)
    {
        return;
    }        

    res = eo_appCanSP_StartTransmitCanFrames(eo_appTheSP_GetCanServiceHandle(appTheSP), eOcanport2);
    if(eores_OK != res)
    {
        return;
    }        
#endif        


}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    uint8_t a =1;
    EOappTheSP* appTheSP = eo_appTheSP_GetHandle();
    eo_appCanSP_WaitTransmitCanFrames(eo_appTheSP_GetCanServiceHandle(appTheSP), eOcanport1);
    eo_appCanSP_WaitTransmitCanFrames(eo_appTheSP_GetCanServiceHandle(appTheSP), eOcanport2);
    a =a;
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOappTheSP *p)
{
    s_eom_emsrunner_hid_readSkin(p); 
}

static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOappTheSP *p)
{
	/* TAG_ALE*/
    eo_appCanSP_read(eo_appTheSP_GetCanServiceHandle(p), eOcanport1, 4, NULL); 
}
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOappTheSP *p)
{
        
    /* 1) get can frames about skin and process them*/
    s_eom_emsrunner_hid_readSkin(p);
    
    /* 2) get can frame from mais and mc4 board and process them */
    s_eom_emsrunner_hid_readMc4andMais(p);
    
    //something else about mc4 ??????

}

static void s_eom_emsrunner_hid_readSkin(EOappTheSP *p)
{
    void *memRef;
    EOarray *sk_array;
    eOresult_t res;
    
    //1) reset nv array
    res = eo_appTheNVmapRef_GetSkinNVMemoryRef(eo_appTheSP_GetTheNVmapRefHandle(p), 0, skinNVindex_sstatus__arrayof10canframe, &memRef);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinNVMemoryRef");

    sk_array = (EOarray*)memRef;
    eo_array_Reset(sk_array);

    eo_appCanSP_read(eo_appTheSP_GetCanServiceHandle(p), eOcanport2, 10, NULL); //10 is the max size of sk_array

}

static void s_eom_emsrunner_hid_readMc4andMais(EOappTheSP *p)
{
    void *memRef;
    eOsnsr_arrayofupto36bytes_t *maisArray;
    eOresult_t res;
    
    //1) reset nv array
    res = eo_appTheNVmapRef_GetSensorsMaisNVMemoryRef(eo_appTheSP_GetTheNVmapRefHandle(p), 0, maisNVindex_mstatus__the15values, &memRef);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSensorsMaisNVMemoryRef");

    //reset array
    maisArray = (eOsnsr_arrayofupto36bytes_t*)memRef;
    maisArray->head.size = 0;
    maisArray->data[0] = 0;

    eo_appCanSP_read(eo_appTheSP_GetCanServiceHandle(p), eOcanport1, 2, NULL); //2...boh!!!

}



static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOappTheSP *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t res;
    eo_icubCanProto_msgCommand_t msgCmd = 
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
    res = eo_appCanSP_SendCmd2Joint(eo_appTheSP_GetCanServiceHandle(p), 3/*jid*/, msgCmd, (void*)&mySetPoint_current);
    return(res);
}



static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          res;
    EOappTheSP          *appTheSP = eo_appTheSP_GetHandle();
    uint32_t            encvalue;
    int16_t             *pwm;

    if (eo_appEncReader_isReady(eo_appTheSP_GetEncoderReaderHandle(appTheSP)))
    {     
        res = eo_appEncReader_GetValue(eo_appTheSP_GetEncoderReaderHandle(appTheSP), eOeOappEncReader_encoder3, &encvalue);
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
        s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
#endif   
#warning VALE--> for ALE: put in s_eom_emsrunner_hid_UpdateJointstatus update status of joint.    

    s_eom_emsrunner_hid_UpdateJointstatus(p);
    s_eom_emsrunner_hid_UpdateMotorstatus(p);
}


static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p)
{
    eOmc_joint_status_ofpid_t       *jstatus_ofpid_ptr = NULL;
    eOmc_joint_status_basic_t       *jstatus_basic_ptr = NULL;
    eOmc_jointId_t                  jId;
    void                            *nv_mem_ptr;
    eOresult_t                      res;
    
    for(jId = 0; jId<jointNumberMAX; jId++)
    {    
        res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__basic, &nv_mem_ptr);
        if(eores_OK != res)
        {
            if(eores_NOK_nodata == res)
            {
                continue; //this jId is not manage by this board
            }
            else
            {
                return; //error
            }
        }
        jstatus_basic_ptr = jstatus_basic_ptr; //delete this when set values to jstatus. Put here to remove warning.
//         jstatus_basic_ptr = (eOmc_joint_status_basic_t*)nv_mem_ptr;
//         jstatus_basic_ptr->position =  
//         jstatus_basic_ptr->velocity =  
//         jstatus_basic_ptr->acceleration =  
//         jstatus_basic_ptr->torque =  
//         jstatus_basic_ptr->motionmonitorstatus =  
        
        
        res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__ofpid, &nv_mem_ptr);
        if(eores_OK != res)
        {
            if(eores_NOK_nodata == res)
            {
                continue; //this jId is not manage by this board
            }
            else
            {
                return; //error
            }
        }
        jstatus_ofpid_ptr = jstatus_ofpid_ptr;  //delete this when set values to jstatus. Put here to remove warning.
//         jstatus_ofpid_ptr = (eOmc_joint_status_ofpid_t*)nv_mem_ptr;
//         jstatus_ofpid_ptr->reference = 
//         jstatus_ofpid_ptr->error = 
//         jstatus_ofpid_ptr->output = 
        

    }
}



static void s_eom_emsrunner_hid_UpdateMotorstatus(EOMtheEMSrunner *p)
{
    eOmc_motor_status_basic_t       *mstatus_basic_ptr = NULL;
    eOmc_motorId_t                  mId;
    void                            *nv_mem_ptr;
    eOresult_t                      res;
    
    for(mId = 0; mId<motorNumberMAX; mId++)
    {    
        res = eo_appTheNVmapRef_GetMotorNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mstatus__basic, &nv_mem_ptr);
        if(eores_OK != res)
        {
            if(eores_NOK_nodata == res)
            {
                continue; //this jId is not manage by this board
            }
            else
            {
                return; //error
            }
        }
        mstatus_basic_ptr = mstatus_basic_ptr; //delete this when set values to jstatus. Put here to remove warning.
//         mstatus_basic_ptr = (mstatus_basic_ptr*)nv_mem_ptr;
//         mstatus_basic_ptr->position =  
//         mstatus_basic_ptr->velocity =  
//         mstatus_basic_ptr->current =  
    }



}

static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eOmc_jointId_t                  jId;
    void                            *nv_mem_ptr;
    eOmc_joint_status_basic_t       *jstatusbasic_ptr;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__MOTION_DONE
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    
    for(jId = 0; jId<jointNumberMAX; jId++)
    {    
        res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__basic, &nv_mem_ptr);
        if(eores_OK != res)
        {
            if(eores_NOK_nodata == res)
            {
                continue; //this jId is not manage by this board
            }
            else
            {
                return; //error
            }
        }

        jstatusbasic_ptr = (eOmc_joint_status_basic_t*)nv_mem_ptr;
        
        if(jstatusbasic_ptr->motionmonitorstatus != eomc_motionmonitorstatus_setpointnotreachedyet)
        {
            /* if motionmonitorstatus is different from _setpointnotreachedyet, 
            that is it is _notmonitored or _setpointisreached, 
            i don't need to send motion done message, so return. */
            continue;
        }

        res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jId, &canLoc, NULL);
        if(eores_OK != res)
        {
            return; //I should never be here!!!
        }
        eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, NULL);
    }   
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




