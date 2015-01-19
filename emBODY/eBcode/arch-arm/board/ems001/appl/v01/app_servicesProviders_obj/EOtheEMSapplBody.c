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


//icub
#include "EoMotionControl.h"
#include "EoSkin.h"
#include "EoManagement.h"
#include "eOicubCanProto_specifications.h"



//application
#include "EOMtheEMSapplCfg.h"
#include "eOcfg_appTheDataBase.h"

#include "EOaction_hid.h"

#include "EOtheProtocolWrapper.h"

#include "EoError.h"

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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


const eOemsapplbody_cfg_t eo_emsapplbody_cfg_default = 
{
    .encoderstreams =
    {
        {
            .type       = hal_encoder_type1,
            .numberof   = 2,
            .encoders   = { hal_encoder1, hal_encoder3, hal_encoderNONE }        
        },
        {
            .type       = hal_encoder_type1,
            .numberof   = 2,
            .encoders   = { hal_encoder2, hal_encoder4, hal_encoderNONE }     
        }        
    },
    .hasdevice  =
    {
        eobool_false, eobool_false, eobool_true
    },
    .icubcanprotoimplementedversion =
    {
        .major                      = 1,
        .minor                      = 2
    },
    .connectedEncodersMask          = 0,   
    .configdataofMC4boards          =
    {
        .shiftvalues    =
        {
            .jointVelocityShift                 = 8,
            .jointVelocityEstimationShift       = 8,
            .jointAccelerationEstimationShift   = 5
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
static void s_eo_emsapplBody_checkConfig(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_EnableTxMais(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_DisableTxMais(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_SendTxMode2Strain(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_DisableTxStrain(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t dontaskmask);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheEMSapplBody s_applBody = 
{
    .config                 = {0},  // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    .st                     = eo_emsApplBody_st__NOTinitted,
    .appRunMode             = applrunMode__default,
    .checkCanBoards_timer   = NULL,
    .bodyobjs               =
    {
        .appDB              = NULL,
        .appCanSP           = NULL,
        .appEncReader       = NULL,
        .emsController      = NULL,
        .appMeasConv        = NULL
    }
};

static const char s_eobj_ownname[] = "EOtheEMSapplBody";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOemsapplbody_cfg_t *cfg)
{
    eOresult_t res;
    EOtheEMSapplBody *p = NULL;

    if(NULL == cfg)
    {
        cfg = &eo_emsapplbody_cfg_default;
    }
    
    p = &s_applBody;

    // save in obj its configuration
    memcpy(&p->config, cfg, sizeof(eOemsapplbody_cfg_t));
    
    s_eo_emsapplBody_getRunMode(p); // the run mode depends on connected can board (mc4, 2foc, only skin, etc)
    
    p->checkCanBoards_timer = eo_timer_New();
    
    eo_protocolwrapper_Initialise();
        
    s_eo_emsapplBody_objs_init(p);      // if anything fails... it calls errormanager with fatal error

    s_eo_emsapplBody_checkConfig(p);    // check config: if anything is wrong .... it calls errormanager with fatal error
    
    res = eo_emsapplBody_checkCanBoards_Start(p);
    
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "error in startCheckCanboards", s_eobj_ownname, &eo_errman_DescrTobedecided);
    
    p->st = eo_emsApplBody_st__initted;
        
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, "EOtheEMSapplBody started", s_eobj_ownname, &eo_errman_DescrRunningHappily);
    
    
    return(p);
}



extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void)
{
    return((s_applBody.st == eo_emsApplBody_st__initted) ? (&s_applBody) : (NULL));
}


extern const eOemsapplbody_cfg_t* eo_emsapplBody_GetConfig(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(&p->config);
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
        res = s_eo_emsapplBody_EnableTxMais(p);
        if(eores_OK != res)
        {
            return(res);
        }

        numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
        
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
        numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
        
        for(i=0; (i<numofjoint) && (eores_OK == res); i++)
        {
            res = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, (void*)&(bcastpolicy.val2bcastList[0]));
        }
        
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
    
    return(p->config.hasdevice[dev]);
}


extern eOresult_t eo_emsapplBody_checkCanBoardsAreReady(EOtheEMSapplBody *p, uint32_t dontaskmask)
{
    return(s_eo_emsapplBody_sendGetFWVersion(p, dontaskmask));
}


extern eOresult_t eo_emsapplBody_checkCanBoards_Stop(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_OK);
    }
    return(eo_timer_Stop(p->checkCanBoards_timer));
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



extern eOresult_t eo_emsapplBody_StopSkin(EOtheEMSapplBody *p)
{
    eOresult_t                      res;
    uint8_t                         i, j;
    EOappCanSP                      *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_cfg_skinInfo_t       *skconfig_ptr = NULL;
    uint8_t                         boardEndAddr;
    eOicubCanProto_msgDestination_t msgdest;
    
    uint8_t numofskin = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    for(j=0; j<numofskin; j++) //for each skid
    {
        res = eo_appTheDB_GetSkinConfigPtr(eo_appTheDB_GetHandle(), j,  &skconfig_ptr);
        if(eores_OK != res)
        {
            return(eores_NOK_generic);
        }
            
        boardEndAddr = skconfig_ptr->boardAddrStart + skconfig_ptr->numofboards;
    
        icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
            
        for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++) //for each skid 
        {
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
            
            res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &sigmode);
            if(eores_OK != res)
            {
                return(eores_NOK_generic);
            }
        }
    }
    return(eores_OK);

}


