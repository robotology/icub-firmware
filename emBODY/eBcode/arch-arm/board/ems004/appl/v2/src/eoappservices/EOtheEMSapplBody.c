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

#include "EOtheCANdiscovery.h"


#include "EOtheMC4boards.h"

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

//static void s_eo_emsapplBody_mc4data_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_CanServices_Init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p);

//static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_computeRunMode(EOtheEMSapplBody *p);


static void s_eo_emsapplBody_hid_canSP_cbkonrx(void *arg);

static eObool_t s_eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev);

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
//    .configMC4boards2use    = {0},
    .hasdevice              = {eobool_false, eobool_false, eobool_false}
};

static const char s_eobj_ownname[] = "EOtheEMSapplBody";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




extern EOtheEMSapplBody* eo_emsapplBody_Initialise(const eOemsapplbody_cfg_t *cfg)
{
    //eOresult_t res;
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
        

    eo_mc4boards_Initialise(NULL);
    
    //s_eo_emsapplBody_mc4data_init(p);  

    
    s_eo_emsapplBody_CanServices_Init(p);
    s_eo_emsapplBody_encodersReader_init(p);        
    s_eo_emsapplBody_emsController_init(p);
    
    // now i set the appl body as initted
    p->st = eo_emsApplBody_st__initted;
    

// debug   
//    eo_emsapplBody_EnableTxAllJointOnCan(p);
//    eo_mc4boards_BroadcastStart(eo_mc4boards_GetHandle());
//    s_eo_emsapplBody_sendConfig2canboards(p);
//    eo_mc4boards_ConfigShiftValues(eo_mc4boards_GetHandle());

    
    // and i start some services   
    eo_candiscovery_Initialise();
    eo_candiscovery_Start(eo_candiscovery_GetHandle());
            
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
//        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
//        uint8_t i=0;
//        // get the broadcast policy. it is somewhere in this object. the bcast policy is equal for all joints...
//        eo_emsapplbody_can_bcastpolicy_t *bcastpolicy = &p->configMC4boards2use.bcastpolicy;
//        eOcanprot_command_t command = {0};
//        command.class = eocanprot_msgclass_pollingMotorControl;
//        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
//        command.value = bcastpolicy;
//        for(i=0; i<numofjomos; i++)
//        {
//            // ok, now i send the value to the relevant address
//            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//        }
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
        
//        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
//        uint8_t i=0;
//        // get a broadcast policy of all zeros.
//        eo_emsapplbody_can_bcastpolicy_t bcastpolicy = {0};
//        eOcanprot_command_t command = {0};
//        command.class = eocanprot_msgclass_pollingMotorControl;
//        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
//        command.value = &bcastpolicy;
//        for(i=0; i<numofjomos; i++)
//        {
//            // ok, now i send the value to the relevant address
//            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//        }
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



//extern eOresult_t eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
//{    
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    static eObool_t already_send = eobool_false;
//    eOresult_t res;
//    
//    if(eobool_true == already_send)
//    {
//        return(eores_OK);
//    }
//        
//    
//    res = s_eo_emsapplBody_sendConfig2canboards(p);
//    
//    if(eores_OK == res)
//    {
//        already_send = eobool_true;
//    }
//    
//    return(res);
//}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static void s_eo_emsapplBody_hid_canSP_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}



//static void s_eo_emsapplBody_mc4data_init(EOtheEMSapplBody *p)
//{
//      
//    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
//    {
////        // must init the bcast policy and the shiftvalues and put it into configMC4boards2use
////        eo_emsapplbody_configMC4boards_t *cfgmc4 = &p->configMC4boards2use;
////        
////        // shiftvalues are all equal for the mc4 boards
////        cfgmc4->shiftvalues.jointVelocityShift                  = p->config.configdataofMC4boards.shiftvalues.jointVelocityShift;
////        cfgmc4->shiftvalues.jointVelocityEstimationShift        = p->config.configdataofMC4boards.shiftvalues.jointVelocityEstimationShift;
////        cfgmc4->shiftvalues.jointAccelerationEstimationShift    = p->config.configdataofMC4boards.shiftvalues.jointAccelerationEstimationShift;  
////        
////        // bcast policies are all equal for mc4 boards. we just convert them into flags
////        memset(&cfgmc4->bcastpolicy, 0, sizeof(cfgmc4->bcastpolicy)); 
////        uint8_t i = 0;
////        for(i=0; i<eoemsapplbody_bcastpolicylistsize; i++)
////        {
////            if(0 == p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i])
////            {
////                continue;
////            }
////            if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<9)
////            {
////                cfgmc4->bcastpolicy.val2bcastList[0] |= (1 <<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
////            }
////            else if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<17)
////            {
////                cfgmc4->bcastpolicy.val2bcastList[1] |= (1<<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
////            }
////        } 

//        // init the measure converter
////        eOmeasconv_cfg_t cfg = {0};
////        cfg.jointVelocityShift = p->configMC4boards2use.shiftvalues.jointVelocityShift;
////        cfg.jointVelocityEstimationShift = p->configMC4boards2use.shiftvalues.jointVelocityEstimationShift;    
////        cfg.jointAccEstimationShift = p->configMC4boards2use.shiftvalues.jointAccelerationEstimationShift;
////        eo_measconv_Initialise(&cfg);
//        
//    }
//    

//}

static void s_eo_emsapplBody_CanServices_Init(EOtheEMSapplBody *p)
{
    eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};
    
    config.mode             = eocanserv_mode_straight;
    config.rxqueuesize[0]   = 64;
    config.rxqueuesize[1]   = 64;
    config.txqueuesize[0]   = 64;
    config.txqueuesize[1]   = 64;  
    config.onrxcallback[0]  = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[0]  = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
    config.onrxcallback[1]  = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[1]  = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
    
    
    eo_canserv_Initialise(&config);
    
}


static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p)
{
    eOappEncReader_cfg_t cfg = { 0 }; // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    
    memcpy(&cfg.streams, &p->config.encoderstreams, sizeof(cfg.streams));
    cfg.callbackOnLastRead = NULL;
    cfg.callback_arg = NULL;
    
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


//static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
//{
//    // in here i send to mc4 boards the commands to assign the shift values

//    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
//    {
//        // only mc4
//        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
//        uint8_t i=0;
//        // get the shift value policy. it is somewhere in this object. the shift calues are equal for all joints...
//        eo_emsapplbody_can_shiftvalues_t *shiftvalues = &p->configMC4boards2use.shiftvalues;
//        eOcanprot_command_t command = {0};
//        command.class = eocanprot_msgclass_pollingMotorControl;

//        for(i=0; i<numofjomos; i++)
//        {
//            // ok, now i send the value to the relevant address
//            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
//            
//            // first is ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT
//            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
//            command.value = &shiftvalues->jointVelocityShift;                       
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//            
//            // second is ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT
//            icubCanProto_estimShift_t estimshift = {0};
//            estimshift.estimShiftJointVel = shiftvalues->jointVelocityEstimationShift;
//            estimshift.estimShiftJointAcc = shiftvalues->jointAccelerationEstimationShift;
//            estimshift.estimShiftMotorVel = 0;
//            estimshift.estimShiftMotorAcc = 0;
//            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
//            command.value = &estimshift;                       
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//        }        
//        
//    }
//    
//    return(eores_OK);

//}


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



