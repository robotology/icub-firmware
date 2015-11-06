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

#include "EoCommon.h"
#include "EOtheErrorManager.h"


//icub
#include "EoMotionControl.h"
#include "EoSkin.h"
#include "EoManagement.h"




//application
#include "EOMtheEMSapplCfg.h"


#include "EOaction.h"

#include "EOtheEntities.h"

#include "EoError.h"

#include "EOVtheSystem.h"


#include "iCubCanProto_motorControlMessages.h"

#include "EOtheCANservice.h"

#include "EOtheSTRAIN.h"

#include "EOtheCANdiscovery2.h"


#include "EOtheMC4boards.h"

#include "EOtheBoardConfig.h"

#include "EOtheMAIS.h"

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


static void s_eo_emsapplBody_CanServices_Init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_computeRunMode(EOtheEMSapplBody *p);


static void s_eo_emsapplBody_hid_canSP_cbkonrx(void *arg);

static eObool_t s_eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev);

static eOresult_t s_onstop_search42foc(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);
static eOresult_t s_onstop_search4mc4(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);
static eOresult_t s_onstop_search4mais(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);
static eOresult_t s_onstop_search4strain(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);

//static eOresult_t s_onstop_search4test(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheEMSapplBody s_applBody = 
{
    .config                 = {0},  // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    .st                     = eo_emsApplBody_st__NOTinitted,
    .appRunMode             = applrunMode__default,
    .appEncReader           = NULL,
    .emsController          = NULL,
    .hasdevice              = {eobool_false, eobool_false, eobool_false},
    
    .mctype                 = eoMCtype_none,
    .isMCall_ready          = eobool_false,
    .isASstrain_ready       = eobool_false,
    .isASmais_ready         = eobool_false,
    .BOARDisreadyforcontrolloop = eobool_false
//    .failedDetection        = {0}
};

static const char s_eobj_ownname[] = "EOtheEMSapplBody";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOemsapplbody_cfg_t *cfg)
{
    EOtheEMSapplBody *p = NULL;

    if(NULL == cfg)
    {
        return(NULL);
    }
    
    p = &s_applBody;

    // save in obj its configuration
    memcpy(&p->config, cfg, sizeof(eOemsapplbody_cfg_t));
    
   
    eo_entities_Initialise();  

    
    s_eo_emsapplBody_computeRunMode(p); // the run mode depends on connected can board (mc4, 2foc, only skin, etc)
    
    // compute mctype
    p->mctype = eoMCtype_none;
    if((applrunMode__mc4Only == p->appRunMode) || (applrunMode__skinAndMc4 == p->appRunMode))
    {
        p->mctype = eoMCtype_mc4maisbased;        
    }
    else if(applrunMode__2foc == p->appRunMode)
    {
        p->mctype = eoMCtype_2focbased;  
    } 

    
    s_eo_emsapplBody_CanServices_Init(p);
    
    
//    eo_mc4boards_Initialise(NULL);
            
    
//    s_eo_emsapplBody_encodersReader_init(p);        
//    s_eo_emsapplBody_emsController_init(p);
    
    // now i set the appl body as initted
    p->st = eo_emsApplBody_st__initted;
    
    eo_candiscovery2_Initialise(NULL);
    
    eo_emsapplBody_StartResourceCheck(p);
    
    // and i start some services   
//    eo_candiscovery_Initialise();
//    eo_candiscovery_Start(eo_candiscovery_GetHandle());
            
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


extern EOappEncReader* eo_emsapplBody_GetEncoderReader(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->appEncReader);
}



extern eOmn_appl_runMode_t eo_emsapplBody_GetAppRunMode(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(applrunMode__default);
    }
    return(p->appRunMode);
}



extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p)
{

    // in here we want to:
    // 1. case 2foc: do nothing for mc 
    // 2. case mc4: send broadcast policies
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(applrunMode__2foc == p->appRunMode)
    {
        return(eores_OK);
        //return(eo_strain_SendTXmode(eo_strain_GetHandle()));
    } 
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        eo_mc4boards_BroadcastStart(eo_mc4boards_GetHandle());
    }  

    return(eores_OK);
    
}


extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p)
{    
    // in here we want to:
    // 1. case 2foc: stop mc ....
    // 2. case mc4: reset broadcast policies 
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(applrunMode__2foc == p->appRunMode)
    {
        eo_emsMotorController_GoIdle();
        return(eores_OK);
    } 
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        eo_mc4boards_BroadcastStop(eo_mc4boards_GetHandle());        
    }  

    return(eores_OK);
        
}




extern eOemsapplbody_MCtype eo_emsapplBody_GetMCtype(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eoMCtype_none);
    }
    
    return(p->mctype);
}



extern eObool_t eo_emsapplBody_isMCready(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->isMCall_ready);    
}

extern eObool_t eo_emsapplBody_isMAISready(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->isASmais_ready);    
}


extern eObool_t eo_emsapplBody_isSTRAINready(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->isASstrain_ready);    
}


extern eObool_t eo_emsapplBody_isreadyforcontrolloop(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->BOARDisreadyforcontrolloop);    
}

extern eOresult_t eo_emsapplBody_SendDiscoveryFailureReport(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    
    return(eores_OK);    
}

//extern const eOcandiscovery_detection_t* eo_emsapplBody_GetFailedDetection(EOtheEMSapplBody *p)
//{
//    if(NULL == p)
//    {
//        return(NULL);
//    }   
//    
//    if(eobool_false == p->BOARDisreadyforcontrolloop)
//    {
//        return(&p->failedDetection);
//    }     
//}


extern eOresult_t eo_emsapplBody_StartResourceCheck(EOtheEMSapplBody *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
        
    p->BOARDisreadyforcontrolloop = eobool_false;
    

#if 0

    // test code with a strain board

    
    eOcandiscovery_target_t target = {0};

    target.boardtype = eobrd_cantype_strain; //eobrd_cantype_mais;//eobrd_cantype_strain;
    target.canmap[0] = 0x0008; //0x0018;
    target.protocolversion.major = 1; // 1.0
    target.protocolversion.minor = 0;
    target.firmwareversion.major = 3; // 3.0
    target.firmwareversion.minor = 0;    
    
    target.onStop = s_onstop_search4test;

    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &target);    

#else

    // that is an intermediate code. so far, at the date of 15sept15, we check only the mc boards (and the mais).
    
    if(eoMCtype_none == p->mctype)
    {
        p->BOARDisreadyforcontrolloop = eobool_true;
        return(eores_OK);
    }
    
    if(eoMCtype_2focbased == p->mctype)
    {
        // start the discovery on the 2foc boards. 
        // as callback we just set BOARDisreadyforcontrolloop true
        
        const eOcandiscovery_target_t *t = eoboardconfig_code2mcdiscoverytarget(eoprot_board_local_get());
        eOcandiscovery_onstop_t onstop = {0};
        onstop.function = s_onstop_search42foc;
        onstop.parameter = NULL;
        
        eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), t, &onstop);
        
    }
    else if(eoMCtype_mc4maisbased == p->mctype)
    {
        // start the discovery on the mc4 boards.
        // as callback we start discovery on mais. as further callback we start the mais, activate broadcast on mc4 boards, and then we set BOARDisreadyforcontrolloop true
      
        const eOcandiscovery_target_t *t = eoboardconfig_code2mcdiscoverytarget(eoprot_board_local_get());
        eOcandiscovery_onstop_t onstop = {0};
        onstop.function = s_onstop_search4mc4;
        onstop.parameter = NULL;

        eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), t, &onstop);      
    }