extern eOresult_t eo_emsapplBody_checkCanBoards_Start(EOtheEMSapplBody *p)
{
    eOresult_t                              res = eores_OK;
    EOaction                                action;
    
    //note: can protocol version check is done only for motor control
    
//     if((applrunMode__skinAndMc4 != p->appRunMode) && (applrunMode__mc4Only != p->appRunMode))
//     {
//         return(eores_OK);
//     }

   
    eo_action_SetEvent(&action, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_timer_Start(p->checkCanBoards_timer, eok_abstimeNOW, 10*eok_reltime1ms, eo_tmrmode_FOREVER, &action);
    
    uint32_t dontaskmask = 0; // the first time we ask to every board
    res = s_eo_emsapplBody_sendGetFWVersion(p, dontaskmask); 
    
    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eo_emsapplBody_hid_canSP_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


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
        EO_INIT(.canboardsList)     eo_cfg_appDB_thecanboards,
        EO_INIT(.jointsList)        eo_cfg_appDB_thejoints_mapping2canboards,
        EO_INIT(.motorsList)        eo_cfg_appDB_themotors_mapping2canboards,
        EO_INIT(.snsrMaisList)      eo_cfg_appDB_themaises_mapping2canboards,
        EO_INIT(.snsrStrainList)    eo_cfg_appDB_thestrains_mapping2canboards,
        EO_INIT(.skinList)          eo_cfg_appDB_theskins_info
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
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), "error in appTheDB_Initialise", s_eobj_ownname, &eo_errman_DescrTobedecided);
    
//     res = eo_appTheDB_SetjointsShiftValues(EOappTheDB *p, &shiftval);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");


//     res = eo_appTheDB_SetJointsBcastpolicyPtr(EOappTheDB *p, &bcastpolicy);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");

    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        res = eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), 0, &shiftval_ptr);
        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "error in _GetShiftValuesOfJointPtr", s_eobj_ownname, &eo_errman_DescrTobedecided);
        
        //here i don't use memcpy because the two struct have different type
        shiftval_ptr->jointVelocityShift = p->config.configdataofMC4boards.shiftvalues.jointVelocityShift;
        shiftval_ptr->jointVelocityEstimationShift = p->config.configdataofMC4boards.shiftvalues.jointVelocityEstimationShift;
        shiftval_ptr->jointAccelerationEstimationShift = p->config.configdataofMC4boards.shiftvalues.jointAccelerationEstimationShift;  
        
        res = eo_appTheDB_GetJointBcastpolicyPtr(eo_appTheDB_GetHandle(), 0, &bcastpolicy_ptr);
        eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "error in _GetJointBcastpolicyPtr", s_eobj_ownname, &eo_errman_DescrTobedecided);
        
        //set bacast value as mask
        memset(bcastpolicy_ptr, 0, sizeof(eOicubCanProto_bcastpolicy_t)); //reset
        for(i=0; i<eOicubCanProto_bcastpolicy_maxsize; i++)
        {
            if(0 == p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i])
            {
                continue;
            }
            if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<9)
            {
                bcastpolicy_ptr->val2bcastList[0] |= (1 <<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
            }
            else if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<17)
            {
                bcastpolicy_ptr->val2bcastList[1] |= (1<<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
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
                     "error in appCanSP_New", s_eobj_ownname, &eo_errman_DescrTobedecided);
}


