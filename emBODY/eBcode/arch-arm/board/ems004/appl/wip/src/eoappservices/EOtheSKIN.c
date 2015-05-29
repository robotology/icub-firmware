/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolSK.h"

#include "EOMtheEMSappl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void* s_eo_skin_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheSKIN s_eo_theskin = 
{
    .initted            = eobool_false,
    .thereisskin        = eobool_false,
    .numofskins         = 0,
    .command            = {0}
};

//static const char s_eobj_ownname[] = "EOtheSKIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSKIN* eo_skin_Initialise(void)
{
    if(eobool_true == s_eo_theskin.initted)
    {
        return(&s_eo_theskin);
    }
    
    s_eo_theskin.numofskins = eo_entities_NumOfSkins(eo_entities_GetHandle());

    if(0 == s_eo_theskin.numofskins)
    {
        s_eo_theskin.thereisskin = eobool_false;
    }
    else
    {
        s_eo_theskin.thereisskin = eobool_true;
    }

    s_eo_theskin.initted = eobool_true;
    
    return(&s_eo_theskin);   
}


extern EOtheSKIN* eo_skin_GetHandle(void)
{
    return(eo_skin_Initialise());
}


extern eOresult_t eo_skin_DisableTX(EOtheSKIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theskin.thereisskin)
    {   // nothing to do because we dont have a skin board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
    s_eo_theskin.command.class = eocanprot_msgclass_pollingSkin;
    s_eo_theskin.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;       
    s_eo_theskin.command.value = &sigmode;

    uint8_t i=0;
    eOsk_skin_t *skin = NULL;
    for(i=0; i<s_eo_theskin.numofskins; i++)
    {
        // i stop this skin only if it was started before
        if(NULL == (skin = eo_entities_GetSkin(eo_entities_GetHandle(), i)))
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
        // we send the same s_eo_theskin.command to all of them
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, i, 0);
        eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &s_eo_theskin.command, id32);    
    }
    
    return(eores_OK);

}

extern eOresult_t eo_skin_ConfigTXMode(EOtheSKIN *p, uint8_t skinindex, eOsk_sigmode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theskin.thereisskin)
    {   // nothing to do because we dont have a skin board
        return(eores_OK);
    } 

    if(skinindex >= s_eo_theskin.numofskins)
    {
        return(eores_NOK_generic);
    }
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = NULL;   

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, skinindex, eoprot_tag_none);
    
    switch(mode)
    {
        case eosk_sigmode_dontsignal:
        {
            // in old way it does not exist
            // in new way:
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_dontsignal;
            command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32);
            
        } break;
        
        case eosk_sigmode_signal:
        {
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_signal;
            command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32);

        } break;

        case eosk_sigmode_signal_oldway:
        {
            // we need to change the class and type as well
            command.class = eocanprot_msgclass_pollingSkin;    
            command.type  = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;
            command.value = NULL;     
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32);
            
            #warning TBD: VERY IMPORTANT -> see the following code, where we need also a special message to board with address 0xE

//            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++)
//            {
//                
//                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
//                res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd, NULL);
//                if(eores_OK != res)
//                {
//                    return;
//                }
//                
//                if(0xE == i)
//                {
//                    eOicubCanProto_msgCommand_t msgCmd2 = 
//                    {
//                        EO_INIT(.class) icubCanProto_msgCmdClass_skinBoard,
//                        EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2
//                    };
//                    
//                    res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd2, NULL);
//                    if(eores_OK != res)
//                    {
//                        return;
//                    }
//                }
//            }
        } break;
        
        default:
        {
        } break;
    }        

    
    return(eores_OK);
}


extern eOresult_t eo_skin_ConfigBoards(EOtheSKIN *p, uint8_t skinindex, eOsk_cmd_boardsCfg_t *brdcfg)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theskin.thereisskin)
    {   // nothing to do because we dont have a skin board
        return(eores_OK);
    } 

    if(skinindex >= s_eo_theskin.numofskins)
    {
        return(eores_NOK_generic);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, skinindex, eoprot_tag_none);
    
    icubCanProto_skinboard_config_t canProto_skcfg = {0};
    canProto_skcfg.skintype = (icubCanProto_skinType_t)brdcfg->cfg.skintype; 
    canProto_skcfg.period   = brdcfg->cfg.period;
    canProto_skcfg.noload   = brdcfg->cfg.noload;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG;
    command.value = &canProto_skcfg; 
    
    // and now we send the command to all the skin boards
    eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, id32); 
    
    return(eores_OK);          
}

