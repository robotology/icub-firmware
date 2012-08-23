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

/* @file       EOappTheServicesProvider.c
    @brief     This file implements services provider object.
    @author    valentina.gaggero@iit.it
    @date       07/16/2012
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

//nv-cfg
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_sk.h" 
#include "eOcfg_nvsEP_as.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheServicesProvider.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheServicesProvider_hid.h"


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
static void s_eo_appTheSP_services_init(EOappTheSP *p);
static void s_eo_appTheSP_srv_CanServicesProvider_init(EOappTheSP *p);
static void s_eo_appTheSP_srv_EncodersReader_init(EOappTheSP *p);
static void s_eo_appTheSP_srv_CanBoardsManager_init(EOappTheSP *p);
static void s_eo_appTheSP_GetCanConnectedStuff(EOappTheSP *p);
static void s_eo_appTheSP_srv_NVmapRef_init(EOappTheSP *p);
static void s_eo_appTheSP_srv_EmsController_init(EOappTheSP *p);
static void s_eo_apptheSP_GetAppRunMode(EOappTheSP *p);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static EOappTheSP s_theSP = 
{
    EO_INIT(.st)    eO_appTheSP_st__NOTinited
};


static const char s_eobj_ownname[] = "EOMappTheSP";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappTheSP* eo_appTheSP_Initialise(const eOappTheSP_cfg_t *cfg)
{
    EOappTheSP *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = &s_theSP;

    //save in obj its configuration
    memcpy(&(retptr->cfg), cfg, sizeof(eOappTheSP_cfg_t));
    retptr->appRunMode = applrunMode__default;
    
    s_eo_appTheSP_services_init(retptr);
    
    retptr->st = eO_appTheSP_st__inited;
    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "Services provider init OK");
    return(retptr);
}


extern EOappTheSP* eo_appTheSP_GetHandle(void)
{
    return((s_theSP.st == eO_appTheSP_st__inited) ? (&s_theSP) : (NULL));
}



extern EOappCanSP* eo_appTheSP_GetCanServiceHandle(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->srv.appCanSP);
}
    
extern EOappTheCanBrdsMng* eo_appTheSP_GetTheCanBrdsMngHandle(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->srv.appTheCanBrdsMng);
}

    
extern EOappTheNVmapRef* eo_appTheSP_GetTheNVmapRefHandle(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->srv.appTheNVmapRef);
}


extern EOappEncReader* eo_appTheSP_GetEncoderReaderHandle(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->srv.appEncReader);
}


extern EOemsController* eo_appTheSP_GetEmsControllerHandle(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->srv.emsController);
}

extern eOmn_appl_runMode_t eo_appTheSP_GetAppRunMode(EOappTheSP *p)
{
    if(NULL == p)
    {
        return(applrunMode__default);
    }
    return(p->appRunMode);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_appTheSP_services_init(EOappTheSP *p)
{

/* NOTE: the initialization order is important. Pay attaention to change it! */
    
   
/* 1) init can module */
    s_eo_appTheSP_srv_CanServicesProvider_init(p);

/* 2) init encoder reader */
    s_eo_appTheSP_srv_EncodersReader_init(p);
    
/* 3) init encoder reader */
    s_eo_appTheSP_srv_CanBoardsManager_init(p);

/*  4) init and populate NVmapRef */
    s_eo_appTheSP_GetCanConnectedStuff(p);
    s_eo_appTheSP_srv_NVmapRef_init(p);
    
/*  5) init emsController */
    s_eo_appTheSP_srv_EmsController_init(p);

/*  6) get app run mode*/
    s_eo_apptheSP_GetAppRunMode(p);

/*  7) config connected can boards */
    eo_appTheCanBrdsMng_ConfigAllBoards(p->srv.appTheCanBrdsMng);

}



static void s_eo_appTheSP_srv_CanServicesProvider_init(EOappTheSP *p)
{
    eOappCanSP_cfg_t cfg = 
    {
        .waitallframesaresent   = eobool_true
    };
    p->srv.appCanSP = eo_appCanSP_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->srv.appCanSP), 
                     s_eobj_ownname, "error in appCanSP_New");
}


static void s_eo_appTheSP_srv_EncodersReader_init(EOappTheSP *p)
{

    eOappEncReader_cfg_t  cfg = 
    {
        EO_INIT(.connectedEncodersMask)     p->cfg.connectedEncodersMask,
        EO_INIT(.callbackOnLastRead)        NULL
    };

   p->srv.appEncReader = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->srv.appEncReader), 
                     s_eobj_ownname, "error in appEncReader_New");

}


