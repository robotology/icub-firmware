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




//application
#include "EOMtheEMSapplCfg.h"


#include "EOaction.h"

#include "EOtheEntities.h"

#include "EoError.h"

#include "EOVtheSystem.h"


#include "iCubCanProto_motorControlMessages.h"

#include "EOtheCANservice.h"

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

#define OLDMODE 0


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


//const eOemsapplbody_cfg_t eo_emsapplbody_cfg_default = 
//{
//    .encoderstreams =
//    {
//        {
//            .type       = hal_encoder_t1,
//            .numberof   = 2,
//            .encoders   = { hal_encoder1, hal_encoder3, hal_encoderNONE }        
//        },
//        {
//            .type       = hal_encoder_t1,
//            .numberof   = 2,
//            .encoders   = { hal_encoder2, hal_encoder4, hal_encoderNONE }     
//        }        
//    },
//    .icubcanprotoimplementedversion =
//    {
//        .major                      = 1,
//        .minor                      = 2
//    },
//    .configdataofMC4boards          =
//    {
//        .shiftvalues    =
//        {
//            .jointVelocityShift                 = 8,
//            .jointVelocityEstimationShift       = 8,
//            .jointAccelerationEstimationShift   = 5
//        },
//        .bcastpolicy            =
//        {
//            .val2bcastList      =
//            {
//            /* 0 */ ICUBCANPROTO_PER_MC_MSG__POSITION,
//            /* 1 */ ICUBCANPROTO_PER_MC_MSG__STATUS,
//            /* 2 */ ICUBCANPROTO_PER_MC_MSG__PRINT,
//            /* 3 */ ICUBCANPROTO_PER_MC_MSG__PID_VAL
//            }
//        }
//    }        
//};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_emsapplBody_mc4data_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_canServicesProvider_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p);
static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_computeRunMode(EOtheEMSapplBody *p);

//static eOresult_t s_eo_emsapplBody_MaisTXenable(EOtheEMSapplBody *p);
// marco.accame on 19feb15: we dont want to disable mais transmission unless robotinterface does that.
//static eOresult_t s_eo_emsapplBody_MaisTXdisable(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_MaisStart(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_SendTxMode2Strain(EOtheEMSapplBody *p);
static eOresult_t s_eo_emsapplBody_DisableTxStrain(EOtheEMSapplBody *p);

static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t dontaskmask);

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
    .checkCanBoards_timer   = NULL,
    .appEncReader           = NULL,
    .emsController          = NULL,
    .configMC4boards2use    = {0},
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
    
   
    
    s_eo_emsapplBody_computeRunMode(p); // the run mode depends on connected can board (mc4, 2foc, only skin, etc)
        
    eo_entities_Initialise();
        
    s_eo_emsapplBody_mc4data_init(p);    
    s_eo_emsapplBody_canServicesProvider_init(p);
    s_eo_emsapplBody_encodersReader_init(p);        
    s_eo_emsapplBody_emsController_init(p);
    
    // now i set the appl body as initted
    p->st = eo_emsApplBody_st__initted;
    

    
    // and i start some services   
    p->checkCanBoards_timer = eo_timer_New();    
    eo_emsapplBody_checkCanBoards_Start(p);
            
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, "EOtheEMSapplBody started", s_eobj_ownname, &eo_errman_DescrRunningHappily);
       
    return(p);
}



extern EOtheEMSapplBody* eo_emsapplBody_GetHandle(void)
{
    return((s_applBody.st == eo_emsApplBody_st__initted) ? (&s_applBody) : (NULL));
    //return(&s_applBody);
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



extern eOresult_t eo_emsapplBody_MAISstart(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    
    //return(s_eo_emsapplBody_MaisTXenable(p));

    // the following sets tx and datarate according to config of mais.
    return(s_eo_emsapplBody_MaisStart(p));    
}

#warning marco.accame: in here it would be good to separate as-strain from mc.
extern eOresult_t eo_emsapplBody_EnableTxAllJointOnCan(EOtheEMSapplBody *p)
{

    // in here we want to:
    // 1. case 2foc: do nothing for mc and start strain ....
    // 2. case mc4: send broadcast policies
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(applrunMode__2foc == p->appRunMode)
    {
        return(s_eo_emsapplBody_SendTxMode2Strain(p));
    } 
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
        uint8_t i=0;
        // get the broadcast policy. it is somewhere in this object. the bcast policy is equal for all joints...
        eo_emsapplbody_can_bcastpolicy_t *bcastpolicy = &p->configMC4boards2use.bcastpolicy;
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
        command.value = bcastpolicy;
        for(i=0; i<numofjomos; i++)
        {
            // ok, now i send the value to the relevant address
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
        }
    }  

    return(eores_OK);
    
    
    
//#if OLDMODE    
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    eOresult_t                          res;
//    uint16_t                            numofjoint, i;
//    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;
//    eOicubCanProto_msgCommand_t         msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
//    };

//    if(applrunMode__2foc == p->appRunMode)
//    {
//        res = s_eo_emsapplBody_SendTxMode2Strain(p);
//        return(res);
//    }
//    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
//    {   
//        

//        numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
//        
//        for(i=0; i<numofjoint; i++)
//        {
//            //get bcast policy from db
//            res = eo_appTheDB_GetJointBcastpolicyPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)i, &bcastpolicy_ptr);
//            if(eores_OK != res)
//            {
//                return(res);
//            }
//
//            res = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, (void*)&(bcastpolicy_ptr->val2bcastList[0]));
//            
//            
////            eOerrmanDescriptor_t errdes = {0};
////            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_DEB_tag00);
////            errdes.par16                = (i & 0x000f) | (0xc1a0);
////            errdes.par64                = (eores_OK == res) ? 0x11 : 0x10;
////            errdes.sourcedevice         = 0;
////            errdes.sourceaddress        = 0;                
////            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, s_eobj_ownname, &errdes);            
//        }
//        
//        /* per ora non si e' verificato nessun problema se l'applicazione e' in cfg e la pelle spedisce a manetta*/
//        return(res);
//    }
//    else
//    {
//        return(eores_OK);
//    }
//    
//#endif    
}


