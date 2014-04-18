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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/*  @file       EOtheEMSApplBody.c
    @brief      This file implements the body of ems application
    @author     valentina.gaggero@iit.it
    @date       09/19/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"

//hal
#include "hal.h"

#include "eOcommon.h"
#include "EOtheErrorManager.h"
#include "EOtheBOARDtransceiver_hid.h" //==>in order to get nvcfg

//icub
#include "EoMotionControl.h"
#include "EoSkin.h"
#include "EoManagement.h"
#include "eOicubCanProto_specifications.h"

//nv-cfg


//application
#include "EOMtheEMSapplCfg.h"
#include "eOcfg_appTheDataBase.h"

#include "EOaction_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOtheEMSapplBody.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOtheEMSapplBody_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define CANBOARDSRAEDY_CHECK_PERIOD         10000
#define CANBOARDSRAEDY_CHECK_TIMEOUT_EVT    emsconfigurator_evt_userdef




#include "EOMtheEMSapplCfg_cfg.h"   // to see the macros



#if     defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_false
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3) || defined(EOMTHEEMSAPPLCFG_USE_EB5) || defined(EOMTHEEMSAPPLCFG_USE_EB6) || defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB8) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_true
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB10) || defined(EOMTHEEMSAPPLCFG_USE_EB11)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_false
#else
    #error --> you must define an EBx
#endif



//encoders configuration
#define ENC_ENA     1   /* If encoder is connected*/
#define ENC_DISA    0   /* If encoder is NOT connected*/
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3)  || defined(EOMTHEEMSAPPLCFG_USE_EB6)  || defined(EOMTHEEMSAPPLCFG_USE_EB8)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder2)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder3)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder2) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder2)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4) || defined(EOMTHEEMSAPPLCFG_USE_EB10) || defined(EOMTHEEMSAPPLCFG_USE_EB11)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_DISA << eOeOappEncReader_encoder0) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder1) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder2) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)    \
                                                    )

#else
    #error --> you must define an EBx
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

extern const eOtheEMSapplBody_cfg_t eOtheEMSappBody_cfg_default = 
{
    .hasdevice                      =
    {
        EOMTHEEMSAPPLCFG_EBX_hasSKIN, EOMTHEEMSAPPLCFG_EBX_hasMC4, EOMTHEEMSAPPLCFG_EBX_has2FOC
    },
    .icubcanprotoimplementedversion =
    {
        .major                  = 1,
        .minor                  = 1
    },
    .connectedEncodersMask      = EOMTHEEMSAPPLCFG_EBX_encodersMASK,
//     .endpoints                  =
//     {
//         .mc_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_mc,   
//         .as_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_as,
//         .sk_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_sk,
//     },
    .configdataofMC4boards      =
    {
        .shiftvalues            =
        {
            .jointVelocityShift     =  8,
            .jointVelocityEstimationShift = 8,
            .jointAccelerationEstimationShift = 5
        },
        .bcastpolicy            =
        {
            .val2bcastList      =
            {
            /* 0 */ ICUBCANPROTO_PER_MC_MSG__POSITION,
            /* 1 */ ICUBCANPROTO_PER_MC_MSG__STATUS,
            /* 2 */ ICUBCANPROTO_PER_MC_MSG__PRINT,
            /* 3 */ 0
            }
        }
    }    
    
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_emsapplBody_objs_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_theDataBase_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_canServicesProvider_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_measuresConverter_init(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_getRunMode(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_leds_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_checkConfig(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_EnableTxMais(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_DisableTxMais(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_SendTxMode2Strain(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_DisableTxStrain(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t canBoardsMask);
static eOresult_t s_eo_emsapplBody_startCheckCanboards(EOtheEMSapplBody *p);

//static eOresult_t test_4_strain(EOtheEMSapplBody *p);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheEMSapplBody s_applBody = 
{
    EO_INIT(.st)    eo_emsApplBody_st__NOTinited
};


static const char s_eobj_ownname[] = "EOtheEMSapplBody";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOtheEMSapplBody_cfg_t *cfg)
{
    eOresult_t res;
    EOtheEMSapplBody *retptr = NULL;

    if(NULL == cfg)
    {
        cfg = &eOtheEMSappBody_cfg_default;
    }
    
    retptr = &s_applBody;

    //save in obj its configuration
    retptr->cfg_ptr = cfg;
    
    //retptr->appRunMode = applrunMode__default;
    res = s_eo_emsapplBody_getRunMode(retptr); //the run mode is depending on connected can board (mc4, 2foc, only skin, etc)
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in getting run mode");
    
    retptr->canBoardsReady_timer = eo_timer_New();
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != retptr->canBoardsReady_timer), s_eobj_ownname, "error in creating canBoardsReady_timer");
    
    s_eo_emsapplBody_leds_init(retptr);
    
    s_eo_emsapplBody_objs_init(retptr); //if a obj init doesn't success, it calls errorManager with fatal error
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, "obj-body inited OK");

    s_eo_emsapplBody_checkConfig(retptr); //check config: if somethig is wrong then go to error state.
    
    res = s_eo_emsapplBody_startCheckCanboards(retptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in startCheckCanboards");
    
    retptr->st = eo_emsApplBody_st__inited;
        
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "body appl init OK");
    return(retptr);
}


extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void)
{
    return((s_applBody.st == eo_emsApplBody_st__inited) ? (&s_applBody) : (NULL));
}

extern const eOtheEMSapplBody_cfg_t* eo_emsapplBody_GetConfig(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->cfg_ptr);
}

extern EOappTheDB* eo_emsapplBody_GetDataBaseHandle(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->bodyobjs.appDB);
}