#endif
    
    return(eores_OK);       
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#if 0
static eOresult_t s_onstop_search4test(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    
    static uint8_t ciao = 0;
    const eOcandiscovery_detection_t *detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
    const eOcandiscovery_target_t* target = eo_candiscovery2_GetTarget(eo_candiscovery2_GetHandle());
    if((detection->atleastonereplyisincompatible == eobool_true) || (target->boardtype == eobrd_cantype_strain))
    {
        ciao = 1;
    } 
    
//    for(;;);
    
    return(eores_OK);
}
#endif

static eOresult_t s_onstop_search42foc(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        s_applBody.isMCall_ready = eobool_true;
        
        // see if we have a strain and if we need to start its discovery as well
        const eOcandiscovery_target_t *t = eoboardconfig_code2straindiscoverytarget(eoprot_board_local_get());
        if(NULL == t)
        {   // if we dont have a strain to check, i start teh 2foc straight away
            
            s_applBody.BOARDisreadyforcontrolloop = eobool_true;
        
            // start the services for 2foc ...
            s_eo_emsapplBody_encodersReader_init(&s_applBody);        
            s_eo_emsapplBody_emsController_init(&s_applBody);  
        }  
        else
        {   // if i have a strain to search, at first i search for it. then, if i find it i shall start the 2foc ... inside s_onstop_search4strain()
            eOcandiscovery_onstop_t onstop = {0};
            onstop.function = s_onstop_search4strain;
            onstop.parameter = NULL;

            eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), t, &onstop);  
        }  
    }
    else
    {
//        const eOcandiscovery_detection_t* det = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
//        memcpy(&s_applBody.failedDetection, det, sizeof(eOcandiscovery_detection_t));        
    }
    
    return(eores_OK);
}


static eOresult_t s_onstop_search4strain(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        s_applBody.isASstrain_ready = eobool_true;
        // marco.accame: the code in here 
        eo_strain_Initialise();     

        
        s_applBody.BOARDisreadyforcontrolloop = eobool_true;
    
        // start the services for 2foc ...
        s_eo_emsapplBody_encodersReader_init(&s_applBody);        
        s_eo_emsapplBody_emsController_init(&s_applBody);       
    }
    else
    {
//        const eOcandiscovery_detection_t* det = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
//        memcpy(&s_applBody.failedDetection, det, sizeof(eOcandiscovery_detection_t));        
    }
    
    return(eores_OK);
}


static eOresult_t s_onstop_search4mc4(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        s_applBody.isMCall_ready = eobool_true;

        const eOcandiscovery_target_t *t = eoboardconfig_code2maisdiscoverytarget(eoprot_board_local_get());
        eOcandiscovery_onstop_t onstop = {0};
        onstop.function = s_onstop_search4mais;
        onstop.parameter = NULL;

        eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), t, &onstop);         
    }
    else
    {
//        const eOcandiscovery_detection_t* det = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
//        memcpy(&s_applBody.failedDetection, det, sizeof(eOcandiscovery_detection_t));        
    }
    
    return(eores_OK);
}


static eOresult_t s_onstop_search4mais(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        s_applBody.isASmais_ready = eobool_true;
        s_applBody.BOARDisreadyforcontrolloop = eobool_true;
        
        // marco.accame: the code in here 
        eo_mc4boards_Initialise(NULL);
        if(eobool_true == eo_mc4boards_AreThere(eo_mc4boards_GetHandle()))
        {    
            eo_mc4boards_Config(eo_mc4boards_GetHandle());

            eo_mais_Initialise();
            eo_mais_Start(eo_mais_GetHandle());          
        }     
    }
    else
    {
//        const eOcandiscovery_detection_t* det = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
//        memcpy(&s_applBody.failedDetection, det, sizeof(eOcandiscovery_detection_t));        
    }
    
    return(eores_OK);
}


static eObool_t s_eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->hasdevice[dev]);
}

static void s_eo_emsapplBody_hid_canSP_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}