extern eOresult_t eo_emsapplBody_DisableTxAllJointOnCan(EOtheEMSapplBody *p)
{
    
    // in here we want to:
    // 1. case 2foc: stop mc and stop strain ....
    // 2. case mc4: reset broadcast policies 
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(applrunMode__2foc == p->appRunMode)
    {
        eo_emsMotorController_GoIdle();
        eOresult_t res = s_eo_emsapplBody_DisableTxStrain(p);
        return(res);
    } 
    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
        uint8_t i=0;
        // get a broadcast policy of all zeros.
        eo_emsapplbody_can_bcastpolicy_t bcastpolicy = {0};
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
        command.value = &bcastpolicy;
        for(i=0; i<numofjomos; i++)
        {
            // ok, now i send the value to the relevant address
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
        }
    }  

    return(eores_OK);
        
    
//#if OLDMODE    
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    eOresult_t                          res;
//    uint16_t                            numofjoint, i;
//    eOicubCanProto_bcastpolicy_t        bcastpolicy =
//    {   
//        EO_INIT(.val2bcastList)         {0, 0, 0, 0}
//    };
//    
//    eOicubCanProto_msgCommand_t         msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY
//    };

//    if(applrunMode__2foc == p->appRunMode)
//    {
//        eo_emsMotorController_GoIdle();
//        res = s_eo_emsapplBody_DisableTxStrain(p);
//        return(res);
//    }
//    else if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
//    {    
//        // 2) config joints
//        numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
//        
//        for(i=0; (i<numofjoint) && (eores_OK == res); i++)
//        {
//            res = eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i, msgCmd, (void*)&(bcastpolicy.val2bcastList[0]));
//        }
//        
//        #warning marco.accame asks: shall we send the mc4 to idle in here? the 2foc boards are sent to idle.
//        
//        // marco.accame on 19feb15: we dont want to disable mais transmission unless robotinterface does that.
//        // res = s_eo_emsapplBody_MaisTXdisable(p);
//        
//        return(res);
//    }
//    else
//    {
//        return(eores_OK);
//    }
//#endif
}


static eObool_t s_eo_emsapplBody_HasDevice(EOtheEMSapplBody *p, eo_emsapplbody_deviceid_t dev)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->hasdevice[dev]);
}


extern eOresult_t eo_emsapplBody_checkCanBoardsAreReady(EOtheEMSapplBody *p, uint32_t dontaskmask)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(s_eo_emsapplBody_sendGetFWVersion(p, dontaskmask));
}

// must return true only if all can boards have replied and their protocol version matches with target
extern eObool_t eo_emsapplBody_areCanBoardsReady(EOtheEMSapplBody *p, uint32_t *canBoardsReady, uint32_t *canBoardsChecked)
{
    eObool_t res = eobool_true;
    
    if((NULL == p) || (NULL == canBoardsReady))
    {
        return(eobool_false);
    }
    
    *canBoardsReady = 0;
    
    uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {
        if(NULL != canBoardsChecked)
        {
            *canBoardsChecked |= (1<<i);
        }
        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            // must retrieve the extended board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            eObool_t ready = eobool_false;
            uint8_t major = board->detected.protocolversion.major;
            uint8_t minor = board->detected.protocolversion.minor;
            if(0 != major)
            {  // if not zero, then the board has replied and everything is filled. however i must check if there is a match
                if((board->board.props.requiredprotocol.major == major) && (board->board.props.requiredprotocol.minor == minor))
                {
                    ready = eobool_true;
                    *canBoardsReady |= (1<<i);
                }
            } 
            res &= ready;
        }               
    }
    
    return(res);       
}