extern EOappCanSP* eo_emsapplBody_GetCanServiceHandle(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->bodyobjs.appCanSP);
}

extern EOappEncReader* eo_emsapplBody_GetEncoderReaderHandle(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->bodyobjs.appEncReader);
}


// extern EOemsController* eo_emsapplBody_GetEmsControllerHandle(EOtheEMSapplBody *p)
// {
//     if(NULL == p)
//     {
//         return(NULL);
//     }
//     return(p->bodyobjs.emsController);
// }

extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(applrunMode__default);
    }
    return(p->appRunMode);
}


extern EOappMeasConv* eo_emsapplBody_GetMeasuresConverterHandle(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->bodyobjs.appMeasConv);

}



extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p)
{
    eOresult_t                          res;
    uint16_t                            numofjoint, i;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
    };

    if(applrunMode__2foc == p->appRunMode)
    {
        res = s_eo_emsapplBody_SendTxMode2Strain(p);
        return(res);
    }
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only != p->appRunMode))
    {
    
    // 2) enable tx on mais
    res = s_eo_emsapplBody_EnableTxMais(p);
    if(eores_OK != res)
    {
        return(res);
    }

    // 3) config joints
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for(i=0; (i<numofjoint) && (eores_OK == res); i++)
    {
        //get bcast policy from db
        res = eo_appTheDB_GetJointBcastpolicyPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)i, &bcastpolicy_ptr);
        if(eores_OK != res)
        {
            return(res);
        }

        res = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, (void*)&(bcastpolicy_ptr->val2bcastList[0]));
    }
    
    /* per ora non si e' verificato nessun problema se l'applicazione e' in cfg e la pelle spedisce a manetta*/
    return(res);
    }
    else
    {
        return(eores_OK);
    }
}


extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p)
{
    eOresult_t                          res;
    uint16_t                            numofjoint, i;
    eOicubCanProto_bcastpolicy_t        bcastpolicy =
    {   
        EO_INIT(.val2bcastList)         {0, 0, 0, 0}
    };
    
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
    };

    if(applrunMode__2foc == p->appRunMode)
    {
        eo_emsMotorController_GoIdle();
        res = s_eo_emsapplBody_DisableTxStrain(p);
        return(res);
    }
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {    
        // 2) config joints
        numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
        
        for(i=0; (i<numofjoint) && (eores_OK == res); i++)
        {
            res = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, (void*)&(bcastpolicy.val2bcastList[0]));
        }
        #warning disabilita la pelle come si fa?
        
        // 3) disable tx on mais
        res = s_eo_emsapplBody_DisableTxMais(p);
        
        return(res);
    }
    else
    {
        return(eores_OK);
    }

}


extern eObool_t eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->cfg_ptr->hasdevice[dev]);
}

extern eOresult_t eo_emsapplBody_checkCanBoardsAreReady(EOtheEMSapplBody *p, uint32_t canBoardsMask)
{
    return(s_eo_emsapplBody_sendGetFWVersion(p, canBoardsMask));
}

extern eOresult_t eo_emsapplBody_setCanBoardsAreReady(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_OK);
    }
    return(eo_timer_Stop(p->canBoardsReady_timer));
}