static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p)
{
    eOappEncReader_cfg_t cfg = { 0 }; // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    
    memcpy(&cfg.streams, &p->config.encoderstreams, sizeof(cfg.streams));
    cfg.connectedEncodersMask = p->config.connectedEncodersMask;
    cfg.callbackOnLastRead = NULL;
    cfg.callback_arg = NULL;
    
    p->bodyobjs.appEncReader = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appEncReader), 
                     "error in appEncReader_New", s_eobj_ownname, &eo_errman_DescrTobedecided);

}



static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p)
{
    //uint16_t i, numofjoint = 0;
    
    p->bodyobjs.emsController = eo_emsController_Init();

//NOTE: removed check because eo_emsController_Init returns NULL if any 2foc board is connected to ems. (i.e, eb2, eb4, eb10, eb11)
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.emsController), 
//                      s_eobj_ownname, "error in emsController_init");
    
    /*
    numofjoint =  eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
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
                     "error in appMeasConv_New", s_eobj_ownname, &eo_errman_DescrTobedecided);

}


static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t dontaskmask)
{
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION
    };

    eOicubCanProto_msgDestination_t     msgdest;
    uint16_t                            numofboard = eo_appTheDB_GetNumberOfCanboards(eo_appTheDB_GetHandle());
    uint16_t                            i;
    eOresult_t                          res;
    eOappTheDB_canboardinfo_t           *canboardinfo;
    EOappTheDB                          *db = eo_appTheDB_GetHandle();

    for(i=0; i<numofboard; i++)
    {
        
        if( ((1<<i) & dontaskmask) == (1<<i))
        {
            continue;
        }
        
        res = eo_appTheDB_GetCanBoardInfo(db, (eObrd_boardId_t)i, &canboardinfo);
        if(eores_OK != res)
        {
            continue;
        }
        
        if((eobrd_mc4 != canboardinfo->type) && (eobrd_1foc != canboardinfo->type))
        {
            continue; // m.a: i dont process this board index and i go to the next one
        }
        
        
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canboardinfo->addr);
                        
        res = eo_appCanSP_SendCmd(p->bodyobjs.appCanSP, (eOcanport_t)canboardinfo->port, msgdest, msgCmd, (void*)&p->config.icubcanprotoimplementedversion);
        if(eores_OK != res)
        {
            return(res);
        }
    }

    return(eores_OK);
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
    numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
    
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
//             msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, i);
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

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "applrunmode is not valid", s_eobj_ownname, &eo_errman_DescrTobedecided);
    return(eores_NOK_generic);
}


static eOresult_t s_eo_emsapplBody_EnableTxMais(EOtheEMSapplBody *p)
{
    eOresult_t                  res = eores_NOK_generic;
    eOas_maisId_t               sId = 0; //exist only one mais per ep
    eOas_mais_config_t          *maiscfg = NULL;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    
    maiscfg = eo_protocolwrapper_GetMaisConfig(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == maiscfg)
    {
        return(res); //error
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
    eOresult_t                  res = eores_NOK_generic;
    eOas_strainId_t             sId = 0; //exist only one mais per ep
    eOas_strain_config_t        *straincfg = NULL;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    
    straincfg = eo_protocolwrapper_GetStrainConfig(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == straincfg)
    {
         //if no strain is connected to ems ==> nothing to do ==> ok
        return(eores_OK); 
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
        numofjoint =  eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
        if(numofjoint > 4)
        {
             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "More then 4 motor for ems connected to 2foc!!", s_eobj_ownname, &eo_errman_DescrTobedecided);
        }
        
        for(jid = 0; jid <numofjoint; jid++)
        {
            res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jid,  &canLoc, NULL);
            if(eores_OK != res)
            {
                 eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "err in checkConfig", s_eobj_ownname, &eo_errman_DescrTobedecided);
            }
            if(canLoc.addr > 4)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "can address bigger than 4!!", s_eobj_ownname, &eo_errman_DescrTobedecided);
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
//     eOappTheDB_board_canlocation_t      canLoc;
//     eOicubCanProto_msgCommand_t         msgCmd = 
//     {
//         EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//         EO_INIT(.cmdId) 0
//     };
//
//     eOas_strain_config_t               straincfg = 
//     {
//         EO_INIT(.mode)                  eoas_strainmode_txcalibrateddatacontinuously,
//         EO_INIT(.datarate)              1,
//         EO_INIT(.signaloncefullscale)   eobool_true,
//         EO_INIT(.filler01)           {0}                           
//     };
//
//     
//     EOappCanSP *appCanSP_ptr = p->bodyobjs.appCanSP; //eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//     eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), (eOas_strainId_t)0, &canLoc);
//     
//     msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
//     
// //     msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
// //     eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg.mode));
//     
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
//
//     return(eores_OK);
// }


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