extern eOresult_t eo_emsapplBody_checkCanBoards_Stop(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_emsapplBody_SignalDetectedCANboards(eo_emsapplBody_GetHandle());
    return(eo_timer_Stop(p->checkCanBoards_timer));
}


extern eOresult_t eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    static eObool_t already_send = eobool_false;
    eOresult_t res;
    
    if(eobool_true == already_send)
    {
        return(eores_OK);
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
    // i must see if there are skin entities.
    // if any, then for each skin: for each board in teh skin i send a SET_TX_MODE equal to icubCanProto_as_sigmode_dontsignal
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    uint8_t numofskins = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    
    if(0 == numofskins)
    {
        return(eores_OK); 
    }

    
    eOcanprot_command_t command = {0};
    icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
    command.class = eocanprot_msgclass_pollingSkin;
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;       
    command.value = &sigmode;

    uint8_t i=0;
    eOsk_skin_t *skin = NULL;
    for(i=0;i<numofskins; i++)
    {
        // i stop this skin only if it was started before
        if(NULL == (skin = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)i)))
        {   
            continue;   // i dont have this skin ... i go the next one
        }
            
        if(eosk_sigmode_dontsignal == skin->config.sigmode)
        {   
            // if the skin was not initted by robot-interface, then i dont deinit it. the reason is twofold:
            // 1. if the skin boards dont transmit, there is no need to silence them,
            // 2. in case the .ini file of robotinterface has disable the skin because the skin is not mounted, i dont want to tx on a disconnected can bus.
            continue;   // i dont need to silence this skin ... i go the next one
        }
            
        // i set the skin as not transmitting as soon as possible. because in such a way, can messages being received
        // are not pushed back in skin->status.arrayofcandata and its does not overflow.
        skin->config.sigmode = eosk_sigmode_dontsignal;
              
        // then i stop transmission of each skin can board
       
        // i get the addresses of the can boards of the i-th skin.
        // the simplification we use is that they all are on the same CAN bus and all have consecutive addresses.
        // we send the same command to all of them
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, i, 0);
        eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32);    
    }
    
    return(eores_OK);
    
    

//#if OLDMODE    
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    uint8_t numofskin = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
//    EOappCanSP *appCanSP = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    eOresult_t res = eores_NOK_generic;
//            
//    if(numofskin > 0)
//    {
//        uint8_t                         i, j;
//        eOappTheDB_cfg_skinInfo_t       *skconfig_ptr = NULL;
//        uint8_t                         boardEndAddr = 0;;
//        eOicubCanProto_msgDestination_t msgdest = {0};
//              
//        eOicubCanProto_msgCommand_t msgCmd = 
//        {
//            EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//            EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
//        };
//        
//        
//        for(j=0; j<numofskin; j++) 
//        {
//            // i stop the skin only if it was started before
//            eOsk_skin_t *skin = (eOsk_skin_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)j);
//            if(NULL == skin)
//            {   // i dont have a skin on j
//                return(eores_NOK_generic);
//            }
//            
//            if(eosk_sigmode_dontsignal == skin->config.sigmode)
//            {   
//                // if the skin was not initted by robot-interface, then i dont deinit it. the reason is twofold:
//                // 1. if the skin boards dont transmit, there is no need to silence them,
//                // 2. in case the .ini file of robotinterface has disable the skin because the skin is not mounted, i dont want to tx on a disconnected can bus.
//                return(eores_OK);
//            }
//            
//            
//            res = eo_appTheDB_GetSkinConfigPtr(eo_appTheDB_GetHandle(), j,  &skconfig_ptr);
//            if(eores_OK != res)
//            {  
//                return(eores_NOK_generic);
//            }
//            
//            // i set the skin as not transmitting as soon as possible. because in such a way, can messages being received
//            // are not pushed back in skin->status.arrayofcandata and its does not overflow.
//            skin->config.sigmode = eosk_sigmode_dontsignal;
//              
//            // then i stop transmission of each skin can board
//            
//            boardEndAddr = skconfig_ptr->boardAddrStart + skconfig_ptr->numofboards;
//                             
//            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++) 
//            {   // for each skin board 
//                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
//                icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
//                
//                res = eo_appCanSP_SendCmd(appCanSP, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &sigmode);
//                if(eores_OK != res)
//                {
//                    return(eores_NOK_generic);
//                }
//            }
//        }    
//    }
//    
//    return(eores_OK);
//#endif    
}


// marco.accame: at the date of 20feb2015 it highly temporary.

static eObool_t maisfound = eobool_false;