extern eOresult_t eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
{
    static eObool_t already_send = eobool_false;
    eOresult_t res;
    
    if(already_send)
    {
        return(eores_OK);
    }
    
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    res = s_eo_emsapplBody_sendConfig2canboards(p);
    if(eores_OK == res)
    {
        already_send = eobool_true;
    }
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
extern void eo_emsapplBody_hid_canSP_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
 static void s_eo_emsapplBody_leds_init(EOtheEMSapplBody *p)
 {
    hal_led_cfg_t cfg = {.dummy=0};

// //    p->ledtimer = eo_timer_New();
    
    hal_led_init(hal_led0, &cfg);
    hal_led_off(hal_led0);
    hal_led_init(hal_led1, &cfg); //led green
    hal_led_off(hal_led1);
    hal_led_init(hal_led2, &cfg);
    hal_led_off(hal_led2);
    hal_led_init(hal_led3, &cfg);
    hal_led_off(hal_led3);

 }

static void s_eo_emsapplBody_objs_init(EOtheEMSapplBody *p)
{

/* NOTE: the initialization order is important. Pay attaention to change it! */
    
   
/* 1) init data base */
    s_eo_emsapplBody_theDataBase_init(p);
    
/* 2) init can module */
    s_eo_emsapplBody_canServicesProvider_init(p);

/* 3) init encoder reader */
    s_eo_emsapplBody_encodersReader_init(p);
        
/* 4) init emsController */
    s_eo_emsapplBody_emsController_init(p);

/* 5) init measures converter */
    s_eo_emsapplBody_measuresConverter_init(p);

}

static void s_eo_emsapplBody_theDataBase_init(EOtheEMSapplBody *p)
{
    eOresult_t                          res;
    uint8_t                             i;
    eOappTheDB_jointShiftValues_t       *shiftval_ptr;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
    
    eOappTheDB_cfg_t cfg = 
    {
        //EO_INIT(.nvsCfg)            eo_boardtransceiver_hid_GetNvsCfg(),
//         EO_INIT(.mc_endpoint)       p->cfg_ptr->endpoints.mc_endpoint,                 
//         EO_INIT(.as_endpoint)       p->cfg_ptr->endpoints.as_endpoint,   
//         EO_INIT(.sk_endpoint)       p->cfg_ptr->endpoints.sk_endpoint,   
        EO_INIT(.canboardsList)     eo_cfg_appDB_constvec_boards__ptr,
        EO_INIT(.jointsList)        eo_cfg_appDB_constvec_joints__ptr,
        EO_INIT(.motorsList)        eo_cfg_appDB_constvec_motors__ptr,
        EO_INIT(.snsrMaisList)      eo_cfg_appDB_constvec_snsrMais__ptr,
        EO_INIT(.snsrStrainList)    eo_cfg_appDB_constvec_snsrStrain__ptr,
        EO_INIT(.skinList)          eo_cfg_appDB_constvec_skin__ptr
    };
    
//     eOappTheDB_jointShiftValues_t shiftval = 
//     {
//         EO_INIT(.jointVelocityShift)            p->cfg_ptr->jointVelocityShift,
//         EO_INIT(.jointVelocityEstimationShift)  p->cfg_ptr->jointVelocityEstimationShift
//     };
//     
//     eOicubCanProto_bcastpolicy_t  bcastpolicy = 
//     {
//         EO_INIT(.val2bcastList)            =
//         {
//             /* 0 */   ICUBCANPROTO_PER_MC_CMD_POSITION,
//             /* 1 */   ICUBCANPROTO_PER_MC_CMD_PID_VAL,
//             /* 2 */   ICUBCANPROTO_PER_MC_CMD_PID_ERROR,
//             /* 3 */   ICUBCANPROTO_PER_MC_CMD_VELOCITY
//         }
//     };

    p->bodyobjs.appDB =  eo_appTheDB_Initialise(&cfg);
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in appTheDB_Initialise");
    
//     res = eo_appTheDB_SetjointsShiftValues(EOappTheDB *p, &shiftval);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");


//     res = eo_appTheDB_SetJointsBcastpolicyPtr(EOappTheDB *p, &bcastpolicy);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");

    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        res = eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), 0, &shiftval_ptr);
        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in _GetShiftValuesOfJointPtr");
        
        //here i don't use memcpy because the two struct have different type
        shiftval_ptr->jointVelocityShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointVelocityShift;
        shiftval_ptr->jointVelocityEstimationShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointVelocityEstimationShift;
        shiftval_ptr->jointAccelerationEstimationShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointAccelerationEstimationShift;  
        
        res = eo_appTheDB_GetJointBcastpolicyPtr(eo_appTheDB_GetHandle(), 0, &bcastpolicy_ptr);
        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in _GetJointBcastpolicyPtr");
        
        //set bacast value as mask
        memset(bcastpolicy_ptr, 0, sizeof(eOicubCanProto_bcastpolicy_t)); //reset
        for(i=0; i<eOicubCanProto_bcastpolicy_maxsize; i++)
        {
            if(0 == p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList[i])
            {
                continue;
            }
            if(p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList[i]<9)
            {
                bcastpolicy_ptr->val2bcastList[0] |= (1 <<(p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
            }
            else if(p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList[i]<17)
            {
                bcastpolicy_ptr->val2bcastList[1] |= (1<<(p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
            }
        }
    }
}

static void s_eo_emsapplBody_canServicesProvider_init(EOtheEMSapplBody *p)
{
    eOappCanSP_cfg_t cfg = 
    {
        .cbkonrx   = 
        {
            // port1
            {
                .fn = eo_emsapplBody_hid_canSP_cbkonrx,
                .argoffn = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle())
            },
            // port2
            {
                .fn = NULL, //eo_emsapplBody_hid_canSP_cbkonrx,
                .argoffn = NULL, //eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle())
            },
        }
    };

    if(!(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin)))
    {
        cfg.cbkonrx[eOcanport2].fn = eo_emsapplBody_hid_canSP_cbkonrx;
        cfg.cbkonrx[eOcanport2].argoffn = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
    }
    p->bodyobjs.appCanSP = eo_appCanSP_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appCanSP), 
                     s_eobj_ownname, "error in appCanSP_New");
}


