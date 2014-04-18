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

/* @file       eOcfg_nvsEP_sk_usrcbk_ebx.c
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
#include "EOnv_hid.h"

#include "EOSkin.h"
#include "EoProtocol.h"

//application
#include "EOtheEMSapplBody.h"
#include "EOicubCanProto_specifications.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

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

//sk-init

extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv)
{
    eOsk_sigmode_t                  *sigmode = (eOsk_sigmode_t*)nv->ram;
    
    *sigmode = eosk_sigmode_signal;
}


extern void eoprot_fun_INIT_sk_skin_status_arrayof10canframes(const EOnv* nv)
{
    EOarray_of_10canframes *array_ptr = (EOarray_of_10canframes *)nv->ram;
    
    array_ptr->head.capacity = 10;       
    array_ptr->head.itemsize = sizeof(eOutil_canframe_t);
    array_ptr->head.size = 0;   

}
//sk-update
extern void eoprot_fun_UPDT_sk_skin_config_sigmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                      res;
    uint8_t                         i;
    eOsk_skinId_t                   skId = eoprot_ID2index(rd->id32);
    eOsk_sigmode_t                  *sigmode = (eOsk_sigmode_t*)nv->ram;
    EOappCanSP                      *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_cfg_skinInfo_t       *skconfig_ptr = NULL;
    uint8_t                         boardEndAddr;
    eOicubCanProto_msgDestination_t msgdest;
    
    
    
    res = eo_appTheDB_GetSkinConfigPtr(eo_appTheDB_GetHandle(), skId,  &skconfig_ptr);
    if(eores_OK != res)
    {
        return;
    }
            
    boardEndAddr = skconfig_ptr->boardAddrStart + skconfig_ptr->numofboards;
    
    
    
    switch(*sigmode)
    {
        case eosk_sigmode_dontsignal:
        {
            //in old way it not exist
            //in new way:
           icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
            
            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++)
            {
                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
                eOicubCanProto_msgCommand_t   msgCmd = 
                {
                    EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
                    EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
                };
                res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &sigmode);
                if(eores_OK != res)
                {
                    return;
                }
            }
        }break;
        
        case eosk_sigmode_signal:
        {
            icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_signal;
            
            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++)
            {
                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
                eOicubCanProto_msgCommand_t   msgCmd = 
                {
                    EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
                    EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
                };
                res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &sigmode);
                if(eores_OK != res)
                {
                    return;
                }
            }
        }break;

        case eosk_sigmode_signal_oldway:
        {

            eOicubCanProto_msgCommand_t msgCmd = 
            {
                EO_INIT(.class) icubCanProto_msgCmdClass_skinBoard,
                EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__TACT_SETUP
            };
            
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
            
            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++)
            {
                
                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
                res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd, NULL);
                if(eores_OK != res)
                {
                    return;
                }
                
                if(i == 0xE)
                {
                    eOicubCanProto_msgCommand_t msgCmd2 = 
                    {
                        EO_INIT(.class) icubCanProto_msgCmdClass_skinBoard,
                        EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2
                    };
                    
                    res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd2, NULL);
                    if(eores_OK != res)
                    {
                        return;
                    }
                }
            }
        }
        break;
    };

}



extern void eoprot_fun_UPDT_sk_skin_commands_boardscfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                      res;
    uint8_t                         i;
    eOsk_skinId_t                   skId = eoprot_ID2index(rd->id32);
    eOsk_cmd_boardsCfg_t            *brdCfg = (eOsk_cmd_boardsCfg_t*)nv->ram;
    EOappCanSP                      *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_cfg_skinInfo_t       *skconfig_ptr = NULL;
    eOicubCanProto_msgDestination_t msgdest;
    icubCanProto_skinboard_config_t canProto_skcfg;
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_skinBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG
    };
    
    res = eo_appTheDB_GetSkinConfigPtr(eo_appTheDB_GetHandle(), skId,  &skconfig_ptr);
    if(eores_OK != res)
    {
        return;
    }
    // I add this check at compile time to be sure i can assign values of eosk to icubcanProto
    #if (!(ICUBCANPROTO_SKINTYPE__WITHTEMPCOMP == EOSK_SKINTYPE_WITHTEMPCOMP))
        #error _skinType__withtempcomp has change value in eth proto or can proto
    #endif
    
    #if (!(ICUBCANPROTO_SKINTYPE__PALMFINGERTIP == EOSK_SKINTYPE_PALMFINGERTIP))
        #error _skinType__palmfingertip has change value in eth proto or can proto
    #endif
    #if (!(ICUBCANPROTO_SKINTYPE__WITHOUTTEMPCOMP == EOSK_SKINTYPE_WITHOUTTEMPCOMP))
        #error _skinType__withouttempcomp has change value in eth proto or can proto
    #endif
    
    canProto_skcfg.skintype = brdCfg->cfg.skintype;
    canProto_skcfg.period   = brdCfg->cfg.period;
    canProto_skcfg.noload   = brdCfg->cfg.noload;
    
    for(i = brdCfg->addrstart; i<=brdCfg->addrend; i++ )
    {
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
        res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &canProto_skcfg);
        if(eores_OK != res)
        {
            return;
        }
    }

}


extern void eoprot_fun_UPDT_sk_skin_commands_trianglescfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                          res;
    eOsk_skinId_t                       skId = eoprot_ID2index(rd->id32);
    eOsk_cmd_trianglesCfg_t             *trgsCfg = (eOsk_cmd_trianglesCfg_t*)nv->ram;
    EOappCanSP                          *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_cfg_skinInfo_t           *skconfig_ptr = NULL;
    eOappTheDB_cfg_skinInfo_t           **prova = NULL;
    eOicubCanProto_msgDestination_t     msgdest;
    icubCanProto_skintriangles_config_t canProto_trgscfg;
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_skinBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG
    };

    prova = &skconfig_ptr;
    prova = prova;
    res = eo_appTheDB_GetSkinConfigPtr(eo_appTheDB_GetHandle(), skId,  &skconfig_ptr);
    if(eores_OK != res)
    {
        return;
    }
    
    
    canProto_trgscfg.idstart   = trgsCfg->idstart;
    canProto_trgscfg.idend     = trgsCfg->idend;
    canProto_trgscfg.flags     = trgsCfg->cfg.enable;
    canProto_trgscfg.shift     = trgsCfg->cfg.shift;
    canProto_trgscfg.CDCoffset = trgsCfg->cfg.CDCoffset;

    

    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, trgsCfg->boardaddr);

    res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd,  &canProto_trgscfg);
    if(eores_OK != res)
    {
        return;
    }

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