extern eOresult_t eo_emsapplBody_discovery_Mais_Start(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // marco.accame: in here is should start a discovery procedure about mais board.
    // proposal:
    // we instantiate a timer with a callback that checks its fw version by sending a can frame
    // when the mais sends it back, the callback call eo_emsapplBody_discovery_Mais_Stop()
    // whcih in turns call s_eo_emsapplBody_MaisStart().
    
    
    // this variable or a similar one will be set to true by the can callback upon rx of the correct fw version
    maisfound = eobool_true;
    
    eo_emsapplBody_discovery_Mais_Stop(p);
        
        
    return(eores_OK);
}


extern eOresult_t eo_emsapplBody_discovery_Mais_Stop(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // after i have found the mais, then i start it.
    // this 
    
    if(eobool_true == maisfound)
    {
        s_eo_emsapplBody_MaisStart(p);
    }
    
    return(eores_OK);   
}


extern eOresult_t eo_emsapplBody_checkCanBoards_Start(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    EOaction_strg astg = {0};
    EOaction *action = (EOaction*)&astg;
       
    eo_action_SetEvent(action, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_timer_Start(p->checkCanBoards_timer, eok_abstimeNOW, 250*eok_reltime1ms, eo_tmrmode_FOREVER, action);
    
    return(eores_OK);
}


extern eOresult_t eo_emsapplBody_SignalDetectedCANboards(EOtheEMSapplBody *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eOerrmanDescriptor_t des = {0};
    des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_board_detected);
    
    uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t loc = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc, NULL, NULL))
        {
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), loc);
            des.sourcedevice    = (eOcanport1 == loc.port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            des.sourceaddress   = loc.addr;
            des.par16           = i; 
            memcpy(&des.par64, &board->detected, sizeof(eObrd_typeandversions_t));
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &des);
        }               
    }
     
    return(eores_OK);
    
//#if OLDMODE    
//    
//    // maybe in here we can put an info diagnostics message    
//    // send message about the ready boards
//    uint8_t numcanboards = eo_appTheDB_GetNumberOfCanboards(eo_appTheDB_GetHandle());
//    uint8_t i = 0;
//    eOappTheDB_board_canlocation_t loc = {0};
//    //eObrd_cantype_t exptype = eobrd_cantype_unknown;
//    eObrd_typeandversions_t expected = {0};
//    expected.boardtype = eobrd_cantype_unknown;
//    eObrd_typeandversions_t detected = {0};
//    
//    eOerrmanDescriptor_t des = {0};
//    des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
//    
//    for(i=0; i<numcanboards; i++)
//    {
//        if(eores_OK == eo_appTheDB_GetCanDetectedInfo(eo_appTheDB_GetHandle(), i, &loc, &expected, &detected))
//        {
//            // fill the message. so far i use a debug with can-id-typedetected-typeexpectde
//            des.sourcedevice    = (eOcanport1 == loc.emscanport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
//            des.sourceaddress   = loc.addr;
//            des.par16           = (expected.boardtype << 8) | ((detected.boardtype) & 0xff); 
//            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &des);
//        }                    
//    }
//    
//    return(eores_OK);
//#endif    
}


//extern eObool_t eom_emsapplBody_IsCANboard_usedbyMC(EOtheEMSapplBody *p, eObrd_cantype_t type)
//{
//    eObool_t ret = eobool_true;
//    
//    switch(type)
//    {
//        case eobrd_cantype_mc4: 
//        case eobrd_cantype_1foc:
//        { 
//            ret = eobool_true; 
//        } break;
//        
//        case eobrd_cantype_strain:
//        { 
//            ret = eobool_false; 
//        } break;            
//        
//        case eobrd_cantype_mais:
//        case eobrd_cantype_skin:            
//        case eobrd_cantype_unknown:
//        default:
//        {
//            ret = eobool_false; 
//        } break;                    
//    }
// 
//    return(ret);
//}


//extern eObool_t eom_emsapplBody_IsCANboard_usedbyAS(EOtheEMSapplBody *p, eObrd_cantype_t type)
//{
//    eObool_t ret = eobool_true;
//    
//    switch(type)
//    {
//        case eobrd_cantype_mc4: 
//        case eobrd_cantype_1foc:
//        { 
//            ret = eobool_false; 
//        } break;
//        
//        case eobrd_cantype_mais:
//        case eobrd_cantype_strain:
//        { 
//            ret = eobool_true; 
//        } break;            
//        
//        
//        case eobrd_cantype_skin:            
//        case eobrd_cantype_unknown:
//        default:
//        {
//            ret = eobool_false; 
//        } break;                    
//    }
// 
//    return(ret);
//}


//extern eObool_t eom_emsapplBody_IsCANboard_usedbySK(EOtheEMSapplBody *p, eObrd_cantype_t type)
//{
//    eObool_t ret = eobool_true;
//    
//    switch(type)
//    {
//        case eobrd_cantype_mc4: 
//        case eobrd_cantype_1foc:
//        { 
//            ret = eobool_false; 
//        } break;
//        
//        case eobrd_cantype_mais:
//        case eobrd_cantype_strain:
//        { 
//            ret = eobool_false; 
//        } break;            
//        
//        
//        case eobrd_cantype_skin:  
//        {
//            ret = eobool_true;
//        } break;
//        
//        case eobrd_cantype_unknown:
//        default:
//        {
//            ret = eobool_false; 
//        } break;                    
//    }
// 
//    return(ret);
//}