static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p)
{

    eOappEncReader_cfg_t  cfg = 
    {
        EO_INIT(.connectedEncodersMask)     p->cfg_ptr->connectedEncodersMask,
        EO_INIT(.callbackOnLastRead)        NULL
    };

   p->bodyobjs.appEncReader = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appEncReader), 
                     s_eobj_ownname, "error in appEncReader_New");

}



static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p)
{
    //uint16_t i, numofjoint = 0;
    
    p->bodyobjs.emsController = eo_emsController_Init();

//NOTE: removed check because eo_emsController_Init returns NULL if any 2foc board is connected to ems. (i.e, eb2, eb4, eb10, eb11)
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.emsController), 
//                      s_eobj_ownname, "error in emsController_init");
    
    /*
    numofjoint =  eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    for(i=0; i<numofjoint; i++)
    {
        eo_emsController_AddAxis(i);
    }
    */
}


static void s_eo_emsapplBody_measuresConverter_init(EOtheEMSapplBody *p)
{
    eOappMeasConv_cfg_t cfg = {0};

    if((applrunMode__skinAndMc4 != p->appRunMode) && (applrunMode__mc4Only != p->appRunMode))
    {
        return;
    }
    
    p->bodyobjs.appMeasConv = eo_appMeasConv_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appMeasConv), 
                     s_eobj_ownname, "error in appMeasConv_New");

}



static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t canBoardsMask)
{
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION
    };

    eOicubCanProto_msgDestination_t     msgdest;
        //EOMtheEMSapplCfg                    *emsapplCfg_ptr = eom_emsapplcfg_GetHandle();
    uint16_t                            numofboard = eo_appTheDB_GetNumeberOfCanboards(eo_appTheDB_GetHandle());
    uint16_t                            i;
    eOresult_t                          res;
    eOappTheDB_cfg_canBoardInfo_t       *cfg_canbrd_ptr;
    EOappTheDB                          *db= eo_appTheDB_GetHandle();
    
    for(i=0; i<numofboard; i++)
    {
        
        if( ((1<<i) & canBoardsMask) == (1<<i))
        {
            continue;
        }
        
        res = eo_appTheDB_GetCanBoardCfg(db, (eObrd_boardId_t)i, &cfg_canbrd_ptr);
        if(eores_OK != res)
        {
            continue;
        }
        
        if((eobrd_mc4 != cfg_canbrd_ptr->type) && (eobrd_1foc != cfg_canbrd_ptr->type))
        {
            continue;
        }
        
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, cfg_canbrd_ptr->canLoc.addr);
        
        res = eo_appCanSP_SendCmd(p->bodyobjs.appCanSP, cfg_canbrd_ptr->canLoc.emscanport, msgdest, msgCmd, (void*)&p->cfg_ptr->icubcanprotoimplementedversion);
        if(eores_OK != res)
        {
            return(res);
        }
    }

    return(eores_OK);
}