static void s_eo_appTheSP_srv_CanBoardsManager_init(EOappTheSP *p)
{
    eOappTheCanBrdsMng_cfg_t  cfg = 
    {
        EO_INIT(.appCanSP_ptr)          p->srv.appCanSP
    };

    p->srv.appTheCanBrdsMng = eo_appTheCanBrdsMng_Initialise(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->srv.appTheCanBrdsMng), 
                     s_eobj_ownname, "error in eo_appTheCanBrdsMng_Initialise");

}

static void s_eo_appTheSP_GetCanConnectedStuff(EOappTheSP *p)
{
    uint16_t jointMaxNumber = eo_cfg_nvsEP_mc_joint_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint);
    uint16_t motorMaxNumber = eo_cfg_nvsEP_mc_motor_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint);
    uint16_t sensorStrainMaxNumber = eo_cfg_nvsEP_as_strain_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint);
    uint16_t sensorMaisMaxNumber = eo_cfg_nvsEP_as_mais_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint);
    uint16_t skinMaxNumber = eo_cfg_nvsEP_sk_sknumbermax_Get((eOcfg_nvsEP_sk_endpoint_t)p->cfg.sk_endpoint);

   //get joints, motors, sensors and skin from can configuration
    p->jointsList =  eo_array_New(jointMaxNumber, sizeof(eOmc_jointId_t), NULL);
    p->motorsList =  eo_array_New(motorMaxNumber, sizeof(eOmc_motorId_t), NULL); 
    p->skinList =    eo_array_New(skinMaxNumber, sizeof(eOsk_skinId_t), NULL); 

    eo_appCanSP_GetConnectedJoints(p->srv.appCanSP, p->jointsList);
    eo_appCanSP_GetConnectedMotors(p->srv.appCanSP, p->motorsList);
    eo_appCanSP_GetConnectedSkin(p->srv.appCanSP, p->skinList);


    if( (0 != sensorStrainMaxNumber) && (0 == sensorMaisMaxNumber) )
    {    
        p->sensorsStrainList = eo_array_New(sensorStrainMaxNumber, sizeof(eOsnsr_sensorId_t), NULL); 
        p->sensorsMaisList = NULL;

        eo_appCanSP_GetConnectedSensors(p->srv.appCanSP, p->sensorsStrainList);
    }
    else if( (0 == sensorStrainMaxNumber) && (0 != sensorMaisMaxNumber) )
    {
        p->sensorsMaisList = eo_array_New(sensorMaisMaxNumber, sizeof(eOsnsr_sensorId_t), NULL); 
        p->sensorsStrainList = NULL;
        eo_appCanSP_GetConnectedSensors(p->srv.appCanSP, p->sensorsMaisList);
    }
    else
    {
         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "no sensors are connected");
    }

}



static void s_eo_appTheSP_srv_NVmapRef_init(EOappTheSP *p)
{

    eOappTheNVmapRef_cfg_t cfg =
    {
        EO_INIT(.jointsList)            p->jointsList,
        EO_INIT(.motorsList)            p->motorsList,
        EO_INIT(.sensorsStrainList)     p->sensorsStrainList,
        EO_INIT(.sensorsMaisList)       p->sensorsMaisList,
        EO_INIT(.skinList)              p->skinList,
        EO_INIT(.nvsCfg)                eo_boardtransceiver_hid_GetNvsCfg(),
        EO_INIT(.mc_endpoint)           p->cfg.mc_endpoint, 
        EO_INIT(.sk_endpoint)           p->cfg.sk_endpoint,
        EO_INIT(.as_endpoint)           p->cfg.as_endpoint
    };


    p->srv.appTheNVmapRef = eo_appTheNVmapRef_Initialise(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->srv.appTheNVmapRef), 
                     s_eobj_ownname, "error in appTheNVmapRef_Initialise");

}


static void s_eo_appTheSP_srv_EmsController_init(EOappTheSP *p)
{
    p->srv.emsController = eo_emsController_Init(p->cfg.emsControllerCfg.emsboard_type);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->srv.emsController), 
                     s_eobj_ownname, "error in emsController_init");
	/* TAG_ALE*/
#warning VALE--> mettere qui la corretta init del ems controller con dfault values
   //io ho messo questi, ma non so se sono giusti
//     eo_emsController_SetLimits(0, -100000, 100000, 2048);
//     eo_emsController_SetPosPid(0, 100.0f, 20.0f, 0.001f);
//     eo_emsController_SetPosPidLimits(0, 8000.0f, 750.0f);

    eo_emsController_SetControlMode(0, CM_IDLE);

}

static void s_eo_apptheSP_GetAppRunMode(EOappTheSP *p)
{
    eOresult_t res = eo_appTheCanBrdsMng_GetAppRunMode(p->srv.appTheCanBrdsMng, &p->appRunMode);
    
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), 
                     s_eobj_ownname, "error in GetAppRunMode");
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