// for now it is the same as eom_emsapplBody_IsCANboard_usedbyMC()
extern eObool_t eom_emsapplBody_IsCANboardToBeChecked(EOtheEMSapplBody *p, eObrd_cantype_t type)
{
    eObool_t ret = eobool_true;
    
    switch(type)
    {
        case eobrd_cantype_mc4: 
        case eobrd_cantype_1foc:
        { 
            ret = eobool_true; 
        } break;
        
        case eobrd_cantype_strain:
        { 
            ret = eobool_false; 
        } break;            
        
        case eobrd_cantype_mais:
        case eobrd_cantype_skin:            
        case eobrd_cantype_unknown:
        default:
        {
            ret = eobool_false; 
        } break;                    
    }
 
    return(ret);
}

extern eOresult_t eom_emsapplBody_checkCanBoards_ManageDetectedFWversion(EOtheEMSapplBody *p, eOcanmap_location_t loc, eObool_t match, eObrd_typeandversions_t *detected)
{
    eOresult_t res = eores_OK;
    
    // at first we send diagnostics
    if(eobool_false == match)
    {
        char str[64] = {0};
        snprintf(str, sizeof(str), "wrong fwver: %d %d", detected->firmwareversion.major, detected->firmwareversion.minor); 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &eo_errman_DescrUnspecified);
    }
    
    // then we put the detected inside the EOtheCANmapping
    eo_canmap_BoardSetDetected(eo_canmap_GetHandle(), loc, detected);
 

    #warning marco.accame: i prefer not to stop the request procedure in here. i would rather do it at tick of the timer inside eom_emsconfigurator_hid_userdef_ProcessUserdef01Event()
//    // if all connected can boards are ready, then we stop the request procedure
//    if(eobool_true == eo_appTheDB_areConnectedCanBoardsReady(db, &canBoardsReady, NULL))
//    {
//        eo_emsapplBody_checkCanBoards_Stop(eo_emsapplBody_GetHandle());
//        eo_emsapplBody_sendConfig2canboards(eo_emsapplBody_GetHandle());
//        //if MC4, enable MAIS and BCastPolicy
//        eOmn_appl_runMode_t appl_run_mode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
//        if((applrunMode__skinAndMc4 == appl_run_mode) || (applrunMode__mc4Only == appl_run_mode))
//        {   
//			eo_emsapplBody_MAISstart(eo_emsapplBody_GetHandle());
//        }
//    }    
    
    return(res);
}


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



static void s_eo_emsapplBody_mc4data_init(EOtheEMSapplBody *p)
{
      
    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        // must init the bcast policy and the shitvalues and put it into configMC4boards2use
        eo_emsapplbody_configMC4boards_t *cfgmc4 = &p->configMC4boards2use;
        
        // shiftvalues are all equal for the mc4 boards
        cfgmc4->shiftvalues.jointVelocityShift                  = p->config.configdataofMC4boards.shiftvalues.jointVelocityShift;
        cfgmc4->shiftvalues.jointVelocityEstimationShift        = p->config.configdataofMC4boards.shiftvalues.jointVelocityEstimationShift;
        cfgmc4->shiftvalues.jointAccelerationEstimationShift    = p->config.configdataofMC4boards.shiftvalues.jointAccelerationEstimationShift;  
        
        // bcast policies are all equal for mc4 boards. we just convert them into flags
        memset(&cfgmc4->bcastpolicy, 0, sizeof(cfgmc4->bcastpolicy)); 
        uint8_t i = 0;
        for(i=0; i<eoemsapplbody_bcastpolicylistsize; i++)
        {
            if(0 == p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i])
            {
                continue;
            }
            if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<9)
            {
                cfgmc4->bcastpolicy.val2bcastList[0] |= (1 <<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
            }
            else if(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]<17)
            {
                cfgmc4->bcastpolicy.val2bcastList[1] |= (1<<(p->config.configdataofMC4boards.bcastpolicy.val2bcastList[i]-1));
            }
        } 

        // init the measure converter
        eOmeasconv_cfg_t cfg = {0};
        cfg.jointVelocityShift = p->configMC4boards2use.shiftvalues.jointVelocityShift;
        cfg.jointVelocityEstimationShift = p->configMC4boards2use.shiftvalues.jointVelocityEstimationShift;    
        cfg.jointAccEstimationShift = p->configMC4boards2use.shiftvalues.jointAccelerationEstimationShift;
        eo_measconv_Initialise(&cfg);
        
    }
    

}