static eOresult_t s_eo_emsapplBody_startCheckCanboards(EOtheEMSapplBody *p)
{
    eOresult_t                              res = eores_OK;
    EOaction                                action;
    
    //note: can protocol version check is done only for motor control
    
//     if((applrunMode__skinAndMc4 != p->appRunMode) && (applrunMode__mc4Only != p->appRunMode))
//     {
//         return(eores_OK);
//     }
    
    eo_action_SetEvent(&action, CANBOARDSRAEDY_CHECK_TIMEOUT_EVT, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_timer_Start(p->canBoardsReady_timer, eok_abstimeNOW, CANBOARDSRAEDY_CHECK_PERIOD, eo_tmrmode_FOREVER, &action);
    //res = s_eo_emsapplBody_sendGetFWVersion(p, 0xFFFF); //here all boards are not ready, i.e. they didn't receive get_fw_ver can msg!
    return(res);
}

static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
{
    //eOresult_t                              res;
    uint16_t                                numofjoint, i;
    eOappTheDB_jointShiftValues_t           *shiftval_ptr;
    eOicubCanProto_estimShift_t             estimshift;
    icubCanProto_velocityShift_t            shift_icubCanProtValue;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    

    // 2) config joints
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for(i=0; i<numofjoint; i++)
    {
        if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
        {            
            //get shift values from DB
            eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)i, &shiftval_ptr);
            
            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
            shift_icubCanProtValue = shiftval_ptr->jointVelocityShift;
            eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i,  msgCmd, (void*)&shift_icubCanProtValue);
        
            //set estim vel shift
            estimshift.estimShiftJointVel= shiftval_ptr->jointVelocityEstimationShift;
            estimshift.estimShiftJointAcc = 0;
            estimshift.estimShiftMotorVel = 0;
            estimshift.estimShiftMotorAcc = 0;
            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
            eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i,  msgCmd, (void*)&estimshift);
        }
    }
    
////VALE:removed because i send it when r.i. send me commnadS
//     //config skin if connected
//     if(eo_appTheDB_isSkinConnected(eo_appTheDB_GetHandle()))
//     {

//         eOappTheDB_jointOrMotorCanLocation_t    canLoc; //here I don't use eOappTheDB_SkinCanLocation_t because i need jmindexId filed to set triangle id
//         eOappTheDB_SkinCanLocation_t            skincanLoc;

//         msgCmd.class =  icubCanProto_msgCmdClass_skinBoard; //currently this class not exist and it is remaped on sensor class
//         msgCmd.cmdId =  ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;


//         res = eo_appTheDB_GetSkinCanLocation(eo_appTheDB_GetHandle(), 0, &skincanLoc);
//         if(eores_OK != res)
//         {
//             return(res);
//         }
//         canLoc.emscanport = skincanLoc.emscanport;


//         for(i=8; i<15; i++)
//         {
//             eOicubCanProto_msgDestination_t msgdest;
//             msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, i);
//             res = eo_appCanSP_SendCmd(p->bodyobjs.appCanSP, canLoc.emscanport, msgdest, msgCmd, NULL);
//             if(eores_OK != res)
//             {
//                 return(res);
//             }
//         }

//     }
    
    return(eores_OK);
}


static eOresult_t s_eo_emsapplBody_getRunMode(EOtheEMSapplBody *p)
{
    p->appRunMode = applrunMode__default;
    
    if(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_2foc))
    {
        p->appRunMode = applrunMode__2foc;
        return(eores_OK);
    }
    
    if(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_mc4))
    {
        if(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin))
        {
            p->appRunMode = applrunMode__skinAndMc4;
            return(eores_OK);
        }
        else
        {
            p->appRunMode = applrunMode__mc4Only;
            return(eores_OK);
        }
    }
    
    if(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin))
    {
        p->appRunMode = applrunMode__skinOnly;
        return(eores_OK);
    }

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "applrunmode is not valid");
    return(eores_NOK_generic);
}


