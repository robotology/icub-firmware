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

#include "eOcommon.h"
#include "EOtheErrorManager.h"
#include "EOtheBOARDtransceiver_hid.h" //==>in order to get nvcfg

//icub
#include "EoMotionControl.h"
#include "EoSkin.h"
#include "EoManagement.h"
#include "eOicubCanProto_specifications.h"

//nv-cfg
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_sk.h" 
#include "eOcfg_nvsEP_as.h" 

//application
#include "EOMtheEMSapplCfg.h"
#include "eOcfg_appTheDataBase.h"

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




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


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
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static struct EOtheEMSapplBody_hid s_applBody = 
{
    EO_INIT(.st)    eo_emsApplBody_st__NOTinited
};


static const char s_eobj_ownname[] = "EOtheEMSapplBody";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOtheEMSappBody_cfg_t *cfg)
{
    eOresult_t res;
    EOtheEMSapplBody *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = &s_applBody;

    //save in obj its configuration
    retptr->cfg_ptr = cfg;
    
    retptr->appRunMode = applrunMode__default;
    
    s_eo_emsapplBody_objs_init(retptr); //if a obj init doesn't success, it calls errorManager with fatal error
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, "obj-body inited OK");
    
    res = s_eo_emsapplBody_sendConfig2canboards(retptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in sending cfg 2 canboards");
    
    res = s_eo_emsapplBody_getRunMode(retptr); //the run mode is depending on connected can board (mc4, 2foc, only skin, etc)
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in getting run mode");
    
    retptr->st = eo_emsApplBody_st__inited;
    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "body appl init OK");
    return(retptr);
}


extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void)
{
    return((s_applBody.st == eo_emsApplBody_st__inited) ? (&s_applBody) : (NULL));
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


extern EOemsController* eo_emsapplBody_GetEmsControllerHandle(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->bodyobjs.emsController);
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
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
    eOappTheDB_jointShiftValues_t       *shiftval_ptr;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
    
    eOappTheDB_cfg_t cfg = 
    {
        EO_INIT(.nvsCfg)            eo_boardtransceiver_hid_GetNvsCfg(),
        EO_INIT(.mc_endpoint)       p->cfg_ptr->endpoints.mc_endpoint,                 
        EO_INIT(.as_endpoint)       p->cfg_ptr->endpoints.as_endpoint,   
        EO_INIT(.sk_endpoint)       p->cfg_ptr->endpoints.sk_endpoint,   
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
//             /* 0 */   ICUBCANPROTO_PER_MB_CMD_POSITION,
//             /* 1 */   ICUBCANPROTO_PER_MB_CMD_PID_VAL,
//             /* 2 */   ICUBCANPROTO_PER_MB_CMD_PID_ERROR,
//             /* 3 */   ICUBCANPROTO_PER_MB_CMD_VELOCITY
//         }
//     };

    p->bodyobjs.appDB =  eo_appTheDB_Initialise(&cfg);
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in appTheDB_Initialise");
    
//     res = eo_appTheDB_SetjointsShiftValues(EOappTheDB *p, &shiftval);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");


//     res = eo_appTheDB_SetJointsBcastpolicyPtr(EOappTheDB *p, &bcastpolicy);
//     eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appDB), s_eobj_ownname, "error in SetjointsShiftValues");

    
    res = eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), 0, &shiftval_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in _GetShiftValuesOfJointPtr");
    
    //here i don't use memcpy because the two struct have different type
    shiftval_ptr->jointVelocityShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointVelocityShift;
    shiftval_ptr->jointVelocityEstimationShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointVelocityEstimationShift;
    shiftval_ptr->jointAccelerationEstimationShift = p->cfg_ptr->configdataofMC4boards.shiftvalues.jointAccelerationEstimationShift;  
    
    res = eo_appTheDB_GetJointBcastpolicyPtr(eo_appTheDB_GetHandle(), 0, &bcastpolicy_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), s_eobj_ownname, "error in _GetJointBcastpolicyPtr");
    
    memcpy(&bcastpolicy_ptr->val2bcastList, p->cfg_ptr->configdataofMC4boards.bcastpolicy.val2bcastList, 4);

}