static void s_eo_emsapplBody_canServicesProvider_init(EOtheEMSapplBody *p)
{
    
    #warning TBD: init the can services  in here 

    eOcanserv_cfg_t config = {0};
    
    config.mode             = eocanserv_mode_straight;
    config.rxqueuesize[0]  = 64;
    config.rxqueuesize[1]  = 64;
    config.txqueuesize[0]  = 64;
    config.txqueuesize[1]  = 64;  
    config.onrxcallback[0] = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[0] = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
    config.onrxcallback[1] = s_eo_emsapplBody_hid_canSP_cbkonrx; 
    config.onrxargument[1] = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
    
    
    eo_canserv_Initialise(&config);
    
    
    
//#if OLDMODE
//    eOappCanSP_cfg_t cfg = 
//    {
//        .cbkonrx   = 
//        {
//            // port1
//            {
//                .fn = eo_emsapplBody_hid_canSP_cbkonrx,
//                .argoffn = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle())
//            },
//            // port2
//            {
//                .fn = eo_emsapplBody_hid_canSP_cbkonrx,
//                .argoffn = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle())
//            },
//        }
//    };
//
//    
//    if(!(eo_emsapplBody_HasDevice(p, eo_emsapplbody_deviceid_skin)))
//    {
//        cfg.cbkonrx[eOcanport2].fn = eo_emsapplBody_hid_canSP_cbkonrx;
//        cfg.cbkonrx[eOcanport2].argoffn = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
//    }
//    p->bodyobjs.appCanSP = eo_appCanSP_New(&cfg);
//
//    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->bodyobjs.appCanSP), 
//                     "error in appCanSP_New", s_eobj_ownname, &eo_errman_DescrTobedecided);
//    
//#endif
}


static void s_eo_emsapplBody_encodersReader_init(EOtheEMSapplBody *p)
{
    eOappEncReader_cfg_t cfg = { 0 }; // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    
    memcpy(&cfg.streams, &p->config.encoderstreams, sizeof(cfg.streams));
    //cfg.connectedEncodersMask = p->config.connectedEncodersMask;
    cfg.callbackOnLastRead = NULL;
    cfg.callback_arg = NULL;
    
    p->appEncReader = eo_appEncReader_New(&cfg);

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p->appEncReader), 
                     "error in appEncReader_New", s_eobj_ownname, &eo_errman_DescrTobedecided);

}



static void s_eo_emsapplBody_emsController_init(EOtheEMSapplBody *p)
{
    //uint16_t i, numofjoint = 0;
    
    p->emsController = eo_emsController_Init(NAXLES);

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



static eOresult_t s_eo_emsapplBody_sendGetFWVersion(EOtheEMSapplBody *p, uint32_t dontaskmask)
{  
    // i check only mc. thus, i get the number of mc jomos, i search for the i-th board and i send a get fw version query to it.
    
    uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {
        if( ((1<<i) & dontaskmask) == (1<<i))
        {
            continue;
        }
        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            // i decide to send the request only if the board has not replied. if it replied but the protocol does
            // not match then i dont send the command anymore.
            if(0 == board->detected.protocolversion.major)
            {  // must ask if the board has not responded yet
                eOcanprot_command_t command = {0};
                command.class = eocanprot_msgclass_pollingMotorControl;
                command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
                command.value = (void*)&board->board.props.requiredprotocol;
                eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);                  
            }                        
        }               
    }
    
    return(eores_OK);
                
//#if OLDMODE    
//    
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION
//    };

//    eOicubCanProto_msgDestination_t     msgdest;
//    uint16_t                            numofboard = eo_appTheDB_GetNumberOfCanboards(eo_appTheDB_GetHandle());
//    uint16_t                            i;
//    eOresult_t                          res;
//    eOappTheDB_canboardinfo_t           *canboardinfo;
//    EOappTheDB                          *db = eo_appTheDB_GetHandle();

//    for(i=0; i<numofboard; i++)
//    {
//        
//        if( ((1<<i) & dontaskmask) == (1<<i))
//        {
//            continue;
//        }
//        
//        res = eo_appTheDB_GetCanBoardInfo(db, (eObrd_boardId_t)i, &canboardinfo);
//        if(eores_OK != res)
//        {
//            continue;
//        }
//        
////        if((eobrd_cantype_mc4 != canboardinfo->type) && (eobrd_cantype_1foc != canboardinfo->type))
////        {
////            continue; // m.a: i dont process this board index and i go to the next one
////        }