static eOresult_t s_eo_emsapplBody_EnableTxMais(EOtheEMSapplBody *p)
{
    eOresult_t                  res;
    eOas_maisId_t               sId = 0; //exist only one mais per ep
    eOas_mais_config_t          *maiscfg;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    
    res = eo_appTheDB_GetSnrMaisConfigPtr(eo_appTheDB_GetHandle(), sId,  &maiscfg);
    if(eores_OK != res)
    {
        return(res);
    }
    res = eo_appCanSP_SendCmd2SnrMais(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&(maiscfg->mode));

    return(res);
}


static eOresult_t s_eo_emsapplBody_DisableTxMais(EOtheEMSapplBody *p)
{
    eOresult_t                  res;
    eOas_maisId_t               sId = 0; //exist only one mais per ep
    eOas_maismode_t             mode = eoas_maismode_acquirebutdonttx;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };


    res = eo_appCanSP_SendCmd2SnrMais(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&mode);

    return(res);
}



static eOresult_t s_eo_emsapplBody_SendTxMode2Strain(EOtheEMSapplBody *p)
{
    eOresult_t                  res;
    eOas_strainId_t             sId = 0; //exist only one mais per ep
    eOas_strain_config_t        *straincfg;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    
    res = eo_appTheDB_GetSnrStrainConfigPtr(eo_appTheDB_GetHandle(), sId,  &straincfg);
    if(eores_OK != res)
    {
        if(eores_NOK_nodata == res)
        {
            //if no strain is connected to ems ==> nothing to do ==>ok
            res = eores_OK;
        }
        return(res);
    }
    
    res = eo_appCanSP_SendCmd2SnrStrain(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&(straincfg->mode));

    return(res);
}


static eOresult_t s_eo_emsapplBody_DisableTxStrain(EOtheEMSapplBody *p)
{
    eOresult_t                  res;
    eOas_strainId_t           sId = 0; //exist only one mais per ep
    eOas_strainmode_t         mode = eoas_strainmode_acquirebutdonttx;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    
    res = eo_appCanSP_SendCmd2SnrStrain(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&mode);
    if(eores_NOK_nodata == res)
    {
        //if no strain is connected to ems ==> nothing to do ==>ok
        return(eores_OK);
    }
    return(res);
}

static void s_eo_emsapplBody_checkConfig(EOtheEMSapplBody *p)
{
    uint16_t                                numofjoint = 0, jid;
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;    
    /*Since emscontroller uses 4 joints max, here check the max number of joint for ems connected to 2foc
    in order to evoid useless chack during controller loop  */
    if(applrunMode__2foc == p->appRunMode)
    {
        numofjoint =  eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
        if(numofjoint > 4)
        {
             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "More then 4 motor for ems connected to 2foc!!");
        }
        
        for(jid = 0; jid <numofjoint; jid++)
        {
            res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jid,  &canLoc, NULL);
            if(eores_OK != res)
            {
                 eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "err in checkConfig");
            }
            if(canLoc.addr > 4)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "can address bigger than 4!!");
            }
        }

    }
}

// static eOresult_t test_4_strain(EOtheEMSapplBody *p)
// {
//     uint8_t                             channel = 0;
//     eOresult_t                          res;
//     eOicubCanProto_msgDestination_t     msgdest;
//     eOas_strain_status_t              *sstatus_ptr;
//     eOappTheDB_sensorCanLocation_t      canLoc;
//     eOicubCanProto_msgCommand_t         msgCmd = 
//     {
//         EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//         EO_INIT(.cmdId) 0
//     };

//     eOas_strain_config_t               straincfg = 
//     {
//         EO_INIT(.mode)                  eoas_strainmode_txcalibrateddatacontinuously,
//         EO_INIT(.datarate)              1,
//         EO_INIT(.signaloncefullscale)   eobool_true,
//         EO_INIT(.filler01)           {0}                           
//     };

//     
//     EOappCanSP *appCanSP_ptr = p->bodyobjs.appCanSP; //eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//     eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), (eOas_strainId_t)0, &canLoc);
//     
//     msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
//     
// //     msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
// //     eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg.mode));
//     

//     msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//     eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg.datarate));
//     
//     if(straincfg.signaloncefullscale)
//     {
//         //clear array in strainstatus
//         res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), 0,  &sstatus_ptr);
//         if(eores_OK != res)
//         {
//             return(res);
//         }
//         
//         sstatus_ptr->fullscale.head.size = 0;
//         memset(&sstatus_ptr->fullscale.data[0], 0, 12);
//         channel = 0;
//         msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
//         eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &channel);
//     }

//     return(eores_OK);
// }
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