static void s_eo_emsapplBody_CanServices_Init(EOtheEMSapplBody *p)
{
    eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};
    
    config.mode                 = eocanserv_mode_straight;
    config.canstabilizationtime = 7*OSAL_reltime1sec;
    config.rxqueuesize[0]       = 64;
    config.rxqueuesize[1]       = 64;
    config.txqueuesize[0]       = 64;
    config.txqueuesize[1]       = 64;  
    config.onrxcallback[0]      = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
    config.onrxcallback[1]      = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
        
    eo_canserv_Initialise(&config);   
}


static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p)
{
    eOappEncReader_cfg_t cfg = { 0 }; // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    
    memcpy(&cfg, &p->config.encoderreaderconfig, sizeof(cfg));
    cfg.SPI_callbackOnLastRead = NULL;
    cfg.SPI_callback_arg = NULL;
    
    p->appEncReader = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->appEncReader), 
                     "error in appEncReader_New", s_eobj_ownname, &eo_errman_DescrTobedecided);

}

static eOemscontroller_board_t s_emscontroller_getboard(void)
{
    eOemscontroller_board_t type = emscontroller_board_NO_CONTROL;
    
    uint8_t n = eoprot_board_local_get();
    
    switch(n)
    {
        case 1:
        case 3:
        case 9:
        case 10:
        {
            type = emscontroller_board_NO_CONTROL;
        } break;

        case 0:
        case 2:
        {
            type = emscontroller_board_SHOULDER;
        } break;

        case 4:
        {
            type = emscontroller_board_WAIST;
        } break;

        case 5:
        case 7:
        {
            type = emscontroller_board_UPPERLEG;
        } break;
        
        case 6:
        case 8:
        {
            type = emscontroller_board_ANKLE;
        } break;

        case 14:
        {
            type = emscontroller_board_CER_WAIST;
        } break;
        
        default:
        {
            type = emscontroller_board_NO_CONTROL;
        } break;
    }
    
    
    return(type);   
}

static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p)
{
    uint8_t numofjoints = eo_entities_NumOfJoints(eo_entities_GetHandle());
    
    eOemscontroller_board_t boardtype = s_emscontroller_getboard();  
    p->emsController = eo_emsController_Init(boardtype, emscontroller_actuation_2FOC, numofjoints);
}



static eOresult_t s_eo_emsapplBody_computeRunMode(EOtheEMSapplBody *p)
{
    p->appRunMode = applrunMode__default;
        
    // at first i need to see if i have the device
    
    eOprotID32_t id32 = 0;
    eOcanmap_location_t location = {0};
    { // skin
        
        p->hasdevice[eo_emsapplbody_deviceid_skin] = eobool_false;
        // however, if we have a skin can board, then ...
        id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, 0, 0);
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            p->hasdevice[eo_emsapplbody_deviceid_skin] = eobool_true;
        }       
    }
    { // mc4 or 1foc
        
        p->hasdevice[eo_emsapplbody_deviceid_mc4] = eobool_false;
        p->hasdevice[eo_emsapplbody_deviceid_2foc] = eobool_false;
        // however, if we have a mc can board, then ... see the type
        id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 0, 0);
        eObrd_cantype_t board = eobrd_cantype_unknown;
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, &board))
        {
            if(eobrd_cantype_mc4 == board)
            {
                p->hasdevice[eo_emsapplbody_deviceid_mc4] = eobool_true;
            }
            else if(eobrd_cantype_1foc == board)
            {
                p->hasdevice[eo_emsapplbody_deviceid_2foc] = eobool_true;
            }            
        }       
    }    
    
    
    if(s_eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_2foc))
    {
        p->appRunMode = applrunMode__2foc;
        return(eores_OK);
    }
    
    if(s_eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_mc4))
    {
        if(s_eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin))
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
    
    if(s_eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin))
    {
        p->appRunMode = applrunMode__skinOnly;
        return(eores_OK);
    }

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "applrunmode is not valid", s_eobj_ownname, &eo_errman_DescrTobedecided);
    return(eores_NOK_generic);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