//        if(eobool_false == eom_emsapplBody_IsCANboardToBeChecked(eo_emsapplBody_GetHandle(), canboardinfo->type))
//        {
//            continue;
//        }
//        
//        // marco.accame: dont know why if i add the strain to the types of board to be checked, then the strain (orn can2) does not reply
//        // maybe it is because it exits the bootloader 5 seconds after the application starts ... so, i have tried to delay the query about 6-7 seconds
//        // but it does not solve. 
//        //if(eobrd_cantype_strain == canboardinfo->type)
//        //{
//        //    eOabstime_t timefromboot = eov_sys_LifeTimeGet(eov_sys_GetHandle());
//        //    
//        //    if(timefromboot < 7*eok_reltime1sec)
//        //    {
//        //        continue;
//        //    }
//        //}
//        
//        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canboardinfo->addr);
//                        
//        res = eo_appCanSP_SendCmd(p->appCanSP, (eOcanport_t)canboardinfo->port, msgdest, msgCmd, (void*)&canboardinfo->canprotversion);
//        if(eores_OK != res)
//        {
//            return(res);
//        }
//    }

//    return(eores_OK);
//    
//#endif

    
}


static eOresult_t s_eo_emsapplBody_sendConfig2canboards(EOtheEMSapplBody *p)
{
    // in here i send to mc4 boards the commands to assign the shift values

    if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
    {
        // only mc4
        uint8_t numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
        uint8_t i=0;
        // get the shift value policy. it is somewhere in this object. the shift calues are equal for all joints...
        eo_emsapplbody_can_shiftvalues_t *shiftvalues = &p->configMC4boards2use.shiftvalues;
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;

        for(i=0; i<numofjomos; i++)
        {
            // ok, now i send the value to the relevant address
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
            
            // first is ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
            command.value = &shiftvalues->jointVelocityShift;                       
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
            
            // second is ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT
            eOtmp_estimShift_t estimshift = {0};
            estimshift.estimShiftJointVel= shiftvalues->jointVelocityEstimationShift;
            estimshift.estimShiftJointAcc = 0;
            estimshift.estimShiftMotorVel = 0;
            estimshift.estimShiftMotorAcc = 0;
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
            command.value = &estimshift;                       
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
        }        
        
    }
    
    return(eores_OK);
    

//#if OLDMODE    
//    
//    uint16_t                                numofjoint, i;
//    eOappTheDB_jointShiftValues_t           *shiftval_ptr;
//    eOicubCanProto_estimShift_t             estimshift;
//    icubCanProto_velocityShift_t            shift_icubCanProtValue;
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };
//    

//    // 2) config joints
//    numofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
//    
//    for(i=0; i<numofjoint; i++)
//    {
//        if((applrunMode__skinAndMc4 == p->appRunMode) || (applrunMode__mc4Only == p->appRunMode))
//        {            
//            // get shift values from DB
//            eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)i, &shiftval_ptr);
//            
//            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
//            shift_icubCanProtValue = shiftval_ptr->jointVelocityShift;
//            eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i,  msgCmd, (void*)&shift_icubCanProtValue);
//        
//            //set estim vel shift
//            estimshift.estimShiftJointVel= shiftval_ptr->jointVelocityEstimationShift;
//            estimshift.estimShiftJointAcc = 0;
//            estimshift.estimShiftMotorVel = 0;
//            estimshift.estimShiftMotorAcc = 0;
//            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
//            eo_appCanSP_SendCmd2Joint(p->bodyobjs.appCanSP, (eOmc_jointId_t)i,  msgCmd, (void*)&estimshift);
//        }
//    }
//    
//////VALE:removed because i send it when r.i. send me commnadS
////     //config skin if connected
////     if(eo_appTheDB_isSkinConnected(eo_appTheDB_GetHandle()))
////     {

////         eOappTheDB_jointOrMotorCanLocation_t    canLoc; //here I don't use eOappTheDB_SkinCanLocation_t because i need jmindexId filed to set triangle id
////         eOappTheDB_SkinCanLocation_t            skincanLoc;

////         msgCmd.class =  eocanprot_msgclass_pollingAnalogSensor; //currently this class not exist and it is remaped on sensor class
////         msgCmd.cmdId =  ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;


////         res = eo_appTheDB_GetSkinCanLocation(eo_appTheDB_GetHandle(), 0, &skincanLoc);
////         if(eores_OK != res)
////         {
////             return(res);
////         }
////         canLoc.emscanport = skincanLoc.emscanport;


////         for(i=8; i<15; i++)
////         {
////             eOicubCanProto_msgDestination_t msgdest;
////             msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, i);
////             res = eo_appCanSP_SendCmd(p->bodyobjs.appCanSP, canLoc.emscanport, msgdest, msgCmd, NULL);
////             if(eores_OK != res)
////             {
////                 return(res);
////             }
////         }

////     }
//    
//    return(eores_OK);
//#endif

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



// marco.accame on 19feb15: we dont want to disable mais transmission unless robotinterface does that.
//static eOresult_t s_eo_emsapplBody_MaisTXdisable(EOtheEMSapplBody *p)
//{
//    eOresult_t                  res;
//    eOas_maisId_t               sId = 0; //exist only one mais per ep
//    eOas_maismode_t             mode = eoas_maismode_acquirebutdonttx;
//    eOicubCanProto_msgCommand_t msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
//    };
//    res = eo_appCanSP_SendCmd2SnrMais(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&mode);
//    return(res);
//}