extern eOresult_t eo_skin_ConfigTriangles(EOtheSKIN *p, uint8_t skinindex, eOsk_cmd_trianglesCfg_t *trgcfg)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theskin.thereisskin)
    {   // nothing to do because we dont have a skin board
        return(eores_OK);
    } 

    if(skinindex >= s_eo_theskin.numofskins)
    {
        return(eores_NOK_generic);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, skinindex, eoprot_tag_none);
    
    icubCanProto_skintriangles_config_t canProto_trgscfg = {0};
    canProto_trgscfg.idstart   = trgcfg->idstart;
    canProto_trgscfg.idend     = trgcfg->idend;
    canProto_trgscfg.flags     = trgcfg->cfg.enable;
    canProto_trgscfg.shift     = trgcfg->cfg.shift;
    canProto_trgscfg.CDCoffset = trgcfg->cfg.CDCoffset;
    
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG;
    command.value = &canProto_trgscfg; 
    
    eOcanmap_location_t location = {0};
    eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL);
    // the function eo_canmap_GetEntityLocation() puts in location.addr the address of the first board. we want to override that.
    location.addr = trgcfg->boardaddr;
    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location);    
   
    return(eores_OK);          
}

//extern eOresult_t eo_skin_ProcessRXframe(EOtheSKIN *p, eOcanframe_t *frame, eOcanport_t port)
//{
//    if((NULL == p) || (NULL == frame))
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    if(eobool_false == s_eo_theskin.thereisskin)
//    {   // nothing to do because we dont have a skin board
//        return(eores_OK);
//    }    


//    // marco.accame:        
//    // if skin->config.sigmode is eosk_sigmode_dontsignal then we dont need using the payload of the can frame. 
//    // however, also if skin->config.sigmode is eosk_sigmode_signal but we are not in RUN mode we should not put 
//    // frames inside the arrayofcandata. this latter for example is tha case if we are still in the cfg->run transition 
//    // and thus not  yet inside the control-loop which empties the arrayofcandata, or also if  the udp packet with go2run 
//    // rop<> gets lost.
//    
//    
//    eOsk_skin_t *skin = NULL;
//    eOprotIndex_t index = EOK_uint08dummy;
//    
//    if(NULL == (skin = s_eo_skin_get_entity(eoprot_endpoint_skin, eoprot_entity_sk_skin, frame, port, &index)))
//    {
//        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
//        return(eores_OK);  
//    }            
//    
//    
//    // we may decode some canframes of this kind if we pass from run to config mode and we process frame buffered in the rx-fifo    
//    if(eosk_sigmode_dontsignal == skin->config.sigmode)
//    {
//        return(eores_OK);
//    } 

//    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STcfg;
//    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
//    if(eo_sm_emsappl_STrun != applstate)
//    {
//        return(eores_OK);
//    }
//    
//    // otherwise we put the canframe content inside the arrayofcandata
//    eOsk_candata_t candata = {0};
//    uint16_t info = EOSK_CANDATA_INFO(frame->size, frame->id);
//    candata.info = info;    
//    memcpy(candata.data, frame->data, sizeof(candata.data));   
//    
//    if(eores_OK != eo_array_PushBack((EOarray*)(&skin->status.arrayofcandata), &candata))
//    {   
//        eOerrmanDescriptor_t des = {0};
//        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_arrayofcandataoverflow);
//        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
//        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
//        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
//        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des); 
//    }


//    return(eores_OK);
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the skin ... better in here rather than elsewhere.

// marco.accame: we start with a silent skin and with an empty status_arrayofcandata.
// then we put skin in sigmode eosk_sigmode_signal only if robotInterface orders it to the board.


extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv)
{
    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)eo_nv_RAM(nv);
    
    *sigmode = eosk_sigmode_dontsignal; 
}


extern void eoprot_fun_INIT_sk_skin_status_arrayofcandata(const EOnv* nv)
{
    EOarray_of_skincandata_t *tmp = (EOarray_of_skincandata_t*)eo_nv_RAM(nv);
    tmp = tmp;
    // marco.accame: items of array are eOsk_candata_t. its capacity is:
    uint16_t capacity = sizeof(tmp->data) / sizeof(eOsk_candata_t);    
    // eo_array_New() initialises capacity and itemsize and also sets size to 0 
    EOarray *array = eo_array_New(capacity, sizeof(eOsk_candata_t), eo_nv_RAM(nv));
}

extern eObool_t eocanprotSKperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_SKIN_MSG(eOsk_skin_t *skin)
{    
    if(NULL == skin)
    {
        return(eobool_true);
    }
    
    // marco.accame:        
    // if skin->config.sigmode is eosk_sigmode_dontsignal then we dont need using the payload of the can frame. 
    // however, also if skin->config.sigmode is eosk_sigmode_signal but we are not in RUN mode we should not put 
    // frames inside the arrayofcandata. this latter for example is tha case if we are still in the cfg->run transition 
    // and thus not  yet inside the control-loop which empties the arrayofcandata, or also if  the udp packet with go2run 
    // rop<> gets lost.
    
    // we may decode some canframes of this kind if we pass from run to config mode and we process frame buffered in the rx-fifo    
    if(eosk_sigmode_dontsignal == skin->config.sigmode)
    {
        return(eobool_true);
    } 

    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun != applstate)
    {
        return(eobool_true);
    }
    
    return(eobool_false);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void* s_eo_skin_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eOcanmap_location_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    ii = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, endpoint, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, endpoint, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