static void s_eo_emsapplBody_canServicesProvider_init(EOtheEMSapplBody *p)
{
    eOappCanSP_cfg_t cfg = 
    {
        .waitallframesaresent   = eobool_true
    };
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
    uint16_t i, numofjoint = 0;
    
    p->bodyobjs.emsController = eo_emsController_Init(p->cfg_ptr->emsControllerCfg.emsboard_type);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.emsController), 
                     s_eobj_ownname, "error in emsController_init");
    
    numofjoint =  eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    for(i=0; i<numofjoint; i++)
    {
        eo_emsController_AddAxis(i);
	}
    
    /* TAG_ALE*/
//     eo_emsController_AddAxis(3);
//     eo_emsController_SetLimits(3, -65535, 65535, 2048);
//     eo_emsController_SetPosPid(3, 100.0f, 20.0f, 0.001f);
//     eo_emsController_SetPosPidLimits(3, 8000.0f, 750.0f);
//     eo_emsController_SetControlMode(3, CM_IDLE);
}


static void s_eo_emsapplBody_measuresConverter_init(EOtheEMSapplBody *p)
{
    eOappMeasConv_cfg_t cfg = {0};
    p->bodyobjs.appMeasConv = eo_appMeasConv_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appMeasConv), 
                     s_eobj_ownname, "error in appMeasConv_New");

}

static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
{
    eOresult_t                  res;
    uint16_t                    numofjoint, i;
    eOmc_controlmode_t          controlmode = eomc_controlmode_idle;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    //config joints
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for(i=0; i<numofjoint; i++)
    {
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD;
        res  = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, NULL);
        if(eores_OK != res)
        {
            return(res);
        }
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
        res  = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, NULL);
        if(eores_OK != res)
        {
            return(res);
        }

        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
        res  = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, &controlmode);
        if(eores_OK != res)
        {
            return(res);
        }
    }
    
    //config skin if connected
    if(eo_appTheDB_isSkinConnected(eo_appTheDB_GetHandle()))
    {

        eOappTheDB_jointOrMotorCanLocation_t    canLoc; //here I don't use eOappTheDB_SkinCanLocation_t because i need jmindexId filed to set triangle id
        eOappTheDB_SkinCanLocation_t            skincanLoc;

        msgCmd.class =  eo_icubCanProto_msgCmdClass_skinBoard; //currently this class not exist and it is remaped on sensor class
        msgCmd.cmdId =  ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;


        res = eo_appTheDB_GetSkinCanLocation(eo_appTheDB_GetHandle(), 0, &skincanLoc);
        if(eores_OK != res)
        {
            return(res);
        }
        canLoc.emscanport = skincanLoc.emscanport;


        for(i=8; i<15; i++)
        {
            eOicubCanProto_msgDestination_t msgdest;
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, i);
            res = eo_appCanSP_SendCmd(p->bodyobjs.appCanSP, canLoc.emscanport, msgdest, msgCmd, NULL);
            if(eores_OK != res)
            {
                return(res);
            }
        }

    }
    
    return(eores_OK);
}


static eOresult_t s_eo_emsapplBody_getRunMode(EOtheEMSapplBody *p)
{
    p->appRunMode = applrunMode__default;
    
    if(eom_emsapplcfg_HasDevice(eom_emsapplcfg_GetHandle(), eom_emsappl_deviceid_2foc))
    {
        p->appRunMode = applrunMode__2foc;
        return(eores_OK);
    }
    
    if(eom_emsapplcfg_HasDevice(eom_emsapplcfg_GetHandle(), eom_emsappl_deviceid_mc4))
    {
        if(eom_emsapplcfg_HasDevice(eom_emsapplcfg_GetHandle(), eom_emsappl_deviceid_skin))
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
    
    if(eom_emsapplcfg_HasDevice(eom_emsapplcfg_GetHandle(), eom_emsappl_deviceid_skin))
    {
        p->appRunMode = applrunMode__skinOnly;
        return(eores_OK);
    }

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "applrunmode is not valid");
    return(eores_NOK_generic);
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