#warning marco.accame: see THIS...


static eOresult_t s_eo_emsapplBody_MaisStart(EOtheEMSapplBody *p)
{
    const uint8_t number = 0; 
    eOas_mais_config_t *cfg = eo_entities_GetMaisConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have mais
        return(eores_OK);
    }

    uint8_t datarate = cfg->datarate;       // it must be 10
    eOenum08_t mode = cfg->mode;            // it must be eoas_maismode_txdatacontinuously
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, number, 0);

    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    
    // set txmode
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    // set datarate
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    command.value = &datarate;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);    
    
    return(eores_OK);
    
    
//#if OLDMODE
////    eOerrmanDescriptor_t errdes = {0};
//            
//    // we start the mais with the values inside the mais.config data structure.
//    eOresult_t res = eores_NOK_generic;
//    const eOas_maisId_t maisId = 0; 
//    eOas_mais_config_t *maiscfg = eo_entities_GetMaisConfig(eo_entities_GetHandle(), maisId);
//    
//    if(NULL == maiscfg)
//    {   // we dont have mais
//        return(eores_OK);
//    }
//    
//    uint8_t datarate = maiscfg->datarate;       // it must be 10
//    eOenum08_t mode = maiscfg->mode;            // it must be eoas_maismode_txdatacontinuously
//    
//    // ok, now i go on
//    
//    eOicubCanProto_msgCommand_t msgCmd = {0};
//    EOappCanSP *cansp = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    

////    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_DEB_tag02);
////    errdes.par16                = 1;
////    errdes.par64                = 0x112233;
////    errdes.sourcedevice         = 0;
////    errdes.sourceaddress        = 0;                
////    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, s_eobj_ownname, &errdes);  
//    
//          
//    // set txmode       
//    msgCmd.class = eocanprot_msgclass_pollingAnalogSensor;
//    msgCmd.cmdId = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//    res = eo_appCanSP_SendCmd2SnrMais(cansp, maisId, msgCmd, (void*)&mode);     
//    if(eores_OK != res)
//    {
//        return(res);
//    }  

////    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_DEB_tag02);
////    errdes.par16                = 2;
////    errdes.par64                = 0x112233;
////    errdes.sourcedevice         = 0;
////    errdes.sourceaddress        = 0;                
////    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, s_eobj_ownname, &errdes);   

//    // set datarate    
//    msgCmd.class = eocanprot_msgclass_pollingAnalogSensor;
//    msgCmd.cmdId = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    res = eo_appCanSP_SendCmd2SnrMais(cansp, maisId, msgCmd, (void*)&datarate); 
//    if(eores_OK != res)
//    {
//        return(res);
//    }    

////    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_DEB_tag02);
////    errdes.par16                = 3;
////    errdes.par64                = 0x112233;
////    errdes.sourcedevice         = 0;
////    errdes.sourceaddress        = 0;                
////    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, s_eobj_ownname, &errdes);    
//        
//    return(res);     
//#endif
}


static eOresult_t s_eo_emsapplBody_SendTxMode2Strain(EOtheEMSapplBody *p)
{
    const uint8_t number = 0; 
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }

    eOenum08_t mode = cfg->mode;       
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, number, 0);
    // set txmode
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    return(eores_OK);
        
//#if OLDMODE    
//    eOresult_t                  res = eores_NOK_generic;
//    eOas_strainId_t             sId = 0; //exist only one mais per ep
//    eOas_strain_config_t        *straincfg = NULL;
//    eOicubCanProto_msgCommand_t msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
//    };

//    
//    straincfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), sId);
//    if(NULL == straincfg)
//    {
//         //if no strain is connected to ems ==> nothing to do ==> ok
//        return(eores_OK); 
//    }    
//   
//    
//    res = eo_appCanSP_SendCmd2SnrStrain(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&(straincfg->mode));

//    return(res);
//#endif    
}


static eOresult_t s_eo_emsapplBody_DisableTxStrain(EOtheEMSapplBody *p)
{
    const uint8_t number = 0; 
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }

    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, number, 0);
    // set txmode
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    return(eores_OK);  
    
//#if OLDMODE    
//    eOresult_t                  res;
//    eOas_strainId_t             sId = 0; //exist only one mais per ep
//    eOas_strainmode_t           mode = eoas_strainmode_acquirebutdonttx;
//    eOicubCanProto_msgCommand_t msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
//    };

//    
//    res = eo_appCanSP_SendCmd2SnrStrain(p->bodyobjs.appCanSP, sId, msgCmd, (void*)&mode);
//    if(eores_NOK_nodata == res)
//    {
//        //if no strain is connected to ems ==> nothing to do ==>ok
//        return(eores_OK);
//    }
//    return(res);
//#endif    
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



