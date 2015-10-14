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

/* @file       EoProtocolSK_fun_ems4rd.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "hal.h"

#include "EoCommon.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoSkin.h"
#include "EoProtocol.h"


#include "EOtheCANService.h"

//#include "EOtheEMSapplBody.h"

#include "EOtheSKIN.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolSK.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//// marco.accame: we start with a silent skin and with an empty status_arrayofcandata.
//// then we put skin in sigmode eosk_sigmode_signal only if robotInterface orders it to the board.


//extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv)
//{
//    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)eo_nv_RAM(nv);
//    
//    *sigmode = eosk_sigmode_dontsignal; 
//}


//extern void eoprot_fun_INIT_sk_skin_status_arrayofcandata(const EOnv* nv)
//{
//    EOarray_of_skincandata_t *tmp = (EOarray_of_skincandata_t*)eo_nv_RAM(nv);
//    tmp = tmp;
//    // marco.accame: items of array are eOsk_candata_t. its capacity is:
//    uint16_t capacity = sizeof(tmp->data) / sizeof(eOsk_candata_t);    
//    // eo_array_New() initialises capacity and itemsize and also sets size to 0 
//    EOarray *array = eo_array_New(capacity, sizeof(eOsk_candata_t), eo_nv_RAM(nv));
//}


extern void eoprot_fun_UPDT_sk_skin_config_sigmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
#if 1
    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)rd->data;  
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eo_skin_ConfigTXMode(eo_skin_GetHandle(), index, *sigmode); 
#else    
    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)rd->data;  
    //icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_dontsignal
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = NULL;       
    
    switch(*sigmode)
    {
        case eosk_sigmode_dontsignal:
        {
            // in old way it does not exist
            // in new way:
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_dontsignal;
            command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, rd->id32);
            
        } break;
        
        case eosk_sigmode_signal:
        {
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_signal;
            command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, rd->id32);

        } break;

        case eosk_sigmode_signal_oldway:
        {
            // we need to change the class and type as well
            command.class = eocanprot_msgclass_pollingSkin;    
            command.type  = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;
            command.value = NULL;     
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, rd->id32);
            
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
#endif        
}


extern void eoprot_fun_UPDT_sk_skin_cmmnds_boardscfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
#if 1
    eOsk_cmd_boardsCfg_t *brdCfg = (eOsk_cmd_boardsCfg_t*)rd->data;
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eo_skin_ConfigBoards(eo_skin_GetHandle(), index, brdCfg);
#else    
    eOsk_cmd_boardsCfg_t *brdCfg = (eOsk_cmd_boardsCfg_t*)rd->data;
    
    icubCanProto_skinboard_config_t canProto_skcfg = {0};
    canProto_skcfg.skintype = (icubCanProto_skinType_t)brdCfg->cfg.skintype; 
    canProto_skcfg.period   = brdCfg->cfg.period;
    canProto_skcfg.noload   = brdCfg->cfg.noload;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG;
    command.value = &canProto_skcfg; 
    
    // and now we send the command to all the skin boards
    eo_canserv_SendCommandToAllBoardsInEntity(eo_canserv_GetHandle(), &command, rd->id32);   
#endif    
}


extern void eoprot_fun_UPDT_sk_skin_cmmnds_trianglescfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
#if 1
    eOsk_cmd_trianglesCfg_t *trgsCfg = (eOsk_cmd_trianglesCfg_t*)rd->data;
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eo_skin_ConfigTriangles(eo_skin_GetHandle(), index, trgsCfg);
#else    
    eOsk_cmd_trianglesCfg_t *trgsCfg = (eOsk_cmd_trianglesCfg_t*)rd->data;
    
    icubCanProto_skintriangles_config_t canProto_trgscfg = {0};
    canProto_trgscfg.idstart   = trgsCfg->idstart;
    canProto_trgscfg.idend     = trgsCfg->idend;
    canProto_trgscfg.flags     = trgsCfg->cfg.enable;
    canProto_trgscfg.shift     = trgsCfg->cfg.shift;
    canProto_trgscfg.CDCoffset = trgsCfg->cfg.CDCoffset;
    
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingSkin;    
    command.type  = ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG;
    command.value = &canProto_trgscfg; 
    
    eOcanmap_location_t location = {0};
    eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), rd->id32, &location, NULL, NULL);
    // the function eo_canmap_GetEntityLocation() puts in location.addr the address of the first board. we want to override that.
    location.addr = trgsCfg->boardaddr;
    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, location);
#endif        
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



